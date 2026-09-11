// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_FighterUltimateAttack.generated.h"

class ACharacter;
class AFireball_Projectile;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_FighterUltimateAttack : public UGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess=true))
	TSubclassOf<AFireball_Projectile> Projectile;

	UPROPERTY(editDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> GEDamage;
	UPROPERTY()
	ACharacter* AvatarCharacter;
	UFUNCTION()
	void OnRelease(const FGameplayEventData Data);
	UFUNCTION()
	void OnReleaseEnd(const FGameplayEventData Data);
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

	
};
