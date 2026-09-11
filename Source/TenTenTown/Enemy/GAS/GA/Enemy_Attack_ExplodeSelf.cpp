// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Attack_ExplodeSelf.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/Beholder_AttributeSet.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Structure/Crossbow/CrossbowStructure.h"

UEnemy_Attack_ExplodeSelf::UEnemy_Attack_ExplodeSelf()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Attack;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);
}

bool UEnemy_Attack_ExplodeSelf::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UEnemy_Attack_ExplodeSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (TriggerEventData)
	{
		Actor = const_cast<AEnemyBase*>(Cast<AEnemyBase>(TriggerEventData->Instigator.Get()));
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());

		// 공격시 타겟으로 회전
		FVector TargetLocation = TriggerEventData->Target->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
		FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		
		Actor->SetActorRotation(NewRotation);

		PlayAttackMontage();		
	}
	
}

void UEnemy_Attack_ExplodeSelf::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Attack_ExplodeSelf::PlayAttackMontage()
{
	if (!Actor || !Actor->AttackMontage)
	{
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
		this,
		NAME_None,
		Actor->AttackMontage,
		1.0f
	);
	
	if (!Task)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Task"));
		return;
	}
	
	Task->OnCompleted.AddDynamic(this, &UEnemy_Attack_ExplodeSelf::OnMontageEnded);
	Task->OnBlendOut.AddDynamic(this, &UEnemy_Attack_ExplodeSelf::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UEnemy_Attack_ExplodeSelf::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UEnemy_Attack_ExplodeSelf::OnMontageEnded);


	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInst = Actor->GetMesh()->GetAnimInstance();
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_Attack_ExplodeSelf::OnNotifyBegin);
	}
	
	Task->ReadyForActivation();
	
}

void UEnemy_Attack_ExplodeSelf::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UEnemy_Attack_ExplodeSelf::OnNotifyBegin(FName NotifyName,
                                              const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == FName("AttackHit") && Actor && Actor->HasAuthority())
	{
		if (TargetActor)
		{
			//이펙트와 사운드 재생
			//if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
			//{
			//	FGameplayCueParameters SoundCueParams;
			//	ASC->GetOwnedGameplayTags(SoundCueParams.AggregatedSourceTags);
			//	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Sound_Attack, SoundCueParams);
           //
			//	FGameplayCueParameters EffectCueParams;
			//	EffectCueParams.Instigator = Actor;
			//	EffectCueParams.Location = TargetActor->GetActorLocation();
			//	ASC->GetOwnedGameplayTags(EffectCueParams.AggregatedSourceTags);
			//	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Effect_Attack,EffectCueParams);
			//}
			ExecuteExplosion();

		}
		
	}
	
}

void UEnemy_Attack_ExplodeSelf::ExecuteExplosion()
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	// 비주얼 이펙트 추가
	FGameplayEffectContextHandle ExplodeContext = ASC->MakeEffectContext();
	ExplodeContext.AddInstigator(Actor, Actor);
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ExplodeEffect, GetAbilityLevel(), ExplodeContext);
	
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	//ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Effect_ExplodeSelf, ExplodeContext);


	//건물 감지 못함!!

	TArray<AActor*> OverlapActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; 
	TArray<AActor*> IgnoreActors;

	bool bResult = UKismetSystemLibrary::SphereOverlapActors(
		Actor->GetWorld(),
		Actor->GetActorLocation(),
		ASC->GetNumericAttribute(UBeholder_AttributeSet::GetExplodeRangeAttribute()),
		ObjectTypes, 
		(ABaseCharacter::StaticClass(), ACrossbowStructure::StaticClass()),
		IgnoreActors,
		OverlapActors
	);

	float Attack = ASC->GetNumericAttributeBase(UBeholder_AttributeSet::GetAttackAttribute());
    
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddInstigator(Actor, Actor);
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);

	if (SpecHandle.IsValid()) 
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, Attack);
		
		for (const AActor* Target : OverlapActors)
		{
			if (const ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Target))
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetCharacter);
				
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
		
		if (ASC && ASC->GetOwnerActor()->HasAuthority())
		{
			ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Dead);

			FGameplayEventData Payload;
			Payload.EventTag = GASTAG::Enemy_Ability_Dead;
			ASC->HandleGameplayEvent(Payload.EventTag, &Payload);

		}
	}
}
