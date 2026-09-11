#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Priest_Atonement.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class AAtonementActor;

UCLASS()
class TENTENTOWN_API UGA_Priest_Atonement : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Priest_Atonement();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAtonementActor> AoEActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Anim")
	TObjectPtr<UAnimMontage> AtonementMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag SetTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Priest.Atonement.Set"));
	
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnSetEvent(const FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void Server_SpawnArea(const FVector& SpawnLoc, const FRotator& SpawnRot);
	void SpawnArea(const FVector& SpawnLoc, const FRotator& SpawnRot);

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;
	
	UPROPERTY(EditDefaultsOnly)
	float AoERadius = 400.f;
	UPROPERTY(EditDefaultsOnly)
	float AoEHalfHeight = 100.f;
	UPROPERTY(EditDefaultsOnly)
	float AoELifeTime = 5.f;
};