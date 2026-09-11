#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ReadyHUD.generated.h"


UCLASS()
class TENTENTOWN_API AReadyHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	//호출할 위젯 블루프린트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> ReadyWidgetClass;
	//호출 함수
	virtual void BeginPlay() override;
};
