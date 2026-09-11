#include "UI/WaitWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"


void UWaitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AGameStateBase* GS = GetWorld() ? UGameplayStatics::GetGameState(GetWorld()) : nullptr;
	ALobbyGameState* LobbyGS = Cast<ALobbyGameState>(GS);

	if (LobbyGS)
	{	
		LobbyGS->OnCountdownChanged.AddDynamic(this, &UWaitWidget::UpdateCountdownText);
		// ... 필요한 다른 델리게이트도 바인딩
	}
}

void UWaitWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
void UWaitWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}


void UWaitWidget::SetWaitTime(FText NewText)
{
	UE_LOG(LogTemp, Warning, TEXT("SetWaitTime called"));
}

void UWaitWidget::SetNameText(int32 IndexNum, FText NewText)
{
	if (IndexNum == 1)
	{
		NameText1->SetText(NewText);
	}
	else if (IndexNum == 2)
	{
		NameText2->SetText(NewText);
	}
	else if (IndexNum == 3)
	{
		NameText3->SetText(NewText);
	}
}

void UWaitWidget::SetHeadImage(int32 IndexNum, UTexture2D* NewTexture)
{
	if (IndexNum == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 1 called"));
	}
	else if (IndexNum == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 2 called"));
	}
	else if (IndexNum == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 3 called"));
	}
}

void UWaitWidget::SetReadyImage(int32 IndexNum, UTexture2D* NewTexture)
{
	if (IndexNum == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 1 called"));
	}
	else if (IndexNum == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 2 called"));
	}
	else if (IndexNum == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 3 called"));
	}
}


void UWaitWidget::UpdateCountdownText(int32 NewReadyCount)
{
	WaitTime->SetText(FText::AsNumber(NewReadyCount));
}