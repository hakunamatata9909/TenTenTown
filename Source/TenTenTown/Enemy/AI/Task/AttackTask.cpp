// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/AttackTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/World.h"
#include "TTTGamePlayTags.h" 
#include "Animation/AnimMontage.h"

EStateTreeRunStatus UAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                            const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (!Actor || !TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	ExecuteAbility();

	return EStateTreeRunStatus::Running;
}

void UAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus UAttackTask::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
{
	if (!Actor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		
		return EStateTreeRunStatus::Failed;
	}

	if (TargetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is nullptr"));
		
		return EStateTreeRunStatus::Succeeded;
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
	}

	if (Actor && Actor->AttackMontage)
	{
		float MontageLength = Actor->AttackMontage->GetPlayLength(); 
		float ActualDuration = MontageLength / AttackSpeed;          // 실제 재생 시간
	

		// 공격 간격 타이머 갱신
		ElapsedTime += DeltaTime;
		if (ElapsedTime >= ActualDuration)
		{
			ElapsedTime = 0.f;

			if (Actor->HasAuthority())
			{
				// Ability 실행
				ExecuteAbility();
			}
		}
	}
	return EStateTreeRunStatus::Running;
}

void UAttackTask::ExecuteAbility()
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		FGameplayEventData EventData;
 	  	
		EventData.Instigator = Actor;
		EventData.Target = TargetActor;
		EventData.EventTag = GASTAG::Enemy_Ability_Attack;
		
		ASC->HandleGameplayEvent(
			EventData.EventTag, 
			&EventData
		);
	}
}