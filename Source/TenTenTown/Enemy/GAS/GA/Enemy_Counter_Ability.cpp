// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/GAS/GA/Enemy_Counter_Ability.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Counter_Ability::UEnemy_Counter_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Counter;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);
}

void UEnemy_Counter_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (TriggerEventData)
	{
		Actor = const_cast<ABlackKnight*>(Cast<ABlackKnight>(TriggerEventData->Instigator.Get()));
		CurrentTarget = const_cast<AActor*>(TriggerEventData->Target.Get());

		if (!Actor || !CurrentTarget)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		FVector TargetLocation = CurrentTarget->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
		Actor->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		PlayCounterMontage();
	}
}



void UEnemy_Counter_Ability::PlayCounterMontage()
{
	if (!Actor || !Actor->CounterMontage)
	{
		return;
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
	}
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Actor->CounterMontage,
		AttackSpeed
	);

	if (!Task)
	{
		return;
	}
	Task->OnCompleted.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnBlendOut.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);

	if (Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_Counter_Ability::OnNotifyBegin);
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(Actor->EnemyGuard, ASC);
	}

	Task->ReadyForActivation();
}

void UEnemy_Counter_Ability::ApplyDamageToTarget(AActor* TargetActor)
{
	if (!TargetActor || !Actor)
		return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if (!ASC || !TargetASC)
		return;

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddInstigator(Actor, Actor);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackAttribute())*2);
		ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		FVector KnockBackDir = (TargetCharacter->GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal();
		FVector KnockBackVelocity = KnockBackDir * 2000.f + FVector(0,0,500.f); 
		TargetCharacter->LaunchCharacter(KnockBackVelocity, true, true);
	}
}

void UEnemy_Counter_Ability::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == FName("CounterHit") && Actor && Actor->HasAuthority())
	{
		TArray<TWeakObjectPtr<AActor>> Targets = Actor->GetOverlappedPawns();
		for (TWeakObjectPtr<AActor> WeakTarget : Targets)
		{
			AActor* TargetActor = WeakTarget.Get();
			if (TargetActor)
			{
				ApplyDamageToTarget(TargetActor);

				if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
				{
					FGameplayCueParameters SoundCueParams;
					ASC->GetOwnedGameplayTags(SoundCueParams.AggregatedSourceTags);
					ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Sound_Attack, SoundCueParams);

					FGameplayCueParameters EffectCueParams;
					EffectCueParams.Instigator = Actor;
					EffectCueParams.Location = TargetActor->GetActorLocation();
					ASC->GetOwnedGameplayTags(EffectCueParams.AggregatedSourceTags);
					ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Effect_Attack, EffectCueParams);
				}
			}
		}
	}
}

void UEnemy_Counter_Ability::OnMontageEnded()
{
	if (!Actor)
		return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Counter);
		ASC->SetNumericAttributeBase(UBlackKnight_AttributeSet::GetFrontHitCountAttribute(), 0.f);
	}

	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UEnemy_Counter_Ability::OnNotifyBegin);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UEnemy_Counter_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UEnemy_Counter_Ability::OnNotifyBegin);
	}

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Counter);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
