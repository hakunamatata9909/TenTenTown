#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PingWidget.generated.h"


class UPlayPCComponent;

UCLASS()
class TENTENTOWN_API UPingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UButton* PingButtonA;
	UPROPERTY(meta = (BindWidget))
	UButton* PingButtonB;
	UPROPERTY(meta = (BindWidget))
	UButton* PingButtonC;
	UPROPERTY(meta = (BindWidget))
	UButton* PingButtonD;



	TObjectPtr<UPlayPCComponent> PCC;

	UFUNCTION()
	void ClearHoveredTypes();
	UFUNCTION()
	void SetHoveredTypeA();
	UFUNCTION()
	void SetHoveredTypeB();
	UFUNCTION()
	void SetHoveredTypeC();
	UFUNCTION()
	void SetHoveredTypeD();





public:
	
	void SetPCC(UPlayPCComponent* InPCC);
};
