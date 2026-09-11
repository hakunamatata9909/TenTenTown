
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "GameFramework/PlayerController.h"

void UQuickSlotManagerViewModel::InitializeViewModel(ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI)
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


    CachedInventory->OnInventoryItemsChangedDelegate.AddDynamic(this, &UQuickSlotManagerViewModel::OnInventoryUpdatedItem);
    CreateQuickSlotEntriesItem(TTGI);
    OnInventoryUpdatedItem(CachedInventory->GetInventoryItems());

    CachedInventory->OnQuickSlotListChangedDelegate.AddDynamic(this, &UQuickSlotManagerViewModel::OnInventoryUpdated);
    CreateQuickSlotEntries(TTGI);
    
}

void UQuickSlotManagerViewModel::InitializeViewModel()
{
}

void UQuickSlotManagerViewModel::CreateQuickSlotEntriesItem(UTTTGameInstance* TTGI)
{
    if (!TTGI || !TTGI->ItemDataTable) { return; }

    UDataTable* ItemTable = TTGI->ItemDataTable;

    TArray<FName> AllRowNames;
    ItemTable->GetRowMap().GetKeys(AllRowNames);
    int32 IndexCounts = AllRowNames.Num();    

    QuickSlotEntryVMsItem.Empty();
    QuickSlotEntryVMsItem.SetNum(SlotCountBase);
    
    for (int32 i = 0; i < SlotCountBase; ++i)
    {
        if (i < IndexCounts)
        {
            const FName CurrentRowName = AllRowNames[i];

            const FItemData* ItemDataPtr = ItemTable->FindRow<FItemData>(CurrentRowName, TEXT("QuickSlotCreation"));

            UQuickSlotEntryViewModel* NewSlotVM = NewObject<UQuickSlotEntryViewModel>(this);

            if (ItemDataPtr)
            {
                NewSlotVM->SetSlotItem(*ItemDataPtr, CurrentRowName);
            }
            else
            {
				UE_LOG(LogTemp, Warning, TEXT("No Item Data found for Row: %s. Initializing with empty data."), *CurrentRowName.ToString());
            }

            QuickSlotEntryVMsItem[i] = NewSlotVM;
            NewSlotVM->SlotIndex = i;

            NewSlotVM->BroadcastAllFieldValues();
        }
        else
        {
            const FName CurrentRowName;
            FItemData EmptyData;
            UQuickSlotEntryViewModel* NewSlotVM = NewObject<UQuickSlotEntryViewModel>(this);
            NewSlotVM->SetSlotItem(EmptyData, CurrentRowName);

            QuickSlotEntryVMsItem[i] = NewSlotVM;
            NewSlotVM->SlotIndex = i;

            NewSlotVM->BroadcastAllFieldValues();
        }
    }
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(QuickSlotEntryVMsItem);
}
void UQuickSlotManagerViewModel::OnInventoryUpdatedItem(const TArray<FItemInstance>& NewItems)
{
    for (UQuickSlotEntryViewModel* SlotVM : QuickSlotEntryVMsItem)
    {
        if (IsValid(SlotVM))
        {
            SlotVM->UpdateItemDataItem(NewItems);
        }
    }
}



void UQuickSlotManagerViewModel::CreateQuickSlotEntries(UTTTGameInstance* TTGI)
{
    if (!TTGI || !TTGI->StructureDataTable) { return; }

    UDataTable* ItemTable = TTGI->StructureDataTable;

    TArray<FName> AllRowNames;
    ItemTable->GetRowMap().GetKeys(AllRowNames);
    int32 IndexCounts = AllRowNames.Num();

    QuickSlotEntryVMs.Empty();
    QuickSlotEntryVMs.SetNum(SlotCountBase);

    //for (int32 i = 0; i < IndexCounts; ++i)
    for (int32 i = 0; i < SlotCountBase; ++i)
    {
        if (i < IndexCounts)
        {
            const FName CurrentRowName = AllRowNames[i];

            // 3. RowName을 사용하여 데이터 테이블에서 FStructureData를 가져옵니다. (가장 효율적)
            const FStructureData* ItemDataPtr = ItemTable->FindRow<FStructureData>(CurrentRowName, TEXT("QuickSlotCreation"));

            UQuickSlotEntryViewModel* NewSlotVM = NewObject<UQuickSlotEntryViewModel>(this);

            if (ItemDataPtr)
            {   
                NewSlotVM->SetSlotStructure(*ItemDataPtr, CurrentRowName);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No Item Data found for Row: %s. Initializing with empty data."), *CurrentRowName.ToString());
            }
            
            QuickSlotEntryVMs[i] = NewSlotVM;
            NewSlotVM->SlotIndex = i;

            NewSlotVM->BroadcastAllFieldValues();
        }
        else
        {
            const FName CurrentRowName;
            FStructureData EmptyData;
            UQuickSlotEntryViewModel* NewSlotVM = NewObject<UQuickSlotEntryViewModel>(this);
            NewSlotVM->SetSlotStructure(EmptyData, CurrentRowName);

            QuickSlotEntryVMs[i] = NewSlotVM;
            NewSlotVM->SlotIndex = i;

            NewSlotVM->BroadcastAllFieldValues();
        }
    }
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(QuickSlotEntryVMs);
}
void UQuickSlotManagerViewModel::OnInventoryUpdated(const TArray<FInventoryItemData>& NewItems)
{
    for (UQuickSlotEntryViewModel* SlotVM : QuickSlotEntryVMs)    
    {
        if (IsValid(SlotVM))
        {
            SlotVM->UpdateItemData(NewItems);
        }
    }
}



