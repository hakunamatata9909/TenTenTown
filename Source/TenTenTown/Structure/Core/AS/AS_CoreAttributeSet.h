#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_CoreAttributeSet.generated.h"

UCLASS()
class TENTENTOWN_API UAS_CoreAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAS_CoreAttributeSet();

	// 체력 클램핑
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// 멀티플레이어 복제
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAS_CoreAttributeSet, Health);

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAS_CoreAttributeSet, MaxHealth);

protected:
	// Health 값이 복제될 때 클라이언트에서 호출될 함수입니다. (UI 업데이트용)
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	// MaxHealth 값이 복제될 때 클라이언트에서 호출될 함수입니다.
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
