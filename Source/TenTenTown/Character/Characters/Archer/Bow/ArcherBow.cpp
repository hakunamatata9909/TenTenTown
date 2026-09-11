// Fill out your copyright notice in the Description page of Project Settings.
#include "ArcherBow.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AArcherBow::AArcherBow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SetRootComponent(SkeletalMeshComponent);
	bReplicates = true;
}

// Called when the game starts or when spawned
void AArcherBow::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = SkeletalMeshComponent->GetAnimInstance();
}

void AArcherBow::PlayNormalAttackMontage_Start()
{
	if (!AnimInstance)
	{
		AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	}
	
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(NormalAttackBowAnimation,0.5f);
		AnimInstance->Montage_SetNextSection("Start","LoopStart",NormalAttackBowAnimation);
		AnimInstance->Montage_SetNextSection("LoopStart","LoopStart",NormalAttackBowAnimation);
	}
	
	else
	{
		if(!AnimInstance) 
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Bow: AnimInstance is Null!"));
		if(!NormalAttackBowAnimation) 
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Bow: Montage Asset is Null!"));	
	}
	
}

void AArcherBow::PlayNormalAttackMontage_Release()
{
	if (!AnimInstance)
	{
		AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	}
	if (AnimInstance)
	{
		AnimInstance->Montage_JumpToSection("Release",NormalAttackBowAnimation);
		AnimInstance->Montage_SetPlayRate(NormalAttackBowAnimation,1.f);
	}
}

USkeletalMeshComponent* AArcherBow::GetMesh()
{
	return SkeletalMeshComponent;
}
