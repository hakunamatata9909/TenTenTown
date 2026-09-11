#include "UI/Widget/ResultWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"




void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UResultWidget::OnQuitButtonClicked);
	}
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UResultWidget::OnRestartButtonClicked);
	}
}


void UResultWidget::SetLobbyViewModel(ULobbyViewModel* InViewModel)
{
	LobbyViewModel = InViewModel;
}

void UResultWidget::SetResultListView()
{
	ResultListView->SetListItems(LobbyViewModel->GetResultVMs());
}

void UResultWidget::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Clicked"));
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		GetWorld()->GetFirstPlayerController(),
		EQuitPreference::Quit,
		false // true로 하면 플랫폼 체크를 무시하고 강제 종료하지만, 보통 false로 둡니다.
	);
}

void UResultWidget::OnRestartButtonClicked()
{
	LobbyViewModel->LobbyResetBt();
}
