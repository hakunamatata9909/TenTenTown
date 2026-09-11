// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "DemonKing.generated.h"

UCLASS()
class TENTENTOWN_API ADemonKing : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category="Berserk")
	float BerserkHealthThreshold = 0.5f;

	UPROPERTY(VisibleAnywhere, Category="Berserk")
	bool bBerserkPlayed = false;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>BerserkMontage;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>SPAttackMontage;
	
	UPROPERTY(EditAnywhere, Category="GE")
	TSubclassOf<UGameplayEffect> EnemyBerserk;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category="State")
	bool bIsBerserk = false;

	

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UFUNCTION()
	void OnBerserkTagChanged(FGameplayTag Tag, int32 NewCount);
	
private:
	virtual void InitializeEnemy() override;
};
