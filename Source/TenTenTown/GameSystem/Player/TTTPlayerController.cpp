// TTTPlayerController.cpp


#include "GameSystem/Player/TTTPlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/PS/TTTPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InputComponent.h"
#include "GameSystem/GameMode/LobbyGameMode.h"
#include "UI/PCC/LobbyPCComponent.h"
#include "UI/PCC/PlayPCComponent.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "UI/LobbyWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "Sound/SoundBase.h"


ATTTPlayerController::ATTTPlayerController()
{
}

void ATTTPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
	if (!GM)
	{
		//UE_LOG(LogTemp, Error, TEXT("[TTTPlayerController] No LobbyGameMode found in BeginPlay"));
		return;
	}
	GM->EffectSet(this);

	
}

void ATTTPlayerController::CloseCharacterSelectUI()
{
	if (CharacterSelectInstance && CharacterSelectInstance->IsInViewport())
	{
		CharacterSelectInstance->RemoveFromParent();
		CharacterSelectInstance = nullptr;
	}

	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void ATTTPlayerController::OpenCharacterSelectUI()
{
	if (!IsLocalController() || !CharacterSelectClass)
	{
		return;
	}

	if (!CharacterSelectInstance)
	{
		CharacterSelectInstance = CreateWidget<UUserWidget>(this, CharacterSelectClass);
	}

	if (CharacterSelectInstance && !CharacterSelectInstance->IsInViewport())
	{
		CharacterSelectInstance->AddToViewport();
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void ATTTPlayerController::ShowResultUI(const FTTTLastGameResult& Result)
{

}
void ATTTPlayerController::OnResultRestartClicked()
{
	if (!IsLocalController())
	{
		return;
	}

	// 1) 결과창 닫기
	if (ResultWidgetInstance && ResultWidgetInstance->IsInViewport())
	{
		ResultWidgetInstance->RemoveFromParent();
	}
	

	// 3) 로비 캐릭터 선택 UI 다시 열기
	OpenCharacterSelectUI();

	// 필요하면 Ready 상태 초기화
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->ServerSetReady(false);
	}
	PlayBGM(LobbyBGM.Get());

	Server_RequestRestartFromResult();
}

void ATTTPlayerController::OnResultExitClicked()
{
	if (!IsLocalController())
	{
		return;
	}

	// 그냥 게임 종료
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, true);
}

void ATTTPlayerController::Server_RequestRestartFromResult_Implementation()
{
	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
	{
		GM->HandleResultRestartRequest(this); // 아래 B)
	}
}

void ATTTPlayerController::Server_SelectMapIndex_Implementation(int32 MapIndex)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 1) 지금 로비가 맞는지 체크 (선택)
	const FString MapName = World->GetMapName();
	if (!MapName.Contains(TEXT("Lobby")))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server_SelectMapIndex] Not in Lobby map."));
		return;
	}

	// 2) 방장인지 체크
	ALobbyGameMode* GM = World->GetAuthGameMode<ALobbyGameMode>();
	if (!GM || !GM->IsHost(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server_SelectMapIndex] Only host can select map."));
		return;
	}

	// 3) GameInstance 가져오기
	UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server_SelectMapIndex] No GameInstance."));
		return;
	}

	// 4) 인덱스 유효성 검사 (미리 Resolve로 체크)
	FString DummyPath;
	if (!GI->ResolvePlayMapPath(MapIndex, DummyPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server_SelectMapIndex] Invalid MapIndex=%d"), MapIndex);
		return;
	}

	// 5) 저장
	GI->SaveSelectedMapIndex(MapIndex);
	UE_LOG(LogTemp, Warning, TEXT("[Server_SelectMapIndex] Host selected map index=%d"), MapIndex);

	// 6) LobbyGameState에도 세팅해서 클라 UI에 복제
	if (ALobbyGameState* LGS = World->GetGameState<ALobbyGameState>())
	{
		LGS->SetSelectedMapIndex(MapIndex); // Rep + OnRep 브로드캐스트
	}

	// 7) 태그 제거
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			// 2. 게임모드에 설정해둔 'CharacterSelectGEClass'가 있는지 확인합니다.
			if (GM->MapSelectGEClass)
			{
				// 3. 해당 GE 클래스로 적용된 효과를 ASC에서 싹 제거합니다.
				// (이러면 태그도 같이 사라지고 -> 클라 UI도 꺼집니다)
				ASC->RemoveActiveGameplayEffectBySourceEffect(GM->MapSelectGEClass, ASC);

				UE_LOG(LogTemp, Warning, TEXT("Server: Removed Host GE from %s"), *GetNameSafe(PS));
			}
		}
	}
	Client_OnCharacterSelected();
}
void ATTTPlayerController::SetMap(int32 MapIndex)
{
	// 로컬에서 호출 -> 서버 RPC로 전달
	Server_SelectMapIndex(MapIndex);
}

