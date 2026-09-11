#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "Character/PS/TTTPlayerState.h"
#include "PlayHUD.generated.h"


UCLASS()
class TENTENTOWN_API APlayHUD : public AHUD
{
	GENERATED_BODY()
	

//protected:
//	virtual void BeginPlay() override;
//
//	
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
//	TSubclassOf<class UPlayWidget> PlayWidgetClass;		
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
//	TSubclassOf<class UTradeMainWidget> TradeWidgetClass;
//	
//	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widgets")
//	class UPlayWidget* PlayWidgetInstance;
//	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widgets")
//	class UTradeMainWidget* TradeWidgetInstance;
//
//public:
//
//	int32 SettingCount = 0;
//	TArray<bool> bSettingArray = { false, false, false, false };
//
//	UPROPERTY()
//	class ATTTGameStateBase* GameStateRef;
//
//	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
//	class ATTTPlayerState* PlayerStateRef;
//
//	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<UAbilitySystemComponent> MyASC;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Data")
//	class UDataTable* StructureDataTable;
//	UPROPERTY(EditDefaultsOnly, Category = "Data")
//	class UDataTable* ItemDataTable;
//
//
//	void StartWidgetSetting();
//	void SetBindDelegates();
//	//UFUNCTION()
//	void OnHealthChanged(const FOnAttributeChangeData& Data);
//	UFUNCTION()
//	void HandlePhaseChanged(ETTTGamePhase NewPhase);
//	UFUNCTION()
//	void HandleRemainingTimeChanged(int32 NewRemainingTime);
//	
//	
//
//	UFUNCTION()
//	void OnPlayerGoldChanged(int32 NewGold);	
//	UFUNCTION()
//	void OnPlayerInventoryStructureChanged();
//	UFUNCTION()
//	void OnPlayerInventoryItemChanged();
//
//	void OpenTradeWidget(bool bIsOpen);
//	
//	//게임모드에서 실행시켜서 스크롤 설정
//	void SetTradeScroll();


};
