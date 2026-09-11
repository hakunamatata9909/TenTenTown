// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Sprint.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Sprint : public UBaseGameplayAbility
{
	GENERATED_BODY()

	UGA_Sprint();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> SprintGEClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	float SprintSpeed;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveSprintGEHandle;

	UPROPERTY()
	UCharacterMovementComponent* CMC;

	UPROPERTY()
	float BaseWalkSpeed;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);
};