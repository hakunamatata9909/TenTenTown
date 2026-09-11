// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_FighterUltimate.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_FighterUltimate : public UBaseGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;
	UPROPERTY()
	float OriginWalkSpeed;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> UltimateAnimMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DurationGE;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveGEHandle;

	UFUNCTION()
	void OnGERemoved(const FGameplayEffectRemovalInfo& RemovalInfo);

	UFUNCTION()
	void OnMontageEnd();

	UFUNCTION()
	void OnMontageCancel();
};
