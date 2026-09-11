#pragma once

#include "CoreMinimal.h"
#include "Structure/Base/StructureBase.h"
#include "BarricadeStructure.generated.h"

UCLASS()
class TENTENTOWN_API ABarricadeStructure : public AStructureBase
{
	GENERATED_BODY()
	
public:	
	ABarricadeStructure();

protected:
	virtual void BeginPlay() override;
	
	// 실제 스탯과 메시를 변경하는 내부 함수
	void ApplyStructureStats(int32 Level) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;
	
	virtual void UpgradeStructure() override;

	// 파괴 시 콜백
	virtual void HandleDestruction() override;
};
