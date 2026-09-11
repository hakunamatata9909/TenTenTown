// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Enemy_Attack_Effect_Cue.generated.h"

class USoundCue;
class UNiagaraSystem;

UCLASS()
class TENTENTOWN_API AEnemy_Attack_Effect_Cue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float Offset = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TMap<FGameplayTag, UNiagaraSystem*> EffectMap;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TMap<FGameplayTag, USoundCue*> SoundMap;
	
	virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
