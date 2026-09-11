// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "Enemy_Counter_Ability.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Counter_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEnemy_Counter_Ability();

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

	void PlayCounterMontage();
	void ApplyDamageToTarget(AActor* TargetActor);
	UFUNCTION()
	void OnMontageEnded();
	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABlackKnight> Actor;

	float AttackSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
};