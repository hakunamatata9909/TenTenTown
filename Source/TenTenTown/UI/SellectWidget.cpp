#include "UI/SellectWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USellectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SellectButton)
	{
		SellectButton->OnClicked.AddDynamic(this, &USellectWidget::OnSellectButtonClicked);
	}
	if (FrontButton)
	{
		FrontButton->OnClicked.AddDynamic(this, &USellectWidget::OnFrontButtonClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USellectWidget::OnBackButtonClicked);
	}
}

void USellectWidget::SetHeadText(FText NewText)
{
	HeadText->SetText(NewText);
}

void USellectWidget::SetDesText(FText NewText)
{
	DesText->SetText(NewText);
}

void USellectWidget::OnSellectButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Sellect Button Clicked!"));
}

void USellectWidget::OnFrontButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Front Button Clicked!"));
}
void USellectWidget::OnBackButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Back Button Clicked!"));
}
