#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "EnemyHealthBarWidget.generated.h"


UCLASS()
class TENTENTOWN_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthProgressBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthText;

public:
    UFUNCTION(BlueprintCallable, Category = "Health UI")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
	
};
