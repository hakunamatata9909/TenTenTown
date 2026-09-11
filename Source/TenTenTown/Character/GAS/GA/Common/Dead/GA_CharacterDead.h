// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_CharacterDead.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_CharacterDead : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	UGA_CharacterDead();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<ABaseCharacter> AvatarCharacter;
	
	UPROPERTY()
	TEnumAsByte<EMovementMode> LastMovementMode;
	UFUNCTION()
	void OnWaitDelayEnd();
};
