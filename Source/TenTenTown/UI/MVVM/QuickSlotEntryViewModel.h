#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "UObject/WeakObjectPtr.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "Structure/Data/StructureData.h"
#include "Components/SlateWrapperTypes.h"
#include "QuickSlotEntryViewModel.generated.h"


class ATTTPlayerState;
class UTexture2D;

UCLASS()
class TENTENTOWN_API UQuickSlotEntryViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
public:    
    void InitializeViewModel();

    void CleanupViewModel();

    // 현재 설치 개수 / 최대 설치 개수
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CountText;

    // 설치 비용
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CostText;

    // 아이템 아이콘
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    TObjectPtr<UTexture2D> IconTexture;

    // 현재 보유 재화로 설치 가능한지 여부..?
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    bool bCanAfford = false;

    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	ESlateVisibility bIsEmptySlot = ESlateVisibility::Collapsed;


protected:
    TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
    

public:
    int32 SlotIndex = INDEX_NONE;
    int32 GetSlotIndex() const { return SlotIndex; }



private:
    // FieldNotify Setter 함수 정의
    void SetCountText(const FText& NewValue);
    void SetCostText(const FText& NewValue);
    void SetIconTexture(UTexture2D* NewValue);
    void SetCanAfford(bool bNewValue);
	void SetIsEmptySlot(ESlateVisibility NewValue);

	void OnSetCountTextItem(const FItemData& NewItemData);
    void OnSetCountText(const FStructureData& NewItemData);

    int32 CurrentCountText;
    int32 MaxCountText;

public:
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    int32 SlotNumber = 0; // 초기값 0-> 이 후 고정(VM에서 수행)

    void SetSlotNumber(int32 NewNumber);


    UPROPERTY()
	FName SlotItemRowName = NAME_None;

    void SetSlotItem(const FItemData& NewItemData, const FName& RowName);
    void SetSlotStructure(const FStructureData& NewItemData, const FName& RowName);


public:
    void UpdateItemDataItem(const TArray<FItemInstance>& NewItemData);
    void UpdateItemData(const TArray<FInventoryItemData>& NewItemData);
    void ClearItemData();

    void BroadcastAllFieldValues();

};
