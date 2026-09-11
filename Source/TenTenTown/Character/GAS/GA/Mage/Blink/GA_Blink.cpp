#include "GA_Blink.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_Blink::UGA_Blink()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedDir = Char->GetVelocity();
	CachedDir.Z = 0.f;

	//이동이 없다면 정면으로 점멸
	if (CachedDir.IsNearlyZero())
	{
		CachedDir = Char->GetActorForwardVector();
	}
	CachedDir.Normalize();
	
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
	
	if (BlinkMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			BlinkMontage,
			1.f,
			NAME_None,
			false);

		PlayTask->OnCompleted.AddDynamic(this, &UGA_Blink::OnMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &UGA_Blink::OnMontageCancelled);
		PlayTask->OnCancelled.AddDynamic(this, &UGA_Blink::OnMontageCancelled);
		PlayTask->ReadyForActivation();
	}
	
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, BlinkTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &UGA_Blink::OnBlinkEventReceived);
	WaitTask->ReadyForActivation();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Blink::OnBlinkEventReceived(FGameplayEventData Payload)
{
	if (!CurrentActorInfo) return;

	ACharacter* Char = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Char) return;

	FVector Dest;
	if (!bFindBlinkDest(Char, Dest))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		Char->TeleportTo(Dest, Char->GetActorRotation(), false, true);
	}
}

void UGA_Blink::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Blink::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool UGA_Blink::bFindBlinkDest(const ACharacter* Character, FVector& OutDest) const
{
	const FVector Start = Character->GetActorLocation();

	FVector Dir = CachedDir;
	
	const FVector End = Start + Dir * MaxDist;

	//벽을 감지하여 벽보다 조금 앞으로 점멸
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Blink), false, Character);
	const bool bHit = Character->GetWorld()->SweepSingleByChannel(
		Hit,
		Start, End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params);

	//벽이 없다면 최대거리 점멸
	if (!bHit)
	{
		OutDest = End;
		return true;
	}

	OutDest = Hit.Location - Dir * (TraceRadius * 5.f);
	
	//도착지점 바닥을 감지하여 땅으로 점멸
	FHitResult Ground;
	const FVector Up = OutDest + FVector(0, 0, 200);
	const FVector Down = OutDest - FVector(0, 0, 1000);
	const bool bGround = Character->GetWorld()->LineTraceSingleByChannel(
		Ground,
		Up, Down,
		ECC_Visibility,
		Params);

	if (bGround)
	{
		const float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		OutDest = Ground.Location + FVector(0, 0, HalfHeight);
	}
	
	return true;
}

void UGA_Blink::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (CurrentActorInfo)
	{
		if (ACharacter* C = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
		{
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
			
			if (CurrentActorInfo->IsNetAuthority())
				Unlock(C);
			
			if (C->IsLocallyControlled())
				Unlock(C);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}