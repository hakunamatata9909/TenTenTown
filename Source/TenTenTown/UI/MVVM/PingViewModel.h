#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Components/SlateWrapperTypes.h"
#include "PingViewModel.generated.h"


UCLASS()
class TENTENTOWN_API UPingViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ESlateVisibility bIsVisible = ESlateVisibility::Collapsed;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FVector2D IconPosition = FVector2D::ZeroVector;

public:
	void SetbIsVisible(ESlateVisibility InVisible);
	void SetIconPosition(const FVector2D& InPosition);
};
