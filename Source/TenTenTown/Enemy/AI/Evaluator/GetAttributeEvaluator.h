// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "GetAttributeEvaluator.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGetAttributeEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	float HealthPercent;

	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	// float AttackSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "output")
	FGameplayTagContainer TagContainer;

public:
	void virtual Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	void virtual TreeStart(FStateTreeExecutionContext& Context) override;
};
