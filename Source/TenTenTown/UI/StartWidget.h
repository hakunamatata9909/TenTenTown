#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "StartWidget.generated.h"


UCLASS()
class TENTENTOWN_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;
	virtual void NativeOnInitialized() override;

public:
	void HideWidget();
	void ShowWidget();

private:
	UFUNCTION()
	void OnMyButtonClicked();
};
