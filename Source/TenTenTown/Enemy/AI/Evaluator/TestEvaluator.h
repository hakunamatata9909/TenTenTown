// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "Enemy/Route/SplineActor.h"
#include "TestEvaluator.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UTestEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AActor> Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	TObjectPtr<ASplineActor> SplineActor;
	
public:
	void virtual TreeStart(FStateTreeExecutionContext& Context) override;
};
