// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Enemy/Route/SplineActor.h"
#include "MoveTask.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;
class AEnemyBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UMoveTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="input")
	TObjectPtr<ASplineActor> SplineActor;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="output")
	float Distance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="none")
	float MovementSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="none")
	float SavedDistance = 0.f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float SpreadDistance = 100.f;
	
protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	
	FDelegateHandle MovementSpeedChangedHandle;
	FDelegateHandle MovementSpeedRateChangedHandle;
};
