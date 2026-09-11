#include "UI/MVVM/TradeViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Engine/World.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"





void UTradeViewModel::InitializeViewModel()
{
}

UInventoryPCComponent* UTradeViewModel::GetInventoryPCComponent() const
{
    return CachedInventory;
}

void UTradeViewModel::InitializeViewModel(UPlayPCComponent* CachedPlayPCC, ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI)
{
    if (!InPlayerState) // ASC 유효성 검사 추가
    {
        UE_LOG(LogTemp, Error, TEXT("ManagerVM 초기화 실패: PlayerState 또는 ASC가 유효하지 않습니다."));
        return;
    }


    //인벤토리 찾음
    APlayerController* PC = Cast<APlayerController>(InPlayerState->GetOwner());
    if (!PC) { return; }
    CachedInventory = PC->FindComponentByClass<UInventoryPCComponent>();
    if (!CachedInventory) { return; }

    //델리게이트 구독
    //변경된 정보를 받고 처리해야됨..
    CachedInventory->OnInventoryItemsChangedDelegate.AddDynamic(this, &UTradeViewModel::OnInventoryUpdated);
    


    CachedPlayPCComponent = CachedPlayPCC;
	CachedPlayPCComponent->GetPCCPlayerStateRef()->OnGoldChangedDelegate.AddDynamic(this, &UTradeViewModel::SetCanTrade);


    CreateTradeSlotEntries(TTGI);
    
    //퀵슬롯 초기화
    OnInventoryUpdated(CachedInventory->GetInventoryItems());
}


void UTradeViewModel::CreateTradeSlotEntries(UTTTGameInstance* TTGI)
{   
    if (!TTGI || !TTGI->ItemDataTable) { return; }

    UDataTable* ItemTable = TTGI->ItemDataTable;

    TArray<FName> AllRowNames;
    ItemTable->GetRowMap().GetKeys(AllRowNames);
    int32 IndexCounts = AllRowNames.Num();

    TradeSlotEntryVMs.Empty();
    TradeSlotEntryVMs.SetNum(IndexCounts);

    for (int32 i = 0; i < IndexCounts; ++i)
    {
        const FName CurrentRowName = AllRowNames[i];

        const FItemData* ItemDataPtr = ItemTable->FindRow<FItemData>(CurrentRowName, TEXT("TradeSlotCreation"));

        if (ItemDataPtr)
        {
            UTradeSlotViewModel* NewSlotVM = NewObject<UTradeSlotViewModel>(this);

            NewSlotVM->SetSlotItem(*ItemDataPtr, CurrentRowName);

            NewSlotVM->SetUpPlayPCC(CachedPlayPCComponent);


            TradeSlotEntryVMs[i] = NewSlotVM;
        }
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TradeSlotEntryVMs);
}

TArray<UTradeSlotViewModel*> UTradeViewModel::GetPartyMembers() const
{
    TArray<UTradeSlotViewModel*> RawPtrArray;
    for (const TObjectPtr<UTradeSlotViewModel>& Member : TradeSlotEntryVMs)
    {
        RawPtrArray.Add(Member.Get());
    }
    return RawPtrArray;
}




void UTradeViewModel::OnTradeSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList)
{
    if (NewQuickSlotList.Num() != TradeSlotEntryVMs.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickSlotManagerViewModel: Received list size mismatch (%d vs %d)."), NewQuickSlotList.Num(), TradeSlotEntryVMs.Num());
    }

    
}

void UTradeViewModel::CallSlotDelegate()
{
    for (UTradeSlotViewModel* SlotVM : TradeSlotEntryVMs)
    {
        if (SlotVM)
        {
            SlotVM->BroadcastAllFieldValues();
        }
    }
}

void UTradeViewModel::OnInventoryUpdated(const TArray<FItemInstance>& NewItems)
{
    for (UTradeSlotViewModel* SlotVM : TradeSlotEntryVMs)
    {
        if (IsValid(SlotVM))
        {
            SlotVM->UpdateCurrentCount(NewItems);
        }
    }
}

void UTradeViewModel::SetTradeHeadSlotMV(UTradeSlotViewModel* NewTradeHeadSlotMV)
{
    TradeHeadSlotMV = NewTradeHeadSlotMV;
    SetCanTrade(0);
}

void UTradeViewModel::SetCanTrade(int32 golds)
{
    if (!TradeHeadSlotMV || !CachedPlayPCComponent)
    {
        return;
    }
    
    int32 PlayerGold = CachedPlayPCComponent->GetPlayerStateRef()->GetGold();
    bool CanMoney;	
    if (PlayerGold < TradeHeadSlotMV->GetCostInt())
    {
        CanMoney = false;
    }
    else
    {
        CanMoney = true;
    }
    bool CanItem;
    int MaxCounts = TradeHeadSlotMV->GetMaxCountText();
    int CurrentCounts = TradeHeadSlotMV->GetCurrentCountText();
    if (CurrentCounts < MaxCounts)
    {
        CanItem = true;
    }
    else
    {
        CanItem = false;
    }

    if (CanMoney && CanItem)
    {
        CanTrade = true;
    }
    else
    {
        CanTrade = false;
    }


    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CanTrade);
}


