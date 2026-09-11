// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Beholder_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UBeholder_AttributeSet : public UAS_EnemyAttributeSetBase
{
	GENERATED_BODY()

public:
	UBeholder_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_Explode)
	FGameplayAttributeData ExplodeRange;
	ATTRIBUTE_ACCESSORS(ThisClass, ExplodeRange);

	UFUNCTION()
	void OnRep_Explode(const FGameplayAttributeData& OldExplode);

	
};
