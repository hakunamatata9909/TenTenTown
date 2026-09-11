// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/MoveTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"


EStateTreeRunStatus UMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (!Actor->HasAuthority())
	{
		return EStateTreeRunStatus::Running;
	}

	if (Actor->HasAuthority())
	{
		Actor->SplineActor = SplineActor;
		
		Distance = Actor->MovedDistance;
		
		if (Actor->DistanceOffset == 0.0f)
		{
			Actor->DistanceOffset = FMath::RandRange(-SpreadDistance, SpreadDistance);
		}
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		CachedASC = ASC;
		ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Move);
	}
	
	return EStateTreeRunStatus::Running;
}
EStateTreeRunStatus UMoveTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!Actor->HasAuthority())
	{
		return EStateTreeRunStatus::Running;
	}

	if (!Actor || !SplineActor)
	{
		return EStateTreeRunStatus::Failed;
	}
	if (Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		if (Actor->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
		{
			return EStateTreeRunStatus::Running;
		}
	}
	USplineComponent* SplineComp = SplineActor->SplineActor;
	if (!SplineComp)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		const float BaseSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());
		const float SpeedRate = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedRateAttribute());

		MovementSpeed = FMath::Max(BaseSpeed * SpeedRate, 0.f);
	}
	
	float SplineLength = SplineComp->GetSplineLength();
	float NewDistance = Distance + MovementSpeed * DeltaTime;

	if (Distance < SplineLength)
	{
		NewDistance = FMath::Min(NewDistance, SplineLength);

		FVector SplineLocation = SplineComp->GetLocationAtDistanceAlongSpline(
			NewDistance, ESplineCoordinateSpace::World);

		FVector Direction = SplineComp->GetDirectionAtDistanceAlongSpline(
			NewDistance, ESplineCoordinateSpace::World).GetSafeNormal();

		FVector RightVector = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();

		FVector OffsetVector = RightVector * Actor->DistanceOffset;

		FVector NewLocation = SplineLocation + OffsetVector;

		FRotator SplineRotation = Direction.Rotation();
		FRotator NewRotation(0.f, SplineRotation.Yaw, 0.f);
		//FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		
		if (!Actor->bIsFly)
		{
			//지상 몬스터 바닥 보정
			FHitResult Hit;
			FVector TraceStart = NewLocation + FVector(0.f, 0.f, 300.f);
			FVector TraceEnd   = NewLocation - FVector(0.f, 0.f, 300.f);

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Actor);

			FCollisionObjectQueryParams ObjectParams;
			ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

			if (GetWorld()->LineTraceSingleByObjectType(Hit, TraceStart, TraceEnd, ObjectParams, Params))
			{
				UCapsuleComponent* Capsule = Actor->GetCapsuleComponent();
				const float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
				NewLocation.Z = Hit.Location.Z + HalfHeight;
			}
		}
		//비행 몬스터는 스플라인 z축 따라가도록
		Actor->SetActorLocationAndRotation(NewLocation, NewRotation);
		Distance = NewDistance;

		return EStateTreeRunStatus::Running;
	}
	
	return EStateTreeRunStatus::Succeeded;
	
}

void UMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	Actor->MovedDistance = Distance;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Move);

		if (MovementSpeedRateChangedHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UAS_EnemyAttributeSetBase::GetMovementSpeedRateAttribute()).Remove(MovementSpeedRateChangedHandle);
		}
	}
}