#include "UI/Widget/MapIconWidget.h"
#include "Components/CanvasPanelSlot.h"

void UMapIconWidget::SetMapIconViewModel(UMapIconViewModel* NewMapIconViewModel)
{
	MapIconViewModel = NewMapIconViewModel;
}

void UMapIconWidget::SetCachedCanvas(UCanvasPanel* InCanvas)
{
	CachedCanvas = InCanvas;

    CachedCanvasSlot = Cast<UCanvasPanelSlot>(this->Slot);
}


void UMapIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!MapIconViewModel || !CachedCanvas || !CachedCanvasSlot) return;

    const FVector2D CanvasSize = CachedCanvas->GetCachedGeometry().GetLocalSize();
    if (CanvasSize.X <= 0.f || CanvasSize.Y <= 0.f) return;

    const FVector2D RatioPos = MapIconViewModel->GetIconPosition();
    const FVector2D PixelPos = RatioPos * CanvasSize;

    CachedCanvasSlot->SetPosition(PixelPos);
}