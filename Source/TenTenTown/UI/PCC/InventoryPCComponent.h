#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/ItemInstance.h"
#include "InventoryPCComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructureListChanged);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemListChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemsChanged, const TArray<FItemInstance>&, NewItems);

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickSlotListChanged, const TArray<FInventoryItemData>&, NewQuickSlotList);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UInventoryPCComponent : public UActorComponent
{
	GENERATED_BODY()

    
public:
	UInventoryPCComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItems, meta = (AllowPrivateAccess = true))
	TArray<FItemInstance> InventoryItems;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Gold, meta = (AllowPrivateAccess = true))
	int32 PlayerGold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_QuickSlotList, meta = (AllowPrivateAccess = true))
	TArray<FInventoryItemData> QuickSlotList;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryStructure, meta = (AllowPrivateAccess = true))
	//TArray<FInventoryItemData> StructureList;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItem, meta = (AllowPrivateAccess = true))
	//TArray<FInventoryItemData> ItemList;
	
	UFUNCTION()
	void OnRep_Gold();
	UFUNCTION()
	void OnRep_QuickSlotList();
	UFUNCTION()
	void OnRep_InventoryItems();



public:
	UPROPERTY(BlueprintAssignable, Category = "Gold")
	FOnGoldChanged OnGoldChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryItemsChanged OnInventoryItemsChangedDelegate;
	
	int32 GetPlayerGold() const { return PlayerGold; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddGold(int32 Amount);

	UPROPERTY(BlueprintAssignable, Category= "QuickSlotList")
	FOnQuickSlotListChanged OnQuickSlotListChangedDelegate;

	const TArray<FInventoryItemData>& GetQuickSlotList() const { return QuickSlotList; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateQuickSlotList(const TArray<FInventoryItemData>& NewQuickSlotList);

	const TArray<FItemInstance>& GetInventoryItems() const { return InventoryItems; }

	//Item
	UFUNCTION(BlueprintCallable, Category="Item")
	bool GetItemData(FName ItemID, FItemData& OutItemData) const;
	void InitFixedSlots();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(FName ItemID, int32 Count);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItemWithCost(FName ItemID, int32 Count, int32 AddCost);
	UFUNCTION(Server, Reliable)
	void Server_ItemEventNotify(int32 InventoryIndex, FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category="Item")
	void UseItem(int32 InventoryIndex);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UseItem(int32 InventoryIndex);

	bool GetItemDataFromSlot(int32 SlotIndex, FName& OutItemID, FItemData& OutItemData) const;
	void ConsumeItemFromSlot(int32 SlotIndex, int32 Amount);


private:
	void SendEventToASC(int32 InventoryIndex);
};
