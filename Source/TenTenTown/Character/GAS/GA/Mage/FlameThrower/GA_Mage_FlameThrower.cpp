#include "GA_Mage_FlameThrower.h"

#include "FlameThrowerActor.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Mage_FlameThrower::UGA_Mage_FlameThrower()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.FlameThrower")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
}

void UGA_Mage_FlameThrower::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bInputHeld = true;
	bShotStarted = false;
	SpawnedActor = nullptr;

	ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle,  CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	auto Lock = [this](ACharacter* C)
	{
		if (auto* Move = C->GetCharacterMovement())
		{
			SavedMoveMode = Move->MovementMode;
			Move->StopMovementImmediately();
			Move->DisableMovement();
		}
		if (APlayerController* PC = Cast<APlayerController>(C->GetController()))
		{
			PC->SetIgnoreMoveInput(true);
		}
	};
	
	if (CurrentActorInfo->IsNetAuthority())
		Lock(Char);
	if (Char->IsLocallyControlled())
		Lock(Char);

	if (ChargeMontage)
	{
		ChargeTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChargeMontage, 1,NAME_None, false);
		ChargeTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargeComplete);
		ChargeTask->ReadyForActivation();
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Mage_FlameThrower::OnChargeComplete()
{
	if (bShotStarted) return;
	bShotStarted = true;

	if (!bInputHeld)
	{
		EndAbility(CurrentSpecHandle,  CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AMageCharacter* Mage = Cast<AMageCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Mage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (UAnimInstance* Anim = Mage->GetMesh() ? Mage->GetMesh()->GetAnimInstance() : nullptr)
	{
		if (ChargeMontage)
		{
			Anim->Montage_Stop(0.2f, ChargeMontage);
		}
	}
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
		{
			ASC->AddGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.FlameThrower.Shoot")));
		}
	}
	
	if (ChannelMontage)
	{
		ChannelTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChannelMontage, 1,NAME_None, false);
		ChannelTask->ReadyForActivation();
	}
	
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ShootTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnShootEvent);
	WaitTask->ReadyForActivation();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEventData Data;
		Data.EventTag = ShootTag;
		ASC->HandleGameplayEvent(ShootTag, &Data);
	}
	
	Mage->GetWorldTimerManager().SetTimer(
		ChannelTimer,
		[this](){ EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); },
		MaxChannelTime,
		false
	);
}

void UGA_Mage_FlameThrower::OnShootEvent(FGameplayEventData Payload)
{
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		SpawnFlame();
	}
}
 
void UGA_Mage_FlameThrower::SpawnFlame()
{
	ACharacter* Char = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());

	if (!Char || !Char->HasAuthority() || !FlameClass) return;
	if (SpawnedActor) return;

	FVector Start = Char->GetActorLocation();
	FRotator Rot = Char->GetControlRotation();
	
	if (AMageCharacter* Mage = Cast<AMageCharacter>(Char))
	{
		if (UStaticMeshComponent* Wand = Mage->GetWandMesh())
		{
			if (Wand->DoesSocketExist(MuzzleSocketName))
			{
				const FTransform SocketTM = Wand->GetSocketTransform(MuzzleSocketName, RTS_World);
				
				Start = SocketTM.GetLocation();
				
				const FRotator ControlRot = Char->GetControlRotation();
				Rot = FRotator(0.f, ControlRot.Yaw, 0.f); // 위/아래 회전 막고 싶으면 이렇게
			}
		}
	}
	
	FActorSpawnParameters P;
	P.Owner = Char;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AFlameThrowerActor* Flame = Char->GetWorld()->SpawnActor<AFlameThrowerActor>(FlameClass, Start, Rot, P))
	{
		Flame->SetOwner(Char);
		Flame->SetReplicateMovement(true);
		Flame->SetNetUpdateFrequency(120.f);
		Flame->SetMinNetUpdateFrequency(60.f);
		Flame->SetNetDormancy(DORM_Awake);
		
		Flame->Init(TraceInterval, ConeHalfAngleDeg, MaxChannelTime);

		Char->MoveIgnoreActorAdd(Flame);
		SpawnedActor = Flame;
	}
}

void UGA_Mage_FlameThrower::OnChannelMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_FlameThrower::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bInputHeld = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UGA_Mage_FlameThrower::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		auto& TM = ActorInfo->AvatarActor->GetWorldTimerManager();
		TM.ClearTimer(ChannelTimer);
	}

	if (ActorInfo && ActorInfo->IsNetAuthority() && SpawnedActor)
	{
		if (AFlameThrowerActor* FlameThrower = SpawnedActor)
		{
			FlameThrower->EndFlameVFX();
		}
		SpawnedActor = nullptr;
	}

	if (ChargeTask)
	{
		ChargeTask->EndTask();
		ChargeTask = nullptr;
	}
	if (ChannelTask)
	{
		ChannelTask->EndTask();
		ChannelTask = nullptr;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
		{
			ASC->RemoveGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.FlameThrower.Shoot")));
		}
	}
	
	if (ActorInfo)
	{
		if (ACharacter* C = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UAnimInstance* Anim = C->GetMesh() ? C->GetMesh()->GetAnimInstance() : nullptr)
			{
				if (ChargeMontage) Anim->Montage_Stop(0.2f, ChargeMontage);
				if (ChannelMontage) Anim->Montage_Stop(0.2f, ChannelMontage);
			}
			auto Unlock = [this](ACharacter* X)
			{
				if (auto* Move = X->GetCharacterMovement())
				{
					const bool bSavedValid = (SavedMoveMode != MOVE_None && SavedMoveMode != MOVE_Custom);
					Move->SetMovementMode(bSavedValid ? SavedMoveMode : MOVE_Walking);
				}
				if (APlayerController* PC = Cast<APlayerController>(X->GetController()))
				{
					PC->SetIgnoreMoveInput(false);
				}
			};
			
			if (ActorInfo->IsNetAuthority())
				Unlock(C);
			
			if (C->IsLocallyControlled())
				Unlock(C);
		}
	}

	bInputHeld = false;
	bShotStarted = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
