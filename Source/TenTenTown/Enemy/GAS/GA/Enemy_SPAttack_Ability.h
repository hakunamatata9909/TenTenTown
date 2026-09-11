// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/GA/Enemy_Attack_Ability.h"
#include "Enemy_SPAttack_Ability.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_SPAttack_Ability : public UEnemy_Attack_Ability
{
	GENERATED_BODY()
	virtual void PlayAttackMontage() override;
	UPROPERTY()
	bool bNotifyBound = false;
};
