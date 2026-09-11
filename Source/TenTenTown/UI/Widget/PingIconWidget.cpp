#include "UI/Widget/PingIconWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "TimerManager.h"
#include "Engine/World.h"



void UPingIconWidget::SetCachedCanvas(UCanvasPanel* InCanvas)
{
	CachedCanvas = InCanvas;
}

void UPingIconWidget::SetLocation(FVector2D InLocation)
{
    UCanvasPanelSlot* PingSlot = Cast<UCanvasPanelSlot>(this->Slot);
    if (!PingSlot) return;

    FAnchors NewAnchors;
    NewAnchors.Minimum = InLocation;
    NewAnchors.Maximum = InLocation;
    PingSlot->SetAnchors(NewAnchors);

    PingSlot->SetPosition(FVector2D::ZeroVector);

    PingSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    PingSlot->SetZOrder(100);
    PingSlot->SetAutoSize(true);

}



void UPingIconWidget::StartDestroyTimer()
{
    GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &UPingIconWidget::HandleDestroy, 3.0f, false);
}

void UPingIconWidget::HandleDestroy()
{
    RemoveFromParent();
}