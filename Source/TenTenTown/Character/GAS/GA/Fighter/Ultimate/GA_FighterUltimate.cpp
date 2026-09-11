// Fill out your copyright notice in the Description page of Project Settings.
#include "GA_FighterUltimate.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_FighterUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bool bCommit = CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo);
	if (!bCommit)
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
		return;
	}
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	CMC = Cast<ACharacter>(GetAvatarActorFromActorInfo())->GetCharacterMovement();
	OriginWalkSpeed = CMC->MaxWalkSpeed;
	
	auto* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("UltimateAbilityMontageTask"),UltimateAnimMontage);
	
	MontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageCancel);
	MontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	
	FGameplayEffectSpecHandle GEHandle = ASC->MakeOutgoingSpec(DurationGE,GetAbilityLevel(),ASC->MakeEffectContext());
	GEHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Duration,15.f);
	GEHandle.Data->AddDynamicAssetTag(GASTAG::Data_Duration);
	GEHandle.Data->DynamicGrantedTags.AddTag(GASTAG::State_UltimateOnGoing);
	ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	
	auto* WaitGameplayEffectRemoveTask = UAbilityTask_WaitGameplayEffectRemoved::WaitForGameplayEffectRemoved(
			this,ActiveGEHandle);
	WaitGameplayEffectRemoveTask->OnRemoved.AddUniqueDynamic(this,&ThisClass::OnGERemoved);

	CMC->MaxWalkSpeed=0;
	ASC->AddLooseGameplayTag(GASTAG::State_Block_Everything);
	MontageTask->ReadyForActivation();
	WaitGameplayEffectRemoveTask->ReadyForActivation();
	
	ASC->AddGameplayCue(GASTAG::GameplayCue_Fighter_Ultimate);
	ASC->ForceReplication();
	
}

void UGA_FighterUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ASC->RemoveGameplayCue(GASTAG::GameplayCue_Fighter_Ultimate);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FighterUltimate::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	CMC->MaxWalkSpeed=OriginWalkSpeed;
	ASC->RemoveLooseGameplayTag(GASTAG::State_Block_Everything);
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_FighterUltimate::OnGERemoved(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_FighterUltimate::OnMontageEnd()
{
	ASC->RemoveLooseGameplayTag(GASTAG::State_Block_Everything);
	CMC->MaxWalkSpeed=OriginWalkSpeed;
}

void UGA_FighterUltimate::OnMontageCancel()
{
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
}
