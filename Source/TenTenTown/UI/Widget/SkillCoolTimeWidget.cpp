#include "UI/Widget/SkillCoolTimeWidget.h"
#include "UI/MVVM/SkillSlotViewModel.h"

void USkillCoolTimeWidget::NativeConstruct()
{

}

void USkillCoolTimeWidget::SetSkillSlotViewModel(USkillSlotViewModel* InViewModel)
{
	SkillSlotViewModel = InViewModel;
}


void USkillCoolTimeWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    SkillSlotViewModel = Cast<USkillSlotViewModel>(ListItemObject);

    /*if (SkillSlotViewModel)
    {
    }*/
}