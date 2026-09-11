#include "UI/Widget/QuickSlotBarWidget.h"
#include "UI/Widget/QuickSlotEntryWidget.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"

void UQuickSlotBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 1. 9개의 개별 위젯을 관리 배열에 담습니다.
    QuickSlotEntryWidgets.Empty(9);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_0);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_1);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_2);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_3);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_4);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_5);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_6);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_7);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_8);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_9);

    
    // UMG 바인딩 실패로 nullptr이 들어갔다면 제거합니다.
    QuickSlotEntryWidgets.RemoveAll([](const TObjectPtr<UQuickSlotEntryWidget>& Widget) {
        return Widget == nullptr;
        });

    // NativeConstruct 시점에 이미 ManagerVM이 설정되어 있다면 바로 바인딩을 시도합니다.
    if (bIsItemSlotsBar)
    {
        BindEntryViewModelsItem();
    }
    else
    {
        BindEntryViewModels();
    }
    
}

void UQuickSlotBarWidget::SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* ManagerVM, bool bIsItem)
{
    if (!ManagerVM) return;

    QuickSlotManagerViewModel = ManagerVM;

    bIsItemSlotsBar = bIsItem;

    if (bIsItemSlotsBar)
    {
        BindEntryViewModelsItem();
    }
    else
    {
        BindEntryViewModels();
    }
    
}


void UQuickSlotBarWidget::BindEntryViewModelsItem()
{
    if (!QuickSlotManagerViewModel || QuickSlotEntryWidgets.IsEmpty()) return;

    const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& EntryViewModels = QuickSlotManagerViewModel->GetQuickSlotEntryVMsItem();



    int32 NumToBind = FMath::Min(QuickSlotEntryWidgets.Num(), EntryViewModels.Num());

    for (int32 i = 0; i < NumToBind; ++i)
    {
        UQuickSlotEntryWidget* EntryWidget = QuickSlotEntryWidgets[i].Get();
        UQuickSlotEntryViewModel* EntryVM = EntryViewModels[i].Get();

        if (EntryWidget && EntryVM)
        {
            EntryWidget->SetEntryViewModel(EntryVM);
            EntryVM->SetSlotNumber(i + 1);
        }
    }
}


void UQuickSlotBarWidget::BindEntryViewModels()
{
    if (!QuickSlotManagerViewModel || QuickSlotEntryWidgets.IsEmpty()) return;

    const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& EntryViewModels = QuickSlotManagerViewModel->GetQuickSlotEntryVMs();

    int32 NumToBind = FMath::Min(QuickSlotEntryWidgets.Num(), EntryViewModels.Num());

    for (int32 i = 0; i < NumToBind; ++i)
    {
        UQuickSlotEntryWidget* EntryWidget = QuickSlotEntryWidgets[i].Get();
        UQuickSlotEntryViewModel* EntryVM = EntryViewModels[i].Get();

        if (EntryWidget && EntryVM)
        {
            EntryWidget->SetEntryViewModel(EntryVM);
            EntryVM->SetSlotNumber(i + 1);
        }
    }
}
