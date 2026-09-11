// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Attack_Range_Ability.generated.h"

class AEnemyBase;
class AEnemyProjectileBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Attack_Range_Ability : public UGameplayAbility
{
	GENERATED_BODY()

	UEnemy_Attack_Range_Ability();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void PlayAttackMontage();

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AEnemyBase> Actor;
	
	UPROPERTY(BlueprintReadWrite)
	float AttackSpeed = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	
};
