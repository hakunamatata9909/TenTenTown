#pragma once

#include "CoreMinimal.h"
#include "Structure/Base/StructureBase.h"
#include "GameplayEffectTypes.h"
#include "IceTrapStructure.generated.h"

UCLASS()
class TENTENTOWN_API AIceTrapStructure : public AStructureBase
{
	GENERATED_BODY()

public:
	AIceTrapStructure();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 데이터 테이블 기반 스탯 적용
	virtual void ApplyStructureStats(int32 Level) override;

public:
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SlowArea;

	// --- GAS 슬로우 설정 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> SlowGameplayEffectClass;

	// 스탯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float SlowMagnitude = 0.5f;

	// 공격
	int32 EnemyCountInRange = 0;
	float FireTimer = 0.0f;
	void PulseAttack();

	// --- 내부 로직 변수 ---
	UPROPERTY()
	TMap<AActor*, FActiveGameplayEffectHandle> ActiveSlowEffects;

	// 적이 들어왔을 때/나갔을 때 호출될 함수
	UFUNCTION()
	void OnEnemyEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEnemyExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void UpgradeStructure() override;
	virtual void HandleDestruction() override;
};
