// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_ArcherNormalAttack.generated.h"

class AArcher_Arrow;
class AArcherBow;
class AArcherCharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_ArcherNormalAttack : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bow",meta=(AllowPrivateAccess=true))
	TObjectPtr<AArcherBow> Bow;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ASC",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character",meta=(AllowPrivateAccess=true))
	TObjectPtr<AArcherCharacter> ArcherCharacter;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AnimInstance",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimMontage",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ArrowClass",meta=(AllowPrivateAccess=true))
	TSubclassOf<AArcher_Arrow> ArrowClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SetByCallerClass",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> SetByCallerClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LifeSpan",meta=(AllowPrivateAccess=true))
	float LifeSpan = 10.f;
	
	UPROPERTY()
	TObjectPtr<AArcher_Arrow> Arrow;
	
	UPROPERTY()
	FVector LaunchDirection;
	
	UPROPERTY()
	float DamageMultiplier=6.f;
	
	UPROPERTY()
	float StartTime=0.f;
	
	UPROPERTY()
	float EndTime = 0.f;
	
	UPROPERTY()
	float HoldTime = 0.f;
	
	UFUNCTION()
	void OnMontageEnd();
	
	UPROPERTY()
	bool bInputReleasedProcessed = false;
};
