// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "NagaWizard_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UNagaWizard_AttributeSet : public UAS_EnemyAttributeSetBase
{
	GENERATED_BODY()

	UNagaWizard_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_BuffScale)
	FGameplayAttributeData BuffScale;
	ATTRIBUTE_ACCESSORS(ThisClass, BuffScale);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_BuffDuration)
	FGameplayAttributeData BuffDuration;
	ATTRIBUTE_ACCESSORS(ThisClass, BuffDuration);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_BuffCoolDown)
	FGameplayAttributeData BuffCoolDown;
	ATTRIBUTE_ACCESSORS(ThisClass, BuffCoolDown);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_BuffRadius)
	FGameplayAttributeData BuffRadius;
	ATTRIBUTE_ACCESSORS(ThisClass, BuffRadius);


	UFUNCTION()
	void OnRep_BuffScale(const FGameplayAttributeData& OldBuffScale);

	UFUNCTION()
	void OnRep_BuffDuration(const FGameplayAttributeData& OldBuffDuration);

	UFUNCTION()
	void OnRep_BuffCoolDown(const FGameplayAttributeData& OldBuffCoolDown);

	UFUNCTION()
	void OnRep_BuffRadius(const FGameplayAttributeData& OldBuffRadius);
	
};
