// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Archer_SkillB_MultipleArrows.generated.h"

class AArcherCharacter;
class AArcherBow;
class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Archer_SkillB_MultipleArrows : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimMontage",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditAnywhere, Category = "GAS|Arrow")
	TSubclassOf<class AArcher_Arrow> ArrowClass;

	UPROPERTY(EditAnywhere, Category = "GAS|Arrow")
	TSubclassOf<class UGameplayEffect> SetByCallerClass;

	UPROPERTY(EditAnywhere, Category = "GAS|Arrow")
	float DamageMultiplier = 6.0f;

	UPROPERTY(EditAnywhere, Category = "GAS|Arrow")
	float SpreadAngle = 15.f;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<AArcherCharacter> AvatarCharacter;
	UPROPERTY()
	TObjectPtr<AArcherBow> EquippedBow;
	UPROPERTY()
	TArray<TObjectPtr<class AArcher_Arrow>> SpawnedArrows;

	UPROPERTY()
	float FirstMovementSpeed;
	
	UFUNCTION()
	void OnMontageEnd();

	UFUNCTION()
	void FireArrows(FGameplayEventData Payload);
	
};

