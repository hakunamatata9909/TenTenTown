#include "UI/MVVM/LobbyViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "FieldNotification/IFieldValueChanged.h"
#include "TenTenTown/GameSystem/Player/TTTPlayerController.h"
#include "TenTenTown/GameSystem/GameInstance/TTTGameInstance.h"


void ULobbyViewModel::Initialize(ALobbyGameState* InGS, ATTTPlayerController* InPC, ULobbyPCComponent* InLobbyPCComponent)
{
    CachedGameState = InGS;
    CachedPlayerController = InPC;
    CachedLobbyPCComponent = InLobbyPCComponent;
    InitializeViewModel();
}

void ULobbyViewModel::InitializeViewModel()
{
    // 부모 클래스의 초기화 호출
    Super::InitializeViewModel();

    if (CachedGameState)
    {
        HandlePlayerCountChanged();
        HandleCountdownChanged(CachedGameState->CountdownSeconds);

        CachedGameState->OnCountdownChanged.AddDynamic(
            this, &ULobbyViewModel::HandleCountdownChanged
        );

        CachedGameState->OnPlayerCountChanged.AddDynamic(
            this, &ULobbyViewModel::HandlePlayerCountChanged
        );
        CachedGameState->OnSelectedMapChanged.AddDynamic(
            this, &ULobbyViewModel::HandleSelectedMapChanged
        );
    }
}

void ULobbyViewModel::CleanupViewModel()
{
    Super::CleanupViewModel();

    if (CachedGameState)
    {
        CachedGameState->OnCountdownChanged.RemoveAll(this);
    }
    
    CachedGameState = nullptr;
}



void ULobbyViewModel::SetReadyCountText(const FText& NewText)
{
    if (!ReadyCountText.EqualTo(NewText))
    {
        ReadyCountText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ReadyCountText);
    }
}


void ULobbyViewModel::SetTimerText(const FText& NewText)
{
    if (!TimerText.EqualTo(NewText))
    {
        TimerText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TimerText);
    }
}
void ULobbyViewModel::SetMapIconTexture(UTexture2D* NewTexture)
{
    if (MapIconTexture != NewTexture)
    {
        MapIconTexture = NewTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapIconTexture);
    }
}


void ULobbyViewModel::HandleCountdownChanged(int32 NewSeconds)
{
    if (NewSeconds > 0)
    {
        TimerVisibility = ESlateVisibility::Visible;
    }
    else
    {
		TimerVisibility = ESlateVisibility::Collapsed;
    }
    //SetTimerText(FText::Format(NSLOCTEXT("Lobby", "TimerFormat", "STARTING IN: {0}"), FText::AsNumber(NewSeconds)));
    SetTimerText(FText::AsNumber(NewSeconds));
}

void ULobbyViewModel::HandlePlayerCountChanged()
{
    if (CachedGameState)
    {
        int32 Ready = CachedGameState->ReadyPlayers;
        int32 Connected = CachedGameState->ConnectedPlayers;
                
        FText FormattedText = FText::Format(
            NSLOCTEXT("Lobby", "ReadyCountFormat", "{0} / {1}"),
            FText::AsNumber(Ready),
            FText::AsNumber(Connected)
        );

        SetReadyCountText(FormattedText);
    }
}

void ULobbyViewModel::HandleSelectedMapChanged(int32 NewMapIndex)
{
    if (!CachedPlayerController) { return; }
    UTTTGameInstance* TTTGI = CachedPlayerController->GetGameInstance<UTTTGameInstance>();

    if (!TTTGI) { return; }	
	UTexture2D* NewIcon = TTTGI->GetMapIconByIndex(NewMapIndex);
    
    SetMapIconTexture(NewIcon);
}


