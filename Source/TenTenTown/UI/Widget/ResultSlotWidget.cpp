#include "UI/Widget/ResultSlotWidget.h"



void UResultSlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ResultSlotViewModel = Cast<UResultSlotViewModel>(ListItemObject);
}

void UResultSlotWidget::SetResultSlotViewModel(UResultSlotViewModel* InViewModel)
{
	ResultSlotViewModel = InViewModel;
}
