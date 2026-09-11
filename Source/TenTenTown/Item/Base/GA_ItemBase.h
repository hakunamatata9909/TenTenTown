#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "Item/Data/ItemData.h"
#include "GA_ItemBase.generated.h"


class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UInventoryPCComponent;
class ABaseCharacter;

UCLASS()
class TENTENTOWN_API UGA_ItemBase : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ItemBase();
	
protected:
	bool InitItem(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData);
	
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void ConsumeItemOnServer();
	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName ItemID;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FItemData ItemData;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<ABaseCharacter> Char = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<APlayerController> PC = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> ASC = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UInventoryPCComponent> InventoryComp = nullptr;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* EquipWaitTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* ApplyWaitTask = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Equip"));
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ApplyTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Apply"));

	bool bFinished = false;
	
	UFUNCTION()
	void OnEquipEvent(const FGameplayEventData Payload);
	UFUNCTION()
	void OnApplyEvent(const FGameplayEventData Payload);
	UFUNCTION()
	void OnMontageEnded();

	virtual void ApplyOnServer(const FGameplayEventData& Payload) {}
};
