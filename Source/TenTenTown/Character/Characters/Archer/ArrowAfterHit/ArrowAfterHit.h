// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowAfterHit.generated.h"

UCLASS()
class TENTENTOWN_API AArrowAfterHit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrowAfterHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="SceneComponent");
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
public:	


};
