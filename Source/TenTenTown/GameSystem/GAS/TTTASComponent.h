// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TTTASComponent.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UTTTASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	float GetCooldownRemainingByTag(FGameplayTag CooldownTag) const;
	float GetCooldownDurationByTag(FGameplayTag CooldownTag) const;
};
