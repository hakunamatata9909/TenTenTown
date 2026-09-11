#include "UI/ReadyHUD.h"
#include "Blueprint/UserWidget.h"

void AReadyHUD::BeginPlay()
{
	Super::BeginPlay();
	if (ReadyWidgetClass)
	{
		UUserWidget* PlayWidget = CreateWidget<UUserWidget>(GetWorld(), ReadyWidgetClass);
		if (PlayWidget)
		{
			PlayWidget->AddToViewport();
		}
	}
}
