#include "UI/SlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/MVVM/TradeSlotViewModel.h"
#include "UI/MVVM/TradeViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"







void USlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SlotButton)
    {   
        SlotButton->OnClicked.AddDynamic(this, &USlotWidget::OnButtonClicked);  //외부로 연결 가능한지?
    }
}

void USlotWidget::OnButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("USlotWidget::OnButtonClicked - "));
    EntryViewModel->SendHeadSlot();
}

void USlotWidget::SetEntryViewModel(UTradeSlotViewModel* InViewModel)
{
    EntryViewModel = InViewModel;

    if (EntryViewModel)
    {
        EntryViewModel->BroadcastAllFieldValues();
    }
}




void USlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    UE_LOG(LogTemp, Warning, TEXT("[PartyWidget] NativeOnListItemObjectSet called."));
    // 부모 클래스 함수 호출
    //Super::NativeOnListItemObjectSet(ListItemObject);

    // 1. 전달받은 UObject를 캐스팅하여 위젯의 변수에 할당합니다. <--- 이 코드가 필수입니다!
    EntryViewModel = Cast<UTradeSlotViewModel>(ListItemObject);

    if (EntryViewModel)
    {
        // 2. 바인딩이 작동하기 위한 초기 데이터 설정 (선택적이지만 권장)
        // 예: NameText를 즉시 설정하여 바인딩 시스템에 부하를 줄일 수 있습니다.
        // NameText->SetText(PartyStatusViewModel->GetNameText());
    }
}