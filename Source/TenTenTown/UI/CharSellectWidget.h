#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharSellectWidget.generated.h"

class UButton;
class ULobbyViewModel;

UCLASS()
class TENTENTOWN_API UCharSellectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* WarriorButton;
	UPROPERTY(meta = (BindWidget))
	UButton* MageButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ArcherButton;
	UPROPERTY(meta = (BindWidget))
	UButton* RogueButton;
	/*UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;*/

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<ULobbyViewModel> LobbyViewModel;

public:
	UFUNCTION()
	void OnWarriorButtonClicked();
	UFUNCTION()
	void OnMageButtonClicked();
	UFUNCTION()
	void OnArcherButtonClicked();
	UFUNCTION()
	void OnRogueButtonClicked();
	/*UFUNCTION()
	void OnConfirmButtonClicked();*/

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetViewModel(ULobbyViewModel* InViewModel);
};