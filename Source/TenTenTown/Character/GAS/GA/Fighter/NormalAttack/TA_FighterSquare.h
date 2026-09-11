// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_FighterSquare.generated.h"

class ACharacter;
class UAbilitySystemComponent;

UCLASS()
class TENTENTOWN_API ATA_FighterSquare : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

	ATA_FighterSquare();
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void ConfirmTargeting() override;
	virtual void CancelTargeting() override;

public:
	UPROPERTY()
	TObjectPtr<ACharacter> Character;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shape",meta=(AllowPrivateAccess=true))
	FVector Extent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Pos",meta=(AllowPrivateAccess=true))
	float ShapePos;
	
};
