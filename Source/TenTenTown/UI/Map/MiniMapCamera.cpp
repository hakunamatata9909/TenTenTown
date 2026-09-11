#include "UI/Map/MiniMapCamera.h"
#include "TimerManager.h"
#include "ContentStreaming.h"

AMiniMapCamera::AMiniMapCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	MiniMapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	RootComponent = MiniMapCaptureComponent;

	MiniMapCaptureComponent->OrthoWidth = OrthoWidth;

	MiniMapCaptureComponent->bCaptureEveryFrame = false;
	MiniMapCaptureComponent->bAlwaysPersistRenderingState = true;
}


void AMiniMapCamera::BeginPlay()
{
	Super::BeginPlay();


	if (MiniMapRenderTarget)
	{
		SetActorLocation(MiniMapLocation);
		SetActorRotation(MiniMapRotation);
		MiniMapCaptureComponent->OrthoWidth = OrthoWidth;
		MiniMapCaptureComponent->TextureTarget = MiniMapRenderTarget;

		// 약 1초 뒤에 실행 (맵이 로드될 최소한의 시간)
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMiniMapCamera::CaptureMap, 1.0f, false);
	}

	/*
	if (MiniMapRenderTarget)
	{
		SetActorLocation(MiniMapLocation);
		SetActorRotation(MiniMapRotation);
	
		MiniMapCaptureComponent->OrthoWidth = OrthoWidth;

		MiniMapCaptureComponent->TextureTarget = MiniMapRenderTarget;

		MiniMapCaptureComponent->CaptureScene();
	}
	*/
}

void AMiniMapCamera::CaptureMap()
{
	// 중요: 캡처 전 텍스처 스트리밍을 강제로 완료시킴 (뭉개짐 방지)
	IStreamingManager::Get().StreamAllResources(2.0f); // 2초간 데이터 스트리밍 집중

	if (MiniMapCaptureComponent)
	{
		MiniMapCaptureComponent->CaptureScene();
		UE_LOG(LogTemp, Log, TEXT("MiniMap Captured!"));
	}
}

FVector2D AMiniMapCamera::GetPlayerMiniMapPosition(FVector PlayerLocation)
{
	const FVector MapCenter = GetActorLocation();

	const float MapSize = MiniMapCaptureComponent->OrthoWidth;
		
	if (MapSize <= 0.f) return FVector2D(0.5f, 0.5f);

	float MapLeftBound = MapCenter.Y - (MapSize * 0.5f);
	float MapTopBound = MapCenter.X + (MapSize * 0.5f);

	float Ratio_X = (PlayerLocation.Y - MapLeftBound) / MapSize;
	float Ratio_Y = (MapTopBound - PlayerLocation.X) / MapSize;

	FVector CenterOffset = MapCenter;
	


	return FVector2D(
		FMath::Clamp(Ratio_X, 0.f, 1.f),
		FMath::Clamp(Ratio_Y, 0.f, 1.f)
	);

	
}
