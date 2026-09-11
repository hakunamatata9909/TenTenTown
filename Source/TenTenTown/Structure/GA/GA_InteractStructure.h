#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_InteractStructure.generated.h"

UCLASS()
class TENTENTOWN_API UGA_InteractStructure : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_InteractStructure();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 골드 처리
	bool CheckCostAndDeduct(int32 Cost);
	void AddGold(int32 Amount);
};
