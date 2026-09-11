#include "UI/PlayWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
 #include "UI/MVVM/GameStatusViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PCC/PlayPCComponent.h"
#include "Components/ListView.h"
#include "UI/MVVM/PartyManagerViewModel.h"
#include "UI/Widget/QuickSlotBarWidget.h"
#include "Components/Button.h"
#include "UI/MVVM/SkillCoolTimeViewModel.h"
#include "UI/Widget/SkillCoolTimeWidget.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/Widget/MapIconWidget.h"
#include "UI/Widget/PingIconWidget.h"


void UPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!PartyListView)
    {
        return;
    }

    if (OnTradeButton)
    {
        OnTradeButton->OnClicked.AddDynamic(this, &UPlayWidget::OnOffButtonClicked);
    }
}

void UPlayWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel)
{
    PlayerStatusViewModel = InViewModel;
}

void UPlayWidget::SetPartyManagerViewModel(UPartyManagerViewModel* ViewModel)
{
    PartyManagerViewModel = ViewModel;

    if (PartyManagerViewModel && PartyListView)
    {
        PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers());
    }
    else if (!PartyListView)
    {
        UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetPartyManagerViewModel: PartyListView is NULL. Check UMG layout."));
    }
}

void UPlayWidget::SetGameStatusViewModel(UGameStatusViewModel* InViewModel)
{
    GameStatusViewModel = InViewModel;
}
void UPlayWidget::SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* InViewModel)
{
    QuickSlotManagerViewModel = InViewModel;
    if (!QuickSlotManagerViewModel) { return; }

    if (QuickSlotBar)
    {
		QuickSlotBar->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetQuickSlotManagerViewModel: QuickSlotBar is NULL. Check UMG layout."));
	}
    if (QuickSlotBarItem)
    {
        QuickSlotBarItem->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel, true);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetQuickSlotManagerViewModel: QuickSlotBarItem is NULL. Check UMG layout."));
    }
}
void UPlayWidget::SetSkillCoolTimeViewModel(USkillCoolTimeViewModel* InViewModel)
{
    SkillCoolTimeViewModel = InViewModel;
    
	SetSkillCoolTimeListView();
}
void UPlayWidget::SetMapViewModel()
{   
    InitializeMiniMapIcons();
}


void UPlayWidget::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UPlayWidget::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UPlayWidget::SetsPartyListView()
{
    PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers());
}

void UPlayWidget::SetSkillCoolTimeListView()
{
    if (SkillCoolTimeViewModel)
    {
        SkillListView->SetListItems(SkillCoolTimeViewModel->GetSlotVMs());
    }
}

void UPlayWidget::OnOffButtonClicked()
{
    if (PlayerStatusViewModel)
    {
        PlayerStatusViewModel->OnOffTraderWindow(true);
    }
}

void UPlayWidget::InitializeMiniMapIcons()
{
    if (!MiniMapCanvas || !MapIconWidgetClass)
    {
		UE_LOG(LogTemp, Error, TEXT("InitializeMiniMapIcons: MiniMapCanvas or MapIconWidgetClass is NULL. Cannot initialize minimap icons."));
        return;
    }

    for (int32 i = 0; i < MiniMapIconCount; ++i)
    {
        UMapIconWidget* MapIcon = CreateWidget<UMapIconWidget>(this, MapIconWidgetClass);
        if (MapIcon)
        {
            MiniMapCanvas->AddChild(MapIcon);

            MapIcon->SetCachedCanvas(MiniMapCanvas);

            MiniMapIconWidgets.Add(MapIcon);
        }
        else
        {
			UE_LOG(LogTemp, Error, TEXT("Failed to create MapIconWidget instance! Check MapIconWidgetClass."));
        }

    }
}

void UPlayWidget::SetWidgetToMVs()
{
    InitializeMiniMapIcons();

    if (GameStatusViewModel)
    {
        GameStatusViewModel->CreateMapIconVMs(MiniMapIconCount);

        for (int32 i = 0; i < MiniMapIconWidgets.Num(); ++i)
        {
            if (i < GameStatusViewModel->GetMapIconVMs().Num())
            {
                MiniMapIconWidgets[i]->SetMapIconViewModel(GameStatusViewModel->GetMapIconVMs()[i]);
            }
            else
            {
				UE_LOG(LogTemp, Warning, TEXT("MapIconWidget NativeTick: No VM available for Slot %d"), i);
            }
		}


        GameStatusViewModel->StartMinimapUpdate();
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("SetWidgetToMVs: GameStatusViewModel is NULL. Cannot set up minimap icons."));
    }
}

UPingIconWidget* UPlayWidget::CreatePingIconWidget(int32 PingID)
{
    if (PingWidgetClasses.Num() < PingID)
    {
		UE_LOG(LogTemp, Error, TEXT("CreatePingIconWidget: Invalid PingID %d"), PingID);
        return nullptr;
    }

    UPingIconWidget* NewPingWidget = CreateWidget<UPingIconWidget>(this, PingWidgetClasses[PingID]);
    if (NewPingWidget)
    {
        MiniMapCanvas->AddChild(NewPingWidget);
        NewPingWidget->SetCachedCanvas(MiniMapCanvas);

        NewPingWidget->StartDestroyTimer();
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("CreatePingIconWidget: Failed to create PingIconWidget instance for PingID %d"), PingID);
        return nullptr;
    }

	return NewPingWidget;
}


