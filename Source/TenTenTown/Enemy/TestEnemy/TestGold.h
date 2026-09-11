// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestGold.generated.h"

class URotatingMovementComponent;

UCLASS()
class TENTENTOWN_API ATestGold : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestGold();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RotateComponent")
	TObjectPtr<URotatingMovementComponent> RotateMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gold")
	float GoldValue = 10.0f;
	
	UFUNCTION()
    void OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetGoldValue(float Value) {GoldValue = Value;};
	float GetGoldValue() const {return GoldValue;};
	
protected:
	virtual void BeginPlay() override;

public:	

};
