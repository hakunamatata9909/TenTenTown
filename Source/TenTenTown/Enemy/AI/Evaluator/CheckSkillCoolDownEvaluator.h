// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "CheckSkillCoolDownEvaluator.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UCheckSkillCoolDownEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="context")
	AEnemyBase* Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="output")
	bool bIsCoolDown;
	
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
