#include "UI/PCC/StartPCComponent.h"
#include "UI/StartWidget.h"

UStartPCComponent::UStartPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UStartPCComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetOwner());

	if (StartWidgetClass && PC)
	{
		StartWidgetInstance = CreateWidget<UStartWidget>(PC, StartWidgetClass);

		if (StartWidgetInstance)
		{
			StartWidgetInstance->AddToViewport();
		}
	}
}




void UStartPCComponent::RemoveStartWidget()
{
	if (StartWidgetInstance && StartWidgetInstance->IsInViewport())
	{
		StartWidgetInstance->RemoveFromParent();

		StartWidgetInstance = nullptr;
	}
}