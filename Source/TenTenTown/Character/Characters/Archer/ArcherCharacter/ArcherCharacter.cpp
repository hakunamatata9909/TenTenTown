// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherCharacter.h"

#include "Animation/AnimInstance.h"
#include "Character/Characters/Archer/Bow/ArcherBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AArcherCharacter::AArcherCharacter()
{
	JumpMaxCount = 2;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;
}

AArcherBow* AArcherCharacter::GetEquippedBow()
{
	return EquippedBow;
}

void AArcherCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass,EquippedBow);
}

void AArcherCharacter::Multicast_JumpToSection_Implementation(UAnimMontage* Montage, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AArcherCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		EquipBow();
	}
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		//PC->ConsoleCommand("ShowDebug AbilitySystem 1");
	}
}

void AArcherCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (ASC)
	{
		ASC->ApplyGameplayEffectToSelf(GE_ArcherRegen->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());
	}
}

void AArcherCharacter::EquipBow()
{
	if (BowClass && !EquippedBow)
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.Instigator = GetInstigator();
		
		EquippedBow = GetWorld()->SpawnActor<AArcherBow>(BowClass,GetActorTransform(),Parameters);
		
		if (EquippedBow)
		{
			EquippedBow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,FName("Weapon_L"));
		}
	}
}
