#include "GA_PotionItem.h"

#include "AbilitySystemComponent.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterMana.h"

UGA_PotionItem::UGA_PotionItem()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_PotionItem::ApplyOnServer(const FGameplayEventData& Payload)
{
	if (!ASC) return;
	if (ItemData.PassiveEffect)
	{
		FGameplayTag CueTag;
		float CurValue = 0;
		float MaxValue = 0;
		
		if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Potion.HP"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Potion.HP"));
			CurValue = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
			MaxValue = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());
		}
		else if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Potion.MP"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Potion.MP"));
			CurValue = ASC->GetNumericAttribute(UAS_CharacterMana::GetManaAttribute());
			MaxValue = ASC->GetNumericAttribute(UAS_CharacterMana::GetMaxManaAttribute());
		}
		else
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Apply"));
		}

		if (CurValue >= MaxValue - KINDA_SMALL_NUMBER)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
		
		FGameplayCueParameters CueParams;
		CueParams.Location = CurrentActorInfo->AvatarActor.Get()->GetActorLocation();
		ASC->ExecuteGameplayCue(CueTag, CueParams);

		const float RecoveryRate = ItemData.Magnitude / 100;
		const float RecoveryAmount = MaxValue * RecoveryRate;
		
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ItemData.PassiveEffect, 1.f, Ctx);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(ItemData.ItemTag, RecoveryAmount);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}


