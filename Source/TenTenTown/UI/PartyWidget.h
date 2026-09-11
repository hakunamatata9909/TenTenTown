#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/PartyStatusViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PartyWidget.generated.h"


UCLASS()
class TENTENTOWN_API UPartyWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	// ⭐ 오버라이드 함수 선언
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;

public:
	/*void SetTargetItemImage(UTexture2D* HeadTexture);
	void SetHealthBar(float HealthPer);
	void SetNameText(FText NewText);*/

	UPROPERTY(BlueprintReadOnly, Category = "MVVM|Context")
	TObjectPtr<class UPartyStatusViewModel> PartyStatusViewModel; // TSubclassOf -> TObjectPtr<class U...* > 로 변경


	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "List View Entry")
	TSubclassOf<class UPartyStatusViewModel> ImplementedViewModelType;*/
};
