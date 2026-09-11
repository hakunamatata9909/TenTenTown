#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Components/SlateWrapperTypes.h"
#include "UI/MVVM/ResultSlotViewModel.h"
#include "UI/PCC/LobbyPCComponent.h"
#include "LobbyViewModel.generated.h"

// Forward Declaration
class ATTTPlayerState;
class ALobbyGameState;
class ATTTPlayerController;
class UTexture2D;

UCLASS(BlueprintType)
class TENTENTOWN_API ULobbyViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	// **로비 뷰모델 초기화:** PCC에서 PlayerState를 받아와 구독을 시작합니다.
	void Initialize(ALobbyGameState* InGS, ATTTPlayerController* InPC, ULobbyPCComponent* InLobbyPCComponent);

	// UBaseViewModel의 핵심 함수 오버라이드
	virtual void InitializeViewModel() override;
	virtual void CleanupViewModel() override;

protected:
	// GameState 참조 포인터
	TObjectPtr<class ALobbyGameState> CachedGameState;

	UPROPERTY()
	TObjectPtr<ULobbyPCComponent> CachedLobbyPCComponent;

	UPROPERTY(BlueprintGetter = GetReadyCountText, FieldNotify)
	FText ReadyCountText;

public:
	UFUNCTION(BlueprintPure)
	FText GetReadyCountText() const { return ReadyCountText; }

protected:
	UPROPERTY(BlueprintGetter = GetTimerText, FieldNotify)
	FText TimerText;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility TimerVisibility = ESlateVisibility::Collapsed;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> MapIconTexture;

public:
	UFUNCTION(BlueprintPure)
	FText GetTimerText() const { return TimerText; }

	// 뷰모델에서 이 값을 업데이트할 Setter 함수 (C++ 전용)
	void SetReadyCountText(const FText& NewText);
	void SetTimerText(const FText& NewText);
	void SetMapIconTexture(UTexture2D* NewTexture);




	// GameState 델리게이트 구독 핸들러 함수
	UFUNCTION()
	void HandleCountdownChanged(int32 NewSeconds);

	UFUNCTION()
	void HandlePlayerCountChanged();

	UFUNCTION()
	void HandleSelectedMapChanged(int32 NewMapIndex);


#pragma region CharSellectRegion
protected:
	TObjectPtr<class ATTTPlayerController> CachedPlayerController;
public:
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void SelectCharacter(int32 CharIndex);
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void ConfirmSelection();
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void SelectMap(int32 MapIndex);
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void ReSelectCharacter();
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void ReSelectMap();

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "UI|State")
	ESlateVisibility CharButtonVisibility = ESlateVisibility::Visible;
	void SetCharButtonVisibility(const ESlateVisibility NewVisibility);
	UFUNCTION(BlueprintPure)
	ESlateVisibility GetCharButtonVisibility() const { return CharButtonVisibility; }

#pragma endregion
protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "UI|State")
	ESlateVisibility MapButtonVisibility = ESlateVisibility::Visible;

	UPROPERTY(BlueprintReadWrite, FieldNotify)
	bool bIsHost = false;

public:	
	void SetMapButtonVisibility(const ESlateVisibility NewVisibility);
	UFUNCTION(BlueprintPure)
	ESlateVisibility GetMapButtonVisibility() const { return MapButtonVisibility; }

	void SetIsHost(bool bNewIsHost);
	bool GetIsHost() const { return bIsHost; }

#pragma region Result_Region
protected:
	//base
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility bIsWin = ESlateVisibility::Collapsed;
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility bIsLose = ESlateVisibility::Collapsed;

	//slots
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UResultSlotViewModel>> ResultVMs;

public:
	void SetResultVMs();


	UFUNCTION(BlueprintPure, Category = "Lobby|Result")
	TArray<UResultSlotViewModel*> GetResultVMs() const;

	void SetMainResult(ALobbyGameState* LobbyGS);

	void LobbyResetBt();
#pragma endregion


};