// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoinLootComponent.generated.h"


class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UCoinLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoinLootComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UBoxComponent> OverlapBox;
	
	// 끌어당겨지고 있는 아이템들을 관리하는 배열
	UPROPERTY(VisibleInstanceOnly, Category = "Loot")
	TArray<TObjectPtr<AActor>> LootingItems;

	// 아이템이 날아오는 속도
	UPROPERTY(EditAnywhere, Category = "Loot")
	float LootSpeed = 1500.0f;

	// 아이템 획득으로 간주할 거리
	UPROPERTY(EditAnywhere, Category = "Loot")
	float ReachThreshold = 50.0f;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
