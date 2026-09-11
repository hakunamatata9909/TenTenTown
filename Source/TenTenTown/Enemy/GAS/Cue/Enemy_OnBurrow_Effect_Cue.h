// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Enemy_OnBurrow_Effect_Cue.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEnemy_OnBurrow_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:

	AEnemy_OnBurrow_Effect_Cue();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> BurrowEffect;

protected:
	
	virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, 
								   const FGameplayCueParameters& Parameters) override;

private:
	
	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> ActiveBurrowEffect;
};
