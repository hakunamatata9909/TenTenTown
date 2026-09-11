// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_BaseAbilityCombatState.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GameplayTagsManager.h"

UGE_BaseAbilityCombatStateInfinite::UGE_BaseAbilityCombatStateInfinite()
{
	DurationPolicy=EGameplayEffectDurationType::Infinite;
	
	UTargetTagsGameplayEffectComponent* TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
	
	FInheritedTagContainer GrantedTagContainer;
	GrantedTagContainer.AddTag(GASTAG::State_Character_Combat);
	
	TargetTagsComponent->SetAndApplyTargetTagChanges(GrantedTagContainer);
	
	GEComponents.Add(TargetTagsComponent);
	
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount=1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackExpirationPolicy =EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
}

UGE_BaseAbilityCombatStateDuration::UGE_BaseAbilityCombatStateDuration()
{
	DurationPolicy=EGameplayEffectDurationType::HasDuration;
	
	UTargetTagsGameplayEffectComponent* TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
	
	FInheritedTagContainer TagContainer;
	TagContainer.AddTag(GASTAG::State_Character_Combat);
	
	TargetTagsComponent->SetAndApplyTargetTagChanges(TagContainer);
	GEComponents.Add(TargetTagsComponent);
	
	DurationMagnitude = FScalableFloat(5.0f);
	
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount=1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackExpirationPolicy =EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
	
}
