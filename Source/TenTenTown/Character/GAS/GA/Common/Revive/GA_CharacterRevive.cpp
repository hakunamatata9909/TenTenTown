// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CharacterRevive.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

UGA_CharacterRevive::UGA_CharacterRevive()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag= GASTAG::Event_Character_Revive;
	TriggerData.TriggerSource=EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_CharacterRevive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("Hello this is revive activate")));
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter) return;
	
	ReviveMontage = Cast<ABaseCharacter>(AvatarCharacter)->GetReviveMontage();
	if (!ReviveMontage) return;
	
	LastMovementMode =static_cast<EMovementMode>(TriggerEventData->EventMagnitude);
	
	TArray<AActor*> PlayerStartLocation;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),PlayerStartLocation);
	
	if (HasAuthority(&ActivationInfo)&&PlayerStartLocation.Num()>0)
	{
		int32 RandomInt = FMath::RandRange(0,PlayerStartLocation.Num()-1);
		FVector RandomSpawnPos = PlayerStartLocation[RandomInt]->GetActorLocation();
		AvatarCharacter->SetActorLocation(RandomSpawnPos);

		const float MaxH = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());
		const float NewH = FMath::Max(0.f, MaxH * 0.5);
		ASC->SetNumericAttributeBase(UAS_CharacterBase::GetHealthAttribute(),NewH);
	}
	
	UAbilityTask_PlayMontageAndWait* ReviveAMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("ReviveAnim"),ReviveMontage);
	ReviveAMTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	ReviveAMTask->ReadyForActivation();
}

void UGA_CharacterRevive::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CharacterRevive::OnMontageEnd()
{
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	AvatarCharacter->bUseControllerRotationYaw=true;
	ASC->RemoveLooseGameplayTag(GASTAG::State_Character_Dead);

	if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(AvatarCharacter))
	{
		if (UCapsuleComponent* Capsule = BaseChar->GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("CharacterMesh"));
			Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Capsule->SetGenerateOverlapEvents(true);
		}

		if (USkeletalMeshComponent* Mesh = AvatarCharacter->GetMesh())
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Mesh->SetGenerateOverlapEvents(false);
		}
	}
	
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
