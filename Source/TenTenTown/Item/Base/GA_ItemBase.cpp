#include "GA_ItemBase.h"

#include "BaseItem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "Engine/World.h"

UGA_ItemBase::UGA_ItemBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Character.Item")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.ItemUsing")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.ItemUsing")));
}

bool UGA_ItemBase::InitItem(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !TriggerEventData) return false;
	
	SlotIndex = TriggerEventData->EventMagnitude;
	if (SlotIndex < 0) return false;

	Char = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!Char || !PC || !ASC) return false;

	InventoryComp = PC->FindComponentByClass<UInventoryPCComponent>();
	if (!InventoryComp) return false;

	FName OutItemID;
	FItemData OutItemData;
	if (!InventoryComp->GetItemDataFromSlot(SlotIndex, OutItemID, OutItemData)) return false;
	
	ItemID = OutItemID;
	ItemData = OutItemData;
	
	ASC->ForceReplication();
	
	return true;
}

void UGA_ItemBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bFinished = false;
	SlotIndex = -1;
	ItemID = NAME_None;
	PlayTask = nullptr;
	EquipWaitTask = nullptr;
	ApplyWaitTask = nullptr;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!InitItem(ActorInfo, TriggerEventData))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ItemData.UseType == EItemUseType::None)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ItemData.UseMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ItemData.UseMontage, 1.f, NAME_None, false);
	PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
	PlayTask->ReadyForActivation();

	EquipWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EquipTag, nullptr, true, true);
	EquipWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnEquipEvent);
	EquipWaitTask->ReadyForActivation();
		
	ApplyWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ApplyTag, nullptr, true, true);
	ApplyWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnApplyEvent);
	ApplyWaitTask->ReadyForActivation();
}

void UGA_ItemBase::OnEquipEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !Char) return;
	if (!ItemData.ItemMesh) return;

	if (CurrentActorInfo->IsLocallyControlled())
	{
		Char->Multicast_ItemEquip(ItemData.ItemMesh);
	}
	
	if (CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority())
	{
		Char->Server_ItemEquip(ItemData.ItemMesh);
	}
}

void UGA_ItemBase::OnApplyEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !ASC || !Char) return;
	
	if (CurrentActorInfo->IsLocallyControlled())
	{
		Char->Multicast_ItemHide();	
	}
	if (CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority())
	{
		Char->Server_ItemHide();	
	}

	//실제 효과 적용 (서버)
	if (CurrentActorInfo->IsNetAuthority())
	{
		ApplyOnServer(Payload);
		ConsumeItemOnServer();
	}
	else if (CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority() && InventoryComp)
	{
		const int32 InventoryIndex = Payload.EventMagnitude;
		InventoryComp->Server_ItemEventNotify(InventoryIndex, ApplyTag);
	}
}

void UGA_ItemBase::OnMontageEnded()
{
	if (bFinished) return;
	bFinished = true;
	
	if (CurrentActorInfo && Char)
	{
		if (CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority())
		{
			Char->Server_RestoreWeapon();
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ItemBase::ConsumeItemOnServer()
{
	if (!InventoryComp) return;
	if (!Char || !Char->HasAuthority()) return;
	if (SlotIndex < 0) return;
	
	InventoryComp->ConsumeItemFromSlot(SlotIndex, 1);
}