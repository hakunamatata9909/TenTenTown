#include "Structure/GridSystem/GridFloorActor.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

AGridFloorActor::AGridFloorActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	// 루트 컴포넌트 설정
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DefaultSceneRoot->SetMobility(EComponentMobility::Movable);
	
	GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("GridBounds"));
	GridBounds->SetupAttachment(RootComponent);
	
	// 오브젝트 타입을 커스텀 오브젝트 타입(ECC_GameTraceChannel3)으로 설정
	GridBounds->SetCollisionObjectType(ECC_GameTraceChannel3); 
	// 콜리전은 쿼리(라인 트레이스)에만 사용, 물리 X
	GridBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 다른 채널과의 반응 무시
	GridBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	GridBounds->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	// GridFloor(ECC_GameTraceChannel3)에 대해서만 Block으로 반응
	GridBounds->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void AGridFloorActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 에디터에서 설정 값으로 초기화
	OccupancyGrid.Init(false, GridX * GridY);
	
	// 게임 시작 시에도 박스 크기를 한 번 더 업데이트(안전성)
	UpdateBoxSize();
	
	// 에디터에서 GridX, GridY 값이 변경되면 OccupancyGrid의 크기도 재조정
	if (GridX * GridY != OccupancyGrid.Num())
	{
		OccupancyGrid.Init(false, GridX * GridY);
	}
}

void AGridFloorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 에디터에서 GridSizeX, GridSizeY, CellSize 값을 변경하는 즉시 변경
	UpdateBoxSize();
}

void AGridFloorActor::UpdateBoxSize()
{
	if (GridBounds)
	{
		// 박스의 절반 계산
		const FVector BoxExtent = FVector(
			(GridX * CellSize) * 0.5f,
			(GridY * CellSize) * 0.5f,
			10.0f 
		);
		GridBounds->SetBoxExtent(BoxExtent);

		// 박스의 중심을 (박스 절반, 박스 절반) 크기로 계산해서 박스의 중앙을 위치로 계산
		GridBounds->SetRelativeLocation(FVector(BoxExtent.X, BoxExtent.Y, -10.0f));
	}
}

int32 AGridFloorActor::CellIndexToLinearIndex(int32 X, int32 Y) const
{
	return Y * GridX + X;
}

bool AGridFloorActor::IsCellOccupied(int32 X, int32 Y) const
{
	if (X < 0 || X >= GridX || Y < 0 || Y >= GridY)
	{
		return true; // 범위 밖은 설치 불가로 간주
	}

	int32 LinearIndex = CellIndexToLinearIndex(X, Y);
	if (OccupancyGrid.IsValidIndex(LinearIndex))
	{
		return OccupancyGrid[LinearIndex];
	}
	return true;
}

bool AGridFloorActor::TryInstallStructure(const FVector& WorldLocation)
{
	int32 CellX, CellY;
    
	// 월드 위치를 변환
	if (WorldToCellIndex(WorldLocation, CellX, CellY))
	{
		// 이미 점유되었는지 확인
		if (!IsCellOccupied(CellX, CellY))
		{
			int32 LinearIndex = CellIndexToLinearIndex(CellX, CellY);
			if (OccupancyGrid.IsValidIndex(LinearIndex))
			{
				// 점유 o
				OccupancyGrid[LinearIndex] = true;
				return true;
			}
		}
	}
	return false;
}

bool AGridFloorActor::TryRemoveStructure(const FVector& WorldLocation)
{
	int32 CellX, CellY;
    
	if (WorldToCellIndex(WorldLocation, CellX, CellY))
	{
		int32 LinearIndex = CellIndexToLinearIndex(CellX, CellY);
		if (OccupancyGrid.IsValidIndex(LinearIndex))
		{
			// 점유 x
			OccupancyGrid[LinearIndex] = false;
			return true;
		}
	}
	return false;
}

bool AGridFloorActor::WorldToCellIndex(const FVector& WorldLocation, int32& OutX, int32& OutY) const
{
	// 그리드 액터의 원점(ActorLocation)을 기준으로 계산
	const FVector RelativeLocation = WorldLocation - GetActorLocation();

	// 상대 위치를 CellSize로 나누어 계산
	OutX = FMath::FloorToInt(RelativeLocation.X / CellSize);
	OutY = FMath::FloorToInt(RelativeLocation.Y / CellSize);

	// 유효한 범위인지 확인
	return IsValidCellIndex(OutX, OutY);
}

FVector AGridFloorActor::GetCellCenterWorldLocation(int32 X, int32 Y) const
{
	// 셀의 중앙 계산(설치용)
	const float CenterOffsetX = (X * CellSize) + (CellSize * 0.5f);
	const float CenterOffsetY = (Y * CellSize) + (CellSize * 0.5f);

	// 위치 값 반환
	return GetActorLocation() + FVector(CenterOffsetX, CenterOffsetY, 0.f);
}

bool AGridFloorActor::IsValidCellIndex(int32 X, int32 Y) const
{
	// X가 0보다 크거나 같고, GridSizeX보다 작은지 확인
	// Y가 0보다 크거나 같고, GridSizeY보다 작은지 확인
	return (X >= 0 && X < GridX && Y >= 0 && Y < GridY);
}

void AGridFloorActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// OccupancyGrid 배열 등록
	DOREPLIFETIME(AGridFloorActor, OccupancyGrid);
}
