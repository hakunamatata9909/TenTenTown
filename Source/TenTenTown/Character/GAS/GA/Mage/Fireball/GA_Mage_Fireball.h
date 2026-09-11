#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "GA_Mage_Fireball.generated.h"

class AMageCharacter;
class UNiagaraSystem;
class AFireballProjectile;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_Mage_Fireball : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_Fireball();

	//파이어 볼
	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	TSubclassOf<AFireballProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Anim")
	TObjectPtr<UAnimMontage> FireballMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag ShootTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Mage.Fireball.Shoot"));

	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	FName MuzzleSocketName = TEXT("Muzzle");
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	bool bUseCameraAim = false;
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float SpawnForwardOffset = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MuzzleSpeed = 2400.f;


	//메테오 레인
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	TObjectPtr<UNiagaraSystem> MeteorCircleVFX;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	int32 MeteorCount = 20;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float MeteorRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float MeteorHeight = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float MeteorRandomExtraHeight = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float MeteorSpeed = 2400.f;
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float TelegraphTime = 0.8;
	UPROPERTY()
	FVector MeteorCenter;

	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();


	UFUNCTION()
	void OnShootEvent(const FGameplayEventData Payload);
	void EmpoweredShot(AMageCharacter* Mage);
	
	UFUNCTION()
	void SpawnMeteorRainAtPositions();
	UFUNCTION()
	void FinishMeteorTelegraph();
	
	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(const FVector& SpawnLoc, const FRotator& SpawnRot);
	void SpawnFireball(const FVector& SpawnLoc, const FRotator& ViewRot);
	
	UFUNCTION(Server, Reliable)
	void ServerStartMeteorTelegraph(const FVector& Center);
	void StartMeteorTelegraph(const FVector& Center);
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;

private:
	bool bSkillEmpowered = false;
};
