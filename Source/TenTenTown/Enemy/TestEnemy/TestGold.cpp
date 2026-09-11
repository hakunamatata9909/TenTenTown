// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TestEnemy/TestGold.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

ATestGold::ATestGold()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetNotifyRigidBodyCollision(true); 

	Mesh->SetCollisionProfileName(TEXT("PhysicsActor")); 

	Mesh->OnComponentHit.AddDynamic(this, &ATestGold::OnHitCallback); 

	Mesh->SetSimulatePhysics(false);
	
	RotateMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>("RotateMovementComponent");
}

void ATestGold::OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitComp->IsSimulatingPhysics()) return;

	if (Hit.Normal.Z > 0.7f) 
	{
		HitComp->SetSimulatePhysics(false);
        
		//HitComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		//HitComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		HitComp->OnComponentHit.RemoveDynamic(this, &ATestGold::OnHitCallback); 
		
		FRotator CurrentRot = GetActorRotation();
		SetActorRotation(FRotator(0.0f, CurrentRot.Yaw, 0.0f));
    
		AddActorWorldOffset(FVector(0.0f, 0.0f, 15.0f)); 

		RotateMovementComponent->Activate(true);
	}
}

void ATestGold::BeginPlay()
{
	Super::BeginPlay();
	
}


