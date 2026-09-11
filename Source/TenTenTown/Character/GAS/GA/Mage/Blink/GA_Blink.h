#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Blink.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class ACharacter;

UCLASS()
class TENTENTOWN_API UGA_Blink : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Blink();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	TObjectPtr<UAnimMontage> BlinkMontage;
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	FGameplayTag BlinkTag;
	UPROPERTY(EditDefaultsOnly, Category="Blink")
	float MaxDist = 800.f;
	UPROPERTY(EditDefaultsOnly, Category="Blink")
	float TraceRadius = 10.f;

	UFUNCTION()
	void OnBlinkEventReceived(FGameplayEventData Payload);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();

private:
	//도착지점 계산
	bool bFindBlinkDest(const ACharacter* Character, FVector& OutDest) const;
	FVector CachedDir = FVector::ZeroVector;
	
	EMovementMode SavedMoveMode = MOVE_Walking;
};
