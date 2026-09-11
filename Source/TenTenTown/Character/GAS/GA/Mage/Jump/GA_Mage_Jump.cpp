#include "GA_Mage_Jump.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Fighter/FighterCharacter.h"

UGA_Mage_Jump::UGA_Mage_Jump()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Jump")));

	Char = nullptr;
}

void UGA_Mage_Jump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	UAbilityTask_WaitMovementModeChange* WaitMovementModeChange =
		UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this,EMovementMode::MOVE_Walking);
	WaitMovementModeChange->OnChange.AddUniqueDynamic(this,&ThisClass::OnLanded);
	WaitMovementModeChange->ReadyForActivation();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters Params;
		Params.Location = Char->GetActorLocation();
			
		ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.Jump")), Params);
	}
	Char->Jump();
}

void UGA_Mage_Jump::OnLanded(EMovementMode MovementMode)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_Jump::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}