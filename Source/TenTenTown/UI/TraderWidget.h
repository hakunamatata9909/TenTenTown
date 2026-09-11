#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ScrollWidget.h"
#include "TraderWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
UCLASS()
class TENTENTOWN_API UTraderWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	bool bIsItem;

public:
	void HideWidget();
	void ShowWidget();

protected:
	UPROPERTY(meta = (BindWidget))
	UScrollWidget* ScrollWidget01;

	UPROPERTY(meta = (BindWidget))
	UImage* TargetItemImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDesText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPriceText;
	UPROPERTY(meta = (BindWidget))
	UButton* BuyButton;
public:
	UScrollWidget* GetScrollWidgets();


	void AddSlotToScroll(FName ItemName);	//이게 필요한지 확인할 것

	void SetTargetItemImage(UTexture2D* ItemTexture);
	void SetItemNameText(FName ItemName);
	void SetItemDesText(FName ItemDes);
	void SetItemPriceText(int32 ItemPrice);

	void SetHeadSlot(USlotWidget* slots);

	UFUNCTION()
	void OnBuyButtonClicked();
	void BuyButtonControl(bool bIsEnableds);


	//스크롤 버튼
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ScrollFront;
	UPROPERTY(meta = (BindWidget))
	UButton* ScrollBack;
	
public:
	UFUNCTION()
	void OnScrollFrontClicked();
	UFUNCTION()
	void OnScrollBackClicked();
	
	void ChangeHeadSlot(FText SlotName);

	
};
