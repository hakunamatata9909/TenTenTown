#include "UI/CharSellectWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "Components/Button.h"

void UCharSellectWidget::NativeConstruct()
{	
    Super::NativeConstruct();

    if (WarriorButton)
    {
        WarriorButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnWarriorButtonClicked);
    }
    if (MageButton)
    {
        MageButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnMageButtonClicked);
    }
    if (ArcherButton)
    {
        ArcherButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnArcherButtonClicked);
	}
    if (RogueButton)
    {
        RogueButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnRogueButtonClicked);
    }    
}

void UCharSellectWidget::SetViewModel(ULobbyViewModel* InViewModel)
{
    LobbyViewModel = InViewModel;
}


void UCharSellectWidget::OnWarriorButtonClicked()
{
    if (LobbyViewModel)
    {           
        LobbyViewModel->SelectCharacter(0);
		UE_LOG(LogTemp, Warning, TEXT("전사 버튼 클릭"));
    }
    UE_LOG(LogTemp, Warning, TEXT("전사 버튼 끝"));
}
void UCharSellectWidget::OnMageButtonClicked()
{
    if (LobbyViewModel)
    {   
        LobbyViewModel->SelectCharacter(1);
    }
    UE_LOG(LogTemp, Warning, TEXT("법사 버튼 끝"));
}
void UCharSellectWidget::OnArcherButtonClicked()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectCharacter(2);
    }
}
void UCharSellectWidget::OnRogueButtonClicked()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectCharacter(3);
    }
}


