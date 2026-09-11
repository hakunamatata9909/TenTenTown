// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ArcherSkillAKnockback.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/World.h"

void UGA_ArcherSkillAKnockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
		return;
	}
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	if (AttackAnimMontage)
	{
		UAbilityTask_PlayMontageAndWait* AttackAnimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,FName("Montage_Attack"),AttackAnimMontage);
		
		AttackAnimMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		AttackAnimMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		AttackAnimMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		
		AttackAnimMontageTask->ReadyForActivation();
	}

	TArray<FOverlapResult> OverlapResults;
	FVector OverlapPos = AvatarCharacter->GetActorLocation();
	FQuat OverlapRot = AvatarCharacter->GetActorRotation().Quaternion();
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarCharacter);
	
	FCollisionShape CollisionShape(FCollisionShape::MakeBox(FVector(150.f,150.f,150.f)));
	
	GetWorld()->OverlapMultiByObjectType(OverlapResults,OverlapPos,OverlapRot,ObjectQueryParams,CollisionShape,QueryParams);

	TSet<AActor*> OverlappedActors;
	
	for (auto& OR :OverlapResults)
	{
		AActor* OverlappedActor = OR.GetActor();
		
		if (OverlappedActor)
		{
			OverlappedActors.Add(OverlappedActor);
		}
	}

	for (const auto& Actor : OverlappedActors)
	{
		if (!Cast<AEnemyBase>(Actor)) continue;
		
		if (!DamageGEClass)
		{
			UE_LOG(LogTemp, Error, TEXT("No DamageGEClass In %s"),TEXT(__FUNCTION__));
			return;
		}
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);

		if (TargetASC)
		{
			float Damage = ASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageGEClass,1.f,ASC->MakeEffectContext());
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG::Data_Damage,Damage*9.f);

			ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),TargetASC);
		}
	
	}
}

void UGA_ArcherSkillAKnockback::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ArcherSkillAKnockback::OnEndMontage()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
