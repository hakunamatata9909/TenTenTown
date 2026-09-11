#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorArrowRain.generated.h"

class UStaticMeshComponent;
class AArrow_ObjectPooling;
class AArrowAfterHit_ObjectPooling;
class UGameplayEffect;

UCLASS()
class TENTENTOWN_API AActorArrowRain : public AActor
{
    GENERATED_BODY()

public:
    AActorArrowRain();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void InitializeArrowRain(float InDamage, TSubclassOf<UGameplayEffect> InGEClass);
    void ReturnArrowToPool(AArrow_ObjectPooling* Arrow);
    void SpawnAfterHitFromPool(const FVector& Location, const FRotator& Rotation, AActor* TargetActor, FName BoneName);
    void ReturnAfterHitToPool(AArrowAfterHit_ObjectPooling* AfterHit);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void SpawnSingleArrow();
    void CheckRainFinished();

    UPROPERTY(ReplicatedUsing = OnRep_RainSeed)
    int32 RainSeed;

    UFUNCTION()
    void OnRep_RainSeed();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Classes")
    TSubclassOf<AArrow_ObjectPooling> ArrowClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Classes")
    TSubclassOf<AArrowAfterHit_ObjectPooling> AfterHitClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    int32 TotalArrowCount = 400;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    float SpawnInterval = 0.015f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    float RainRadius = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    float SpawnHeight = 1200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    float MaxTiltAngle = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArrowRain|Config")
    float RemnantDuration = 2.5f;

private:
    float ArrowDamage = 0.0f;
    TSubclassOf<UGameplayEffect> DamageGEClass;

    int32 ArrowsSpawnedSoFar = 0;
    int32 InFlightArrowCount = 0;

    FTimerHandle RainTimerHandle;

    UPROPERTY()
    TArray<TObjectPtr<AArrow_ObjectPooling>> ArrowPool;

    UPROPERTY()
    TArray<TObjectPtr<AArrowAfterHit_ObjectPooling>> AfterHitPool;

    FRandomStream RandStream;
};