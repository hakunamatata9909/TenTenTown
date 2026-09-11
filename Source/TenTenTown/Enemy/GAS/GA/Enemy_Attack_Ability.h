// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy_Attack_Ability.generated.h"

struct FBranchingPointNotifyPayload;

UCLASS()
class UEnemy_Attack_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEnemy_Attack_Ability();
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void ApplyDamageToTarget(AActor* TargetActor);

	UFUNCTION()
	virtual void PlayAttackMontage();

	UFUNCTION()
	virtual void OnMontageEnded();

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);


protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AEnemyBase> Actor;
	
	UPROPERTY(BlueprintReadWrite)
	float AttackSpeed = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
};
