 // Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/Task/BerserkTask.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "TTTGamePlayTags.h"
#include "Enemy/EnemyList/DemonKing.h"

EStateTreeRunStatus UBerserkTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(DemonKing);

	if (!DemonKing || !ASC)
		return EStateTreeRunStatus::Failed;

	if (DemonKing->bBerserkPlayed)
		return EStateTreeRunStatus::Succeeded;

	if (!ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Berserk))
		return EStateTreeRunStatus::Failed;

	FGameplayEventData EventData;
	EventData.EventTag = GASTAG::Enemy_Ability_Berserk;
	EventData.Instigator = DemonKing;
	EventData.Target = DemonKing;

	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);


	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UBerserkTask::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
{
	ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
	if (!DemonKing)
		return EStateTreeRunStatus::Failed;

	if (DemonKing->bBerserkPlayed)
		return EStateTreeRunStatus::Succeeded;

	return EStateTreeRunStatus::Running;
}

void UBerserkTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}
