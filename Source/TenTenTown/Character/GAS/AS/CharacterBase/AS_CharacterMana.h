#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AS_CharacterMana.generated.h"

UCLASS()
class TENTENTOWN_API UAS_CharacterMana : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAS_CharacterMana();
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ThisClass,Mana);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxMana);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(ThisClass,ManaRegenRate);
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);
};
