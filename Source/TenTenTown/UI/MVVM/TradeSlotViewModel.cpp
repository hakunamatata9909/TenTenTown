#include "UI/MVVM/TradeSlotViewModel.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "UI/PCC/PlayPCComponent.h"
#include "UI/TradeMainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Item/Data/ItemInstance.h"


void UTradeSlotViewModel::SetBindingDelegate()
{

}

void UTradeSlotViewModel::SetUpPlayPCC(UPlayPCComponent* CachedPlayPCC)
{
	CachedPlayPCComponent = CachedPlayPCC;
}

void UTradeSlotViewModel::InitializeViewModel()
{
}

void UTradeSlotViewModel::SetSlotItem(const FItemData& NewItemData, const FName& RowName)
{
	ItemName = RowName;
	SetIconTexture(NewItemData.ItemImage.Get());
	SetCostText(NewItemData.SellPrice);
	SetItemDesText(NewItemData.Description);
	SetItemName(NewItemData.ItemName);

	OnSetCountText(NewItemData);
}

void UTradeSlotViewModel::OnSetCountText(const FItemData& NewItemData)
{
	MaxCountText = NewItemData.MaxStackCount;
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));
	SetCountText(CountTextValue);
}

void UTradeSlotViewModel::SetCountText(const FText& NewValue)
{
	if (!CountText.EqualTo(NewValue))
	{
		CountText = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	}
}
void UTradeSlotViewModel::SetCostText(int32 NewValue)
{
	CostInt = NewValue;
	CostText = FText::AsNumber(NewValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostInt);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	
}

void UTradeSlotViewModel::SetIconTexture(UTexture2D* NewValue)
{
	if (IconTexture != NewValue)
	{
		IconTexture = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	}
}

void UTradeSlotViewModel::SetItemDesText(const FText& NewValue)
{
	ItemDesText = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ItemDesText);
}

void UTradeSlotViewModel::SetItemName(const FText& NewValue)
{
	ItemNameText = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ItemNameText);
}





void UTradeSlotViewModel::BroadcastAllFieldValues()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ItemDesText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ItemNameText);

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);	
}


void UTradeSlotViewModel::SendHeadSlot()
{	
	if (!CachedPlayPCComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("fail retrieved PlayPCComponent."));
		return;
	}

	CachedPlayPCComponent->GetTradeMainWidgetInstance()->SetTradeHeadSlotMV(this);
}

void UTradeSlotViewModel::UpdateCurrentCount(const TArray<FItemInstance>& InventoryItems)
{
	// 이 슬롯이 담당하는 아이템 ID가 유효한지 확인
	if (ItemName.IsNone())
	{
		return;
	}

	int32 CurrentCount = 0;

	// 1. 인벤토리 배열 전체를 순회하며 이 슬롯의 아이템 개수를 합산
	for (const FItemInstance& ItemInstance : InventoryItems)
	{
		// ItemInstance 구조체에 ItemID 필드가 있다고 가정
		if (ItemInstance.ItemID == ItemName)
		{
			CurrentCount += ItemInstance.Count;
		}
	}

	// 2. ViewModel의 Count 프로퍼티를 갱신하고 View에 알림을 보냅니다.
	// (여기서는 GetCurrentCountText() 함수가 이 Count를 사용한다고 가정합니다.)

	// 만약 Count를 직접 저장하는 변수가 있다면:
	CurrentCountText = CurrentCount;
	CountText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));

	// 3. View에 변경 알림 (MVVM FieldNotify)
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);

	
}