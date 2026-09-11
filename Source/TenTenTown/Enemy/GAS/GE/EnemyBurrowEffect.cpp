// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GE/EnemyBurrowEffect.h"

UEnemyBurrowEffect::UEnemyBurrowEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	
	FInheritedTagContainer InheritedTags;
	InheritedTags.Added.AddTag(GASTAG::Enemy_State_Burrowed);
	InheritedTags.Added.AddTag(GASTAG::GameplayCue_Enemy_Effect_Burrow);
}
