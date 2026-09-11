// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Enemy_ExplodeSelf_Effect_Cue.generated.h"

class USoundCue;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEnemy_ExplodeSelf_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> SoundCue;
	
protected:
	
	virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
