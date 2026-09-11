// Fill out your copyright notice in the Description page of Project Settings.

#include "ArrowAfterHit.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AArrowAfterHit::AArrowAfterHit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
	bReplicates=true;
	bAlwaysRelevant=true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AArrowAfterHit::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(5.f);
}

