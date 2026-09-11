#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Mage_FlameWall.generated.h"

class AFlameWallArea;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_Mage_FlameWall : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_FlameWall();

	UPROPERTY(EditDefaultsOnly, Category = "Wall")
	TSubclassOf<AFlameWallArea> AreaClass;

	UPROPERTY(EditAnywhere, Category = "Anim")
	TObjectPtr<UAnimMontage> FlameWallMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag SetTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Mage.FlameWall.Set"));

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnSetEvent(const FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerSpawnArea(const FVector& SpawnLoc, const FRotator& SpawnRot);
	void SpawnFlameWall(const FVector& SpawnLoc, const FRotator& ViewRot);

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;

private:
	UPROPERTY(EditDefaultsOnly, Category="Wall")
	float WallLifetime = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Wall|Set")
	float MaxSetRange = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category="Wall|Set")
	float MinSetRange = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Wall|Set")
	float GroundTraceUp = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Wall|Set")
	float GroundTraceDown = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="Wall|Set")
	float GroundOffset = 5.f;
};
