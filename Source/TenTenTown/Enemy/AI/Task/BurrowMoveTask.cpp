// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/BurrowMoveTask.h"

#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/GA/Enemy_Burrow_Ability.h"

EStateTreeRunStatus UBurrowMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	FGameplayTagContainer Tags;
	Tags.AddTag(GASTAG::Enemy_Ability_Burrow.GetTag());
	
	ASC->TryActivateAbilitiesByTag(Tags, true);

	UE_LOG(LogTemp, Warning, TEXT("BurrowMoveTask EnterState"));
	
	return EStateTreeRunStatus::Running;	
}

void UBurrowMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	if (ASC)
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(GASTAG::Enemy_Ability_Burrow.GetTag());
		ASC->CancelAbilities(&Tags);

		UE_LOG(LogTemp, Warning, TEXT("BurrowMoveTask ExitState"));

	}
	
	Super::ExitState(Context, Transition);
	
}
