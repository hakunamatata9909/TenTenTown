#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridFloorActor.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class TENTENTOWN_API AGridFloorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridFloorActor();

protected:
	virtual void BeginPlay() override;
	
	// 이 액터의 루트 컴포넌트이자, 라인 트레이스가 감지할 콜리전 영역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TObjectPtr<UBoxComponent> GridBounds;

	// 에디터에서 값이 변경될 때 호출
	virtual void OnConstruction(const FTransform& Transform) override;

	// 박스 크기 및 위치를 업데이트하는 헬퍼 함수
	void UpdateBoxSize();

	// 그리드의 가로 / 세로 칸 수(에디터 변경 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 1))
	int32 GridX = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 1))
	int32 GridY = 3;

	// 한 칸(셀)의 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 10.0))
	float CellSize = 300.f;

public:
	// Grid를 1차원 배열 인덱스로 변환
	int32 CellIndexToLinearIndex(int32 X, int32 Y) const;
	// 해당 셀이 점유되었는지 확인 (프리뷰 액터 사용)
	UFUNCTION(BlueprintPure, Category = "Grid")
	bool IsCellOccupied(int32 X, int32 Y) const;
	// 구조물 설치 시 호출(점유)
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool TryInstallStructure(const FVector& WorldLocation);
	// 구조물 제거 시 호출(점유 해제)
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool TryRemoveStructure(const FVector& WorldLocation);
	// 각 그리드 셀의 점유 상태를 저장
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Grid State")
	TArray<bool> OccupancyGrid;
	
	// 인덱스가 유효한지 검사
	FORCEINLINE bool IsValidIndex(int32 Index) const { return OccupancyGrid.IsValidIndex(Index); }
	
	// 월드 좌표 변환
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool WorldToCellIndex(const FVector& WorldLocation, int32& OutX, int32& OutY) const;
	// 셀의 중앙 좌표 변환
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetCellCenterWorldLocation(int32 X, int32 Y) const;
	// 셀이 그리드 안에 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool IsValidCellIndex(int32 X, int32 Y) const;
};
