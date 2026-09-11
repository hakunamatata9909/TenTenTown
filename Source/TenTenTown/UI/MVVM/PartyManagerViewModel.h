// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "PartyManagerViewModel.generated.h"

class UPartyStatusViewModel;
class ATTTGameStateBase;
class ATTTPlayerState;
class UPlayPCComponent;

UCLASS()
class TENTENTOWN_API UPartyManagerViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
    // UPlayPCComponent에서 호출하여 초기화 및 GameState 구독을 설정하는 함수
    void InitializeViewModel(ATTTPlayerState* PlayerState, ATTTGameStateBase* GameState, UPlayPCComponent* PCComponent);
    virtual void InitializeViewModel() override;
    void CleanupViewModel();

    // -----------------------------------------------------
    // UI 바인딩 속성: UListView에 바인딩될 파티원 목록
    // -----------------------------------------------------

    //TArray<TObjectPtr<UPartyStatusViewModel>> 타입은 UMG UListView의
    // ItemsSource에 직접 바인딩되어 동적 목록을 생성할 수 있습니다.
    UPROPERTY(BlueprintReadOnly, FieldNotify) // Setter와 Getter 속성 제거
    TArray<TObjectPtr<UPartyStatusViewModel>> PartyMembers;

    // -----------------------------------------------------
    // 목록 조작 함수 (PCC/GameMode에서 호출)
    // -----------------------------------------------------

    // 파티원이 게임에 참여했을 때 호출됩니다.
    UFUNCTION(BlueprintCallable, Category = "Party")
    void AddPartyMember(class ATTTPlayerState* NewPlayerState);

    // 파티원이 나갔을 때 호출됩니다.
    UFUNCTION(BlueprintCallable, Category = "Party")
    void RemovePartyMember(class ATTTPlayerState* LeavingPlayerState);

//  UFUNCTION(BlueprintPure, FieldNotify, Category = "Party")
//  TArray<UPartyStatusViewModel*> GetPartyMembers() const { return PartyMembers; }

protected:
    UPROPERTY()
    TObjectPtr<ATTTGameStateBase> CachedGameState;
    UPROPERTY()
    TObjectPtr<ATTTPlayerState> CachedPlayerState;
	UPROPERTY()
	TObjectPtr<UPlayPCComponent> CachedPCComponent;

    // -----------------------------------------------------
    // GameState 델리게이트 콜백
    // -----------------------------------------------------

    // GameState가 새로운 플레이어가 접속했음을 알릴 때 호출됩니다.
    // (ATTTGameStateBase에 OnPlayerJoinedDelegate가 있다고 가정)
    void HandlePlayerJoined(class ATTTPlayerState* NewPlayerState);

    // GameState가 플레이어가 나갔음을 알릴 때 호출됩니다.
    void HandlePlayerLeft(class ATTTPlayerState* LeavingPlayerState);

    // --- Getter & Setter 구현 (FieldNotify용) ---

public:
    UFUNCTION(BlueprintPure, FieldNotify, Category = "Party")
    TArray<UPartyStatusViewModel*> GetPartyMembers() const;
    UFUNCTION()
    void SetPartyMembers(TArray<UPartyStatusViewModel*> NewMembers);
    
    UFUNCTION(BlueprintCallable, Category = "Party")
    void ResetAndRefreshAll();

private:
    // 내부적으로 UPartyStatusViewModel을 찾기 위한 맵 (빠른 검색 및 제거를 위해)
    TMap<TObjectPtr<ATTTPlayerState>, TObjectPtr<UPartyStatusViewModel>> PartyViewModelMap;
	
public:
    void RefreshPartyMembers();

    void HandlePlayerListUpdate();
    
    
};
