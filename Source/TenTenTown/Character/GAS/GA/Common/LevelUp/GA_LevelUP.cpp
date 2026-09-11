// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LevelUP.h"

#include "AbilitySystemComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"

bool UGA_LevelUP::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;
	
	if (GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UAS_CharacterBase::GetLevelAttribute())>=10.f)
	{
		return false;
	}
		return true;
}

void UGA_LevelUP::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	
	if (GE_LevelUp)
	{
		ASC->ApplyGameplayEffectToSelf(GE_LevelUp->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());
	}
	
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		BaseCharacter->LevelUP();
	}
	
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_LevelUP::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

