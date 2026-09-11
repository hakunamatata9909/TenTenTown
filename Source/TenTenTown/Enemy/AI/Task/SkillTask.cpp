// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/SkillTask.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h"
#include "Enemy/Base/EnemyBase.h"

EStateTreeRunStatus USkillTask::EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState"));
	
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (ASC->HasMatchingGameplayTag(GASTAG::Cooldown_Enemy_Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState Blocked by Cooldown"));
		return EStateTreeRunStatus::Failed;
	}
	
	if (!AbilityEndedDelegateHandle.IsValid())
	{
		AbilityEndedDelegateHandle =
			ASC->OnAbilityEnded.AddUObject(this, &USkillTask::OnAbilityEndedDelegate);

		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState AbilityEndedDelegate Registered"));

	}

	
	FGameplayTagContainer Tags;
	Tags.AddTag(GASTAG::Enemy_Ability_BuffNearBy);
	
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



void USkillTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (ASC && AbilityEndedDelegateHandle.IsValid())
	{
		ASC->AbilityEndedCallbacks.Remove(AbilityEndedDelegateHandle);
		AbilityEndedDelegateHandle.Reset();
	}
	
	Super::ExitState(Context, Transition);
}

void USkillTask::OnAbilityEndedDelegate(const FAbilityEndedData& AbilityEndedData)
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

EStateTreeRunStatus USkillTask::EndState(UGameplayAbility* EndAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("SkillTask EndState"));
	
	return EStateTreeRunStatus::Succeeded;
}


