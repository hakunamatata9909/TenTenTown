#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Data/ItemData.h"
#include "BaseItem.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class TENTENTOWN_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	virtual void BeginPlay() override;
	
	void InitItemData(FName InItemID, const FItemData& InItemData);
	void Throw(const FVector& InDir);

protected:
	UFUNCTION()
	void OnBombHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
	void Explode();

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> Move;

	UPROPERTY()
	FName ItemID;
	UPROPERTY()
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere)
	float Power = 4000.f;
};
