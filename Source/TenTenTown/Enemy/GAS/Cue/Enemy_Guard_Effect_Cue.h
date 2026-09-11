// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "NiagaraSystem.h"

#include "Enemy_Guard_Effect_Cue.generated.h"




/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEnemy_Guard_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Effect")
	UNiagaraSystem* GuardEffect;

	UPROPERTY()
	TArray<UNiagaraComponent*> GuardStack;

	void ClearGuardStack();
	
protected:
	virtual void HandleGameplayCue(
		AActor* Target,
		EGameplayCueEvent::Type EventType,
		const FGameplayCueParameters& Parameters
	) override;

	
};