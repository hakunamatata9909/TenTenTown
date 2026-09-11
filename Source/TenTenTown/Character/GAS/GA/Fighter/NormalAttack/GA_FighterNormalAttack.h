// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TA_FighterSquare.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_FighterNormalAttack.generated.h"

class AGameplayAbilityTargetActor;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_FighterNormalAttack : public UBaseGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> FirstSecondComboMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> LastComboMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TSubclassOf<ATA_FighterSquare> TargetActor;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> GE;
	
	UPROPERTY()
	bool bIsComboSectionStart;

	UPROPERTY()
	bool bIsComboInputPressed;
	
	UPROPERTY()
	int32 CurrentComboCount;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY()
	TArray<FName> SectionNames;
	UFUNCTION()
	void ComboStart(const FGameplayEventData Data);
	UFUNCTION()
	void ComboEnd(const FGameplayEventData Data);
	UFUNCTION()
	void OnAttack(const FGameplayEventData Data);
	UFUNCTION()
	void OnFirstSecondMontageEnd();
	UFUNCTION()
	void OnLastMontageEnd();
	UFUNCTION()
	void OnInterrupted();
	UFUNCTION()
	void OnTargetDataCome( const FGameplayAbilityTargetDataHandle& Data);
};
