// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Attack_Range_Ability.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/Base/EnemyProjectileBase.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Attack_Range_Ability::UEnemy_Attack_Range_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Attack;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);

}

bool UEnemy_Attack_Range_Ability::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UEnemy_Attack_Range_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		CurrentTarget = const_cast<AActor*>(TriggerEventData->Target.Get());
        
		// 공격시 타겟으로 회전
		FVector TargetLocation = TriggerEventData->Target->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
		FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		
		Actor->SetActorRotation(NewRotation);

		PlayAttackMontage();
	}
}

void UEnemy_Attack_Range_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UEnemy_Attack_Range_Ability::OnNotifyBegin);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Attack_Range_Ability::PlayAttackMontage()
{
	if (!Actor || !Actor->AttackMontage)
	{
		return;
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
	}

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
		this,
		NAME_None,
		Actor->AttackMontage,
		AttackSpeed
	);
	
	if (!Task)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Task"));
		return;
	}
	Task->OnCompleted.AddDynamic(this, &UEnemy_Attack_Range_Ability::OnMontageEnded);
	Task->OnBlendOut.AddDynamic(this, &UEnemy_Attack_Range_Ability::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UEnemy_Attack_Range_Ability::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UEnemy_Attack_Range_Ability::OnMontageEnded);


	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInst = Actor->GetMesh()->GetAnimInstance();
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_Attack_Range_Ability::OnNotifyBegin);
	}
	
	Task->ReadyForActivation();
}

void UEnemy_Attack_Range_Ability::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UEnemy_Attack_Range_Ability::OnNotifyBegin(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	
	if (NotifyName == FName("AttackHit") && Actor && Actor->HasAuthority())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentTarget);

		FActorSpawnParameters Params;
		Params.Owner = Actor;
		Params.Instigator = Actor;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector SpawnLocation = Actor->GetMesh()->GetSocketLocation(FName("Muzzle"));
		
		FVector Direction = CurrentTarget->GetActorLocation() - SpawnLocation;
		FRotator ProjectileRotation = FRotator::ZeroRotator;

		if (Direction.Size() > 0.0f)
		{
			Direction.Normalize();
			ProjectileRotation = Direction.Rotation(); 
		}
    
		if (AEnemyProjectileBase* Projectile =
			  GetWorld()->SpawnActor<AEnemyProjectileBase>(Actor->GetRangedProjectileClass(), SpawnLocation, ProjectileRotation, Params))
		{
			Projectile->DamageEffect = DamageEffect; 
			Projectile->EffectLevel = GetAbilityLevel();
			Projectile->AttackDamage = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute());
      
			// float MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetProjectileSpeedAttribute());
			// Projectile->SetProjectileSpeed(MovementSpeed); 
		}
		
		// 이펙트 사운드 추가

	}
}

