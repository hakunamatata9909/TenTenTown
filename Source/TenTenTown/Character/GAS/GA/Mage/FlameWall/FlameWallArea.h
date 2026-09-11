#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameWallArea.generated.h"

class UNiagaraComponent;
class UBoxComponent;
class UGameplayEffect;

UCLASS()
class TENTENTOWN_API AFlameWallArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlameWallArea();
	

	void Init(float InLifeTime);

	UFUNCTION()
	void OnDamageZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDamageZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void EndWallVFX();
	void DestroySelf();
	
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> FlameWallVFX;
	UPROPERTY(EditAnywhere, Category="VFX")
	FName SpawnScaleParamName = TEXT("User.WallSpawnScale");
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DotGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_DoT;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTagContainer DotGrantedTags;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamagePerTick = 25.f;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageMultiplier = 0.2f;
	UPROPERTY(EditAnywhere)
	FVector DamageZoneHalfExtent = FVector(80.f, 500.f, 250.f);
	UPROPERTY(EditAnywhere, Category="FlameWall|Collision")
	FVector DamageZoneOffset = FVector(0.f, 0.f, 250.f);
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> OverheatGivenActors;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleDefaultsOnly, Category="Flame|Collision")
	UBoxComponent* DamageZone = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Life")
	float Lifetime = 5.f;
	UPROPERTY(EditAnywhere, Category="Life")
	float FadeOutTime = 1.5f;

	FTimerHandle LifetimeHandle;
	FTimerHandle DestroyHandle;
};
