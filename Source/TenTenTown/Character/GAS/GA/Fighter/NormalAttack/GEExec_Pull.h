// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExec_Pull.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGEExec_Pull : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
