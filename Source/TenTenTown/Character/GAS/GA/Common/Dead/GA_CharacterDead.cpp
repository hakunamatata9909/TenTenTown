// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CharacterDead.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_CharacterDead::UGA_CharacterDead()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag= GASTAG::Event_Character_Dead;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

bool UGA_CharacterDead::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	
	UAbilitySystemComponent* ASCTmp = ActorInfo->AbilitySystemComponent.Get();
	
	float CurrentHealth = ASCTmp->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
	
	if (CurrentHealth <= KINDA_SMALL_NUMBER) return true;
	
	return false;
}

void UGA_CharacterDead::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	AvatarCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter) return;
	
	UAnimMontage* DeadMontage = AvatarCharacter->GetDeathMontage();
	if (!DeadMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("no Dead Montage in CharacterDeadAbility")));
		return;
	}
	
	LastMovementMode = AvatarCharacter->GetCharacterMovement()->GetGroundMovementMode();
	
	

	//주요 사망처리 로직 이후 Revive에서 다시 활성화 해줘야 한다.
	ASC->AddLooseGameplayTag(GASTAG::State_Character_Dead);
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	AvatarCharacter->bUseControllerRotationYaw=false;

	if (UCapsuleComponent* Capsule = AvatarCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetGenerateOverlapEvents(false);
	}
	
	if (USkeletalMeshComponent* Mesh = AvatarCharacter->GetMesh())
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetGenerateOverlapEvents(false);
	}
	
	auto* PlayDeadMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("None"),AvatarCharacter->GetDeathMontage(),1.f);
	PlayDeadMontage->ReadyForActivation();
	
	auto* RespawnTimeTask = UAbilityTask_WaitDelay::WaitDelay(this,5.f);
	RespawnTimeTask->OnFinish.AddUniqueDynamic(this,&ThisClass::OnWaitDelayEnd);
	RespawnTimeTask->ReadyForActivation();
}

void UGA_CharacterDead::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	FGameplayEventData Payload;
	Payload.EventMagnitude = static_cast<float>(LastMovementMode);
	SendGameplayEvent(GASTAG::Event_Character_Revive,Payload);
	ASC->RemoveLooseGameplayTag(GASTAG::State_Character_Dead);
	
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(GASTAG::State_Character_Dead);
	}

	if (AvatarCharacter && AvatarCharacter->GetMesh() && AvatarCharacter->GetMesh()->GetAnimInstance())
	{
		AvatarCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, AvatarCharacter->GetDeathMontage());
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CharacterDead::OnWaitDelayEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
