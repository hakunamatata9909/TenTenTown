// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GE_BaseAbilityCombatState.h"

void UBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (bIsCombatAbility)
	{
		const UGameplayEffect* InfiniteGE = UGE_BaseAbilityCombatStateInfinite::StaticClass()->GetDefaultObject<UGameplayEffect>();
		InfiniteGEHandle = ApplyGameplayEffectToOwner(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,InfiniteGE,1.f);
	}
	
}

void UBaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsActive() || bIsAbilityEnding) return; 
	
	if (bIsCombatAbility)
	{
		if (InfiniteGEHandle.IsValid())
		{
			UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			ASC->RemoveActiveGameplayEffect(InfiniteGEHandle);
			InfiniteGEHandle.Invalidate();
		}
		const UGameplayEffect* DurationGEEffect =UGE_BaseAbilityCombatStateDuration::StaticClass()->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,DurationGEEffect,1.f);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bSatisfies = Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);

	if (!bSatisfies)
	{
		return false;
	}

	const FGameplayTagContainer& BlockedTags = AbilitySystemComponent.GetBlockedAbilityTags();
	const FGameplayTagContainer& CurrentAssetTags = GetAssetTags();

	if (!BlockedTags.IsEmpty() && !CurrentAssetTags.IsEmpty())
	{
		if (CurrentAssetTags.HasAny(BlockedTags))
		{
			return false; 
		}
	}

	return true;
}