#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "Fireball_Projectile.generated.h"

class UGameplayEffect;
class UAudioComponent;
class UAbilitySystemComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
UCLASS()
class TENTENTOWN_API AFireball_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireball_Projectile();

	UFUNCTION(BlueprintCallable)
	void FireProjectile(const FVector& Direction, AActor* IgnoreActor);

	void SetChargeSecFromAbility(float secs){ChargeSecFromAbility = secs;}
	void SetNiagaraScale(float Secs);
	void SetSetbyCallerGameplayEffectClass(TSubclassOf<UGameplayEffect> GameplayEffect);
protected:
	virtual void BeginPlay() override;
	
	//컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<USphereComponent> CollisionComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<USoundBase> ProjectileSound;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<UAudioComponent> AudioComponent;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Components")
	TSubclassOf<UGameplayEffect> SetByCallerGameplayEffectClass;
	
	//속도 및 중력 스케일
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float InitialSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float MaxSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	bool AlignActorWithVelocity;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float GravityScale;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float CollisionRadius;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float LifeSpan;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStop(const FHitResult& HitResult);

	UFUNCTION()
	void DestroyBinding(AActor* DestroyedActor);
	
	UFUNCTION()
	void Explode();
	
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	UPROPERTY()
	FTimerHandle OnTimeOut;

	UPROPERTY()
	float ChargeSecFromAbility;
};
