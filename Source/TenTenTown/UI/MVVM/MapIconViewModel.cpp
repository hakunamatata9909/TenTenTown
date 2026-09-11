#include "UI/MVVM/MapIconViewModel.h"




void UMapIconViewModel::SetbIsVisible(ESlateVisibility InVisible)
{
	bIsVisible = InVisible;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsVisible);
}

void UMapIconViewModel::SetIconPosition(const FVector2D& InPosition)
{
	IconPosition = InPosition;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconPosition);
}
void UMapIconViewModel::SetIconTexture(UTexture2D* InTexture)
{
	IconTexture = InTexture;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}
void UMapIconViewModel::SetbIsMyPlayerIcon(ESlateVisibility InIsMyPlayerIcon)
{
	bIsMyPlayerIcon = InIsMyPlayerIcon;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsMyPlayerIcon);
}

