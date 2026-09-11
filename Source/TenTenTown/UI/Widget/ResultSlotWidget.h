#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UI/MVVM/ResultSlotViewModel.h"
#include "ResultSlotWidget.generated.h"


UCLASS()
class TENTENTOWN_API UResultSlotWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UResultSlotViewModel> ResultSlotViewModel;

public:
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetResultSlotViewModel(UResultSlotViewModel* InViewModel);
};
