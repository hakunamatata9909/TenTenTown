// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Engine/TimerHandle.h"
#include "DeadTask.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UDeadTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AEnemyBase> Actor;
	
protected:
	UPROPERTY(Transient)
	bool bMontageEnded = false;

	bool bGoldDrop = false;
	
public:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
	
	UFUNCTION()
	void OnDeadMontageEnd(UAnimMontage* Montage);
};
