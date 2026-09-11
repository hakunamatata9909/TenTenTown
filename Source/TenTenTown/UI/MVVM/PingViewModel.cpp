#include "UI/MVVM/PingViewModel.h"



void UPingViewModel::SetbIsVisible(ESlateVisibility InVisible)
{
	bIsVisible = InVisible;
}

void UPingViewModel::SetIconPosition(const FVector2D& InPosition)
{
	IconPosition = InPosition;
}