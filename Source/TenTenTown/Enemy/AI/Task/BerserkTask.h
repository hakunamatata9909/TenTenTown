// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Enemy/EnemyList/DemonKing.h"
#include "BerserkTask.generated.h"

UCLASS()
class TENTENTOWN_API UBerserkTask : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition) override;

    virtual void ExitState(FStateTreeExecutionContext& Context,
                           const FStateTreeTransitionResult& Transition) override;

    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
    AEnemyBase* Actor;

};
