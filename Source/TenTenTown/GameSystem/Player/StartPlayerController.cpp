// StartPlayerController.cpp

#include "GameSystem/Player/StartPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AStartPlayerController::AStartPlayerController()
{
	// 시작 화면에서는 마우스 커서 항상 보이게
	bShowMouseCursor = true;
}

void AStartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 컨트롤러만 UI 가짐 (서버에서 돌아가는 컨트롤러든, 클라든)
	if (!IsLocalController())
	{
		return;
	}
	// StartMap BGM
	PlayBGM(StartBGM);
	
	if (!StartMenuClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartPlayerController] StartMenuClass is not set."));
		return;
	}

	if (!StartMenuInstance)
	{
		StartMenuInstance = CreateWidget<UUserWidget>(this, StartMenuClass);
		if (StartMenuInstance)
		{
			StartMenuInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(StartMenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			bShowMouseCursor = true;

			UE_LOG(LogTemp, Log,
				TEXT("[StartPlayerController] StartMenu widget created and added to viewport."));
		}
	}
}

void AStartPlayerController::ApplyConnectInfo(const FString& InIP, int32 InPort)
{
	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		GI->SaveConnectInfo(InIP, InPort);
	}
}

void AStartPlayerController::PlayWithSavedConnectInfo()
{
	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		// 여기서 바로 서버 접속 (ClientTravel → LobbyMap 이 실행됨)
		GI->JoinSavedLobby();
	}
}

void AStartPlayerController::PlayBGM(USoundBase* NewBGM)
{
	if (!IsLocalController() || !NewBGM) return;

	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent = nullptr;
	}

	// 맵 전환되면 자동으로 끊기게(지속 X)
	BGMComponent = UGameplayStatics::SpawnSound2D(this, NewBGM, 1.f, 1.f, 0.f, nullptr, false, false);
}