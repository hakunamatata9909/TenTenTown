#include "EnemyDamage.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

UEnemyDamage::UEnemyDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModifierInfo;

	ModifierInfo.Attribute = UAS_EnemyAttributeSetBase::GetDamageAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Override;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Enemy.Damage"));

	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(ModifierInfo);
}
