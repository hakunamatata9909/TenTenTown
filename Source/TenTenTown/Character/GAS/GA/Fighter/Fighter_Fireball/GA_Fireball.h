// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Fireball.generated.h"

class AFireball_Projectile;
class UAbilityTask_PlayMontageAndWait;
class UAnimSequence;
class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Fireball : public UBaseGameplayAbility
{
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> FireballMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> FireballReleaseMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess=true))
	TSubclassOf<AFireball_Projectile> Projectile;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GEDamage",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> GEDamage;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
	UPROPERTY()
	float OriginSpeed;
	UPROPERTY()
	float ChargingSeconds;
	UPROPERTY()
	FTimerHandle ChargingSecondHandle;

	UFUNCTION()
	void OnAbilityEnd();
	UFUNCTION()
	void ActiveLoopGameplayCue(const FGameplayEventData Data);
	UFUNCTION()
	void LaunchFireball(const FGameplayEventData Data);
};
