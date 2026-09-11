#include "GA_Mage_FlameWall.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FlameWallArea.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"


UGA_Mage_FlameWall::UGA_Mage_FlameWall()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.FlameWall")));
}

void UGA_Mage_FlameWall::ActivateAbility(
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
	WaitTask->EventReceived.AddDynamic(this, &UGA_Mage_FlameWall::OnSetEvent);
	WaitTask->ReadyForActivation();
	
	if (FlameWallMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			FlameWallMontage,
			1.f,
			NAME_None,
			false
			);
		
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->ReadyForActivation();
	}
}

void UGA_Mage_FlameWall::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Mage_FlameWall::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Mage_FlameWall::OnSetEvent(const FGameplayEventData Payload)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !AreaClass) return;

	FVector EyeLoc;
	FRotator EyeRot;
	if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
	{
		PC->GetPlayerViewPoint(EyeLoc, EyeRot);
	}
	else
	{
		Char->GetActorEyesViewPoint(EyeLoc, EyeRot);
	}
	const FVector ViewDir = EyeRot.Vector();
	const FVector Target = EyeLoc + ViewDir * MaxSetRange;

	FHitResult Hit;
	FCollisionQueryParams Q(SCENE_QUERY_STAT(FlameWall_View), false, Char);
	Q.AddIgnoredActor(Char);

	UWorld* World = Char->GetWorld();
	bool bHitGround = World->LineTraceSingleByChannel(Hit, EyeLoc, Target, ECC_Visibility, Q);
	if (bHitGround && Hit.GetActor() && Hit.GetActor()->IsA<APawn>())
	{
		Q.AddIgnoredActor(Hit.GetActor());
		bHitGround = World->LineTraceSingleByChannel(Hit, EyeLoc, Target, ECC_Visibility, Q);
	}
	FVector AimPoint = bHitGround ? Hit.ImpactPoint : Target;

	const float DistFromChar = FVector::Dist(Char->GetActorLocation(), AimPoint);
	if (DistFromChar > MaxSetRange)
	{
		AimPoint = EyeLoc + ViewDir * MaxSetRange;;
	}
	else if (DistFromChar < MinSetRange)
	{
		AimPoint = EyeLoc + ViewDir * MinSetRange;
	}

	FHitResult GroundHit;
	const FVector GroundUp = AimPoint + FVector(0, 0, GroundTraceUp);
	const FVector GroundDown = AimPoint - FVector(0, 0, GroundTraceDown);
	if (!World->LineTraceSingleByChannel(GroundHit, GroundUp, GroundDown, ECC_Visibility, Q)) return;
	
	const FVector Loc = GroundHit.Location + FVector(0, 0, GroundOffset);
	FRotator Rot = EyeRot;
	Rot.Pitch = 0.f;
	Rot.Roll = 0.f;

	if (!CurrentActorInfo->IsNetAuthority())
	{
		ServerSpawnArea(Loc, Rot);
	}
}

void UGA_Mage_FlameWall::ServerSpawnArea_Implementation(const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	SpawnFlameWall(SpawnLoc, SpawnRot);
}

void UGA_Mage_FlameWall::SpawnFlameWall(const FVector& SpawnLoc, const FRotator& ViewRot)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!Char || !AreaClass || !Char->HasAuthority()) return;
	
	FActorSpawnParameters P;
	P.Owner = OwnerActor;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (AFlameWallArea* Area = Char->GetWorld()->SpawnActor<AFlameWallArea>(AreaClass, SpawnLoc, ViewRot, P))
	{
		Area->Init(WallLifetime);
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Char))
	{
		FGameplayCueParameters Params;
		Params.Location = SpawnLoc;
			
		ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.FlameWall.Set")), Params);
	}
}
