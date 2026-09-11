// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_FIghter_Kick.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Animation/AnimMontage.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

void UGA_FIghter_Kick::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo))
	{
		return;
	}
	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	MeshComponent = AvatarCharacter->GetMesh();
	
	FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	
	auto* KickMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,NAME_None,KickMontage,1.f,NAME_None,true,
		1.f,0.f,true
	);

	auto* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,FName("None"),Forward,600.f,0.34f,
		true,nullptr,ERootMotionFinishVelocityMode::SetVelocity,Forward*100.f,100.f,false);

	auto* WaitAttackMomentTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_Attack,nullptr,true,true);

	
	KickMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageInterrupted);
	KickMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	WaitAttackMomentTask->EventReceived.AddUniqueDynamic(this,&ThisClass::OnAttackEventReceived);
	
	RootMotionTask->ReadyForActivation();
	KickMontageTask->ReadyForActivation();
	WaitAttackMomentTask->ReadyForActivation();

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GASTAG::GameplayCue_Fighter_KickWind);
	GetAbilitySystemComponentFromActorInfo()->ForceReplication();
}

void UGA_FIghter_Kick::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FIghter_Kick::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_FIghter_Kick::OnMontageInterrupted()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_FIghter_Kick::OnAttackEventReceived(const FGameplayEventData Data)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	FVector FootLocation =  MeshComponent->GetSocketLocation(FName("foot_r"));

	FVector SpawnLocation = FootLocation + AvatarCharacter->GetActorForwardVector()*BoxExtent.X/2+FVector(0,0,BoxExtent.Z);

	FQuat Rotation = AvatarCharacter->GetActorQuat();
	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(KickAttackOverlap), false, AvatarCharacter);
	FCollisionShape Shape = FCollisionShape::MakeBox(BoxExtent);
	
	GetWorld()->OverlapMultiByObjectType(Overlaps,SpawnLocation,FQuat::Identity,ObjectQueryParams,Shape,QueryParams);

	TSet<AActor*> OverlapActors;
	
	for (const FOverlapResult& R : Overlaps)
	{
		if (AActor* A = R.GetActor())
		{
			OverlapActors.Add(A);
		}
	}

	for (const auto& Actor:OverlapActors)
	{
	
		if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			KnockBackASCActors(Actor);
		}
	}
}

void UGA_FIghter_Kick::KnockBackASCActors(AActor* Actor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(GEKnockBack,1.f);
	float Damage = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
	
	Spec.Data->SetSetByCallerMagnitude(GASTAG::Data_Damage,Damage*13.5f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(),TargetASC);

	FGameplayEffectSpecHandle Spec2 = MakeOutgoingGameplayEffectSpec(GEKnockBackTag,1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec2.Data.Get(),TargetASC);

	TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fighter_PunchHit);
	GetAbilitySystemComponentFromActorInfo()->ForceReplication();
}
