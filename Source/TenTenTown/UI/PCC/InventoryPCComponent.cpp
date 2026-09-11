#include "UI/PCC/InventoryPCComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Item/Base/GA_ItemBase.h"
#include "Character/PS/TTTPlayerState.h"

UInventoryPCComponent::UInventoryPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UInventoryPCComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwnerRole() == ROLE_Authority)
	{
		InitFixedSlots();
		UE_LOG(LogTemp, Warning, TEXT("[InventoryComp] InitFixedSlots on server"));
	}
}

void UInventoryPCComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryPCComponent, PlayerGold);
	DOREPLIFETIME_CONDITION(UInventoryPCComponent, InventoryItems, COND_OwnerOnly);

}

void UInventoryPCComponent::OnRep_Gold()
{	
	OnGoldChangedDelegate.Broadcast(PlayerGold);
}

void UInventoryPCComponent::OnRep_QuickSlotList()
{
	OnQuickSlotListChangedDelegate.Broadcast(QuickSlotList);
}

void UInventoryPCComponent::OnRep_InventoryItems()
{
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

bool UInventoryPCComponent::Server_AddGold_Validate(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_AddGold Validation Failed: Amount must be positive."));
		return false;
	}

	return true;
}

void UInventoryPCComponent::Server_AddGold_Implementation(int32 Amount)
{
	PlayerGold += Amount;

	UE_LOG(LogTemp, Log, TEXT("Server: Added %d Gold. New total: %d"), Amount, PlayerGold);
}


bool UInventoryPCComponent::Server_UpdateQuickSlotList_Validate(const TArray<FInventoryItemData>& NewQuickSlotList)
{
	/*if (NewQuickSlotList.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_UpdateQuickSlotList Validation Failed: Exceeds maximum quick slot count."));
		return false;
	}*/
	return true;
}

void UInventoryPCComponent::Server_UpdateQuickSlotList_Implementation(const TArray<FInventoryItemData>& NewQuickSlotList)
{
	QuickSlotList = NewQuickSlotList;	
	UE_LOG(LogTemp, Log, TEXT("Server: QuickSlotList updated. New count: %d"), QuickSlotList.Num());	
}

bool UInventoryPCComponent::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (ItemID.IsNone()) return false;

	const UTTTGameInstance* GI = GetWorld()->GetGameInstance<UTTTGameInstance>();
	if (!GI) return false;
	
	return GI->GetItemData(ItemID, OutItemData);
}

