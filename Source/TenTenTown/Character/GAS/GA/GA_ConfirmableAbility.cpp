#include "GA_ConfirmableAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "TTTGamePlayTags.h"

UGA_ConfirmableAbility::UGA_ConfirmableAbility()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_ConfirmableAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(GASTAG::State_IsSelecting);
	}
	
	OnSelectionStarted();
	
	ConfirmTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::Event_Confirm);
	if (ConfirmTask)
	{
		ConfirmTask->EventReceived.AddDynamic(this, &ThisClass::HandleConfirmEvent);
		ConfirmTask->ReadyForActivation();
	}
	
	CancelTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::Event_Cancel);
	if (CancelTask)
	{
		CancelTask->EventReceived.AddDynamic(this, &ThisClass::HandleCancelEvent);
		CancelTask->ReadyForActivation();
	}
}

void UGA_ConfirmableAbility::HandleConfirmEvent(FGameplayEventData Payload)
{
	OnConfirmed(Payload);
	
	ClearSelectingState();
}

void UGA_ConfirmableAbility::HandleCancelEvent(FGameplayEventData Payload)
{
	OnCanceled(Payload);
	
	ClearSelectingState();
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_ConfirmableAbility::ClearSelectingState()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
		{
			ASC->RemoveLooseGameplayTag(GASTAG::State_IsSelecting);
		}
	}
}

void UGA_ConfirmableAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearSelectingState();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}