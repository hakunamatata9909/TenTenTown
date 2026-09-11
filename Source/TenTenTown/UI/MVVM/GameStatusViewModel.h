#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Engine/TimerHandle.h"
#include "UI/Map/MiniMapCamera.h"
#include "UI/MVVM/MapIconViewModel.h"
#include "GameStatusViewModel.generated.h"


class ATTTGameStateBase;
class UAbilitySystemComponent;
class APlayerState;
class UUserWidget;
class UPingViewModel;

UCLASS()
class TENTENTOWN_API UGameStatusViewModel : public UBaseViewModel
{
    GENERATED_BODY()

public:
    UGameStatusViewModel();
    virtual void InitializeViewModel() override;
    
    void InitializeViewModel(ATTTGameStateBase* GameState, UAbilitySystemComponent* ASC);

    
    void CleanupViewModel();

    
    UFUNCTION()
    void OnWaveTimerChanged(int32 NewRemainingTime);
    UFUNCTION()
    void OnWaveLevelChanged(int32 NewWaveLevel);
    UFUNCTION()
    void OnRemainEnemyChanged(int32 NewRemainEnemy);
    UFUNCTION()
    void UpdateCoreHealthUI(float NewHealth, float NewMaxHealth);

    
    // 코어 체력
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 CoreHealth = 10;

    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    float CoreHealthPer = 1.0f;

    // 웨이브 레벨
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 WaveLevel = 1;

    // 남은 시간 (타이머)
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    FText RemainingTimeText;

    // 남은 적 수
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 RemainEnemy = 0;

protected:
    UPROPERTY()
    TObjectPtr<ATTTGameStateBase> CachedGameState;

    UFUNCTION()
    int32 GetCoreHealth() const { return CoreHealth; }
    UFUNCTION()
    void SetCoreHealth(int32 NewValue);

    UFUNCTION()
    float GetCoreHealthPer() const { return CoreHealthPer; }
    UFUNCTION()
    void SetCoreHealthPer(float NewValue);

    UFUNCTION()
    int32 GetWaveLevel() const { return WaveLevel; }
    UFUNCTION()
    void SetWaveLevel(int32 NewValue);

    UFUNCTION()
    FText GetRemainingTimeText() const { return RemainingTimeText; }
    UFUNCTION()
    void SetRemainingTimeText(FText NewText);

    UFUNCTION()
    int32 GetRemainEnemy() const { return RemainEnemy; }
    UFUNCTION()
    void SetRemainEnemy(int32 NewValue);
        
    FText FormatTime(int32 TimeInSeconds) const;


	// --- 미니맵 ---
protected:
    UPROPERTY()
    TArray<TObjectPtr<UMapIconViewModel>> MapIconVMs;
    UPROPERTY()
    TMap<APlayerState*, UMapIconViewModel*> PlayerIconMap;

    UMapIconViewModel* GetAvailableVM();

    UPROPERTY(Transient, BlueprintReadOnly, Category = "MVVM")
    TObjectPtr<AMiniMapCamera> CachedMinimapCamera;

    UPROPERTY()
    FTimerHandle MinimapUpdateTimer;

public:
    void SetMinimapCamera(AMiniMapCamera* InCamera);
    
	void CreateMapIconVMs(int32 CreateCount);

    
	void StartMinimapUpdate();

    void SetMapIconVMs();

    const TArray<TObjectPtr<UMapIconViewModel>>& GetMapIconVMs() const
    {
        return MapIconVMs;
    }

    void InitializeIconVM(UMapIconViewModel* VM, APlayerState* PS);

    // 미니맵 핑

public:
    FVector2D CreatePingVM(APlayerState* TargetPS);
  
};