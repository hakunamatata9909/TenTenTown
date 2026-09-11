// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_CharacterRevive.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_CharacterRevive : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	UGA_CharacterRevive();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UAnimMontage> ReviveMontage;
	UPROPERTY()
	TEnumAsByte<EMovementMode> LastMovementMode;
	UFUNCTION()
	void OnMontageEnd();
};
