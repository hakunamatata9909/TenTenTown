#include "Item/Bomb/GA_BombItem.h"

#include "AbilitySystemComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/World.h"
#include "Item/Base/BaseItem.h"

UGA_BombItem::UGA_BombItem()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Throw"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_BombItem::ApplyOnServer(const FGameplayEventData& Payload)
{
	if (!Char) return;
	
	if (!ItemData.ItemActorClass) return;

	FActorSpawnParameters Params;
	Params.Owner = Char;
	Params.Instigator = Char;

	const FVector SpawnLoc = Char->GetActorLocation() + Char->GetActorForwardVector() * 50.f;
	const FRotator SpawnRot = Char->GetActorRotation();

	if (ABaseItem* Item = Char->GetWorld()->SpawnActor<ABaseItem>(ItemData.ItemActorClass, SpawnLoc, SpawnRot, Params))
	{
		Item->InitItemData(ItemID, ItemData);
		
		const FVector Dir = Char->GetControlRotation().Vector().GetSafeNormal();
		Item->Throw(Dir);
	}
}
