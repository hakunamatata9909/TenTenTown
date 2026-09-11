// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/PlayPCComponent.h"
#include "Item/Data/ItemInstance.h"
#include "TradeSlotViewModel.generated.h"



UCLASS()
class TENTENTOWN_API UTradeSlotViewModel : public UBaseViewModel
{
	GENERATED_BODY()

protected:
    virtual void InitializeViewModel() override;
	TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
    
    UPROPERTY()
    TObjectPtr<UPlayPCComponent> CachedPlayPCComponent;

    // 소유 / 최대
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CountText;

    // 구매 비용
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CostText;
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    int32 CostInt;

    // 아이템 아이콘
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    TObjectPtr<UTexture2D> IconTexture;

    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText ItemDesText;

    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText ItemNameText;

    int32 CurrentCountText;
    int32 MaxCountText;

    // 현재 보유 재화로 설치 가능한지 여부..?
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    bool bCanAfford = false;

public:
    UPROPERTY()
    FName ItemName;

	void SetSlotItem(const FItemData& NewItemData, const FName& RowName);
	void OnSetCountText(const FItemData& NewItemData);

    void SetCountText(const FText& NewValue);
    void SetCostText(int32 NewValue);
    void SetIconTexture(UTexture2D* NewValue);
    void SetItemDesText(const FText& NewValue);
    void SetItemName(const FText& NewValue);
	
    void BroadcastAllFieldValues();
    void SetBindingDelegate();

    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetUpPlayPCC(UPlayPCComponent* CachedPlayPCC);

    void SendHeadSlot();
    void UpdateCurrentCount(const TArray<FItemInstance>& InventoryItems);
    
	int32 GetCostInt() const { return CostInt; }

	int32 GetCurrentCountText() const { return CurrentCountText; }
	int32 GetMaxCountText() const { return MaxCountText; }
};
