// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Dash.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Dash : public UBaseGameplayAbility
{
	GENERATED_BODY()

	UGA_Dash();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnTaskFinished();
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Strength",meta=(AllowPrivateAccess=true))
	float Strength;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Duration",meta=(AllowPrivateAccess=true))
	float Duration;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Montage",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> DashMontage4Direction;

	FName GetRecentDirectionName();
	FVector GetRecentDirection();
	float GetMaxSpeed();
};
