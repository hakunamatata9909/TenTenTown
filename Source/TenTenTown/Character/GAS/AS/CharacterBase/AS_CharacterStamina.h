// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AS_CharacterStamina.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UAS_CharacterStamina : public UAttributeSet
{
	GENERATED_BODY()
	
	UAS_CharacterStamina();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Stamina", ReplicatedUsing = OnRep_Stamina,meta=(allowPrivateAccess="true"))
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(ThisClass,Stamina);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Stamina", ReplicatedUsing = OnRep_MaxStamina,meta=(allowPrivateAccess="true"))
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxStamina);
	
protected:
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
};