void ATTTPlayerController::Client_OnCharacterSelected_Implementation()
{
	CloseCharacterSelectUI(); // 내부에서 GameOnly + 마우스 끔
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	UE_LOG(LogTemp, Warning, TEXT("[Client] InputMode restored, UI closed."));
}

void ATTTPlayerController::ServerRequestLobbyUIState_Implementation()
{
	// 서버에서만 의미 있음
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		if (GI->HasLastGameResult())
		{
			// 서버 GameInstance에 저장된 결과를 클라에게 보내서 결과창 띄우게 함
			const FTTTLastGameResult& Result = GI->GetLastGameResult();
			ClientShowLobbyResult(Result);
		}
		else
		{
			// 이번 판 결과가 없다 = 처음 로비 진입 → 캐릭터 선택 열기
			ClientShowLobbyCharacterSelect();
		}
	}
}

void ATTTPlayerController::ClientShowLobbyResult_Implementation(const FTTTLastGameResult& Result)
{
	ShowResultUI(Result);          
}

void ATTTPlayerController::ClientShowLobbyCharacterSelect_Implementation()
{
	// 클라에서 캐릭터 선택 UI를 띄우는 함수
	OpenCharacterSelectUI();
}

void ATTTPlayerController::Client_PlayBGM_Implementation(USoundBase* NewBGM)
{
	if (!IsLocalController()) return;
	PlayBGM(NewBGM);
}

void ATTTPlayerController::PlayBGM(USoundBase* NewBGM)
{
	if (!IsLocalController() || !NewBGM) return;

	if (BGMComponent && BGMComponent->IsPlaying() && BGMComponent->Sound == NewBGM)
	{
		return; // 이미 같은 BGM 재생 중이면 무시
	}

	if (BGMComponent)
	{
		BGMComponent->Stop();
		BGMComponent = nullptr;
	}

	BGMComponent = UGameplayStatics::SpawnSound2D(this, NewBGM, 1.f, 1.f, 0.f, nullptr, false, false);
}

void ATTTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent) return;

	// R키 누르면 Ready 토글
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ATTTPlayerController::OnReadyKeyPressed);
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ATTTPlayerController::TestSelectMap0);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ATTTPlayerController::TestSelectMap1);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ATTTPlayerController::TestSelectMap2);
	InputComponent->BindKey(EKeys::C, IE_Pressed, this, &ATTTPlayerController::SelectCharacter);
	InputComponent->BindKey(EKeys::M, IE_Pressed, this, &ATTTPlayerController::SelectMap);

	
}

void ATTTPlayerController::OnReadyKeyPressed()
{
	// 로컬 컨트롤러만 입력 처리
	if (!IsLocalController()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 로비에서만 동작하게 제한
	const FString MapName = World->GetMapName();
	if (!MapName.Contains(TEXT("LobbyMap")))
	{
		return;
	}

	ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>();
	if (!PS) return;

	// 캐릭터 선택 안 했으면 Ready 못 누르게
	if (!PS->SelectedCharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ReadyKey] No SelectedCharacterClass yet."));
		return;
	}
	// Ready 토글 → 내부에서 ServerSetReady RPC 감 
	PS->ToggleReady();

}
void ATTTPlayerController::TestSelectMap0()
{
	if (!GetWorld()->GetMapName().Contains(TEXT("Lobby"))) return;
	SetMap(0);
}

void ATTTPlayerController::TestSelectMap1()
{
	if (!GetWorld()->GetMapName().Contains(TEXT("Lobby"))) return;
	SetMap(1);
}

void ATTTPlayerController::TestSelectMap2()
{
	if (!GetWorld()->GetMapName().Contains(TEXT("Lobby"))) return;
	SetMap(2);
}

