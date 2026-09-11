// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Teleport_Ability.generated.h"

class ASplineActor;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Teleport_Ability : public UGameplayAbility
{
	GENERATED_BODY()

	UEnemy_Teleport_Ability();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	ASplineActor* FindRandomSpline(const UWorld* World, const class ASplineActor* CurrentSpline) const;
};
