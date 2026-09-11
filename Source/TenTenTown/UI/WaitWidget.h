#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitWidget.generated.h"

class UTextBlock;
class UImage;
UCLASS()
class TENTENTOWN_API UWaitWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaitTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText3;

	UPROPERTY(meta = (BindWidget))
	UImage* HeadImage1;
	UPROPERTY(meta = (BindWidget))
	UImage* HeadImage2;
	UPROPERTY(meta = (BindWidget))
	UImage* HeadImage3;

	UPROPERTY(meta = (BindWidget))
	UImage* ReadyImage1;
	UPROPERTY(meta = (BindWidget))
	UImage* ReadyImage2;
	UPROPERTY(meta = (BindWidget))
	UImage* ReadyImage3;

public:
	void HideWidget();
	void ShowWidget();

public:	
	void SetWaitTime(FText NewText);
	void SetNameText(int32 IndexNum, FText NewText);
	void SetHeadImage(int32 IndexNum, UTexture2D* NewTexture);
	void SetReadyImage(int32 IndexNum, UTexture2D* NewTexture);


	//---
	void UpdateCountdownText(int32 NewReadyCount);
};
