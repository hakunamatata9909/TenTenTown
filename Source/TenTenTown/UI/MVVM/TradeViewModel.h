#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "UI/MVVM/TradeSlotViewModel.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "TradeViewModel.generated.h"

UCLASS()
class TENTENTOWN_API UTradeViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	virtual void InitializeViewModel() override;


	UPROPERTY()
	TObjectPtr<UInventoryPCComponent> CachedInventory;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	TArray<TObjectPtr<UTradeSlotViewModel>> TradeSlotEntryVMs;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	bool CanTrade = false;

	UPROPERTY()
	TObjectPtr<UPlayPCComponent> CachedPlayPCComponent;
	UPROPERTY()
	TObjectPtr<UTradeSlotViewModel> TradeHeadSlotMV;


public:
	UInventoryPCComponent* GetInventoryPCComponent() const;

	void InitializeViewModel(UPlayPCComponent* CachedPlayPCC, ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI);
	void CreateTradeSlotEntries(UTTTGameInstance* TTGI);
	//void TradeSlotListBased(FItemData& NewItemData);
	void OnTradeSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList);

	UFUNCTION()
	TArray<UTradeSlotViewModel*> GetPartyMembers() const;

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void CallSlotDelegate();

	UFUNCTION()
	void OnInventoryUpdated(const TArray<FItemInstance>& NewItems);

	UFUNCTION()
	void SetTradeHeadSlotMV(UTradeSlotViewModel* NewTradeHeadSlotMV);

	UFUNCTION()
	void SetCanTrade(int32 golds);

	
};
