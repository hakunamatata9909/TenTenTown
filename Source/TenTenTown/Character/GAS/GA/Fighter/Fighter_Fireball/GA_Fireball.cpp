// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Fireball.h"

#include "Fireball_Projectile.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Fireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASC = Cast<ATTTPlayerState>(GetOwningActorFromActorInfo())->GetAbilitySystemComponent();
	AvatarCharacter=Cast<ACharacter>(GetAvatarActorFromActorInfo());
	OriginSpeed = AvatarCharacter->GetCharacterMovement()->GetMaxSpeed();
	ChargingSeconds=0.f;
	
	if (!ASC||!AvatarCharacter)
	{
		return;
	}
	
	auto* AMTaskFireball = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskFireball"),FireballMontage,1.f,"Start");
	
	
	auto* WaitGameplayEventTask_Charging = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Charging,nullptr,true);
	WaitGameplayEventTask_Charging->EventReceived.AddUniqueDynamic(this,&ThisClass::ActiveLoopGameplayCue);
	
	auto* WaitGameplayEventTask_Release = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Release,nullptr,true);
	WaitGameplayEventTask_Release->EventReceived.AddUniqueDynamic(this,&ThisClass::LaunchFireball);
	
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=100.f;
	WaitGameplayEventTask_Charging->ReadyForActivation();
	WaitGameplayEventTask_Release->ReadyForActivation();
	AMTaskFireball->ReadyForActivation();
	
	ASC->ForceReplication();
}

void UGA_Fireball::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo);
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=OriginSpeed;
	ASC->RemoveGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Fireball::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	GetWorld()->GetTimerManager().ClearTimer(ChargingSecondHandle);
	ChargingSeconds = FMath::Clamp(ChargingSeconds,0.f,3.f);
	ASC->RemoveGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
	
	if (ChargingSeconds<0.1f)
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
		return;
	}
	
	auto* AMTaskRelease = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskRelease"),FireballReleaseMontage,1.5f);
	AMTaskRelease->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnAbilityEnd);
	
	AMTaskRelease->ReadyForActivation();
	ASC->ForceReplication();
}

void UGA_Fireball::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Fireball::ActiveLoopGameplayCue(const FGameplayEventData Data)
{
	ASC->AddGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
	
	GetWorld()->GetTimerManager().SetTimer(ChargingSecondHandle,
		[&]()
		{ChargingSeconds+=0.05f;},
		0.05f,
		true,
		-1
		);
}

void UGA_Fireball::LaunchFireball(const FGameplayEventData Data)
{
	APlayerController* PC = Cast<APlayerController>(AvatarCharacter->GetController());
	FVector Start;
	FRotator Rotation;
	PC->GetPlayerViewPoint(Start,Rotation);

	FVector End = Rotation.Vector()*10000.f+Start;
	
	FHitResult HitResult;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AvatarCharacter);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	bool bSuccessLineTrace = GetWorld()->LineTraceSingleByObjectType(HitResult,Start,End,ObjectQueryParams,CollisionQueryParams);

	FTransform SpawnTransform;
	FVector SpawnLocation = AvatarCharacter->GetActorLocation()+AvatarCharacter->GetActorForwardVector()*100.f;
	FRotator SpawnRotator = AvatarCharacter->GetActorRotation();
	float ScaleSize = FMath::Clamp(ChargingSeconds/3.f,0.f,1.f);
	
	FVector SpawnScale = FVector(ScaleSize,ScaleSize,ScaleSize);
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotator.Quaternion());
	SpawnTransform.SetScale3D(SpawnScale);
	
	if (bSuccessLineTrace)
	{

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);
		
		
		Proj->SetChargeSecFromAbility(ChargingSeconds);
		Proj->SetNiagaraScale(ChargingSeconds);
		Proj->SetSetbyCallerGameplayEffectClass(GEDamage);
		
		Proj->FinishSpawning(SpawnTransform);
		FVector Direction = HitResult.Location-SpawnLocation;
		Proj->FireProjectile(Direction,AvatarCharacter);
	}
	else
	{

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);
		
		Proj->FinishSpawning(SpawnTransform);
		
		Proj->FireProjectile(End-SpawnLocation,AvatarCharacter);
	}
}