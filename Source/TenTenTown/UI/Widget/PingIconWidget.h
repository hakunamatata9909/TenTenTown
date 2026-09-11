#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/PingViewModel.h"
#include "PingIconWidget.generated.h"


class UCanvasPanel;

UCLASS()
class TENTENTOWN_API UPingIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UCanvasPanel> CachedCanvas;

public:
	void SetCachedCanvas(UCanvasPanel* InCanvas);
	void SetLocation(FVector2D InLocation);


public:
	void StartDestroyTimer();

private:
	FTimerHandle DestroyTimerHandle;

	void HandleDestroy();
	
};
