// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScrollWidget.generated.h"

class USlotWidget;
UCLASS()
class TENTENTOWN_API UScrollWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ItemListScrollBox;
        
    UPROPERTY(EditAnywhere, Category = "Item Widgets")
    TSubclassOf<USlotWidget> ItemWidgetClass;

public:    
    void MoveScrollBox(float Delta);
    
    void AddNewItemToPanel(FText ItemName);
    void RemoveItemFromPanel(int32 IndexToRemove);

    USlotWidget* GetAddSlot();
	void HandleSlotClicked(FText SlotObjectName);
private:
    //积己等 困连甸阑 包府且 硅凯
    TArray<USlotWidget*> ActiveItemWidgets;

public:
    USlotWidget* GetSlot(int32 SlotIndex);
    
};
