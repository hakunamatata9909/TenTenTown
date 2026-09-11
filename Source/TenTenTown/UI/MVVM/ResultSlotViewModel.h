// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "GameSystem/Player/TTTPlayerController.h"
//#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "ResultSlotViewModel.generated.h"


UCLASS()
class TENTENTOWN_API UResultSlotViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TObjectPtr<ATTTPlayerController> CachedPlayerController;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText PlayerName;
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText KillCountText;
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText ScoreText;
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<UTexture2D> IconTexture;

public:
	void InitializeSlot(ATTTPlayerController* InPlayerController, const FPlayerResultData& InPlayerResult);

	void ReCharge();
};
