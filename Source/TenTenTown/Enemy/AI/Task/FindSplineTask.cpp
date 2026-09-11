// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/FindSplineTask.h"

#include "Enemy/AI/Evaluator/GetSplineEvaluator.h"

EStateTreeRunStatus UFindSplineTask::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	// 1. SplineEvaluator 인스턴스 유효성 검사
	if (!SplineEvaluator)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFindSplineTask Failed: SplineEvaluator not bound or null."));
		return EStateTreeRunStatus::Failed;
	}
    
	// 2. Evaluator의 Spline 탐색 로직 실행
	// 💡 Context를 전달하여 Spline 탐색 로직이 안전하게 실행되도록 합니다.
	SplineEvaluator->ExecuteSplineSearch(Context); 

	// 3. Task 종료
	// 탐색은 즉시 완료되므로 Succeeded 반환
	return EStateTreeRunStatus::Succeeded;
}
