// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ASpawnPoint::ASpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(Mesh);

	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
}

//스폰 지역 내 랜덤 위치 스폰
FTransform ASpawnPoint::GetSpawnTransform() const
{
	const FVector Extent = SpawnArea->GetScaledBoxExtent();
    
	FVector RandomOffset;
	RandomOffset.X = FMath::RandRange(-Extent.X, Extent.X);
	RandomOffset.Y = FMath::RandRange(-Extent.Y, Extent.Y);

	FVector SpawnLocation = SpawnArea->GetComponentLocation() + RandomOffset;

	FRotator SpawnRotation = SpawnArea->GetComponentRotation();

	return FTransform(SpawnRotation, SpawnLocation);
}