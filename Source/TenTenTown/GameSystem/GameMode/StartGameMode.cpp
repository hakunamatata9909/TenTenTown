// StartGameMode.cpp

#include "GameSystem/GameMode/StartGameMode.h"
#include "GameSystem/Player/StartPlayerController.h"

AStartGameMode::AStartGameMode()
{
	// 이 GameMode를 쓰는 맵에서는 기본 PlayerController 를 StartPlayerController 로 사용
	PlayerControllerClass = AStartPlayerController::StaticClass();
}

void AStartGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[StartGameMode] BeginPlay - using AStartPlayerController for StartMap"));
}
