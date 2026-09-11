// LobbyGameMode.cpp


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Sound/SoundBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass        = ALobbyGameState::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();
	PlayerStateClass      = ATTTPlayerState::StaticClass();

	MinPlayersToStart = 4; // <- 여기서 2인용 테스트
	InGameMapPath = TEXT("/Game/Maps/InGameMap"); // 실제 인게임 맵 경로로 수정

	CountdownStartValue = 5;

	bUseSeamlessTravel    = true;
	bStartPlayersAsSpectators = true;
}
void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 로비 맵에 들어온 직후(재진입 포함) BGM을 다시 맞춰줌
	GetWorldTimerManager().SetTimerForNextTick(this, &ALobbyGameMode::SyncLobbyBGM);
}
static void ApplyGEToSelf(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GEClass, UObject* SourceObj)
{
	if (!ASC || !GEClass) return;

	FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
	Ctx.AddSourceObject(SourceObj);

	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GEClass, 1.f, Ctx);
	if (Spec.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
	}
}

static void RemoveGEFromSelf(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GEClass)
{
	if (!ASC || !GEClass) return;
	ASC->RemoveActiveGameplayEffectBySourceEffect(GEClass, ASC);
}

bool ALobbyGameMode::IsHost(const APlayerController* PC) const
{
	return HostPC.IsValid() && HostPC.Get() == PC;
}

