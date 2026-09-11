// TTTGameInstance.cpp


#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/PackageName.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/LocalPlayer.h"         
#include "OnlineSubsystemTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogTTTGameInstance, Log, All);
void UTTTGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UTTTGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
				this, &UTTTGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
				this, &UTTTGameInstance::OnJoinSessionComplete);
		}

		UE_LOG(LogTTTGameInstance, Log,
			TEXT("[Init] OnlineSubsystem=%s  SessionInterface valid=%d"),
			Subsystem ? *Subsystem->GetSubsystemName().ToString() : TEXT("None"),
			SessionInterface.IsValid() ? 1 : 0);
	}
	else
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[Init] No OnlineSubsystem found"));
	}

	// Dedicated 서버라면 자동으로 세션 만들고 로비로 이동
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[Init] Dedicated server detected -> HostDedicatedSession"));
		HostDedicatedSession();
	}
}

int32 UTTTGameInstance::ResolvePort(int32 OverridePort) const
{
	if (OverridePort>0)
	{
		return OverridePort;
	}
	int32 CliPort = 0;
	if (FParse::Value(FCommandLine::Get(),TEXT("port="), CliPort)&&CliPort>0)
	{
		return CliPort;
	}
	return (DevListenPort > 0) ? DevListenPort : 7777;
}
bool UTTTGameInstance::ResolveLobbyMapPath(FString& OutMapPath) const
{
	OutMapPath.Empty();

	if (!LobbyMap.IsNull())
	{
		OutMapPath = LobbyMap.ToSoftObjectPath().GetAssetPathString();
	}
	if (OutMapPath.IsEmpty())
	{
		OutMapPath = FallbackLobbyMapPath;
	}
	if (OutMapPath.IsEmpty())
	{
		UE_LOG(LogTTTGameInstance, Error, TEXT("[ResolveLobbyMapPath]:맵 경로 없음 비상..!"));
		return false;
	}
	return true;
}

int32 UTTTGameInstance::GetEffectivePort(int32 OverridePort) const
{
	return ResolvePort(OverridePort);
}
void UTTTGameInstance::HostDedicatedSession()
{
	// Dedicated 서버에서만 사용 (Init에서 호출)
	CreateOrHostSession(true);
}

void UTTTGameInstance::CreateOrHostSession(bool bDedicated /*=false*/)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[CreateOrHostSession] SessionInterface invalid. 세션 없이 그냥 HostLobby로 갑니다."));
		HostLobby(-1);
		return;
	}

	// 이미 세션이 있다면, 아주 단순하게 "한 번만 허용" (필요하면 나중에 DestroySession 로직 추가)
	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[CreateOrHostSession] Already has session -> 그냥 무시"));
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = bDedicated;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = true;          // 로컬 테스트용 LAN 세션
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bAllowJoinInProgress = true;

	bool bStarted = false;

	// Dedicated 서버 or LocalPlayer 없는 경우
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[CreateOrHostSession] DedicatedServer -> CreateSession(0)"));
		bStarted = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	}
	else
	{
		ULocalPlayer* LP = GetFirstGamePlayer();
		FUniqueNetIdPtr UserId = LP? LP->GetPreferredUniqueNetId().GetUniqueNetId(): nullptr;

		if (UserId.IsValid())
		{
			UE_LOG(LogTTTGameInstance, Log, TEXT("[CreateOrHostSession] ListenHost -> CreateSession(LocalPlayer)"));
			bStarted = SessionInterface->CreateSession(*UserId, NAME_GameSession, SessionSettings);
		}
		else
		{
			UE_LOG(LogTTTGameInstance, Warning,
				TEXT("[CreateOrHostSession] Invalid UserId -> CreateSession(0) fallback"));
			bStarted = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
		}
	}

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[CreateOrHostSession] CreateSession started=%d"), bStarted ? 1 : 0);

	// 이후 흐름은 OnCreateSessionComplete 에서 처리
}

void UTTTGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnCreateSessionComplete] SessionName=%s Success=%d"),
		*SessionName.ToString(), bWasSuccessful ? 1 : 0);

	if (!bWasSuccessful)
	{
		return;
	}

	// 세션이 성공적으로 생성되었으면, 실제 로비 맵으로 이동
	// Listen / Dedicated 모두 HostLobby 내부에서 적절히 ServerTravel/OpenLevel 처리
	HostLobby(-1);
}

