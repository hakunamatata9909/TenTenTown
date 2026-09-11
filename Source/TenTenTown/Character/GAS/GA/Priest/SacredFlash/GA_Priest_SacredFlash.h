#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Priest_SacredFlash.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class TENTENTOWN_API UGA_Priest_SacredFlash : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Priest_SacredFlash();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	//Anim
	UPROPERTY(EditAnywhere, Category = "Anim")
	TObjectPtr<UAnimMontage> FlashMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag ShootTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Priest.SacredFlash"));
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;

	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> ShieldGE;
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> ShieldActiveGE;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag DamageTag;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageAmount = 40.f;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category="Shield")
	float ShieldAmount = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="Shield")
	float ShieldMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly)
	float HitRange = 1000.f;
	UPROPERTY(EditDefaultsOnly)
	FVector HitBoxHalfSize = FVector(500.f, 40.f, 40.f);
	UPROPERTY(EditDefaultsOnly)
	float Damage = 10.f;

	FGameplayTag ShieldTag = FGameplayTag::RequestGameplayTag(FName("Data.Buff.Shield"));
	FGameplayTag ShieldActiveTag = FGameplayTag::RequestGameplayTag(FName("State.Buff.Shield"));
	
	UFUNCTION()
	void OnShootEvent(const FGameplayEventData Payload);
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();

	void ApplyGEToASC(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayEffect> GEClass, float Level, FGameplayTag SetByCallerTag, float SetByCallerAmount, float SetByCallerMultiplier) const;
};
