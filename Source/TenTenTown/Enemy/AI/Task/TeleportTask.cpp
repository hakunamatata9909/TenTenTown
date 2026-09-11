// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/TeleportTask.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"



EStateTreeRunStatus UTeleportTask::EnterState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (!AbilityEndedDelegateHandle.IsValid())
	{
		AbilityEndedDelegateHandle =
			ASC->OnAbilityEnded.AddUObject(this, &UTeleportTask::OnAbilityEndedDelegate);
		
	}
	
	FGameplayTagContainer Tags;
	Tags.AddTag(GASTAG::Enemy_Ability_Teleport);
	
	bool bActivated = ASC->TryActivateAbilitiesByTag(Tags, true);

	if (bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState Succeeded"));
		
		return EStateTreeRunStatus::Running;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState Failed"));

		if (AbilityEndedDelegateHandle.IsValid())
		{
			ASC->AbilityEndedCallbacks.Remove(AbilityEndedDelegateHandle);
			AbilityEndedDelegateHandle.Reset();
		}
		
		return EStateTreeRunStatus::Failed;
	}
}

void UTeleportTask::OnAbilityEndedDelegate(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilityThatEnded)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		if (ASC && AbilityEndedDelegateHandle.IsValid())
		{
			ASC->AbilityEndedCallbacks.Remove(AbilityEndedDelegateHandle);
			AbilityEndedDelegateHandle.Reset();
		}

		UE_LOG(LogTemp, Warning, TEXT("SkillTask -> Target Ability Ended: Task Finished"));
		
		FinishTask();
	}
}

void UTeleportTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (ASC && AbilityEndedDelegateHandle.IsValid())
	{
		ASC->AbilityEndedCallbacks.Remove(AbilityEndedDelegateHandle);
		AbilityEndedDelegateHandle.Reset();
	}
	
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus UTeleportTask::EndState(UGameplayAbility* EndAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("SkillTask EndState"));
	
	return EStateTreeRunStatus::Succeeded;
}
