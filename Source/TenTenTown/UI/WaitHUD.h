#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WaitHUD.generated.h"


UCLASS()
class TENTENTOWN_API AWaitHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UStartWidget> StartWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UCharSellectWidget> CharSellectWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UWaitWidget> WaitWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widgets")
	class UStartWidget* StartWidgetInstance;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widgets")
	class UCharSellectWidget* CharSellectWidgetInstance;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widgets")
	class UWaitWidget* WaitWidgetInstance;


	//아래와 같은 느낌으로 페이즈에 따라 위젯 전환
	//void HandlePhaseChanged(ETTTGamePhase NewPhase);
};
