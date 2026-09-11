#include "Structure/BuildSystem/BuildGuideWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void UBuildGuideWidget::UpdateStructureStats(bool bHasTarget, int32 UpgradeCost, int32 SellAmount, int32 RemainingKits)
{
	if (bHasTarget)
	{
		if (UpgradeCost > 0)
		{
			UpgradeInfoBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			UpgradeCostText->SetText(FText::FromString(FString::Printf(TEXT("(%dG)"), UpgradeCost)));
		}
		else
		{
			UpgradeInfoBox->SetVisibility(ESlateVisibility::Collapsed);
		}

		SellInfoBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		SellReturnText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SellAmount)));
	}
	else
	{
		// 타겟이 없으면 정보 박스 숨기기
		UpgradeInfoBox->SetVisibility(ESlateVisibility::Collapsed);
		SellInfoBox->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 수리 키트는 건물 유무와 상관없이 표시 (요구사항에 따라 조절 가능)
	RepairKitCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), RemainingKits)));
}