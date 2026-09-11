#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GA_Mage_FlameThrower.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UNiagaraComponent;
class UNiagaraSystem;
class AFlameThrowerActor;

UCLASS()
class TENTENTOWN_API UGA_Mage_FlameThrower : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_FlameThrower();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Flame Thrower|Flame Actor")
	TSubclassOf<AFlameThrowerActor> FlameClass;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> ChargeMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> ChannelMontage;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ShootTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Mage.FlameThrower.Shoot"));

	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float MaxChannelTime = 5.0f;
	
	UPROPERTY(EditDefaultsOnly,Category="Trace")
	float Range = 1200.f;
	UPROPERTY(EditDefaultsOnly,Category="Trace")
	float ConeHalfAngleDeg =10.f;
	UPROPERTY(EditDefaultsOnly,Category="Trace")
	float TraceInterval = 0.05f;
	UPROPERTY(EditDefaultsOnly,Category="Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
	UPROPERTY(EditDefaultsOnly, Category="Socket")
	FName MuzzleSocketName = TEXT("Muzzle");
	
	UPROPERTY()
	TObjectPtr<AFlameThrowerActor> SpawnedActor = nullptr;
	
	FTimerHandle ChannelTimer;

	bool bInputHeld = false;
	bool bShotStarted = false;

	UFUNCTION()
	void OnShootEvent(FGameplayEventData Payload);
	
	void SpawnFlame();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ChargeTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ChannelTask;
	
	UFUNCTION()
	void OnChannelMontageEnded();
	UFUNCTION()
	void OnChargeComplete();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	EMovementMode SavedMoveMode = MOVE_Walking;
};
