#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowAfterHit_ObjectPooling.generated.h"

class AActorArrowRain;
class USkeletalMeshComponent;
class USceneComponent;

UCLASS()
class TENTENTOWN_API AArrowAfterHit_ObjectPooling : public AActor
{
	GENERATED_BODY()

public:
	AArrowAfterHit_ObjectPooling();

	void ActivateFromPool(const FVector& Location, const FRotator& Rotation, AActor* AttachTarget, FName BoneName, AActorArrowRain* InPoolOwner);
	void DeactivateToPool();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pooling")
	float DisplayDuration = 2.5f;

	FTimerHandle LifeTimerHandle;
	TWeakObjectPtr<AActorArrowRain> PoolOwner;
};