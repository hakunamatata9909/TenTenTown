// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/GAS/GE/Enemy_Berserk.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "TTTGamePlayTags.h"

UEnemy_Berserk::UEnemy_Berserk()
{
	
	FGameplayModifierInfo AttackModifier;
	AttackModifier.Attribute = UAS_EnemyAttributeSetBase::GetAttackAttribute(); 
	AttackModifier.ModifierOp = EGameplayModOp::Additive; 
	AttackModifier.ModifierMagnitude = FScalableFloat(50.0f);
	Modifiers.Add(AttackModifier);

	FGameplayModifierInfo AttackSpeedModifier;
	AttackSpeedModifier.Attribute = UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute();
	AttackSpeedModifier.ModifierOp = EGameplayModOp::Additive;
	AttackSpeedModifier.ModifierMagnitude = FScalableFloat(1.0f);
	Modifiers.Add(AttackSpeedModifier);

	FGameplayModifierInfo MovementSpeedModifier;
	MovementSpeedModifier.Attribute = UAS_EnemyAttributeSetBase::GetMovementSpeedRateAttribute();
	MovementSpeedModifier.ModifierOp = EGameplayModOp::Additive;
	MovementSpeedModifier.ModifierMagnitude = FScalableFloat(1.0f);
	Modifiers.Add(MovementSpeedModifier);
	
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}
