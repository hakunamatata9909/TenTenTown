// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_FIghter_Kick.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_FIghter_Kick : public UBaseGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> KickMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shape",meta=(AllowPrivateAccess=true))
	FVector BoxExtent = FVector(100,100,100);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> GEKnockBack;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE",meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> GEKnockBackTag;
	
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	UFUNCTION()
	void OnMontageEnd();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnAttackEventReceived(const FGameplayEventData Data);

	void KnockBackASCActors(AActor* Actor);
};
