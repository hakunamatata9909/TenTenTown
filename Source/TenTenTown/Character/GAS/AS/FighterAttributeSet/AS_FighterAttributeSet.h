// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_FighterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UAS_FighterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UAS_FighterAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(ThisClass,Level);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass,Health);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(ThisClass,Stamina);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxHealth);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxStamina);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel);

};
