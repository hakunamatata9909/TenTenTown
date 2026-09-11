// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapSelectWidget.generated.h"

class UButton;
class ULobbyViewModel;

UCLASS()
class TENTENTOWN_API UMapSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* MapButton0;
	UPROPERTY(meta = (BindWidget))
	UButton* MapButton1;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<ULobbyViewModel> LobbyViewModel;

public:
	UFUNCTION()
	void OnMapButton0();
	UFUNCTION()
	void OnMapButton1();

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetViewModel(ULobbyViewModel* InViewModel);
};
