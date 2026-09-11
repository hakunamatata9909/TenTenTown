#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Passive_ManaRegen.generated.h"

UCLASS()
class TENTENTOWN_API UGA_Passive_ManaRegen : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Passive_ManaRegen();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ManaRegen")
	TSubclassOf<UGameplayEffect> ManaRegenEffectClass;
};
