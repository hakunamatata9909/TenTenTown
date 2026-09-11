//#include "UI/PlayHUD.h"
//#include "UI/PlayWidget.h"
//#include "UI/TradeMainWidget.h"
//#include "Engine/World.h"
//#include "TimerManager.h"
//#include "GameSystem/GameMode/TTTGameStateBase.h"
//#include "AbilitySystemInterface.h"
//#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
//#include "Character/PS/TTTPlayerState.h"
//#include "Structure/Data/StructureData.h"
//#include "Item/Data/ItemData.h"
//
//
//
//
//void APlayHUD::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (PlayWidgetClass)
//	{	
//		PlayWidgetInstance = CreateWidget<UPlayWidget>(GetWorld(), PlayWidgetClass);
//		if (PlayWidgetInstance)
//		{
//			PlayWidgetInstance->AddToViewport();			
//		}
//		else
//		{
//			return;
//		}
//	}
//	if (TradeWidgetClass)
//	{
//		TradeWidgetInstance = CreateWidget<UTradeMainWidget>(GetWorld(), TradeWidgetClass);
//		if (TradeWidgetInstance)
//		{
//			TradeWidgetInstance->AddToViewport();
//			TradeWidgetInstance->HideWidget();
//		}
//		else
//		{
//			return;
//		}
//	}
//
//
//	StartWidgetSetting();
//}
//
//
//
//void APlayHUD::StartWidgetSetting()
//{
//	APlayerController* PC = GetOwningPlayerController();
//    
//	if (PC)
//	{
//		//ASC
//		if (!bSettingArray[0])
//		{
//			IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PC->PlayerState);
//
//			if (ASCInterface)
//			{
//				MyASC = ASCInterface->GetAbilitySystemComponent();
//
//				if (MyASC)
//				{
//					bSettingArray[0] = true;
//					SettingCount++;
//					SetBindDelegates();
//				}
//			}
//		}
//		//���� ������Ʈ
//		if (!bSettingArray[1])
//		{
//			GameStateRef = PC->GetWorld()->GetGameState<ATTTGameStateBase>();
//			if (GameStateRef)
//			{
//				GameStateRef->OnPhaseChanged.AddDynamic(this, &APlayHUD::HandlePhaseChanged);
//				GameStateRef->OnRemainingTimeChanged.AddDynamic(this, &APlayHUD::HandleRemainingTimeChanged);
//
//				HandlePhaseChanged(GameStateRef->Phase);
//				HandleRemainingTimeChanged(GameStateRef->RemainingTime);
//
//				bSettingArray[1] = true;
//				SettingCount++;
//			}
//		}		
//		//�÷��̾� ������Ʈ
//		if (!bSettingArray[2])
//		{
//			PlayerStateRef = Cast<ATTTPlayerState>(PC->PlayerState);
//			if (PlayerStateRef)
//			{
//				//��������Ʈ ���ε�
//				/*PlayerStateRef->OnGoldChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerGoldChanged);
//				PlayerStateRef->OnStructureListChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerInventoryStructureChanged);
//				PlayerStateRef->OnItemListChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerInventoryItemChanged);*/
//								
//				//OnPlayerGoldChanged(PlayerStateRef->Gold); // �ʱ� ��� �� �ݿ�
//				// �ʱ� �κ��丮 �ݿ�
//				OnPlayerInventoryStructureChanged();
//				OnPlayerInventoryItemChanged();
//
//				bSettingArray[2] = true;
//				SettingCount++;
//			}
//		}
//	}
//
//
//
//	if (SettingCount < 3)
//	{
//		FTimerHandle RetryTimerHandle;
//		GetWorld()->GetTimerManager().SetTimer(RetryTimerHandle, this, &APlayHUD::StartWidgetSetting, 0.2f, false);
//	}
//}
//
//
//void APlayHUD::SetBindDelegates()
//{
//	if (!MyASC) return;
//
//	const UAS_FighterAttributeSet* FighterAttributes = MyASC->GetSet<UAS_FighterAttributeSet>();
//
//	if (FighterAttributes)
//	{
//		MyASC->GetGameplayAttributeValueChangeDelegate(FighterAttributes->GetHealthAttribute())
//			.AddUObject(this, &APlayHUD::OnHealthChanged);
//
//		MyASC->GetGameplayAttributeValueChangeDelegate(FighterAttributes->GetMaxHealthAttribute())
//			.AddUObject(this, &APlayHUD::OnHealthChanged);
//	}
//}
//
//void APlayHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
//{
//	if (!MyASC) return;
//
//	const UAS_FighterAttributeSet* FighterAttributes = MyASC->GetSet<UAS_FighterAttributeSet>();
//
//	if (FighterAttributes)
//	{
//		float CurrentHealth = FighterAttributes->GetHealth();
//		float MaxHealthValue = FighterAttributes->GetMaxHealth();
//
//		float HealthPercent = (MaxHealthValue > 0.0f)
//			? (CurrentHealth / MaxHealthValue)
//			: 0.0f;
//
//		//PlayWidgetInstance->SetHealthPercent(HealthPercent);
//
//		//�ؽ�Ʈ ��� ���� �ִٸ� ���⼭ ������Ʈ
//		// HealthTextBlock->SetText(FText::Format(LOCTEXT("HealthFormat", "{0}/{1}"), FMath::FloorToInt(CurrentHealth), FMath::FloorToInt(MaxHealthValue)));
//	}
//}
//
//void APlayHUD::HandlePhaseChanged(ETTTGamePhase NewPhase)
//{
//	if (NewPhase == ETTTGamePhase::Waiting)
//	{
//
//	}
//	else if(NewPhase == ETTTGamePhase::Build)
//	{
//
//	}
//	else if (NewPhase == ETTTGamePhase::Combat)
//	{
//		TradeWidgetInstance->HideWidget();
//		PlayWidgetInstance->ShowWidget();
//	}
//	else if (NewPhase == ETTTGamePhase::Reward)
//	{
//
//	}
//	else if (NewPhase == ETTTGamePhase::Victory)
//	{
//		TradeWidgetInstance->HideWidget();
//		PlayWidgetInstance->HideWidget();
//	}
//	else if (NewPhase == ETTTGamePhase::GameOver)
//	{
//		TradeWidgetInstance->HideWidget();
//		PlayWidgetInstance->HideWidget();
//	}
//}
//
//void APlayHUD::HandleRemainingTimeChanged(int32 NewRemainingTime)
//{
//	//PlayWidgetInstance->SetWaveTimer(NewRemainingTime);
//}
//
//
//
//void APlayHUD::OnPlayerGoldChanged(int32 NewGold)
//{
//	if (PlayWidgetInstance)
//	{
//		//PlayWidgetInstance->SetMoneyText(NewGold);
//	}
//	if (TradeWidgetInstance)
//	{
//		TradeWidgetInstance->SetMoneyText(NewGold);
//	}
//}
//
//void APlayHUD::OnPlayerInventoryStructureChanged()
//{
//	if (PlayWidgetInstance && PlayerStateRef)
//	{
//		// PlayWidgetInstance�� �Լ��� ȣ���Ͽ� PlayerStateRef->InventoryList ��ü�� �а� UI�� �籸���մϴ�.
//		// ��: PlayWidgetInstance->RebuildInventoryUI(PlayerStateRef->InventoryList);
//		UE_LOG(LogTemp, Warning, TEXT("HUD: Inventory Changed (Needs Rebuild)"));
//	}
//}
//void APlayHUD::OnPlayerInventoryItemChanged()
//{
//	if (PlayWidgetInstance && PlayerStateRef)
//	{
//		
//	}
//}
//
//void APlayHUD::OpenTradeWidget(bool bIsOpen)
//{
//	if (bIsOpen)
//	{
//		TradeWidgetInstance->ShowWidget();
//		PlayWidgetInstance->HideWidget();
//	}
//	else
//	{
//		TradeWidgetInstance->HideWidget();
//		PlayWidgetInstance->ShowWidget();
//	}
//}
//
////���Ӹ�忡�� ������Ѽ� ��ũ�� ����
//void APlayHUD::SetTradeScroll()
//{
//	
//	//ps�� ������ ���Ӹ�忡 �մ� ���������̺��� �̿��Ͽ� ��ũ�� ����
//	//������ �ӽ÷� ���������̺� ���⼭ ����Ѵ� StructureDataTable  ItemDataTable
//	//�׸��� �������� ������ ��ũ���� �����´�.
//	//add�Լ��� ����ؼ� ���������� ��ġ�Ѵ�.
//
//	APlayerController* PC = GetOwningPlayerController();
//	if (!PlayerStateRef) return;
//	if (!TradeWidgetInstance) return;
//	if (!StructureDataTable) return;
//	
//	//������ ó��
//	TArray<FStructureData*> RowArray;
//	StructureDataTable->GetAllRows<FStructureData>(TEXT("ProcessAllStructureData Context"), RowArray);
//		
//	for (const FStructureData* RowData : RowArray)
//	{
//		if (RowData)
//		{
//			USlotWidget* NewSlot = TradeWidgetInstance->GetTraderWidget(1)->GetScrollWidgets()->GetAddSlot();
//
//			UTexture2D* LoadedTexture = RowData->StructureImage.LoadSynchronous();
//			//int32 PSlevel = PlayerStateRef->FindStructureDataByName(RowData->StructureName)->Level;
//			//FText HeadText;
//			//if (PSlevel == 0)
//			//{
//			//	HeadText = FText::FromString(TEXT("-"));
//			//}
//			//else if (PSlevel >= RowData->MaxUpgradeLevel)
//			//{
//			//	HeadText = FText::FromString(TEXT("Max"));
//			//}
//			//else
//			//{
//			//	FFormatOrderedArguments Args;
//			//	Args.Add(FText::AsNumber(PSlevel));         // ���� ����
//			//	Args.Add(FText::AsNumber(RowData->MaxUpgradeLevel)); // �ִ� ����
//
//			//	HeadText = FText::Format(
//			//		NSLOCTEXT("StructureUI", "LevelProgressFormat", "{0}/{1}"),
//			//		Args
//			//	);
//			//}
//			//int32 NewPrice = (PSlevel < RowData->MaxUpgradeLevel) ? RowData->UpgradeCosts[PSlevel] : -1;
//			
//
//			//NewSlot->SetSlotWidgetData(RowData->StructureName, LoadedTexture, RowData->StructureName, NewPrice, -1);
//		}
//	}
//
//	if (!ItemDataTable) return;
//	
//	//������ ó��
//	TArray<FItemData*> RowArrayItem;
//	ItemDataTable->GetAllRows<FItemData>(TEXT("ProcessAllStructureData Context"), RowArrayItem);
//
//	for (const FItemData* RowData : RowArrayItem)
//	{
//		if (RowData)
//		{
//			USlotWidget* NewSlot = TradeWidgetInstance->GetTraderWidget(2)->GetScrollWidgets()->GetAddSlot();
//
//			UTexture2D* LoadedTexture = RowData->ItemImage.LoadSynchronous();
//			//int32 PSlevel = PlayerStateRef->FindItemDataByName(RowData->ItemName)->Level;
//			//int32 PSCount = PlayerStateRef->FindItemDataByName(RowData->ItemName)->Count;
//
//			//NewSlot->SetSlotWidgetData(RowData->ItemName, LoadedTexture, FText::AsNumber(PSCount), RowData->SellPrice, -1);
//		}
//	}
//
//	//TraderWidget ��ǥ ���� �ʱ� ����
//	//FText FirstNameS = TradeWidgetInstance->GetTraderWidget(1)->GetScrollWidgets()->GetSlot(0)->GetDataName();
//	//FText FirstNameI = TradeWidgetInstance->GetTraderWidget(2)->GetScrollWidgets()->GetSlot(0)->GetDataName();
//
//	//TradeWidgetInstance->GetTraderWidget(1)->ChangeHeadSlot(FirstNameS);
//	//TradeWidgetInstance->GetTraderWidget(2)->ChangeHeadSlot(FirstNameI);
//	
//}
//
