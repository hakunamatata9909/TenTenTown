// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Sprint.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UGA_Sprint::UGA_Sprint()
{
	SprintSpeed=600.f;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = true;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AActor* Actor = GetAvatarActorFromActorInfo())
	{
		ACharacter* Character = Cast<ACharacter>(Actor);
		if (Character)
		{
			CMC= Character->GetCharacterMovement();
		}
	}

	if (CMC)
	{
		BaseWalkSpeed = CMC->GetMaxSpeed();
		CMC->MaxWalkSpeed=SprintSpeed;
	}

	FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(SprintGEClass,1.f);
	ActiveSprintGEHandle = ApplyGameplayEffectSpecToOwner(Handle,ActorInfo,ActivationInfo,GEHandle);

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddUniqueDynamic(this,&ThisClass::OnInputReleased);
	WaitInputReleaseTask->ReadyForActivation();
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CMC->MaxWalkSpeed=300.f;
	BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveSprintGEHandle);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

