// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "NagaWizard.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ANagaWizard : public AEnemyBase
{
	GENERATED_BODY()

	
public:
	// Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> BuffMontage;
	
	virtual void InitializeEnemy() override;
};
