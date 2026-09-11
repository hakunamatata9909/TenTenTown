// Fill out your copyright notice in the Description page of Project Settings.


#include "FindClosestEvaluator.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"

void UFindClosestEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

}

bool UFindClosestEvaluator::CheckVisibility(AActor* Target) const
{
	if (!Target || !Actor) return false;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);
	Params.AddIgnoredActor(Target);

	FVector Start = Actor->GetActorLocation() + FVector(0, 0, 50.f);
	FVector End = Target->GetActorLocation() + FVector(0, 0, 50.f);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	return !bHit; // 충돌 없으면 시야 확보(true)
}

void UFindClosestEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!ASC || !Actor) return;
	if (!ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Combat)) return;

	ScanTimer += DeltaTime;
	
	if (ScanTimer < ScanInterval)
	{
		return;
	}
	
	ScanTimer = 0.0f;

	if (TargetActor)
	{
		float DistSq = FVector::DistSquared(Actor->GetActorLocation(), TargetActor->GetActorLocation());

		float DetectRangeSq = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute()); 
        
		if (DistSq <= DetectRangeSq && CheckVisibility(TargetActor))
		{
			return;
		}
	}
	
	
	const TArray<TWeakObjectPtr<AActor>>& OverlappingList = Actor->GetOverlappedPawns();
	TArray<TPair<AActor*, float>> Candidates;
	FVector ActorLocation = Actor->GetActorLocation();

	for (const TWeakObjectPtr<AActor>& TargetPtr : OverlappingList)
	{
		if (AActor* CurrentActor = TargetPtr.Get())
		{
			if (CurrentActor == Actor) continue;
			float DistSq = FVector::DistSquared(ActorLocation, CurrentActor->GetActorLocation());
			Candidates.Add({CurrentActor, DistSq});
		}
	}

	Candidates.Sort([](const TPair<AActor*, float>& A, const TPair<AActor*, float>& B) {
		return A.Value < B.Value;
	});

	AActor* BestTarget = nullptr;
	int32 CheckedCount = 0;
    
	for (const auto& Candidate : Candidates)
	{
		if (CheckedCount >= 3) break; // 최대 3명만 검사하고 포기

		if (CheckVisibility(Candidate.Key))
		{
			BestTarget = Candidate.Key;
			break;
		}
		CheckedCount++;
	}

	TargetActor = BestTarget;

	

	//AActor* NearestTarget = nullptr;
	//float MinDistanceSq = FLT_MAX;
	//FVector OwnerLocation = Actor->GetActorLocation();
	//
	//for (const TWeakObjectPtr<AActor>& TargetPtr : OverlappingList)
	//{
	//	if (AActor* CurrentActor = TargetPtr.Get())
	//	{
	//		float DistanceSq = FVector::DistSquared(OwnerLocation, CurrentActor->GetActorLocation());
    //       
	//		if (DistanceSq < MinDistanceSq)
	//		{
	//			MinDistanceSq = DistanceSq;
	//			NearestTarget = CurrentActor;
	//		}
	//	}
	//}
//
	//
//
	//
	//if (NearestTarget)
	//{
	//	if (NearestTarget != TargetActor)
	//	{
	//		TargetActor = NearestTarget;
	//	}
	//}
	//else
	//{
	//	TargetActor = nullptr;
	//
	//}
	
}
