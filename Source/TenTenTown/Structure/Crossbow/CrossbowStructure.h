#pragma once

#include "CoreMinimal.h"
#include "Structure/Base/StructureBase.h"
#include "Structure/Crossbow/CrossbowBolt.h"
#include "CrossbowStructure.generated.h"

UCLASS()
class TENTENTOWN_API ACrossbowStructure : public AStructureBase
{
	GENERATED_BODY()
	
public:	
	ACrossbowStructure();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 실제 스탯과 메시를 변경하는 내부 함수
	void ApplyStructureStats(int32 Level);
	
public:	
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectSphere;

	// --- 스탯 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage = 10.0f;

	// --- 업그레이드 ---
	virtual void UpgradeStructure() override;

	// --- 풀링 시스템 ---
	UPROPERTY(EditDefaultsOnly, Category = "Pooling")
	TSubclassOf<ACrossbowBolt> BoltClass; // 화살 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Pooling")
	int32 PoolSize = 10; // 미리 만들어둘 개수

	// --- 내부 로직 변수 ---
	UPROPERTY()
	TArray<ACrossbowBolt*> BoltPool;// 실제 화살들이 담길 배열
	// 전투 로직
	UPROPERTY(Replicated)
	AActor* CurrentTarget;
	float FireTimer;
	
	// 풀링 초기화
	void InitializePool();
	// 풀에서 화살 꺼내오기
	ACrossbowBolt* GetBoltFromPool();

	// 적 감지
	float TargetSearchTimer = 0.0f;
	UFUNCTION()
	void OnEnemyExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 공격
	void Fire();
	void FindBestTarget();
	
	// GAS 체력 변경 콜백
	virtual void HandleDestruction() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
