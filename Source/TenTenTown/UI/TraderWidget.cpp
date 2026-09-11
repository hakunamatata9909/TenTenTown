#include "UI/TraderWidget.h"
#include "Components/Button.h"
#include "PlayHUD.h"
#include "Structure/Data/StructureData.h"
#include "Item/Data/ItemData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/TradeMainWidget.h"


void UTraderWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
void UTraderWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}


void UTraderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &UTraderWidget::OnBuyButtonClicked);
	}
	if (ScrollFront)
	{
		ScrollFront->OnClicked.AddDynamic(this, &UTraderWidget::OnScrollFrontClicked);
	}
	if (ScrollBack)
	{
		ScrollBack->OnClicked.AddDynamic(this, &UTraderWidget::OnScrollBackClicked);
	}
}

UScrollWidget* UTraderWidget::GetScrollWidgets()
{
	return ScrollWidget01;
}

void UTraderWidget::AddSlotToScroll(FName ItemName)
{
	ScrollWidget01->AddNewItemToPanel(FText::FromName(ItemName));
}


void UTraderWidget::SetItemNameText(FName ItemName)
{
}

void UTraderWidget::SetItemDesText(FName ItemDes)
{
}

void UTraderWidget::SetItemPriceText(int32 ItemPrice)
{

}

void UTraderWidget::SetHeadSlot(USlotWidget* slots)
{
	//��������
}

void UTraderWidget::OnBuyButtonClicked()
{
	////��ư ����
	//BuyButton->SetIsEnabled(false);

	////�÷��̾� ������Ʈ�� ������Ʈ -> ��ȭ�� �������̳� �������� �ܰ� ����
	//APlayerController* PlayerController = GetOwningPlayer();
	//if (!PlayerController) return;
	//APlayHUD* PlayHUDRef = Cast<APlayHUD>(PlayerController->GetHUD());
	//
	//int32 ItemPrice = FCString::Atoi(*ItemPriceText->GetText().ToString());
	//PlayHUDRef->PlayerStateRef->Server_AddGold(-1 * ItemPrice);

	//if (!bIsItem)
	//{
	//	/*FInventoryItemData* PSItem = PlayHUDRef->PlayerStateRef->FindStructureDataByName(ItemNameText->GetText());		
	//	if (PSItem)
	//	{	
	//		FInventoryItemData NewItemData = *PSItem;
	//		NewItemData.Level += 1;
	//		PlayHUDRef->PlayerStateRef->Server_UpdateStructureData(NewItemData);
	//	}*/
	//}
	//else
	//{
	//	/*FInventoryItemData* PSItem = PlayHUDRef->PlayerStateRef->FindItemDataByName(ItemNameText->GetText());
	//	if (PSItem)
	//	{
	//		FInventoryItemData NewItemData = *PSItem;
	//		NewItemData.Count += 1;
	//		PlayHUDRef->PlayerStateRef->Server_UpdateItemData(NewItemData);
	//	}*/
	//}

	//BuyButton->SetIsEnabled(true);
}

void UTraderWidget::BuyButtonControl(bool bIsEnableds)
{

}

void UTraderWidget::OnScrollFrontClicked()
{
	ScrollWidget01->MoveScrollBox(-100.f);
}

void UTraderWidget::OnScrollBackClicked()
{
	ScrollWidget01->MoveScrollBox(100.f);
}


void UTraderWidget::ChangeHeadSlot(FText SlotName)
{
	//BuyButton->SetIsEnabled(false);
	//APlayerController* PlayerController = GetOwningPlayer();

	//if (!PlayerController) return;
	//APlayHUD* PlayHUDRef = Cast<APlayHUD>(PlayerController->GetHUD());
	//
	//int32 PlayerMoney;
	//UObject* OuterObject = GetOuter();
	//if (UTradeMainWidget* TradeMainWidget = Cast<UTradeMainWidget>(OuterObject))
	//{
	//	//FString StringValue = TradeMainWidget->GetMoneyText()->GetText().ToString();
	//	/*if (StringValue.IsNumeric())
	//	{
	//		PlayerMoney = FCString::Atoi(*StringValue);
	//	}
	//	else
	//	{
	//		PlayerMoney = 0;
	//	}*/
	//}


	//if (!bIsItem)
	//{
	//	UDataTable* StructureDataTable = PlayHUDRef->StructureDataTable;
	//	TArray<FStructureData*> RowArray;
	//	StructureDataTable->GetAllRows<FStructureData>(TEXT("Find Structure by Name"), RowArray);
	//	const FStructureData* FoundRow = nullptr;

	//	for (const FStructureData* Row : RowArray)
	//	{
	//		if (Row && SlotName.EqualTo(Row->DisplayName))
	//		{
	//			FoundRow = Row;
	//			break;
	//		}
	//	}
	//	if (FoundRow)
	//	{
	//		/*UTexture2D* LoadedImage = FoundRow->StructureImage.LoadSynchronous();
	//		TargetItemImage->SetBrushFromTexture(LoadedImage);

	//		ItemNameText->SetText(FoundRow->StructureName);
	//		ItemDesText->SetText(FoundRow->Description);

	//		int32 FindPs = PlayHUDRef->PlayerStateRef->FindItemDataByName(SlotName)->Level;
	//		ItemPriceText->SetText(FText::AsNumber(FoundRow->UpgradeCosts[FindPs]));
	//					
	//		if (PlayerMoney >= FoundRow->UpgradeCosts[FindPs])
	//		{
	//			BuyButton->SetIsEnabled(true);
	//		}*/
	//		
	//	}
	//}
	//else
	//{
	//	/*UDataTable* ItemDataTable = PlayHUDRef->ItemDataTable;
	//	TArray<FItemData*> RowArray;
	//	ItemDataTable->GetAllRows<FItemData>(TEXT("Find Structure by Name"), RowArray);
	//	const FItemData* FoundRow = nullptr;

	//	for (const FItemData* Row : RowArray)
	//	{
	//		if (Row && SlotName.EqualTo(Row->ItemName))
	//		{
	//			FoundRow = Row;
	//			break;
	//		}
	//	}
	//	if (FoundRow)
	//	{
	//		UTexture2D* LoadedImage = FoundRow->ItemImage.LoadSynchronous();
	//		TargetItemImage->SetBrushFromTexture(LoadedImage);

	//		ItemNameText->SetText(FoundRow->ItemName);
	//		ItemDesText->SetText(FoundRow->Description);

	//		ItemPriceText->SetText(FText::AsNumber(FoundRow->SellPrice));

	//		if (PlayerMoney >= FoundRow->SellPrice)
	//		{
	//			BuyButton->SetIsEnabled(true);
	//		}
	//	}*/
	//}
	
}

