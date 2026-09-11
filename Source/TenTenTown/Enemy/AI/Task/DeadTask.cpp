// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/DeadTask.h"

#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/System/PreloadSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus UDeadTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (Actor)
	{
		//bMontageEnded = false;
		//
		//FMontageEnded OnEnded;
		//OnEnded.BindUFunction(this, FName("OnDeadMontageEnd"));
//
		//Actor->PlayMontage(Actor->DeadMontage, OnEnded, 1.0f);
		//Actor->Multicast_PlayMontage(Actor->DeadMontage, 1.0f);
//
		//return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus UDeadTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!Actor)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (bMontageEnded && Actor->HasAuthority())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;

}

void UDeadTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
                               const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

void UDeadTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	//if (Actor && Actor->HasAuthority() && !bGoldDrop)
	//{
	//	if (Actor)
	//	{
	//		Actor->DropGoldItem();
	//		bGoldDrop = true;
//
	//		// 풀 실행
	//		if (UWorld* World = Actor->GetWorld())
	//		{
	//			if (UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
	//			{
	//				if (UPoolSubsystem* PoolSubsystem = GI->GetSubsystem<UPoolSubsystem>())
	//				{
	//					PoolSubsystem->ReleaseEnemy(Actor);
	//				}
	//				else
	//				{
	//					UE_LOG(LogTemp, Warning, TEXT("UDeadTask:Release Failed"));
	//					Actor->SetLifeSpan(0.1f);
	//		
	//				}
	//			}
	//	
	//		}
//
	//	}
	//}
}

void UDeadTask::OnDeadMontageEnd(UAnimMontage* Montage)
{
	if (Actor && Actor->GetLocalRole() == ROLE_Authority)
	{
		bMontageEnded = true;
	}
}

