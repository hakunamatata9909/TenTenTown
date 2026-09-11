// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSystemComponent.generated.h"

class UWidgetComponent;
class UUserWidget;
class UMaterialInstance;
struct FHitResult;
class UBoxComponent;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UInteractionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionSystemComponent();
	
	//어빌리티로 래핑할거라 rpc가 필요 없다.
	bool InterAction();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="OverlapBox")
	TObjectPtr<UBoxComponent> OverlapCollisionBox;
	
	//항상 이벤트가 발생한 첫 액터만 저장한다.
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="CurrentOverlappedActor")
	TObjectPtr<AActor> CurrentOverlappedActor = nullptr;
	
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="OverlayMaterial")
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UFUNCTION()
	void OnStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnActorDestroyed(AActor* DestroyedActor);

};


