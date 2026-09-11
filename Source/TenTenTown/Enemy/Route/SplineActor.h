// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

UCLASS()
class TENTENTOWN_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline")
	TObjectPtr<USplineComponent> SplineActor;
	
protected:
	virtual void BeginPlay() override;
	
};
