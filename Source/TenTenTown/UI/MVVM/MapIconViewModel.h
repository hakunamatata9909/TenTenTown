#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Components/SlateWrapperTypes.h"
#include "MapIconViewModel.generated.h"


class APlayerState;

UCLASS(BlueprintType)
class TENTENTOWN_API UMapIconViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility bIsVisible = ESlateVisibility::Collapsed;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FVector2D IconPosition = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TObjectPtr<class UTexture2D> IconTexture;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility bIsMyPlayerIcon = ESlateVisibility::Collapsed;

	

public:
	void SetbIsVisible(ESlateVisibility InVisible);
	void SetIconPosition(const FVector2D& InPosition);
	void SetIconTexture(UTexture2D* InTexture);
	void SetbIsMyPlayerIcon(ESlateVisibility InIsMyPlayerIcon);



	bool bIsBusy = false;

	UPROPERTY()
	APlayerState* TargetPS = nullptr;
	

	UFUNCTION(BlueprintPure)
	FVector2D GetIconPosition() const { return IconPosition; }

	UFUNCTION(BlueprintPure)
	ESlateVisibility GetbIsVisible() const { return bIsVisible; }

};
