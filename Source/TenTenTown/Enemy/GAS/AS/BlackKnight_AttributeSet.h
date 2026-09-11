// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "BlackKnight_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UBlackKnight_AttributeSet : public UAS_EnemyAttributeSetBase
{
	GENERATED_BODY()
	UBlackKnight_AttributeSet();
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_FrontHitCount)
	FGameplayAttributeData FrontHitCount;
	ATTRIBUTE_ACCESSORS(ThisClass, FrontHitCount);
	
	UFUNCTION()
	void OnRep_FrontHitCount(const FGameplayAttributeData& OldFrontHitCount);

	float MaxFrontHitCount = 5.0f;
};
