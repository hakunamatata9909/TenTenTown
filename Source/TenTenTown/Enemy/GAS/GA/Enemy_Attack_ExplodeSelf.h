// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Attack_ExplodeSelf.generated.h"

struct FBranchingPointNotifyPayload;
class AEnemyBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Attack_ExplodeSelf : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "context")
	AEnemyBase* Actor;

	UPROPERTY(VisibleAnywhere, Category = "context")
	AActor* TargetActor;
	
	UEnemy_Attack_ExplodeSelf();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	virtual void PlayAttackMontage();

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explode")
	TSubclassOf<UGameplayEffect> ExplodeEffect;

	
	
	UFUNCTION()
	void ExecuteExplosion();
};
