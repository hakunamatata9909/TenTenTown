#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_MageAttributeSet.generated.h"

UCLASS()
class TENTENTOWN_API UAS_MageAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UAS_MageAttributeSet();
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_OverheatingStack)
	FGameplayAttributeData OverheatingStack;
	ATTRIBUTE_ACCESSORS(ThisClass,OverheatingStack);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OverheatingStack")
	float NeedOverheatingStack = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OverheatingStack")
	float MaxOverheatingStack = 50.f;
	
	UFUNCTION()
	void OnRep_OverheatingStack(const FGameplayAttributeData& OldOverheatingStack);
};