void UInventoryPCComponent::InitFixedSlots()
{
	InventoryItems.Empty();
	
	InventoryItems.Add(FItemInstance(FName("Item_Potion_HP"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_Potion_MP"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_Bomb"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_Bomb_Ice"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_RepairKit"), 0));

	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

void UInventoryPCComponent::Server_AddItem_Implementation(FName ItemID, int32 Count)
{
	if (Count <= 0 || ItemID.IsNone()) return;

	FItemData ItemData;
	if (!GetItemData(ItemID, ItemData)) return;

	const int32 MaxStack = ItemData.MaxStackCount;
	for (FItemInstance& Slot : InventoryItems)
	{
		if (Slot.ItemID != ItemID) continue;
		
		int32 NewCount = Slot.Count + Count;
		NewCount = FMath::Clamp(NewCount, 0, MaxStack);
		Slot.Count = NewCount;

		OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
		return;
	}
}

bool UInventoryPCComponent::Server_AddItem_Validate(FName ItemID, int32 Count)
{
	return Count > 0;
}

void UInventoryPCComponent::Server_AddItemWithCost_Implementation(FName ItemID, int32 Count, int32 AddCost)
{
	if (Count <= 0 || ItemID.IsNone() || AddCost < 0) return;

	FItemData ItemData;
	if (!GetItemData(ItemID, ItemData)) return;

	const int32 MaxStack = ItemData.MaxStackCount;
	bool bItemAddedSuccessfully = false;

	for (FItemInstance& Slot : InventoryItems)
	{
		if (Slot.ItemID == ItemID)
		{
			int32 NewCount = Slot.Count + Count;

			if (NewCount > MaxStack)
			{
				return;
			}

			Slot.Count = NewCount;
			bItemAddedSuccessfully = true;

			APlayerController* PC = Cast<APlayerController>(GetOwner());
			ATTTPlayerState* PS = PC ? Cast<ATTTPlayerState>(PC->PlayerState) : nullptr;

			if (PS && PS->GetGold() >= AddCost)
			{
				PS->Server_AddGold_Implementation(-AddCost);
			}
			else
			{
				// ��� ����: �̹� �������� �߰�������, �ŷ� ���з� �����ϰ� �������� �ٽ� ���� �մϴ�.
				// ���������Ƿ�, **Ŭ���̾�Ʈ ������ ������ ������ ��带 �ݵ�� �����**�ϵ��� �ؾ� �մϴ�.
				// ������ ����� �κ��丮�� �߰��� �ϰ� �����ڽ��ϴ�. (��� ���� ���� = �ŷ� ����)

				// ���� �� ��Ȳ(���� �����ѵ� �������� �߰���)�� ���� �ʹٸ�, 
				// �� �Լ� ���� ������ ��� �˻縦 ���� �ؾ� �մϴ�.
			}

			OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
			return;
		}
	}

	// �̿��� ��� (���ο� ���� �ʿ� ��)�� ���� ���� �߰� �ʿ�...
}

bool UInventoryPCComponent::Server_AddItemWithCost_Validate(FName ItemID, int32 Count, int32 AddCost)
{
	return Count > 0 && AddCost >= 0;
}

bool UInventoryPCComponent::GetItemDataFromSlot(int32 SlotIndex, FName& OutItemID, FItemData& OutItemData) const
{
	OutItemID = NAME_None;

	if (SlotIndex < 0 || SlotIndex >= InventoryItems.Num()) return false;

	const FItemInstance& Slot = InventoryItems[SlotIndex];
	if (Slot.Count <= 0 || Slot.ItemID.IsNone()) return false;

	FItemData ItemData;
	if (!GetItemData(Slot.ItemID, ItemData)) return false;

	OutItemID = Slot.ItemID;
	OutItemData = ItemData;
	return true;
}

void UInventoryPCComponent::Server_ItemEventNotify_Implementation(int32 InventoryIndex, FGameplayTag EventTag)
{
	if (InventoryIndex < 0 || InventoryIndex >= InventoryItems.Num()) return;
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;
	ACharacter* Char = Cast<ACharacter>(PC->GetPawn());
	if (!Char) return;

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.EventMagnitude = InventoryIndex;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, EventTag, Payload);
}

void UInventoryPCComponent::UseItem(int32 InventoryIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		SendEventToASC(InventoryIndex);
	}
	else
	{
		SendEventToASC(InventoryIndex);
		Server_UseItem(InventoryIndex);
	}
}

void UInventoryPCComponent::Server_UseItem_Implementation(int32 InventoryIndex)
{
	SendEventToASC(InventoryIndex);
}

bool UInventoryPCComponent::Server_UseItem_Validate(int32 InventoryIndex)
{
	return InventoryIndex >= 0 && InventoryIndex < InventoryItems.Num();
}

void UInventoryPCComponent::ConsumeItemFromSlot(int32 SlotIndex, int32 Amount)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (SlotIndex < 0 || SlotIndex >= InventoryItems.Num()) return;
	
	FItemInstance& Slot = InventoryItems[SlotIndex];
	if (Slot.Count <= 0) return;

	Slot.Count = FMath::Max(Slot.Count - Amount, 0);
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

void UInventoryPCComponent::SendEventToASC(int32 InventoryIndex)
{
	if (InventoryIndex < 0 || InventoryIndex >= InventoryItems.Num()) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	ACharacter* Char = Cast<ACharacter>(PC->GetPawn());
	if (!Char) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Char);
	if (!ASC) return;

	FItemInstance& Slot = InventoryItems[InventoryIndex];
	if (Slot.Count <= 0 || Slot.ItemID.IsNone()) return;

	FItemData ItemData;
	if (!GetItemData(Slot.ItemID, ItemData)) return;

	FGameplayTag EventTag;
	switch (ItemData.UseType)
	{
	case EItemUseType::Drink:
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
		break;
		
	case EItemUseType::Throw:
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Throw"));
		break;
		
	default:
		return;
	}
	
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.EventMagnitude = InventoryIndex;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, Payload.EventTag, Payload);
}
