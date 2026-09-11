#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotBarWidget.generated.h"


class UQuickSlotManagerViewModel;
class UQuickSlotEntryWidget;

UCLASS()
class TENTENTOWN_API UQuickSlotBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* ManagerVM, bool bIsItem);

protected:
    UPROPERTY(EditAnywhere, Category = "MVVM")
	bool bIsItemSlotsBar = false;


    UPROPERTY(BlueprintReadOnly, Category = "MVVM")
    TObjectPtr<UQuickSlotManagerViewModel> ManagerViewModel;

    // UMG에 미리 배치된 9개의 QuickSlotEntryWidget을 바인딩할 변수
    // UMG 블루프린트에서 Is Variable 체크 후 여기에 할당해야 합니다.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_0;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_2;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_3;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_4;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_5;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_6;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_7;    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_8;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_9;

    virtual void NativeConstruct() override;

public:
    // 이 위젯이 사용할 매니저 ViewModel 포인터 (주입받음)
    UPROPERTY()
    TObjectPtr<UQuickSlotManagerViewModel> QuickSlotManagerViewModel;

    // 9개의 개별 위젯을 모아 관리하기 위한 배열
    TArray<TObjectPtr<UQuickSlotEntryWidget>> QuickSlotEntryWidgets;

    // ViewModel과 위젯을 연결하는 실제 로직
	void BindEntryViewModelsItem();
    void BindEntryViewModels();


};
