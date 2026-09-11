// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "FindClosestEvaluator.generated.h"

struct FOnAttributeChangeData;
class AEnemyBase;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UFindClosestEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()
	
private:
	FDelegateHandle CurrentTargetDeathDelegateHandle; 

	float ScanTimer = 0.0f;
	
	const float ScanInterval = 0.3f;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="context")
	AEnemyBase* Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="output")
	AActor* TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="none")
	UAbilitySystemComponent* ASC;

	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	bool CheckVisibility(AActor* Target) const;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	
};
