// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "ResultWidget.generated.h"

class UButton;

UCLASS()
class TENTENTOWN_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UListView* ResultListView;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<ULobbyViewModel> LobbyViewModel;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void OnRestartButtonClicked();

public:
	void SetLobbyViewModel(ULobbyViewModel* InViewModel);
	void SetResultListView();

	
	
};
