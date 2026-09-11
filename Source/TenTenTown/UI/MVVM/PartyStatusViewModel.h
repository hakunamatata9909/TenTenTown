#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "AbilitySystemComponent.h"
#include "PartyStatusViewModel.generated.h"

class ATTTPlayerState;
struct FOnAttributeChangeData;

UCLASS()
class TENTENTOWN_API UPartyStatusViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
public:
    UPartyStatusViewModel();
    virtual void InitializeViewModel() override;
    // UPartyManagerViewModel에서 호출하여 초기화 및 GAS 구독을 설정하는 함수
    void InitializeViewModel(ATTTPlayerState* PartyPlayerState);
    void CleanupViewModel();

    // -----------------------------------------------------
    // UI 바인딩 속성
    // -----------------------------------------------------

    // 이름
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    FText NameText = FText::GetEmpty();

    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    TObjectPtr<UTexture2D> IconTexture;
    // 체력 비율 (0.0 ~ 1.0)
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    float HealthPercentage = 1.0f;

    // 헤드 이미지 (UTexture2D* 바인딩은 UMG에서 복잡하므로 FSoftObjectPath 권장)
    // 여기서는 간단히 UTexture2D*로 가정합니다.
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    UTexture2D* HeadTexture = nullptr;

protected:
    UPROPERTY()
    TObjectPtr<ATTTPlayerState> CachedPlayerState;

    // -----------------------------------------------------
    // GAS 콜백 함수 (데이터 수신)
    // -----------------------------------------------------

    void OnHealthChanged(const FOnAttributeChangeData& Data);
    void OnMaxHealthChanged(const FOnAttributeChangeData& Data);


//public:
    // --- Getter & Setter 구현 (FieldNotify용) ---
    UFUNCTION()
    UTexture2D* GetIconTexture() const { return IconTexture; }
    UFUNCTION()
    void SetIconTexture(UTexture2D* InTexture);


    UFUNCTION(BlueprintPure, Category = "MVVM")
    FText GetNameText() const { return NameText; }

    // Setter: 상태를 변경하므로 BlueprintPure 제거 또는 BlueprintCallable 사용 (수정됨)
    UFUNCTION(Category = "MVVM") // 또는 UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetNameText(FText NewText);

    // Getter: 상태를 변경하지 않으므로 BlueprintPure 사용 (OK)
    UFUNCTION(BlueprintPure, Category = "MVVM")
    float GetHealthPercentage() const { return HealthPercentage; }

    // Setter: 상태를 변경하므로 BlueprintPure 제거 또는 BlueprintCallable 사용 (수정됨)
    UFUNCTION(Category = "MVVM") // 또는 UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetHealthPercentage(float NewValue);

    // Getter: 상태를 변경하지 않으므로 BlueprintPure 사용 (OK)
    UFUNCTION(BlueprintPure, Category = "MVVM")
    UTexture2D* GetHeadTexture() const { return HeadTexture; }

    // Setter: 상태를 변경하므로 BlueprintPure 제거 또는 BlueprintCallable 사용 (수정됨)
    UFUNCTION(Category = "MVVM") // 또는 UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetHeadTexture(UTexture2D* NewTexture);

    void RecalculateHealthPercentage();
};
