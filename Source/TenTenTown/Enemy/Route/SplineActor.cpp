// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Route/SplineActor.h"

ASplineActor::ASplineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineActor = CreateDefaultSubobject<USplineComponent>("SplineActor");
}

void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}


