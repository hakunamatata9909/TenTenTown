// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Archer_Ultimate.generated.h"

class AArcherCharacter;
class AArcherBow;
class AArcher_Arrow;
class ACharacter;
class AArcherFloatingPawn;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Archer_Ultimate : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SpectatorPawn",meta=(AllowPrivateAccess=true))
	TSubclassOf<AArcherFloatingPawn> PawnClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ArrowClass",meta=(AllowPrivateAccess=true))
	TSubclassOf<AArcher_Arrow> ArrowClass;
	
	UPROPERTY(EditAnywhere, Category = "Ultimate",meta=(AllowPrivateAccess=true))
	TSubclassOf<class AActorArrowRain> ArrowRainClass;
	
	UPROPERTY(EditAnywhere, Category = "Ultimate",meta=(AllowPrivateAccess=true))
	TSubclassOf<class UGameplayEffect> DamageGEClass;
	
	UPROPERTY()
	float UltimateDamage;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<AArcherBow> Bow;
	UPROPERTY()
	TObjectPtr<AArcherCharacter> AvatarCharacter;
	UPROPERTY()
	TObjectPtr<APlayerController> PC;
	UPROPERTY()
	TObjectPtr<AArcher_Arrow> Arrow;
	
	void PossessSpectatorPawn();
	void SpawnArrowOnBow();
	
	UFUNCTION()
	void OnTargetLocationConfirmed(FGameplayEventData Payload);
	
};
