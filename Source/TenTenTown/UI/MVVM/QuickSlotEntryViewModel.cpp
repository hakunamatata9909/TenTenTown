#include "UI/MVVM/QuickSlotEntryViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/Data/StructureData.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "Engine/World.h"



void UQuickSlotEntryViewModel::InitializeViewModel()
{

}

void UQuickSlotEntryViewModel::CleanupViewModel()
{
	ATTTPlayerState* PS = PlayerStateWeakPtr.Get();
	if (PS)
	{
		//// ⭐⭐ 델리게이트 구독 해제 (메모리 누수 방지) ⭐⭐
		//PS->OnStructureListChangedDelegate.RemoveAll(this);
		//PS->OnGoldChangedDelegate.RemoveAll(this);
	}
	PlayerStateWeakPtr.Reset();
	// 다른 캐싱된 포인터 정리 로직이 있다면 추가합니다.
}


void UQuickSlotEntryViewModel::SetCountText(const FText& NewValue)
{
	if (!CountText.EqualTo(NewValue))
	{
		CountText = NewValue;

		// 1. UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED 매크로 사용
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	}
}
void UQuickSlotEntryViewModel::SetCostText(const FText& NewValue)
{
	if (!CostText.EqualTo(NewValue))
	{
		CostText = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	}
}

void UQuickSlotEntryViewModel::SetIconTexture(UTexture2D* NewValue)
{
	if (IconTexture != NewValue)
	{
		IconTexture = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	}
}

void UQuickSlotEntryViewModel::SetCanAfford(bool bNewValue)
{
	if (bCanAfford != bNewValue)
	{
		bCanAfford = bNewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);
	}
}

void UQuickSlotEntryViewModel::SetIsEmptySlot(ESlateVisibility NewValue)
{
	if (bIsEmptySlot != NewValue)
	{
		bIsEmptySlot = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsEmptySlot);
	}
}

void UQuickSlotEntryViewModel::SetSlotNumber(int32 NewNumber)
{
    if (SlotNumber != NewNumber)
    {
        SlotNumber = NewNumber;
    }
}

void UQuickSlotEntryViewModel::OnSetCountTextItem(const FItemData& NewItemData)
{
	MaxCountText = NewItemData.MaxStackCount;
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));
	SetCountText(CountTextValue);
}

void UQuickSlotEntryViewModel::OnSetCountText(const FStructureData& NewItemData)
{
	MaxCountText = NewItemData.MaxInstallCount;
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));
	SetCountText(CountTextValue);
}

void UQuickSlotEntryViewModel::SetSlotItem(const FItemData& NewItemData, const FName& RowName)
{
	SlotItemRowName = RowName;
	if (RowName.IsNone())
	{
		SetIsEmptySlot(ESlateVisibility::Collapsed);
	}
	else
	{
		UTexture2D* LoadedTexture = NewItemData.ItemImage.LoadSynchronous();

		SetIconTexture(LoadedTexture);
		SetCostText(FText::AsNumber(NewItemData.SellPrice));
		//SetItemDesText(NewItemData.Description);
		//SetItemName(NewItemData.ItemName);
		OnSetCountTextItem(NewItemData);
		SetIsEmptySlot(ESlateVisibility::Visible);
	}
}

void UQuickSlotEntryViewModel::SetSlotStructure(const FStructureData& NewItemData, const FName& RowName)
{
	SlotItemRowName = RowName;
	if (RowName.IsNone())
	{
		SetIsEmptySlot(ESlateVisibility::Collapsed);
	}
	else
	{
		UTexture2D* LoadedTexture = NewItemData.StructureImage.LoadSynchronous();
		SetIconTexture(LoadedTexture);
		SetCostText(FText::AsNumber(NewItemData.InstallCost));
		//SetItemDesText(NewItemData.Description);
		//SetItemName(NewItemData.ItemName);
		OnSetCountText(NewItemData);
		SetIsEmptySlot(ESlateVisibility::Visible);
	}
}



void UQuickSlotEntryViewModel::UpdateItemDataItem(const TArray<FItemInstance>& NewItemData)
{
	// 이 슬롯이 담당하는 아이템 ID가 유효한지 확인
	if (SlotItemRowName.IsNone())
	{
		return;
	}

	int32 CurrentCount = 0;

	// 1. 인벤토리 배열 전체를 순회하며 이 슬롯의 아이템 개수를 합산
	for (const FItemInstance& ItemInstance : NewItemData)
	{
		// ItemInstance 구조체에 ItemID 필드가 있다고 가정
		if (ItemInstance.ItemID == SlotItemRowName)
		{
			CurrentCount += ItemInstance.Count;
		}
	}
	CurrentCountText = CurrentCount;
	CountText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
}

void UQuickSlotEntryViewModel::UpdateItemData(const TArray<FInventoryItemData>& NewItemData)
{
	// 이 슬롯이 담당하는 아이템 ID가 유효한지 확인
	if (SlotItemRowName.IsNone())
	{
		return;
	}

	int32 CurrentCount = 0;

	// 1. 인벤토리 배열 전체를 순회하며 이 슬롯의 아이템 개수를 합산
	for (const FInventoryItemData& ItemInstance : NewItemData)
	{
		// ItemInstance 구조체에 ItemID 필드가 있다고 가정
		if (ItemInstance.ItemName == SlotItemRowName)
		{
			CurrentCount += ItemInstance.Count;
		}
	}

	CurrentCountText = CurrentCount;
	CountText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));

	// 3. View에 변경 알림 (MVVM FieldNotify)
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
}

void UQuickSlotEntryViewModel::ClearItemData()
{
	SetIconTexture(nullptr);
	SetCostText(FText::GetEmpty());
	SetCountText(FText::GetEmpty());
}

void UQuickSlotEntryViewModel::BroadcastAllFieldValues()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SlotNumber);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsEmptySlot);
}

