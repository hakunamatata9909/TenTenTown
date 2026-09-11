// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Evaluator/CheckSkillCoolDownEvaluator.h"

#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"

void UCheckSkillCoolDownEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	bIsCoolDown = ASC->HasMatchingGameplayTag(GASTAG::Cooldown_Enemy_Skill);
	
}
