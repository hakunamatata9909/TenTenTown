// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcherBow.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS()
class TENTENTOWN_API AArcherBow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArcherBow();
	UFUNCTION(BlueprintCallable)
	void PlayNormalAttackMontage_Start();
	UFUNCTION(BlueprintCallable)
	void PlayNormalAttackMontage_Release();
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetMesh();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Animation|NormalAttack")
	TObjectPtr<UAnimMontage> NormalAttackBowAnimation;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Animation|AnimInstance")
	TObjectPtr<UAnimInstance> AnimInstance;
	
};
