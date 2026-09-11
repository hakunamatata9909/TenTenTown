// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Enemy_Buffed_Effect_Cue.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEnemy_Buffed_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AEnemy_Buffed_Effect_Cue();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> BuffedEffect;

protected:
	
	virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, 
								   const FGameplayCueParameters& Parameters) override;
	
};
