// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/AI/Evaluator/GetAttributeEvaluator.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

void UGetAttributeEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());
		//AttackSpeed = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());

		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

void UGetAttributeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());
		//AttackSpeed = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
		HealthPercent = FMath::Clamp(
		ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute()) / ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute()),
		0.f, 1.f);
		
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}


