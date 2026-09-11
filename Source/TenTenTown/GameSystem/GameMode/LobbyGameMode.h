// LobbyGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayEffect.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
class ATTTLobbyGameState;
class ATTTPlayerState;
class UGameplayEffect;
class APlayerController;
class UAbilitySystemComponent;
class USoundBase;
UCLASS()
class TENTENTOWN_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALobbyGameMode();
	void BeginPlay();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** PlayerState에서 Ready가 바뀌면 호출 */
	void HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState);

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	void ServerSpawnOrReplaceLobbyPreview(class ATTTPlayerState* PS, TSubclassOf<APawn> CharacterClass);
	
	// PC가 Host인지 확인 (서버 권한 체크에 사용)
	UFUNCTION(BlueprintCallable, Category="Host")
	bool IsHost(const APlayerController* PC) const;

	// BGM
	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TObjectPtr<USoundBase> LobbyBGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TObjectPtr<USoundBase> LobbyResultBGM = nullptr;
	
	UFUNCTION()
	void SyncLobbyBGM();

	UFUNCTION()
	void HandleResultRestartRequest(APlayerController* RequestPC);
protected:
	
	void UpdateLobbyCounts();

	UFUNCTION()
	void CheckAllReady();

	void TickCountdown();

	void StartGameTravel();

	FTransform GetPreviewSlotTransform(const ATTTPlayerState* PS) const;

	// 맵에 배치한 프리뷰 슬롯 액터들(4개)
	UPROPERTY()
	TArray<TObjectPtr<AActor>> PreviewSlots;

	/** 최소 시작 인원 (지금은 2명으로 테스트) */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	int32 MinPlayersToStart;

	/** 인게임 맵 경로 (예: "/Game/Maps/MainGame") */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	FString InGameMapPath;

	/** 카운트다운 시작 값(초) – 기본 5초 */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	int32 CountdownStartValue = 5;

	/** 카운트다운용 타이머 핸들 */
	FTimerHandle StartCountdownTimerHandle;

	// ===== Host(방장) 시스템 =====
	UPROPERTY(EditDefaultsOnly, Category="Host|Tag")
	TSubclassOf<UGameplayEffect> HostGEClass; // 예: GE_HostRole (GrantedTag: State.Role.Host)

	UPROPERTY()
	TWeakObjectPtr<APlayerController> HostPC;

	void AssignHost(APlayerController* NewHost);
	void ReassignHost();

	
#pragma region UI_Region
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> LobbyStateGEClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> CharSelectGEClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> MapSelectGEClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ResultGEClass;

	void EffectSet(class ATTTPlayerController* PlayerController);

protected:
	void InitGameState();
#pragma endregion
	
};