void ALobbyGameMode::AssignHost(APlayerController* NewHost)
{
	if (!HasAuthority() || !NewHost) return;

	// 이미 같은 Host면 중복 방지
	if (HostPC.IsValid() && HostPC.Get() == NewHost)
	{
		return;
	}

	// 기존 Host 태그 제거(선택)
	if (HostPC.IsValid())
	{
		if (ATTTPlayerState* OldPS = HostPC->GetPlayerState<ATTTPlayerState>())
		{
			if (UAbilitySystemComponent* OldASC = OldPS->GetAbilitySystemComponent())
			{
				RemoveGEFromSelf(OldASC, HostGEClass);
			}
		}
	}

	HostPC = NewHost;

	// 새 Host 태그 부여
	if (ATTTPlayerState* PS = NewHost->GetPlayerState<ATTTPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			ApplyGEToSelf(ASC, HostGEClass, this);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Host assigned: %s"), *GetNameSafe(NewHost));
}

void ALobbyGameMode::ReassignHost()
{
	if (!HasAuthority() || !GameState) return;

	for (APlayerState* PSBase : GameState->PlayerArray)
	{
		if (ATTTPlayerState* PS = Cast<ATTTPlayerState>(PSBase))
		{
			APlayerController* PC = Cast<APlayerController>(PS->GetOwner());
			if (PC)
			{
				AssignHost(PC);
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] No players to assign host."));
}

void ALobbyGameMode::EffectSet(ATTTPlayerController* PlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("EffectSet calls"));
	ATTTPlayerState* TTTPS = PlayerController->GetPlayerState<ATTTPlayerState>();
	if (!TTTPS)
	{
		UE_LOG(LogTemp, Error, TEXT("EffectSet 실패: PlayerState가 없습니다!"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EffectSet -Find TTTPS"));
		UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent();

		// ASC가 있고, 우리가 설정한 GE 클래스도 있는지 확인
		if (ASC && LobbyStateGEClass)
		{
			// 1. Context 생성 (누가 시전했냐? -> 게임모드다)
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			// 2. Spec 생성 (어떤 GE를 적용할 거냐? -> LobbyStateGEClass)
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(LobbyStateGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle2 = ASC->MakeOutgoingSpec(CharSelectGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle3 = ASC->MakeOutgoingSpec(MapSelectGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle4 = ASC->MakeOutgoingSpec(ResultGEClass, 1.0f, ContextHandle);

			UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
			if (GI && GI->HasLastGameResult())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle4.Data.Get(), ASC);
			}

			if (SpecHandle.IsValid())
			{
				// 3. 적용! (이제 서버가 적용하면 클라로 자동 복제됨)
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle2.Data.Get(), ASC);


				//호스트 태그를 갖는 경우
				if (ASC->HasMatchingGameplayTag(GASTAG::State_Role_Host))
				{
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle3.Data.Get(), ASC);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode에 LobbyStateGEClass가 비어있거나 ASC가 없습니다!"));
		}
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	SyncLobbyBGM();
	if (PreviewSlots.Num() == 0)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsWithTag(this, TEXT("LobbyPreviewSlot"), Found);
		for (AActor* A : Found) PreviewSlots.Add(A);
	}
	UpdateLobbyCounts();
	if (GetGameState<ALobbyGameState>())
	{
		ALobbyGameState* GS = GetGameState<ALobbyGameState>();
		GS->ConnectedPlayers = GS->PlayerArray.Num();
		
	}

	/* 방장 지정:첫 입장자 */
	if (!HostPC.IsValid())
	{
		AssignHost(NewPlayer);
	}

	if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(NewPlayer->PlayerState))
	{
		UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent();

		// ASC가 있고, 우리가 설정한 GE 클래스도 있는지 확인
		if (ASC && LobbyStateGEClass)
		{
			// 1. Context 생성 (누가 시전했냐? -> 게임모드다)
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			// 2. Spec 생성 (어떤 GE를 적용할 거냐? -> LobbyStateGEClass)
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(LobbyStateGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle2 = ASC->MakeOutgoingSpec(CharSelectGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle3 = ASC->MakeOutgoingSpec(MapSelectGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle4 = ASC->MakeOutgoingSpec(ResultGEClass, 1.0f, ContextHandle);

			UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
			if (GI && GI->HasLastGameResult())
			{
				UE_LOG(LogTemp, Warning, TEXT("Applying Result GE to %s"), *NewPlayer->GetName());
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle4.Data.Get(), ASC);
			}

			if (SpecHandle.IsValid())
			{
				// 3. 적용! (이제 서버가 적용하면 클라로 자동 복제됨)
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle2.Data.Get(), ASC);

				UE_LOG(LogTemp, Warning, TEXT("Server: Applied Lobby GE to %s"), *NewPlayer->GetName());

				//호스트 태그를 갖는 경우
				if (ASC->HasMatchingGameplayTag(GASTAG::State_Role_Host))
				{
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle3.Data.Get(), ASC);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode에 LobbyStateGEClass가 비어있거나 ASC가 없습니다!"));
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("post Login"));
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	if (HasAuthority() && Exiting)
	{
		if (ATTTPlayerState* PS = Exiting->GetPlayerState<ATTTPlayerState>())
		{
			if (PS->LobbyPreviewPawn)
			{
				PS->LobbyPreviewPawn->Destroy();
				PS->LobbyPreviewPawn = nullptr;
			}
		}
	}

	Super::Logout(Exiting);

	UpdateLobbyCounts();

	if (ALobbyGameState* GS = GetGameState<ALobbyGameState>())
	{
		GS->ConnectedPlayers = GS->PlayerArray.Num();
	}

	if (HostPC.IsValid() && HostPC.Get() == Exiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lobby] Host left. Reassigning..."));
		HostPC = nullptr;
		ReassignHost();
	}
}

void ALobbyGameMode::HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState)
{
	UpdateLobbyCounts();
	CheckAllReady();
}

void ALobbyGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	UE_LOG(LogTemp, Warning, TEXT("[LobbyGM::GetSeamlessTravelActorList] bToTransition=%d"), bToTransition);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>())
			{
				UE_LOG(LogTemp, Warning,
					TEXT("  Add PlayerState %s  SelectedClass=%s  Ready=%d"),
					*PS->GetPlayerName(),
					*GetNameSafe(PS->SelectedCharacterClass),
					PS->IsReady() ? 1 : 0);

				ActorList.Add(PS);
			}
		}
	}
}

void ALobbyGameMode::ServerSpawnOrReplaceLobbyPreview(class ATTTPlayerState* PS, TSubclassOf<APawn> CharacterClass)
{
	if (!HasAuthority() || !PS) return;

	// 1) 내 프리뷰만 정리 (절대 GetAllActorsOfClass 같은 걸로 지우지 마세요)
	if (PS->LobbyPreviewPawn)
	{
		PS->LobbyPreviewPawn->Destroy();
		PS->LobbyPreviewPawn = nullptr;
	}

	if (!CharacterClass)
	{
		return; // 선택 해제면 여기서 종료
	}

	// 2) 내 슬롯 위치에 새 프리뷰 스폰
	FActorSpawnParameters Params;
	Params.Owner = PS->GetOwner(); // 보통 PC
	Params.Instigator = Cast<APawn>(Params.Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FTransform SpawnTM = GetPreviewSlotTransform(PS);
	APawn* NewPreview = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnTM, Params);

	if (NewPreview)
	{
		NewPreview->SetReplicates(true); // 프리뷰를 모두에게 보이게 할거면
		PS->LobbyPreviewPawn = NewPreview;
		if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);

			PC->Possess(NewPreview);
			UE_LOG(LogTemp, Warning, TEXT("[Debug] PC=%s PossessedPawn=%s"),
	*GetNameSafe(PC), *GetNameSafe(PC->GetPawn()));
			PC->ClientRestart(NewPreview); // 입력 초기화(추천)
		}

		UE_LOG(LogTemp, Warning, TEXT("[LobbyPreview] Spawned %s for %s"),
			*GetNameSafe(NewPreview), *PS->GetPlayerName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LobbyPreview] Spawn FAILED for %s (Class=%s)"),
			*PS->GetPlayerName(), *GetNameSafe(CharacterClass));
	}
}

