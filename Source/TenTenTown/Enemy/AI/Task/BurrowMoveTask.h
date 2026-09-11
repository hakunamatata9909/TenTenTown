// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Enemy/AI/Task/MoveTask.h"
#include "BurrowMoveTask.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UBurrowMoveTask : public UMoveTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	FGameplayAbilitySpecHandle ActiveAbilityHandle;
};
