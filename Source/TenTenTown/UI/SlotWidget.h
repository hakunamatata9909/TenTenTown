#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/TradeSlotViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "SlotWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotItemClicked, FText, ItemName);

UCLASS()
class TENTENTOWN_API USlotWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UTradeSlotViewModel> EntryViewModel;
	
	//데이터 저장용 이름..
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FText DataName;

	UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SlotButton;
	UFUNCTION()
	void OnButtonClicked();


public:
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetEntryViewModel(UTradeSlotViewModel* InViewModel);

	
	
};