#pragma region CharSellectRegion
void ULobbyViewModel::SelectCharacter(int32 CharIndex)
{
    // ViewModel이 캐시된 Controller를 통해 Server RPC를 호출합니다.
    if (CachedPlayerController)
    {
        // ATTTPlayerController에 ServerSelectCharacter가 있다고 가정합니다.
        CachedPlayerController->ServerSelectCharacterNew(CharIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::SelectCharacter: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ConfirmSelection()
{
    if (!CachedPlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyViewModel: CachedPlayerController is NULL. Cannot process ConfirmSelection."));
        return;
    }

    ATTTPlayerState* TTTPlayerState = CachedPlayerController->GetPlayerState<ATTTPlayerState>();

    if (TTTPlayerState)
    {
        TTTPlayerState->ToggleReady();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyViewModel: Failed to cast PlayerState to ATTTPlayerState."));
    }
}
#pragma endregion

#pragma region MapSelectRegion
void ULobbyViewModel::SelectMap(int32 CharIndex)
{
    if (CachedPlayerController)
    {
        CachedPlayerController->SetMap(CharIndex);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::SelectMap: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ReSelectCharacter()
{
    if (CachedPlayerController)
    {
        CachedPlayerController->ServerOpenCharacterSelectUI();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::ReSelectCharacter: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ReSelectMap()
{
    if (CachedPlayerController)
    {
        CachedPlayerController->ServerOpenMapSelectUI();
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::ReSelectMap: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::SetCharButtonVisibility(const ESlateVisibility NewVisibility)
{
    CharButtonVisibility = NewVisibility;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharButtonVisibility);
}
void ULobbyViewModel::SetMapButtonVisibility(const ESlateVisibility NewVisibility)
{
    if (!bIsHost && NewVisibility == ESlateVisibility::Visible)
    {
		MapButtonVisibility = ESlateVisibility::Hidden;        
    }
    else
    {
        MapButtonVisibility = NewVisibility;
    }
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapButtonVisibility);
    
}

#pragma endregion

void ULobbyViewModel::SetIsHost(bool bNewIsHost)
{
    if (bIsHost != bNewIsHost)
    {
        bIsHost = bNewIsHost;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsHost);
	}
}

#pragma region Result_Region


void ULobbyViewModel::SetResultVMs()
{
    ResultVMs.Empty();
    if (!CachedPlayerController) { return; }
        
	ALobbyGameState* LobbyGS = CachedPlayerController->GetWorld()->GetGameState<ALobbyGameState>();

	if (!LobbyGS) 
    {
        UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel: Failed to get LobbyGameState."));
        return;
    }
    	
	int32 PlayerResultsCount = LobbyGS->PlayerResults.Num();
	
    for (int32 i = 0; i < PlayerResultsCount; ++i)
    {
        UResultSlotViewModel* NewSlotVM = NewObject<UResultSlotViewModel>(this);

        if (NewSlotVM)
        {
            NewSlotVM->InitializeSlot(CachedPlayerController, LobbyGS->PlayerResults[i]);            

            ResultVMs.Add(NewSlotVM);
        }
    }

    SetMainResult(LobbyGS);
}

TArray<UResultSlotViewModel*> ULobbyViewModel::GetResultVMs() const
{
    TArray<UResultSlotViewModel*> RawPtrArray;
    for (const TObjectPtr<UResultSlotViewModel>& Member : ResultVMs)
    {
        RawPtrArray.Add(Member.Get());
    }
    return RawPtrArray;
}
void ULobbyViewModel::SetMainResult(ALobbyGameState* LobbyGS)
{
    if (LobbyGS->PlayerResults[0].bIsWin)
    {
        bIsWin = ESlateVisibility::Visible;
		bIsLose = ESlateVisibility::Collapsed;
    }
    else
    {
        bIsWin = ESlateVisibility::Collapsed;
		bIsLose = ESlateVisibility::Visible;
    }

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsWin);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsLose);
}

void ULobbyViewModel::LobbyResetBt()
{
    CachedLobbyPCComponent->Server_ResultWindowCloseEffect();
}

#pragma endregion
