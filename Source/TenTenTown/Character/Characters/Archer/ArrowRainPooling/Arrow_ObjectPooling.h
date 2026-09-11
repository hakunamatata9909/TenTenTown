#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Arrow_ObjectPooling.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class AActorArrowRain;

UCLASS()
class TENTENTOWN_API AArrow_ObjectPooling : public AActor
{
	GENERATED_BODY()

public:
	AArrow_ObjectPooling();

protected:
	virtual void BeginPlay() override;

public:
	// 클라이언트/서버 각자 로컬에서 발사
	void ActivateFromPool(const FVector& SpawnLocation, const FVector& Direction, AActorArrowRain* InPoolOwner);
	void DeactivateToPool();

	void SetDamage(float InDamage) { Damage = InDamage; }
	void SetSetByCallerClass(TSubclassOf<UGameplayEffect> InClass) { SetByCallerClass = InClass; }

protected:
	

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float BasicSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float GravityScale = 1.0f;

private:
	FTimerHandle LifeTimerHandle;
	TWeakObjectPtr<AActorArrowRain> PoolOwner;

	float Damage = 0.0f;
	TSubclassOf<UGameplayEffect> SetByCallerClass;
};