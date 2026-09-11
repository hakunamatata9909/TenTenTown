// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Archer_Ultimate.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ActorArrowRain.h"
#include "ArcherFloatingPawn.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Archer/ArcherCharacter/ArcherCharacter.h"
#include "Character/Characters/Archer/Arrow/Archer_Arrow.h"
#include "Character/Characters/Archer/Bow/ArcherBow.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Archer_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo);
	AvatarCharacter = Cast<AArcherCharacter>(GetAvatarActorFromActorInfo());
	PC = Cast<APlayerController>(AvatarCharacter->GetController());
	Bow = AvatarCharacter->GetEquippedBow();
	ASC = GetAbilitySystemComponentFromActorInfo();
	
	UltimateDamage = ASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute()) * 2.f;
	
	auto* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::Event_Archer_UltLocationConfirm);
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &UGA_Archer_Ultimate::OnTargetLocationConfirmed);
		WaitEventTask->ReadyForActivation();
		UE_LOG(LogTemp, Log, TEXT("Ultimate GA: Waiting for Target Location Event..."));
	}
	
	auto* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("None"),AttackMontage);
	
	PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Archer_Ultimate::K2_EndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_Archer_Ultimate::K2_EndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Archer_Ultimate::K2_EndAbility);
	
	PlayMontageTask->ReadyForActivation();
	PossessSpectatorPawn();
	SpawnArrowOnBow();
	
	ASC->ForceReplication();
}

void UGA_Archer_Ultimate::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&ActivationInfo))
	{
		if (Arrow)
		{
			Arrow->Destroy();
			Arrow = nullptr;
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Archer_Ultimate::PossessSpectatorPawn()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		PC = Cast<APlayerController>(AvatarCharacter->GetController());
		
		if (PC && PawnClass)
		{
			if (UCharacterMovementComponent* MoveComp = AvatarCharacter->GetCharacterMovement())
			{
				MoveComp->StopMovementImmediately();
				MoveComp->DisableMovement();
                
			}

			// 고스트 폰 스폰 및 빙의 로직
			FRotator TargetYawRotation = FRotator(0.f, AvatarCharacter->GetActorRotation().Yaw, 0.f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = AvatarCharacter;
			FVector SpawnLocation = AvatarCharacter->GetActorLocation() + FVector(0, 0, 3500.f);

			AArcherFloatingPawn* NewPawn = GetWorld()->SpawnActor<AArcherFloatingPawn>(
				PawnClass, SpawnLocation, TargetYawRotation, SpawnParams);
       
			if (NewPawn)
			{
				PC->SetControlRotation(TargetYawRotation);
				PC->Possess(NewPawn);
			}
		}
	}
}

void UGA_Archer_Ultimate::SpawnArrowOnBow()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (ArrowClass)
		{
			FVector SpawnPos = Bow->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
			FRotator SpawnRot = Bow->GetMesh()->GetSocketRotation(FName("ArrowSocket"));
			FTransform SpawnTransform(SpawnRot, SpawnPos);

			Arrow = GetWorld()->SpawnActorDeferred<AArcher_Arrow>(
				ArrowClass,
				SpawnTransform,
				AvatarCharacter,
				AvatarCharacter,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);
			
			
			if (Arrow)
			{
				// 화살 속성 설정 (데미지 등)
				Arrow->SetIgnoreActor(AvatarCharacter);
				Arrow->SetIgnoreActor(Bow);
				
				Arrow->FinishSpawning(SpawnTransform);
                
				// 활의 소켓에 부착
				Arrow->AttachToComponent(Bow->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ArrowSocket"));
			}
		}
	}
}

void UGA_Archer_Ultimate::OnTargetLocationConfirmed(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Log, TEXT("Ultimate GA: Event Received!"));
	
	if (Payload.TargetData.IsValid(0))
	{
		FTransform TargetTransform = UAbilitySystemBlueprintLibrary::GetTargetDataOrigin(Payload.TargetData, 0);
		FVector TargetLocation = TargetTransform.GetLocation();

		UE_LOG(LogTemp, Warning, TEXT("Ultimate GA: Confirmed Location -> %s"), *TargetLocation.ToString());
		
		if (HasAuthority(&CurrentActivationInfo))
		{
			AvatarCharacter->Multicast_JumpToSection(AttackMontage, FName("Release"));
			
			if (ArrowRainClass)
			{
				FTransform SpawnTransform(TargetLocation);

				AActorArrowRain* RainActor = GetWorld()->SpawnActorDeferred<AActorArrowRain>(
					ArrowRainClass,
					SpawnTransform,
					AvatarCharacter,
					AvatarCharacter,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

				if (RainActor)
				{
					RainActor->InitializeArrowRain(UltimateDamage, DamageGEClass);

					RainActor->FinishSpawning(SpawnTransform);
				}
			}
			
			if (AArcherFloatingPawn* GhostPawn = const_cast<AArcherFloatingPawn*>(Cast<AArcherFloatingPawn>(Payload.Instigator)))
			{
				if (PC && AvatarCharacter)
				{
					PC->Possess(AvatarCharacter);
					
				
					PC->ResetIgnoreMoveInput();
					PC->ResetIgnoreLookInput();

					FInputModeGameOnly InputMode;
					PC->SetInputMode(InputMode);
			
					GhostPawn->Destroy();
				}
			}
		}
	}
}

