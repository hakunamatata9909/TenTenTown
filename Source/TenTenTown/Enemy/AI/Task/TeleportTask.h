// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "TeleportTask.generated.h"

struct FAbilityEndedData;
class UGameplayAbility;
class AEnemyBase;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UTeleportTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	const AEnemyBase* Actor;

private:
	
	FDelegateHandle AbilityEndedDelegateHandle;

	FGameplayAbilitySpecHandle ActiveAbilityHandle;

	void OnAbilityEndedDelegate(const FAbilityEndedData& AbilityEndedData);

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
	EStateTreeRunStatus EndState(UGameplayAbility* EndAbility);
};
