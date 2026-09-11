// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "AttackTask.generated.h"

class UGameplayAbility;
class AEnemyBase;

UCLASS()
class TENTENTOWN_API UAttackTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	AEnemyBase* Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="input")
	AActor* TargetActor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float AttackSpeed = 1.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float ElapsedTime = 0.0f;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;

	void ExecuteAbility();
};