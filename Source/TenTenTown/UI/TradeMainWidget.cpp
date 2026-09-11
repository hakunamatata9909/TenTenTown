#include "UI/TradeMainWidget.h"
#include "Components/Button.h"
#include "UI/PlayHUD.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "UI/PCC/InventoryPCComponent.h"



void UTradeMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
    if (OffTrade)
    {
        OffTrade->OnClicked.AddDynamic(this, &UTradeMainWidget::OnOffButtonClicked);
    }
    if (TargetGetButton)
    {
        TargetGetButton->OnClicked.AddDynamic(this, &UTradeMainWidget::OnGetButtonClicked);
    }
    
}

void UTradeMainWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* ViewModel)
{
    PlayerStatusViewModel = ViewModel;
}

void UTradeMainWidget::SetTradeViewModel(UTradeViewModel* ViewModel)
{
    TradeViewModel = ViewModel;

    if (TradeViewModel && TradeListView)
    {
        TradeListView->SetListItems(TradeViewModel->GetPartyMembers());

        TradeViewModel->CallSlotDelegate();

        if (TradeViewModel->GetPartyMembers().Num() > 0)
        {
            SetTradeHeadSlotMV(TradeViewModel->GetPartyMembers()[0]);
        }
    }
    else if (!TradeListView)
    {
		UE_LOG(LogTemp, Error, TEXT("[UTradeMainWidget] TradeListView is not bound!"));
    }
}

void UTradeMainWidget::SetTradeHeadSlotMV(UTradeSlotViewModel* ViewModel)
{
    TradeHeadSlotMV = ViewModel;
    TradeViewModel->SetTradeHeadSlotMV(TradeHeadSlotMV);
}

void UTradeMainWidget::OnOffButtonClicked()
{
    if (PlayerStatusViewModel)
    {
        PlayerStatusViewModel->OnOffTraderWindow(false);
    }
}

void UTradeMainWidget::OnGetButtonClicked()
{
    if (TradeViewModel)
    {
        int32 CostGold = TradeHeadSlotMV->GetCostInt();
		int32 PlayerGold = CachedPlayPCComponent->GetPCCPlayerStateRef()->GetGold();

        if (PlayerGold >= CostGold)
        {   
            //CachedPlayPCComponent->GetPCCPlayerStateRef()->Server_AddGold(-CostGold);
            TradeViewModel->GetInventoryPCComponent()->Server_AddItemWithCost(TradeHeadSlotMV->ItemName, 1, CostGold);
        }
    }
}

