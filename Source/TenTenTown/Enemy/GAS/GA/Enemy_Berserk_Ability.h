// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Berserk_Ability.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Berserk_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEnemy_Berserk_Ability();

protected:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION()
	void OnMontageEnded();
};
