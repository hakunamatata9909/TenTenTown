// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Teleport_Ability.h"

#include "Components/CapsuleComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList/EvilMage.h"
#include "Enemy/Route/SplineActor.h"
#include "Kismet/GameplayStatics.h"

UEnemy_Teleport_Ability::UEnemy_Teleport_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(GASTAG::Enemy_Ability_Teleport);
	SetAssetTags(Tags);
}

void UEnemy_Teleport_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AEnemyBase* Enemy = Cast<AEnemyBase>(ActorInfo->AvatarActor.Get());
	if (!Enemy || !Enemy->HasAuthority()) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	float ProgressRatio = 0.0f;
	if (Enemy->SplineActor && Enemy->SplineActor->SplineActor)
	{
		float OldSplineLength = Enemy->SplineActor->SplineActor->GetSplineLength();
		if (OldSplineLength > 0.0f)
		{
			ProgressRatio = FMath::Clamp(Enemy->MovedDistance / OldSplineLength, 0.0f, 1.0f);
		}
	}

	ASplineActor* NewSpline = FindRandomSpline(GetWorld(), Enemy->SplineActor);

	if (NewSpline && NewSpline->SplineActor)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Teleporting);
		
		USplineComponent* NewSplineComp = NewSpline->SplineActor;
		float NewSplineLength = NewSplineComp->GetSplineLength();

		float NewDistance = NewSplineLength * (ProgressRatio / 2);

		Enemy->SplineActor = NewSpline;
		Enemy->MovedDistance = NewDistance; 

		FVector NewLocation = NewSplineComp->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		FVector Direction = NewSplineComp->GetDirectionAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
       
		FVector RightVector = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();
		NewLocation += RightVector * Enemy->DistanceOffset;

		if (!Enemy->bIsFly)
		{
			FHitResult Hit;
			FVector TraceStart = NewLocation + FVector(0.f, 0.f, 500.f);
			FVector TraceEnd   = NewLocation - FVector(0.f, 0.f, 500.f);
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Enemy);
            
			if (GetWorld()->LineTraceSingleByObjectType(Hit, TraceStart, TraceEnd, FCollisionObjectQueryParams(ECC_WorldStatic), Params))
			{
				UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent();
				float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
				NewLocation.Z = Hit.Location.Z + HalfHeight;
			}
		}

		Enemy->TeleportTo(NewLocation, Direction.Rotation());

		if (AEvilMage* EvilMage = Cast<AEvilMage>(Enemy))
		{
			EvilMage->bIsTeleported = true;
		}

		UE_LOG(LogTemp, Warning, TEXT("Teleported to New Spline: %s, Ratio: %f"), *NewSpline->GetName(), ProgressRatio);
	}

	Enemy->ForceNetUpdate();

	
	
	/*ASplineActor* NewSpline = FindRandomSpline(GetWorld(), Enemy->SplineActor);

	if (NewSpline)
	{
		Enemy->SplineActor = NewSpline;

		float ProgressRatio = 0.0f;

		if (Enemy->SplineActor && Enemy->SplineActor->SplineActor)
		{
			float CurrentSplineLength = Enemy->SplineActor->SplineActor->GetSplineLength();
			if (CurrentSplineLength > 0.0f)
			{
				ProgressRatio = Enemy->MovedDistance / CurrentSplineLength;
                
				ProgressRatio = FMath::Clamp(ProgressRatio, 0.0f, 1.0f);
			}
		}

		USplineComponent* NewSplineComp = NewSpline->SplineActor;
		float NewSplineLength = NewSplineComp->GetSplineLength();

		float NewDistance = NewSplineLength * ProgressRatio;

		// 데이터 갱신
		Enemy->SplineActor = NewSpline;
		Enemy->MovedDistance = NewDistance; 

		FVector NewLocation = NewSplineComp->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		FVector Direction = NewSplineComp->GetDirectionAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		FRotator NewRotation = Direction.Rotation();

		FVector RightVector = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();
		NewLocation += RightVector * Enemy->DistanceOffset;

		if (!Enemy->bIsFly)
		{
			FHitResult Hit;
			FVector TraceStart = NewLocation + FVector(0.f, 0.f, 500.f);
			FVector TraceEnd   = NewLocation - FVector(0.f, 0.f, 500.f);
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Enemy);
            
			if (World->LineTraceSingleByObjectType(Hit, TraceStart, TraceEnd, FCollisionObjectQueryParams(ECC_WorldStatic), Params))
			{
				UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent();
				float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
				NewLocation.Z = Hit.Location.Z + HalfHeight;
			}
		}

		// 실제 텔레포트
		Enemy->TeleportTo(NewLocation, NewRotation);

		UE_LOG(LogTemp, Warning, TEXT("Teleported"));

		AEvilMage* EvilMage = Cast<AEvilMage>(Enemy);
		
		if (EvilMage)
		{
			EvilMage->bIsTeleported = true;
		}

	}
	
	Enemy->ForceNetUpdate();*/

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UEnemy_Teleport_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Teleporting);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ASplineActor* UEnemy_Teleport_Ability::FindRandomSpline(const UWorld* World,
	const class ASplineActor* CurrentSpline) const
{
	TArray<AActor*> AllSplines;
	UGameplayStatics::GetAllActorsOfClass(World, ASplineActor::StaticClass(), AllSplines);

	TArray<ASplineActor*> ValidSplines;
	
	for (AActor* Actor : AllSplines)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *Actor->GetName());
		
		ASplineActor* Spline = Cast<ASplineActor>(Actor);
		if (Spline && Spline != CurrentSpline)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spline: %s"), *Spline->GetName());
			ValidSplines.Add(Spline);
		}
	}

	if (ValidSplines.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, ValidSplines.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("Index: %d"), Index);
		return ValidSplines[Index];
	}

	UE_LOG(LogTemp, Warning, TEXT("No Spline Found!"));
	return nullptr;
}


