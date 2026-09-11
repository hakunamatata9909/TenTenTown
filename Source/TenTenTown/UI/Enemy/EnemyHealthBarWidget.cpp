#include "UI/Enemy/EnemyHealthBarWidget.h"

void UEnemyHealthBarWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    float HealthPercent = 0.0f;
    if (MaxHealth > 0)
    {
        HealthPercent = CurrentHealth / MaxHealth;
    }

    // 프로그레스 바 업데이트
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(HealthPercent);
    }

    // 텍스트 업데이트
    if (HealthText)
    {
        FText HealthTextValue = FText::Format(
            NSLOCTEXT("HealthUI", "HealthFormat", "{0}/{1}"),
            FText::AsNumber(FMath::CeilToInt(CurrentHealth)), // 정수로 반올림
            FText::AsNumber(FMath::CeilToInt(MaxHealth))
        );
        HealthText->SetText(HealthTextValue);
    }
}