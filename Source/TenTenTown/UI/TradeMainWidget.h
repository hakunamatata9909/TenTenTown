#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/TraderWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
#include "UI/MVVM/TradeViewModel.h"
#include "UI/MVVM/TradeSlotViewModel.h"
#include "TradeMainWidget.generated.h"

class UButton;
class UTextBlock;
UCLASS()
class TENTENTOWN_API UTradeMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<UPlayPCComponent> CachedPlayPCComponent;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;	//°ñµå ¹ÙÀÎµù

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UTradeViewModel> TradeViewModel;	//Æ®·¹ÀÌµå ¸ÞÀÎ ºä¸ðµ¨
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UTradeSlotViewModel> TradeHeadSlotMV;

	UPROPERTY(meta = (BindWidget))
	UButton* OffTrade;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;


	UPROPERTY(meta = (BindWidget))
	UImage* TargetImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TargetName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TargetDes;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TargetPrice;
	UPROPERTY(meta = (BindWidget))
	UButton* TargetGetButton;

	UPROPERTY(meta = (BindWidget))
	class UListView* TradeListView;
	
public:
	UFUNCTION()
	void SetPCC(UPlayPCComponent* InPCC) { CachedPlayPCComponent = InPCC; }	

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetPlayerStatusViewModel(UPlayerStatusViewModel* ViewModel);
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetTradeViewModel(UTradeViewModel* ViewModel);
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetTradeHeadSlotMV(UTradeSlotViewModel* ViewModel);

	UFUNCTION()
	void OnOffButtonClicked();
	UFUNCTION()
	void OnGetButtonClicked();




};