void ATTTPlayerController::CleanupLobbyPreview(bool bClearSelectionInfo)
{
	if (!HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FString MapName = World->GetMapName();
	if (!MapName.Contains(TEXT("Lobby"))) return;

	// 1) GAS 먼저 정리
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->ResetAllGASData();
		
		if (bClearSelectionInfo)
		{
			PS->SelectedCharacterClass = nullptr;
			PS->ServerSetReady(false);
		}
	}

	// 2) 프리뷰 Pawn 제거
	if (APawn* ExistingPawn = GetPawn())
	{
		UnPossess();
		ExistingPawn->Destroy();
	}
}



#pragma region UI_Region
void ATTTPlayerController::ServerSelectCharacterNew_Implementation(int32 CharIndex)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TSubclassOf<APawn> CharClass = nullptr;

	// GameInstance 가져오기
	if (UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>())
	{
		const TArray<TSubclassOf<APawn>>& AvailableClasses = GI->AvailableCharacterClasses;

		if (AvailableClasses.IsValidIndex(CharIndex))
		{
			CharClass = AvailableClasses[CharIndex];
		}
	}

	if (!HasAuthority() || CharClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[ServerSelectCharacter] Invalid CharIndex (%d) or CharClass is null."), CharIndex);
		return;
	}


	//---
	// 1) PlayerName 가져오기
	FString PlayerName = TEXT("Unknown");
	if (APlayerState* PS_Base = PlayerState)
	{
		PlayerName = PS_Base->GetPlayerName();
	}

	// 2) GameInstance에 선택한 캐릭터 저장 (인게임용 진짜 데이터)
	if (UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>())
	{
		GI->SaveSelectedCharacter(PlayerName, CharClass);
	}

	// 3) PlayerState에도 기록 (UI/디버그용)
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->SelectedCharacterClass = CharClass;
		UE_LOG(LogTemp, Warning, TEXT("[ServerSelectCharacter] Server PS Set %s : %s"),
			*PlayerName, *GetNameSafe(PS->SelectedCharacterClass));

		//인덱스 저장
		PS->Server_SetCharacterIndex(CharIndex);

		if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
		{
			GM->ServerSpawnOrReplaceLobbyPreview(PS, CharClass);
		}
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
			{
				// 2. 게임모드에 설정해둔 'CharacterSelectGEClass'가 있는지 확인합니다.
				if (GM->CharSelectGEClass)
				{
					// 3. 해당 GE 클래스로 적용된 효과를 ASC에서 싹 제거합니다.
					// (이러면 태그도 같이 사라지고 -> 클라 UI도 꺼집니다)
					ASC->RemoveActiveGameplayEffectBySourceEffect(GM->CharSelectGEClass, ASC);

					UE_LOG(LogTemp, Warning, TEXT("Server: Removed CharSelect GE via GameMode Class info."));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ServerSelectCharacter] NO PLAYERSTATE on SERVER"));
	}
	
}


void ATTTPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
		
	//ATTTPlayerState* TTTPS = GetPlayerState<ATTTPlayerState>();
	
	// 현재 맵 이름을 확인하여 어느 모드인지 판단
	FString MapName = GetWorld() ? GetWorld()->GetMapName() : FString(TEXT(""));

	ULobbyPCComponent* LobbyComp = GetComponentByClass<ULobbyPCComponent>();
	UPlayPCComponent* PlayComp = GetComponentByClass<UPlayPCComponent>();

	// 로비 레벨에서 초기화
	if (MapName.Contains(TEXT("LobbyMap")) || MapName.Contains(TEXT("Lobby"))) // 'LobbyMap' 또는 'UEDPIE_0_LobbyMap' 대응
	{
		// 1. 로비 컴포넌트 활성화 및 재초기화
		if (LobbyComp)
		{
			LobbyComp->Activate();
			LobbyComp->ReBeginPlay(); // ReBeginPlay: Clean/Bind/Check 루프 시작
		}

		// 2. 플레이 컴포넌트 비활성화 및 정리
		if (PlayComp)
		{
			PlayComp->CloseHUDUI(); // 혹시 켜져 있을 HUD 정리
			PlayComp->Deactivate(); // 인게임 컴포넌트 비활성화
		}
	}
	// 인게임 레벨에서 초기화
	else if (MapName.Contains(TEXT("GameMap")) || MapName.Contains(TEXT("Play"))) // 'GameMap' 또는 'UEDPIE_0_GameMap' 대응
	{
		//// 1. 로비 컴포넌트 비활성화 및 정리
		//if (LobbyComp)
		//{
		//	LobbyComp->CloseLobbyUI(); // 로비 UI 정리
		//	LobbyComp->Deactivate(); // 로비 컴포넌트 비활성화
		//}

		//// 2. 플레이 컴포넌트 활성화 및 재초기화
		//if (PlayComp)
		//{
		//	PlayComp->Activate();
		//	// CheckRequiredGameData는 내부적으로 초기화 및 태그 구독을 시작합니다.
		//	PlayComp->ReBeginPlay();
		//}
	}
	else
	{
		// 기타 맵 (메인 메뉴 등)에서는 모두 정리
		if (LobbyComp) { LobbyComp->CloseLobbyUI(); LobbyComp->Deactivate(); }
		if (PlayComp) { PlayComp->CloseHUDUI(); PlayComp->Deactivate(); }
	}
}

