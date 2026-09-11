// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "Dragon.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ADragon : public AEnemyBase
{
	GENERATED_BODY()

	virtual void InitializeEnemy() override;
};
