#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/MapIconViewModel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "MapIconWidget.generated.h"


UCLASS()
class TENTENTOWN_API UMapIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UMapIconViewModel> MapIconViewModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UCanvasPanelSlot> CachedCanvasSlot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UCanvasPanel> CachedCanvas;



public:
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetMapIconViewModel(UMapIconViewModel* NewMapIconViewModel);

	void SetCachedCanvas(UCanvasPanel* InCanvas);
};
