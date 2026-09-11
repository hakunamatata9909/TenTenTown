#include "UI/MVVM/PartyManagerViewModel.h"
#include "UI/MVVM/PartyStatusViewModel.h"
#include "GameSystem/GameMode/TTTGameStateBase.h" 
#include "Character/PS/TTTPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UI/PCC/PlayPCComponent.h"



void UPartyManagerViewModel::InitializeViewModel(ATTTPlayerState* PlayerState, ATTTGameStateBase* GameState, UPlayPCComponent* PCComponent)
{
    CachedPlayerState = PlayerState;
    CachedGameState = GameState;
	CachedPCComponent = PCComponent;

    if (!CachedPlayerState || !CachedGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PartyManagerVM] Initialization failed: PlayerState or GameState is null."));
        return;
    }
    CachedGameState->OnPlayerJoinedDelegate.AddUObject(this, &UPartyManagerViewModel::HandlePlayerListUpdate);

    ResetAndRefreshAll();
}

void UPartyManagerViewModel::InitializeViewModel()
{
}

void UPartyManagerViewModel::RefreshPartyMembers()
{
    if (!CachedGameState || !CachedPlayerState) return;

    // 1. 기존 뷰모델 정리
    for (auto& Pair : PartyViewModelMap)
    {
        if (Pair.Value) Pair.Value->CleanupViewModel();
    }
    PartyViewModelMap.Empty();
    PartyMembers.Empty();

    // 2. 현재 접속 중인 플레이어 배열 가져오기
    TArray<ATTTPlayerState*> CurrentPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState);

    // 3. 새 뷰모델 생성
    for (ATTTPlayerState* PS : CurrentPlayers)
    {
        if (!PS) continue;

        UPartyStatusViewModel* VM = NewObject<UPartyStatusViewModel>(this);
        VM->InitializeViewModel(PS);
        PartyViewModelMap.Add(PS, VM);
        PartyMembers.Add(VM);
    }

    // 4. UI 브로드캐스트
    SetPartyMembers(PartyMembers);
}

void UPartyManagerViewModel::CleanupViewModel()
{
    if (CachedGameState)
    {
        // 1. GameState 델리게이트 구독 해제
         //CachedGameState->OnPlayerJoinedDelegate.RemoveAll(this);
         //CachedGameState->OnPlayerLeftDelegate.RemoveAll(this);
    }

    // 2. 개별 PartyStatusViewModel 정리 및 목록 비우기
    for (const auto& Pair : PartyViewModelMap)
    {
        // Pair.Value (PartyStatusViewModel)의 CleanupViewModel 호출
        if (Pair.Value)
        {
            Pair.Value->CleanupViewModel();
        }
    }
    PartyViewModelMap.Empty();

    // 3. UI 목록 초기화 및 브로드캐스트
    //SetPartyMembers({});

    CachedGameState = nullptr;
}




void UPartyManagerViewModel::AddPartyMember(ATTTPlayerState* NewPlayerState)
{
    if (!NewPlayerState || PartyViewModelMap.Contains(NewPlayerState))
    {
        return;
    }

    // 1. 새 PartyStatusViewModel 인스턴스 생성 및 초기화
    UPartyStatusViewModel* NewViewModel = NewObject<UPartyStatusViewModel>(this);
    NewViewModel->InitializeViewModel(NewPlayerState); // 여기서 GAS 구독이 시작됨

    // 2. 내부 맵과 UI 목록에 추가
    PartyViewModelMap.Add(NewPlayerState, NewViewModel);
    PartyMembers.Add(NewViewModel);

    // 3. 목록 변경 사항 UI에 브로드캐스트 (UListView 자동 갱신)
    // SetPartyMembers 호출을 통해 TArray<TObjectPtr<...>>가 변경되었음을 알립니다.
    SetPartyMembers(PartyMembers);
}

void UPartyManagerViewModel::RemovePartyMember(ATTTPlayerState* LeavingPlayerState)
{
    if (!LeavingPlayerState || !PartyViewModelMap.Contains(LeavingPlayerState))
    {
        return;
    }

    // 1. 제거할 ViewModel을 맵에서 가져오기
    TObjectPtr<UPartyStatusViewModel> ViewModelToRemove = PartyViewModelMap.FindAndRemoveChecked(LeavingPlayerState);

    // 2. ViewModel 정리 (GAS 구독 해제 등)
    if (ViewModelToRemove)
    {
        ViewModelToRemove->CleanupViewModel();

        // 3. UI 목록에서 제거
        PartyMembers.Remove(ViewModelToRemove);
    }

    // 4. 목록 변경 사항 UI에 브로드캐스트 (UListView 자동 갱신)
    SetPartyMembers(PartyMembers);
}


void UPartyManagerViewModel::HandlePlayerJoined(ATTTPlayerState* NewPlayerState)
{
    if (NewPlayerState)
    {
        AddPartyMember(NewPlayerState);
    }
}

void UPartyManagerViewModel::HandlePlayerLeft(ATTTPlayerState* LeavingPlayerState)
{
    if (LeavingPlayerState)
    {
        RemovePartyMember(LeavingPlayerState);
    }
}


TArray<UPartyStatusViewModel*> UPartyManagerViewModel::GetPartyMembers() const
{
    // 멤버 변수 (TObjectPtr 배열)를 RAW 포인터 배열로 변환하여 반환
    TArray<UPartyStatusViewModel*> RawPtrArray;
    for (const TObjectPtr<UPartyStatusViewModel>& Member : PartyMembers)
    {
        // TObjectPtr에서 RAW 포인터를 얻습니다.
        RawPtrArray.Add(Member.Get());
    }
    return RawPtrArray;
}


void UPartyManagerViewModel::SetPartyMembers(TArray<UPartyStatusViewModel*> NewMembers)
{
    this->PartyMembers.Empty();
    for (UPartyStatusViewModel* Member : NewMembers)
    {
        this->PartyMembers.Add(Member);
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PartyMembers);
}


void UPartyManagerViewModel::ResetAndRefreshAll()
{
    if (!CachedPlayerState || !CachedGameState)
    {
        return;
    }

    // 기존 PartyStatusViewModel 정리
    for (UPartyStatusViewModel* VM : PartyMembers)
    {
        if (VM)
            VM->CleanupViewModel();
    }
    PartyMembers.Empty();

    // 전체 플레이어 목록 가져오기 (로컬 제외)
    TArray<ATTTPlayerState*> AllPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState);

    for (ATTTPlayerState* PS : AllPlayers)
    {
        if (!PS) continue;

        UPartyStatusViewModel* NewVM = NewObject<UPartyStatusViewModel>(this);
        NewVM->InitializeViewModel(PS); // GAS 바인딩 등 처리
        PartyMembers.Add(NewVM);
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PartyMembers);
	UE_LOG(LogTemp, Log, TEXT("[PartyManagerVM] Full Refresh Completed."));
}

void UPartyManagerViewModel::HandlePlayerListUpdate()
{
    ResetAndRefreshAll();
    if (CachedPCComponent)
    {
        CachedPCComponent->SetPartyWidgets();
    }
}