void ATTTPlayerController::SelectMap()
{
	ULobbyPCComponent* LobbyComp = GetComponentByClass<ULobbyPCComponent>();

	if (LobbyComp)
	{
		if (LobbyComp->GetLobbyWidgetInstance())
		{
			if (LobbyComp->GetLobbyViewModel()->GetIsHost())
			{
				LobbyComp->GetLobbyWidgetInstance()->OnMapButtonClicked();
			}
		}
	}
}

void ATTTPlayerController::SelectCharacter()
{
	ULobbyPCComponent* LobbyComp = GetComponentByClass<ULobbyPCComponent>();

	if (LobbyComp)
	{
		if (LobbyComp->GetLobbyWidgetInstance())
		{
			LobbyComp->GetLobbyWidgetInstance()->OnCharButtonClicked();
		}
	}
}



void ATTTPlayerController::ServerOpenCharacterSelectUI_Implementation()
{
	if (ATTTPlayerState* TTTPS = GetPlayerState<ATTTPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent())
		{
			if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
			{
				// GM->CharSelectGEClass를 가져와서 사용
				if (TSubclassOf<UGameplayEffect> GEToApply = GM->CharSelectGEClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GEToApply, 1.0f, ContextHandle);

					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
					}
				}
			}
		}
	}
}

void ATTTPlayerController::ServerOpenMapSelectUI_Implementation()
{
	// 선택된 정보까지 삭제
	if (ATTTPlayerState* TTTPS = GetPlayerState<ATTTPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent())
		{
			if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
			{
				// GM->CharSelectGEClass를 가져와서 사용
				if (TSubclassOf<UGameplayEffect> GEToApply = GM->MapSelectGEClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GEToApply, 1.0f, ContextHandle);

					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
					}
				}
			}
		}
	}
}


void ATTTPlayerController::CharIndex()
{
	if (ATTTPlayerState* TTTPS = GetPlayerState<ATTTPlayerState>())
	{
		if (GetPawn())
		{
			if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(GetPawn()))
			{
				TTTPS->Server_SetCharIndexNeed(BaseChar->CharacterID);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Pawn is not ABaseCharacter."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Pawn possessed."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No TTT PlayerState."));
	}
}

void ATTTPlayerController::SetInputUI()
{
	if (IsLocalController())
	{
		if (ULocalPlayer* LP = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultIMC)
				{
					Subsystem->AddMappingContext(DefaultIMC, 0);
				}
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (UPlayPCComponent* PCC = FindComponentByClass<UPlayPCComponent>())
		{
			if (IA_Ping)
			{
				EIC->BindAction(IA_Ping, ETriggerEvent::Started, PCC, &UPlayPCComponent::OnPingPressed);			
				EIC->BindAction(IA_Ping, ETriggerEvent::Completed, PCC, &UPlayPCComponent::OnPingReleased);
			}
			if (IA_Shop)
			{
				EIC->BindAction(IA_Shop, ETriggerEvent::Completed, PCC, &UPlayPCComponent::OnShopPressed);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[TTTPlayerController] No PlayPCComponent found for Input Bindings."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TTTPlayerController] InputComponent is not UEnhancedInputComponent."));
	}
}
