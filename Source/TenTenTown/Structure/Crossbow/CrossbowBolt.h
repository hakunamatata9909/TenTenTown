#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "CrossbowBolt.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class TENTENTOWN_API ACrossbowBolt : public AActor
{
	GENERATED_BODY()
	
public:	
	ACrossbowBolt();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	USphereComponent* Sphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* BoltMesh;

	// 데미지 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	FTimerHandle LifeTimerHandle;
	
public:
	// 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	float DamageAmount = 100.0f;
	
	// 풀링 시스템 호출 함수
	void ActivateProjectile(FVector StartLocation, AActor* TargetActor, float Range);
	
	// 풀링 시스템 비활성화 함수
	void DeactivateProjectile();

	// 화살 오버랩 시
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
