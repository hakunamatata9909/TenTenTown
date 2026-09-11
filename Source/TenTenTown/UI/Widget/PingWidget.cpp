#include "UI/Widget/PingWidget.h"
#include "UI/PCC/PlayPCComponent.h"


void UPingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PingButtonA)
	{
		PingButtonA->OnHovered.AddDynamic(this, &UPingWidget::SetHoveredTypeA);
		PingButtonA->OnUnhovered.AddDynamic(this, &UPingWidget::ClearHoveredTypes);
	}
	if (PingButtonB)
	{
		PingButtonB->OnHovered.AddDynamic(this, &UPingWidget::SetHoveredTypeB);
		PingButtonB->OnUnhovered.AddDynamic(this, &UPingWidget::ClearHoveredTypes);
	}
	if (PingButtonC)
	{
		PingButtonC->OnHovered.AddDynamic(this, &UPingWidget::SetHoveredTypeC);
		PingButtonC->OnUnhovered.AddDynamic(this, &UPingWidget::ClearHoveredTypes);
	}
	if (PingButtonD)
	{
		PingButtonD->OnHovered.AddDynamic(this, &UPingWidget::SetHoveredTypeD);
		PingButtonD->OnUnhovered.AddDynamic(this, &UPingWidget::ClearHoveredTypes);
	}
}


void UPingWidget::ClearHoveredTypes()
{
	PCC->PingTypeSelected = -1;
}

void UPingWidget::SetHoveredTypeA()
{
	PCC->PingTypeSelected = 0;
}

void UPingWidget::SetHoveredTypeB()
{
	PCC->PingTypeSelected = 1;
}

void UPingWidget::SetHoveredTypeC()
{
	PCC->PingTypeSelected = 2;
}

void UPingWidget::SetHoveredTypeD()
{
	PCC->PingTypeSelected = 3;
}




void UPingWidget::SetPCC(UPlayPCComponent* InPCC)
{
	PCC = InPCC;
}