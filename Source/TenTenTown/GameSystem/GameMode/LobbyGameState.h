// LobbyGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "LobbyGameState.generated.h"

UENUM(BlueprintType)
enum class ELobbyPhase : uint8
{
	Waiting   UMETA(DisplayName = "Waiting"), //캐릭터 선택
	ReadyCheck UMETA(DisplayName = "ReadyCheck"), // 레디 여부체크
	Loading   UMETA(DisplayName = "Loading") // 레디가 완료되면 5초대기를
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownChanged, int32, NewSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCountChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedMapChanged, int32, NewIndex);

UCLASS()
class TENTENTOWN_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	ALobbyGameState();

	/** 접속 인원 수 */
	UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers, BlueprintReadOnly, Category = "Lobby")
	int32 ConnectedPlayers;

	/** Ready 인원 수 */
	UPROPERTY(ReplicatedUsing = OnRep_ReadyPlayers, BlueprintReadOnly, Category = "Lobby")
	int32 ReadyPlayers;

	// 로비 페이즈 (Replicate)
	UPROPERTY(ReplicatedUsing=OnRep_LobbyPhase, BlueprintReadOnly)
	ELobbyPhase LobbyPhase;

	UPROPERTY(ReplicatedUsing=OnRep_CountdownSeconds, BlueprintReadOnly, Category="Lobby")
	int32 CountdownSeconds;

	UFUNCTION()
	void OnRep_ReadyPlayers();

	UFUNCTION()
	void OnRep_ConnectedPlayers();

	UFUNCTION()
	void OnRep_LobbyPhase();

	UFUNCTION()
	void OnRep_CountdownSeconds();

	UPROPERTY(ReplicatedUsing=OnRep_SelectedMapIndex, BlueprintReadOnly, Category="Lobby|Map")
	int32 SelectedMapIndex = INDEX_NONE;

	UFUNCTION()
	void OnRep_SelectedMapIndex();

	UPROPERTY(BlueprintAssignable, Category="Lobby|Map")
	FOnSelectedMapChanged OnSelectedMapChanged;

	UFUNCTION(BlueprintCallable, Category="Lobby|Map")
	void SetSelectedMapIndex(int32 NewIndex);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



#pragma region UIConnection
public:
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnCountdownChanged OnCountdownChanged;
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnPlayerCountChanged OnPlayerCountChanged;

	void SetConnectedPlayers(int32 NewCount);
	void SetReadyPlayers(int32 NewCount);

	//결과 데이터
	UPROPERTY(ReplicatedUsing = OnRep_PlayerResults, BlueprintReadOnly, Category = "GameResult")
	TArray<FPlayerResultData> PlayerResults;

	UFUNCTION()
	void OnRep_PlayerResults();
#pragma endregion

	

};
