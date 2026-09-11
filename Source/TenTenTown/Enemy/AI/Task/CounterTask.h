// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "CounterTask.generated.h"

/**
 * 
 */
class AEnemyBase;

UCLASS()
class TENTENTOWN_API UCounterTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;

	void ExecuteCounterAbility();

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	ABlackKnight* Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="input")
	AActor* TargetActor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float AttackSpeed = 1.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float ElapsedTime = 0.0f;

};