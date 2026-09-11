#include "Structure/Preview/BasePreviewActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/GridSystem/GridFloorActor.h"

ABasePreviewActor::ABasePreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
	bNetLoadOnClient = false;
	bOnlyRelevantToOwner = true;
	SetActorEnableCollision(false);
}

void ABasePreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !PC->IsLocalController())
	{
		return;
	}
	
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	const float ScreenCenterX = ViewportSizeX * 0.5f;
	const float ScreenCenterY = ViewportSizeY * 0.5f;

	FVector WorldLocation;
	FVector WorldDirection;
	if (!PC->DeprojectScreenPositionToWorld(ScreenCenterX, ScreenCenterY, WorldLocation, WorldDirection))
	{
		return;
	}

	FVector TraceStart = WorldLocation;
	FVector TraceEnd = TraceStart + (WorldDirection * MaxInstallDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (GetInstigator())
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}
	
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 0.1f, 0, 1.f);
	
	FString DebugMsg = TEXT("DEFAULT: No Error");
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TraceChannel, // ECC_GameTraceChannel3
		QueryParams
	);

	bool bCanInstall = false;
	FVector FinalLocation = TraceEnd;

	if (bHit)
	{
		AGridFloorActor* HitGridFloor = Cast<AGridFloorActor>(HitResult.GetActor());
		
		if (HitGridFloor) // 성공 시
		{
			int32 CellX, CellY;
			bool bIsValidCell = HitGridFloor->WorldToCellIndex(HitResult.ImpactPoint, CellX, CellY);

			if (bIsValidCell)
			{
				bool bIsOccupied = HitGridFloor->IsCellOccupied(CellX, CellY);
                
				// 설치 가능 조건: 유효한 셀이고 && 점유되지 않아야 함
				bCanInstall = !bIsOccupied;

				FinalLocation = HitGridFloor->GetCellCenterWorldLocation(CellX, CellY);
			}
			else
			{
				// 점유된 칸이면 -> 마우스 닿은 위치(표면)에 표시
				FinalLocation = HitResult.Location;
			}
		}
		else
		{
			// 그리드 액터는 맞았으나 셀 범위 밖 -> 표면에 표시
			FinalLocation = HitResult.Location;
		}
	}
	else
	{
		FHitResult GroundHit;
		bool bHitGround = GetWorld()->LineTraceSingleByChannel(
			GroundHit,
			TraceStart,
			TraceEnd,
			ECC_GameTraceChannel1, // 빌드 전용 채널
			QueryParams
		);

		if (bHitGround)
		{
			// 땅이나 벽에 닿았으면 그 표면 위치 사용
			FinalLocation = GroundHit.Location;
		}
		else
		{
			// 허공이라면 그냥 최대 거리 끝(TraceEnd) 사용
			FinalLocation = TraceEnd;
		}
	}

	SetActorLocation(FinalLocation);
	OnInstallStatusChanged(bCanInstall);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, DebugMsg);
	}
}