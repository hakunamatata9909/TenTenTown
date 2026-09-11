// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "NiagaraSystem.h"
#include "Enemy_Berserk_Effect_Cue.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEnemy_Berserk_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Effect")
	TObjectPtr<UNiagaraSystem> BerserkAuraEffect;

protected:
	virtual void HandleGameplayCue(
		AActor* Target, 
		EGameplayCueEvent::Type EventType,
		const FGameplayCueParameters& Parameters) override;
};
