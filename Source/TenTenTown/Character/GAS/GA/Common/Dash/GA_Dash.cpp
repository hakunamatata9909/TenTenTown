// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dash.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "KismetAnimationLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterStamina.h"

UGA_Dash::UGA_Dash()
{
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Strength = 1500.f;
	Duration = 0.3f;
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		ATTTPlayerState* PS = Cast<ATTTPlayerState>(GetOwningActorFromActorInfo());
		if (!PS) return false;

		UAbilitySystemComponent* ASC =  PS->GetAbilitySystemComponent();

		if (ASC)
		{
			const UAS_CharacterStamina* AS = ASC->GetSet<UAS_CharacterStamina>();
			if (AS)
			{
				float CurrentStamina = AS->GetStamina();
				if (CurrentStamina<5.f)
					return false;
				else
					return true;
			}
		}
	}
	return false;
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo))
	{
		return;
	}

	FVector DirectionVector = GetRecentDirection().GetSafeNormal();
	FGameplayCueParameters Parameters;
	Parameters.Normal = DirectionVector;
	
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GASTAG::GameplayCue_Dash,Parameters);
	
	auto* RootMotionForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,FName("Dash"),GetRecentDirection(),Strength,Duration,false,nullptr,ERootMotionFinishVelocityMode::ClampVelocity,
		FVector::ZeroVector,GetMaxSpeed(),false);
	RootMotionForceTask->OnFinish.AddUniqueDynamic(this,&ThisClass::UGA_Dash::OnTaskFinished);

	auto* PlayAMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("DashAM"),DashMontage4Direction,1.f,GetRecentDirectionName());
	
	RootMotionForceTask->ReadyForActivation();
	PlayAMTask->ReadyForActivation();
}


void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dash::OnTaskFinished()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

FName UGA_Dash::GetRecentDirectionName()
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter) return "FWD";
	
	const FVector RecentDircetion = AvatarCharacter->GetCharacterMovement()->GetCurrentAcceleration();
	const FRotator CurrentRotator = AvatarCharacter->GetActorRotation();
	
	if (RecentDircetion == FVector::ZeroVector) return "FWD";

	float DirectionFloat = UKismetAnimationLibrary::CalculateDirection(RecentDircetion,CurrentRotator);

	if (abs(DirectionFloat)>=135.f) return "BWD";
	if (DirectionFloat>45.f) return "RGT";
	if (DirectionFloat<-45.f) return "LFT";
	return "FWD";
}

FVector UGA_Dash::GetRecentDirection()
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (AvatarCharacter)
	{
		FVector RecentDirection = AvatarCharacter->GetCharacterMovement()->GetCurrentAcceleration();

		if (RecentDirection!=FVector::ZeroVector)
		{
			RecentDirection = RecentDirection.GetSafeNormal();
			return RecentDirection;
		}
		else
		{
			return AvatarCharacter->GetActorForwardVector();
		}
	}
	return AvatarCharacter->GetActorForwardVector();
}

float UGA_Dash::GetMaxSpeed()
{
	
	return 500.f;
}
