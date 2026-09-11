#include "UI/StartWidget.h"
#include "Components/Button.h"

void UStartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UStartWidget::OnMyButtonClicked);
    }
}

void UStartWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UStartWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}


void UStartWidget::OnMyButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked!"));
	//다음 레벨로 이동해야됨 -> 게임모드에서 처리
}


