// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Evaluator/TestEvaluator.h"
#include "EngineUtils.h"
#include "Enemy/Route/SplineActor.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/Route/SplineActor.h"
#include "GameFramework/Pawn.h"

void UTestEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	TArray<AActor*> AllSplineActor;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASplineActor::StaticClass(), AllSplineActor);

	AActor* Nearest = nullptr;
	float MinDistance = FLT_MAX;
	FVector ActorLocation = Actor->GetActorLocation();

	for (AActor* CurrentActor : AllSplineActor)
	{
		float Distance = (ActorLocation - CurrentActor->GetActorLocation()).Size();
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Nearest = CurrentActor;
		}
	}

	if (Nearest)
	{
		SplineActor = Cast<ASplineActor>(Nearest);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTestEvaluator: Could not find any ASplineActor."));
	}
}
