// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "Worm.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API AWorm : public AEnemyBase
{
	GENERATED_BODY()

	virtual void InitializeEnemy() override;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> BurrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UnBurrowMontage;
	
};
