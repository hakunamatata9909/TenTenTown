// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "TestEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ATestEnemy : public AEnemyBase
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
