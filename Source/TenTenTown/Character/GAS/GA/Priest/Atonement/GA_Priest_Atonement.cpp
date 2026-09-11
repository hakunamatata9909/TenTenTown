#include "GA_Priest_Atonement.h"

#include "AbilitySystemComponent.h"
#include "AtonementActor.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

UGA_Priest_Atonement::UGA_Priest_Atonement()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Priest.Atonement")));
}

void UGA_Priest_Atonement::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SetTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnSetEvent);
	WaitTask->ReadyForActivation();
	
	if (AtonementMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AtonementMontage,
			1.f,
			NAME_None,
			false
			);
		
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->ReadyForActivation();
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Priest_Atonement::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Priest_Atonement::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Priest_Atonement::OnSetEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo) return;
	if (!CurrentActorInfo->IsLocallyControlled()) return;
		
	const ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	
	const FVector SpawnLoc = Char->GetActorLocation();
	const FRotator SpawnRot = FRotator::ZeroRotator;
	
	Server_SpawnArea(SpawnLoc, SpawnRot);
}

void UGA_Priest_Atonement::Server_SpawnArea_Implementation(const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	SpawnArea(SpawnLoc, SpawnRot);
}

void UGA_Priest_Atonement::SpawnArea(const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !AoEActorClass || !Char->HasAuthority()) return;
	
	FActorSpawnParameters P;
	P.Owner = Char;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	
	if (AAtonementActor* Area = Char->GetWorld()->SpawnActor<AAtonementActor>(AoEActorClass, SpawnLoc, SpawnRot, P))
	{
		Area->AttachToComponent(Char->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}
