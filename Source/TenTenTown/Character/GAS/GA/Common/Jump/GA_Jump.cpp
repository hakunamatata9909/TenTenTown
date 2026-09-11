// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Jump.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Jump::UGA_Jump()
{
	//InputPressed 이벤트가 서버로 전해져야 한다. 더블 점프니까 
	bReplicateInputDirectly=true;
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	UAbilityTask_WaitMovementModeChange* WaitMovementModeChange =
		UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this,EMovementMode::MOVE_Walking);
	WaitMovementModeChange->OnChange.AddUniqueDynamic(this,&ThisClass::OnLanded);
	WaitMovementModeChange->ReadyForActivation();
	
	Character->Jump();
	JumpCount++;
}

void UGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	JumpCount=0;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Jump::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(GASTAG::State_Movement_IsDashing)) return;

	if (!DoubleJumpMontage||JumpCount >1)
	{
		UE_LOG(LogTemp,Log,TEXT("no montage"));
		return;
	}
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(GASTAG::State_Movement_IsDoubleJumping);

	UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,FName("DoubleJumpAM"),
			DoubleJumpMontage,0.8f,NAME_None,true,1,0,true);
	AbilityTask_PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnInterrupted);
	AbilityTask_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnDoubleJumpEnd);
	AbilityTask_PlayMontageAndWait->ReadyForActivation();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GASTAG::GameplayCue_Jump);
	Character->GetCharacterMovement()->Velocity.Z=0.0f;
	Character->Jump();
	JumpCount++;
}

void UGA_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UGA_Jump::OnLanded(EMovementMode MovementMode)
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Jump::OnInterrupted()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(GASTAG::State_Movement_IsDoubleJumping);
}

void UGA_Jump::OnDoubleJumpEnd()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(GASTAG::State_Movement_IsDoubleJumping);
}


