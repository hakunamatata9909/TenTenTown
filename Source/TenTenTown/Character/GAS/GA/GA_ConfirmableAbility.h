#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_ConfirmableAbility.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_ConfirmableAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ConfirmableAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* ConfirmTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CancelTask = nullptr;
	
	
	virtual void OnSelectionStarted() {};
	virtual void OnConfirmed(const FGameplayEventData& Payload) {};
	virtual void OnCanceled(const FGameplayEventData& Payload) {};
	
	UFUNCTION()
	void HandleConfirmEvent(FGameplayEventData Payload);
	UFUNCTION()
	void HandleCancelEvent(FGameplayEventData Payload);

	void ClearSelectingState();
};