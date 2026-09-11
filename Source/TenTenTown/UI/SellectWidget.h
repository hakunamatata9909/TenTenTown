#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SellectWidget.generated.h"

class UTextBlock;
class UButton;
UCLASS()
class TENTENTOWN_API USellectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeadText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DesText;
	UPROPERTY(meta = (BindWidget))
	UButton* SellectButton;
	UPROPERTY(meta = (BindWidget))
	UButton* FrontButton;
	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	virtual void NativeConstruct() override;
public:
	void SetHeadText(FText NewText);
	void SetDesText(FText NewText);

private:
	UFUNCTION()
	void OnSellectButtonClicked();
	UFUNCTION()
	void OnFrontButtonClicked();
	UFUNCTION()
	void OnBackButtonClicked();
};
