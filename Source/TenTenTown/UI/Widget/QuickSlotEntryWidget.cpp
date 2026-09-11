#include "UI/Widget/QuickSlotEntryWidget.h"
#include "Components/Button.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"

void UQuickSlotEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (QuickSlotButton)
    {
        QuickSlotButton->OnClicked.AddDynamic(this, &UQuickSlotEntryWidget::OnQuickSlotClicked);
    }
}

void UQuickSlotEntryWidget::SetEntryViewModel(UQuickSlotEntryViewModel* InViewModel)
{
    EntryViewModel = InViewModel;

    if (EntryViewModel)
    {
        EntryViewModel->BroadcastAllFieldValues();
    }
}

void UQuickSlotEntryWidget::OnQuickSlotClicked()
{
    if (EntryViewModel)
    {
        // EntryViewModel->ServerRequestUseSlot(); 와 같은 함수 호출 로직이 필요합니다.
    }
}


