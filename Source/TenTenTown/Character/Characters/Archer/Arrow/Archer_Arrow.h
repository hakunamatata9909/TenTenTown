// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Archer_Arrow.generated.h"

class UNiagaraComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class USphereComponent;
class UProjectileMovementComponent;
class AArrowAfterHit;

UCLASS()
class TENTENTOWN_API AArcher_Arrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArcher_Arrow();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayEffects();
	
	UFUNCTION(BlueprintCallable)
	void FireArrow(FVector Direction = FVector::ZeroVector,float SpeedRatio =1.f);
	
	UFUNCTION(BlueprintCallable)
	void SetSetByCallerClass(TSubclassOf<UGameplayEffect> Class);
	UFUNCTION(BlueprintCallable)
	void SetDamage(float NewDamage);
	UFUNCTION()
	void SetIgnoreActor(AActor* ActorToIgnore);
	UFUNCTION()
	void SetASC(UAbilitySystemComponent* NewASC);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="NaiagaraComponent")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="CollisionComponent")
	TObjectPtr<USphereComponent> CollisionComponent;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ProjectileComponents")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AfterArrowHit")
	TSubclassOf<AArrowAfterHit> AfterArrowHitClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GEClass")
	TSubclassOf<UGameplayEffect> SetByCallerClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Settings")
	float MaxSpeed;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Settings")
	float BasicSpeed;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Settings")
	float GravityScale;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Settings")
	bool bAutoActivate;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Settings")
	float bRotationFollowVelocity;
	
	UPROPERTY()
	UAbilitySystemComponent* ASC;
	
	UPROPERTY()
	float Damage;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
