#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildGuideWidget.generated.h"

class UTextBlock;
class UWidget;

UCLASS()
class TENTENTOWN_API UBuildGuideWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> UpgradeInfoBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SellInfoBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeCostText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SellReturnText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RepairKitCountText;

	void UpdateStructureStats(bool bHasTarget, int32 UpgradeCost, int32 SellAmount, int32 RemainingKits);
};
