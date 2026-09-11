// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Enemy_Burrow_Ability.generated.h"

UCLASS()
class TENTENTOWN_API UEnemy_Burrow_Ability : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UEnemy_Burrow_Ability();

    UPROPERTY(EditDefaultsOnly, Category="Burrow")
    TSubclassOf<class UGameplayEffect> BurrowEffect;

    UPROPERTY(EditDefaultsOnly, Category="Burrow")
    TSubclassOf<class UGameplayEffect> InvulnerableEffect;

    
protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle,
                               const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo,
                               bool bReplicateCancelAbility) override;

private:
    UFUNCTION()
    void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

    UFUNCTION()
    void OnBurrowMontageFinished();

    
    UFUNCTION()
    void CleanupState();

    FActiveGameplayEffectHandle ActiveBurrowGEHandle;
    FActiveGameplayEffectHandle ActiveInvulnerableGEHandle;
};
