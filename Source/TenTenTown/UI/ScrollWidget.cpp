#include "UI/ScrollWidget.h"
#include "Components/ScrollBox.h"
#include "UI/SlotWidget.h"
#include "UI/TraderWidget.h"

void UScrollWidget::MoveScrollBox(float Delta)
{
    if (ItemListScrollBox)
    {
        float CurrentOffset = ItemListScrollBox->GetScrollOffset();
                
        float NewOffset = CurrentOffset + Delta;
                
        ItemListScrollBox->SetScrollOffset(NewOffset);
    }
}

void UScrollWidget::AddNewItemToPanel(FText ItemName)
{   
    // 1. 필수 포인터 유효성 검사
    if (!ItemListScrollBox)
    {
        UE_LOG(LogTemp, Error, TEXT("UScrollWidget::AddNewItemToPanel: ItemListScrollBox가 유효하지 않습니다."));
        return;
    }

    if (!ItemWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UScrollWidget::AddNewItemToPanel: ItemWidgetClass 설계도 설정이 누락되었습니다."));
        return;
    }
        
    USlotWidget* NewSlotWidget = CreateWidget<USlotWidget>(this, ItemWidgetClass);

    if (NewSlotWidget)
    {
        //DB찾아서 가져오자->그리고 인수에 넣자
        
        //NewSlotWidget->SetSlotWidgetData();
                
        ItemListScrollBox->AddChild(NewSlotWidget);

        ActiveItemWidgets.Add(NewSlotWidget);

        UE_LOG(LogTemp, Log, TEXT("아이템 '%s' 위젯이 스크롤 박스에 성공적으로 추가되었습니다."), *ItemName.ToString());
    }
    else
    {
        
    }
}

void UScrollWidget::RemoveItemFromPanel(int32 IndexToRemove)
{
}

USlotWidget* UScrollWidget::GetAddSlot()
{
    USlotWidget* NewSlotWidget = CreateWidget<USlotWidget>(this, ItemWidgetClass);
    if (NewSlotWidget)
    {
        ItemListScrollBox->AddChild(NewSlotWidget);
        ActiveItemWidgets.Add(NewSlotWidget);
		//NewSlotWidget->OnSlotItemClicked.AddDynamic(this, &UScrollWidget::HandleSlotClicked);
		return NewSlotWidget;
    }
    return nullptr;
}

void UScrollWidget::HandleSlotClicked(FText SlotObjectName)
{
    UObject* OuterObject = GetOuter();
    if (UTraderWidget* TraderWidget = Cast<UTraderWidget>(OuterObject))
    {
        TraderWidget->ChangeHeadSlot(SlotObjectName);
    }
    //ChangeHeadSlot
}

USlotWidget* UScrollWidget::GetSlot(int32 SlotIndex)
{
    if (ActiveItemWidgets.IsValidIndex(SlotIndex))
    {
        return ActiveItemWidgets[SlotIndex];
    }
    return nullptr;
}