// =============================
//  OnlineSession: Join 쪽
// =============================
void UTTTGameInstance::FindSessionsAndJoin()
{
	UE_LOG(LogTTTGameInstance, Log, TEXT("[FindSessionsAndJoin] called"));

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[FindSessionsAndJoin] SessionInterface invalid"));
		return;
	}

	ULocalPlayer* LP = GetFirstGamePlayer();
	if (!LP)
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[FindSessionsAndJoin] No LocalPlayer"));
		return;
	}

	FUniqueNetIdPtr UserId = LP->GetPreferredUniqueNetId().GetUniqueNetId();
	if (!UserId.IsValid())
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[FindSessionsAndJoin] Invalid UserId"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 50;

	UE_LOG(LogTTTGameInstance, Log, TEXT("[FindSessionsAndJoin] Start FindSessions"));

	const bool bStarted = SessionInterface->FindSessions(*UserId, SessionSearch.ToSharedRef());

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[FindSessionsAndJoin] FindSessions started=%d"), bStarted ? 1 : 0);
}

void UTTTGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnFindSessionsComplete] Success=%d"), bWasSuccessful ? 1 : 0);

	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnFindSessionsComplete] Search failed or SessionSearch invalid"));
		return;
	}

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnFindSessionsComplete] NumResults=%d"), SessionSearch->SearchResults.Num());

	if (SessionSearch->SearchResults.Num() <= 0)
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnFindSessionsComplete] No sessions found"));
		return;
	}

	// 일단 첫 번째 세션에 붙기
	ULocalPlayer* LP = GetFirstGamePlayer();
	FUniqueNetIdPtr UserId = LP? LP->GetPreferredUniqueNetId().GetUniqueNetId(): nullptr;
	if (!UserId.IsValid())
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnFindSessionsComplete] Invalid UserId for JoinSession"));
		return;
	}

	const bool bJoined = SessionInterface->JoinSession(
		*UserId,
		NAME_GameSession,
		SessionSearch->SearchResults[0]);

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnFindSessionsComplete] JoinSession started=%d"), bJoined ? 1 : 0);
}

void UTTTGameInstance::OnJoinSessionComplete(
	FName SessionName,
	EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnJoinSessionComplete] SessionName=%s Result=%d"),
		*SessionName.ToString(), (int32)Result);

	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnJoinSessionComplete] Join failed"));
		return;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnJoinSessionComplete] GetResolvedConnectString failed"));
		return;
	}

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[OnJoinSessionComplete] ClientTravel to %s"), *ConnectString);

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnJoinSessionComplete] World is null"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		// 아직 PC가 없으면 0.5초 뒤에 다시 시도
		UE_LOG(LogTTTGameInstance, Warning,
			TEXT("[OnJoinSessionComplete] No PlayerController yet -> delay travel"));

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this, SessionName]()
			{
				UWorld* InnerWorld = GetWorld();
				if (!InnerWorld)
				{
					return;
				}

				APlayerController* InnerPC = UGameplayStatics::GetPlayerController(InnerWorld, 0);
				if (!InnerPC)
				{
					return;
				}

				if (!SessionInterface.IsValid())
				{
					return;
				}

				FString InnerConnect;
				if (SessionInterface->GetResolvedConnectString(SessionName, InnerConnect))
				{
					InnerPC->ClientTravel(InnerConnect, TRAVEL_Absolute);
				}
			}),
			0.5f,
			false);

		return;
	}

	PC->ClientTravel(ConnectString, TRAVEL_Absolute);
}


void UTTTGameInstance::SaveSelectedCharacter(const FString& PlayerName, TSubclassOf<APawn> CharacterClass)
{
	if (PlayerName.IsEmpty() || !*CharacterClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GameInstance::SaveSelectedCharacter] Invalid data. Name=%s Class=%s"),
			*PlayerName, *GetNameSafe(*CharacterClass));
		return;
	}

	SelectedCharacters.Add(PlayerName, CharacterClass);

	UE_LOG(LogTemp, Warning,
		TEXT("[GameInstance::SaveSelectedCharacter] Saved %s -> %s"),
		*PlayerName, *GetNameSafe(*CharacterClass));
}
void UTTTGameInstance::SaveLastGameResult(bool bVictory, int32 Wave)
{
	LastResult.bValid      = true;
	LastResult.bVictory    = bVictory;
	LastResult.ReachedWave = Wave;

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[GameInstance] SaveLastGameResult: Victory=%d Wave=%d"),
		bVictory ? 1 : 0, Wave);
}

void UTTTGameInstance::ClearLastGameResult()
{
	LastResult = FTTTLastGameResult{};
	UE_LOG(LogTTTGameInstance, Log, TEXT("[GameInstance] ClearLastGameResult"));
}

