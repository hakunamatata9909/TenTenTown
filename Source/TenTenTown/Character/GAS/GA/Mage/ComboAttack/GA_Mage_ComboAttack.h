#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Mage_ComboAttack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_Mage_ComboAttack : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_ComboAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="ComboAttack|AM")
	TObjectPtr<UAnimMontage> ComboAttackAM;
	UPROPERTY(EditDefaultsOnly, Category="ComboAttack|AM")
	TArray<FName> ComboSections = {TEXT("A1"), TEXT("A2")};

	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag OpenTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Open"));
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag CloseTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Close"));
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag HitTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Hit"));
	
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Trace")
	float TraceRadius = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Trace")
	float TraceDistance = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Damage")
	TSubclassOf<UGameplayEffect> DamageGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_Damage;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Damage")
	float DamageAmount = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Damage")
	float HitRange = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Damage")
	float HitRadius = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Damage")
	FName WandSocketName = TEXT("Muzzle");
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UFUNCTION()
	void OnOpen(FGameplayEventData Payload);
	UFUNCTION()
	void OnClose(FGameplayEventData Payload);
	UFUNCTION()
	void OnHit(FGameplayEventData Payload);
	
private:
	int32 ComboIdx = 0;
	bool bWindowOpen = false;
	bool bComboInput = false;
	FTimerHandle ComboResetTimer;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitOpen = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitClose = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitHit = nullptr;

	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

	void DoTraceAndApply();
};
