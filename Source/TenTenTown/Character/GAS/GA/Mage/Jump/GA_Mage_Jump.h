#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Mage_Jump.generated.h"

class ACharacter;

UCLASS()
class TENTENTOWN_API UGA_Mage_Jump : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_Jump();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	void OnLanded(EMovementMode MovementMode);
	
	UPROPERTY()
	TObjectPtr<ACharacter> Char;
};

