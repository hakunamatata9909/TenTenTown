#include "UI/LobbyWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "Components/Button.h"


void ULobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    
    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnConfirmButtonClicked);
    }
    if (MapButton)
    {
        MapButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnMapButtonClicked);
	}
    if (CharButton)
    {
        CharButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnCharButtonClicked);
	}
}

void ULobbyWidget::SetViewModel(ULobbyViewModel* InViewModel)
{
    // 뷰모델 포인터를 저장합니다.
    LobbyViewModel = InViewModel;
}

void ULobbyWidget::OnConfirmButtonClicked()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->ConfirmSelection();
    }
}

void ULobbyWidget::OnMapButtonClicked()
{
    if (LobbyViewModel)
    {
		LobbyViewModel->ReSelectMap();
    }
}

void ULobbyWidget::OnCharButtonClicked()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->ReSelectCharacter();
    }
}
