// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_ArcherSkillAKnockback.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_ArcherSkillAKnockback : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimMontage",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> AttackAnimMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GameplayEffcect",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DamageGEClass;
	
	UFUNCTION()
	void OnEndMontage();
	
};