void ALobbyGameMode::UpdateLobbyCounts()
{
	ALobbyGameState* LobbyGS = GetGameState<ALobbyGameState>();
	if (!LobbyGS || !GameState)
	{
		return;
	}

	int32 Total = 0;
	int32 Ready = 0;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
		{
			++Total;

			// 선택된 캐릭터가 있고, Ready면 카운트
			if (TTTPS->SelectedCharacterClass && TTTPS->IsReady())
			{
				++Ready;
			}
		}
	}

	LobbyGS->SetConnectedPlayers(Total);
	LobbyGS->SetReadyPlayers(Ready);
}

void ALobbyGameMode::CheckAllReady()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	// 이미 Loading/인게임이면 다시 시작하지 않음
	if (GS->LobbyPhase != ELobbyPhase::Waiting)
	{
		return;
	}

	// 모든 플레이어가 Ready && 최소 인원 이상
	if (GS->ReadyPlayers == GS->ConnectedPlayers &&
		GS->ConnectedPlayers >= MinPlayersToStart)
	{
		// 1) Phase를 Loading으로 전환
		GS->LobbyPhase = ELobbyPhase::Loading;
		GS->OnRep_LobbyPhase();

		// 2) 카운트다운 값 세팅 (5초)
		GS->CountdownSeconds = CountdownStartValue;
		GS->OnRep_CountdownSeconds();

		// 3) 1초 간격으로 TickCountdown() 호출
		GetWorld()->GetTimerManager().SetTimer(
			StartCountdownTimerHandle,
			this,
			&ALobbyGameMode::TickCountdown,
			1.0f,
			true  // 반복
		);
	}
}

void ALobbyGameMode::StartGameTravel()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		GS->CountdownSeconds = 0;
		GS->OnRep_CountdownSeconds();
	}

	UWorld* World = GetWorld();
	if (!World) return;

	// 1) GameInstance에서 선택된 맵 경로 얻기
	UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>();
	FString MapPath;

	const bool bHasSelected =
		(GI && GI->HasSelectedMap() && GI->ResolvePlayMapPath(GI->GetSelectedMapIndex(), MapPath));

	// 선택값이 없으면 폴백
	if (!bHasSelected)
	{
		MapPath = InGameMapPath;
	}

	// 2) DedicatedServer면 ?listen 붙이지 않기
	const ENetMode NetMode = World->GetNetMode();
	const bool bDedicated = (NetMode == NM_DedicatedServer);

	const FString Url = bDedicated
		? FString::Printf(TEXT("%s"), *MapPath)
		: FString::Printf(TEXT("%s?listen"), *MapPath);

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] ServerTravel (%s) -> %s"),
		bDedicated ? TEXT("Dedicated") : TEXT("Listen"),
		*Url);

	// 3) 이동
	World->ServerTravel(Url, /*bAbsolute*/ false);
}

