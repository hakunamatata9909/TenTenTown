

#include "UI/Widget/MapSelectWidget.h"
#include "Components/Button.h"
#include "UI/MVVM/LobbyViewModel.h"

void UMapSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (MapButton0)
    {
        MapButton0->OnClicked.AddDynamic(this, &UMapSelectWidget::OnMapButton0);
    }
    if (MapButton1)
    {
        MapButton1->OnClicked.AddDynamic(this, &UMapSelectWidget::OnMapButton1);
    }
}

void UMapSelectWidget::SetViewModel(ULobbyViewModel* InViewModel)
{
    LobbyViewModel = InViewModel;
}


void UMapSelectWidget::OnMapButton0()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectMap(0);
    }
}
void UMapSelectWidget::OnMapButton1()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectMap(1);
    }
}
