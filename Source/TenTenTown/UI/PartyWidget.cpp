#include "UI/PartyWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TraderWidget.h"
#include "UI/MVVM/PartyStatusViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"



void UPartyWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    PartyStatusViewModel = Cast<UPartyStatusViewModel>(ListItemObject);
}
