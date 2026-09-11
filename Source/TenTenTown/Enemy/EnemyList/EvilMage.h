// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "EvilMage.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API AEvilMage : public AEnemyBase
{
	GENERATED_BODY()

	AEvilMage();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsTeleported;
	
};