FTransform ALobbyGameMode::GetPreviewSlotTransform(const ATTTPlayerState* PS) const
{
	if (PreviewSlots.Num() == 0 || !PS)
	{
		return FTransform(); // fallback: 원점(실제로는 슬롯을 꼭 두시는 걸 추천)
	}

	// PlayerId 기반으로 0~3 슬롯 배정(중복 선택 허용과 무관)
	const int32 SlotIdx = FMath::Abs(PS->GetPlayerId()) % PreviewSlots.Num();
	return PreviewSlots[SlotIdx] ? PreviewSlots[SlotIdx]->GetActorTransform() : FTransform();
}

void ALobbyGameMode::TickCountdown()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS)
	{
		return;
	}

	// 안전장치: 로비 Phase가 아니면 카운트다운 중지
	if (GS->LobbyPhase != ELobbyPhase::Loading)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		return;
	}

	// 초 줄이기
	if (GS->CountdownSeconds > 0)
	{
		GS->CountdownSeconds--;
		GS->OnRep_CountdownSeconds();
	}

	// 0이 되면 타이머 정지 후 인게임으로 이동
	if (GS->CountdownSeconds <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		StartGameTravel();
	}
}

void ALobbyGameMode::InitGameState()
{
	UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState called"));
	Super::InitGameState();
	
	// 1. GameInstance 가져오기
	UTTTGameInstance* GI = Cast<UTTTGameInstance>(GetGameInstance());

	// 2. 새로 생성된 GameState 가져오기
	ALobbyGameState* LobbyGS = GetGameState<ALobbyGameState>();

	if (GI && LobbyGS)
	{
		UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState - GameInstance and GameState are valid"));
		// 3. GameInstance에 저장된 이전 플레이 결과가 있는지 확인
		const TArray<FPlayerResultData>& SavedResults = GI->GetPlayerResults();

		if (SavedResults.Num() > 0)
		{
			//SavedResults의 num이 몇개인지 체크
			UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState - SavedResults count: %d"), SavedResults.Num());
			UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState - Found saved results"));
			// 4. (핵심) GI의 데이터를 새로 생성된 GS의 복제 변수로 이관			
			LobbyGS->PlayerResults = SavedResults;
			LobbyGS->ForceNetUpdate();
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState - No saved results found"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode::InitGameState finished"));
}
void ALobbyGameMode::SyncLobbyBGM()
{
	if (!HasAuthority()) return;

	UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
	USoundBase* BGMToPlay = nullptr;

	// “재진입 후 결과창” 상태면 결과 BGM, 아니면 로비 기본 BGM
	if (GI && GI->HasLastGameResult())
	{
		BGMToPlay = LobbyResultBGM;   // <- GM에 UPROPERTY로 들고 있어야 함
	}
	else
	{
		BGMToPlay = LobbyBGM;
	}

	if (!BGMToPlay) return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATTTPlayerController* TPC = Cast<ATTTPlayerController>(It->Get());
		if (!TPC) continue;

		// 소리는 클라에서만 나야 하므로 Client RPC로만 전달
		TPC->Client_PlayBGM(BGMToPlay);
	}
}

void ALobbyGameMode::HandleResultRestartRequest(APlayerController* RequestPC)
{
	if (!HasAuthority()) return;

	UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
	if (GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Restart] Before Clear: HasLast=%d"),
	(GetGameInstance<UTTTGameInstance>() && GetGameInstance<UTTTGameInstance>()->HasLastGameResult()));
		GI->ClearLastGameResult();
		UE_LOG(LogTemp, Warning, TEXT("[Restart] After  Clear: HasLast=%d"),
	(GetGameInstance<UTTTGameInstance>() && GetGameInstance<UTTTGameInstance>()->HasLastGameResult()));
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATTTPlayerController* TPC = Cast<ATTTPlayerController>(It->Get());
		if (!TPC) continue;

		ATTTPlayerState* PS = TPC->GetPlayerState<ATTTPlayerState>();
		UAbilitySystemComponent* ASC = PS ? PS->GetAbilitySystemComponent() : nullptr;
		if (!ASC) continue;

		// ResultGEClass는 PostLogin에서 ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle4, ASC)로 “자기 자신에게” 걸었으니,
		// SourceEffect 제거 시 InstigatorASC로 ASC를 넣어주면 정리됩니다.
		if (ResultGEClass)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(ResultGEClass, ASC);
		}
	}


	SyncLobbyBGM();
}
