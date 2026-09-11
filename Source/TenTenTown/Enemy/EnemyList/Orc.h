// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "Orc.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API AOrc : public AEnemyBase
{
	GENERATED_BODY()

	AOrc();

	virtual void InitializeEnemy() override;
};