TSubclassOf<APawn> UTTTGameInstance::GetSelectedCharacter(const FString& PlayerName) const
{
	if (const TSubclassOf<APawn>* Found = SelectedCharacters.Find(PlayerName))
	{
		return *Found;
	}

	return nullptr;
}

void UTTTGameInstance::SaveSelectedMapIndex(int32 InIndex)
{ SelectedMapIndex = InIndex; }

void UTTTGameInstance::HostLobby(int32 OverridePort)
{
	UWorld* World = GetWorld();
	if (!World) { UE_LOG(LogTTTGameInstance, Warning, TEXT("[HostLobby] World is null")); return; }

	FString MapPath;
	if (!ResolveLobbyMapPath(MapPath))
	{
		UE_LOG(LogTTTGameInstance, Error, TEXT("[HostLobby] Lobby map path resolve failed"));
		return;
	}

	const int32 Port = ResolvePort(OverridePort);
	const FString URL = FString::Printf(TEXT("%s?listen -port=%d"), *MapPath, Port);

	const ENetMode NM = World->GetNetMode();
	if (NM == NM_DedicatedServer || NM == NM_ListenServer)
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[HostLobby] ServerTravel -> %s"), *URL);
		World->ServerTravel(URL, false);
	}
	else
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[HostLobby] OpenLevel -> %s"), *URL);
		UGameplayStatics::OpenLevel(World, FName(*URL));
	}
}


void UTTTGameInstance::JoinLobby(const FString& IP, int32 OverridePort)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[JoinLobby]:월드가 없슴다"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[JoinLobby]:플레이어컨트롤러 어디갔어;"));
		return;
	}
	const int32 Port = ResolvePort(OverridePort);
	const FString TargetIP = IP.IsEmpty() ? TEXT("127.0.0.1") : IP;
	const FString URL = FString::Printf(TEXT("%s:%d"), *TargetIP, Port);

	UE_LOG(LogTTTGameInstance, Log, TEXT("[JoinLobby]:ClientTravel URL:%s"), *URL);
	PC->ClientTravel(URL, TRAVEL_Absolute);
	
}
void UTTTGameInstance::SaveConnectInfo(const FString& InIP, int32 InPort)
{
	SavedConnectIP = InIP;
	SavedConnectPort = InPort;

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[SaveConnectInfo] Saved IP=%s Port=%d"),
		*SavedConnectIP, SavedConnectPort);
}

void UTTTGameInstance::JoinSavedLobby()
{
	// 저장된 IP/Port 사용
	const FString IP = SavedConnectIP;
	const int32 Port = SavedConnectPort;

	UE_LOG(LogTTTGameInstance, Log,
		TEXT("[JoinSavedLobby] Using IP=%s Port=%d"),
		*IP, Port);

	// 기존 JoinLobby 로직 재사용 (여기서 ResolvePort 로 기본 포트도 처리됨)
	JoinLobby(IP, Port);
}

bool UTTTGameInstance::ResolvePlayMapPath(int32 InIndex, FString& OutMapPath) const
{
	OutMapPath.Empty();

	if (!PlayMapsByIndex.IsValidIndex(InIndex) || PlayMapsByIndex[InIndex].IsNull())
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[ResolvePlayMapPath] Invalid index=%d"), InIndex);
		return false;
	}

	// 예) "/Game/.../map_village_night.map_village_night"
	const FString ObjectPath = PlayMapsByIndex[InIndex].ToSoftObjectPath().ToString();

	// 트래블용 패키지명으로 변환: "/Game/.../map_village_night"
	OutMapPath = FPackageName::ObjectPathToPackageName(ObjectPath);

	if (OutMapPath.IsEmpty())
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[ResolvePlayMapPath] Empty package path. object=%s"), *ObjectPath);
		return false;
	}

	UE_LOG(LogTTTGameInstance, Log, TEXT("[ResolvePlayMapPath] index=%d object=%s -> package=%s"),
		InIndex, *ObjectPath, *OutMapPath);

	return true;
}

bool UTTTGameInstance::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (!ItemDataTable || ItemID.IsNone()) return false;

	const FItemData* Row = ItemDataTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));
	if (!Row) return false;

	OutItemData = *Row;
	return true;
}

UTexture2D* UTTTGameInstance::GetMapIconByIndex(int32 InIndex) const
{
	if (MapIcons.IsValidIndex(InIndex))
	{
		return MapIcons[InIndex];
	}
	return nullptr;
}

void UTTTGameInstance::SetPlayerResults(const TArray<FPlayerResultData>& InResults)
{
	PlayerResults = InResults;
}
