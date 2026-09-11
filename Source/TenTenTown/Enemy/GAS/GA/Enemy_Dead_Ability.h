// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Dead_Ability.generated.h"

class AEnemyBase;
/**
 * 
 */

UCLASS()
class TENTENTOWN_API UEnemy_Dead_Ability : public UGameplayAbility
{
	GENERATED_BODY()

	UEnemy_Dead_Ability();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void DropItem(AEnemyBase* Actor);
	
	UFUNCTION()
	void OnDeathMontageFinished();

};
