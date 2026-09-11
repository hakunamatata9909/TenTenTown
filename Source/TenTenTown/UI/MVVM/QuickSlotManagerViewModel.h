#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "QuickSlotManagerViewModel.generated.h"


class UQuickSlotEntryViewModel;


UCLASS()
class TENTENTOWN_API UQuickSlotManagerViewModel : public UBaseViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UInventoryPCComponent> CachedInventory;

	UPROPERTY(EditAnywhere, Category = "QuickSlot")
	int32 SlotCountBase = 10;

public:
	
	void InitializeViewModel(ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI);
	virtual void InitializeViewModel() override;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	TArray<TObjectPtr<UQuickSlotEntryViewModel>> QuickSlotEntryVMsItem;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	TArray<TObjectPtr<UQuickSlotEntryViewModel>> QuickSlotEntryVMs;

	

	//UFUNCTION(BlueprintPure, Category = "QuickSlot")
	const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& GetQuickSlotEntryVMsItem() const
	{
		return QuickSlotEntryVMsItem;
	}
	const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& GetQuickSlotEntryVMs() const
	{
		return QuickSlotEntryVMs;
	}
	

private:
	void CreateQuickSlotEntriesItem(UTTTGameInstance* TTGI);
	UFUNCTION()
	void OnInventoryUpdatedItem(const TArray<FItemInstance>& NewItems);


	void CreateQuickSlotEntries(UTTTGameInstance* TTGI);
	UFUNCTION()
	void OnInventoryUpdated(const TArray<FInventoryItemData>& NewItems);
};
