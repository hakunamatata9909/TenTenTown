// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/CounterTask.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "TTTGamePlayTags.h"
#include "Animation/AnimMontage.h"


EStateTreeRunStatus UCounterTask::EnterState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (!Actor || !TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	ExecuteCounterAbility();

	return EStateTreeRunStatus::Running;
}

void UCounterTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
	
}
EStateTreeRunStatus UCounterTask::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
{
	if (!Actor || !TargetActor)
		return EStateTreeRunStatus::Failed;

	if (!Actor->CounterMontage)
		return EStateTreeRunStatus::Failed;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
	}

	float MontageLength = Actor->CounterMontage->GetPlayLength();
	float ActualDuration = MontageLength / AttackSpeed;

	ElapsedTime += DeltaTime;
	if (ElapsedTime >= ActualDuration)
	{
		ElapsedTime = 0.f;
		return EStateTreeRunStatus::Succeeded; 
	}

	return EStateTreeRunStatus::Running;
}


void UCounterTask::ExecuteCounterAbility()
{
	if (!Actor || !TargetActor)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		FGameplayEventData EventData;
		EventData.Instigator = Actor;
		EventData.Target = TargetActor;
		EventData.EventTag = GASTAG::Enemy_Ability_Counter; 

		ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
	}
}
