#include "GA_Mage_Fireball.h"

#include "GameFramework/Character.h"
#include "FireballProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/StaticMeshComponent.h"

UGA_Mage_Fireball::UGA_Mage_Fireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.Fireball")));
}

void UGA_Mage_Fireball::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bSkillEmpowered = false;
	
	ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AMageCharacter* Mage = Cast<AMageCharacter>(Char);
	if (!Mage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (APlayerState* PS = Cast<APlayerState>(Mage->GetPlayerState()))
	{
		if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
		{
			UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent();
			if (const UAS_MageAttributeSet* MageAS = Cast<UAS_MageAttributeSet>(ASC->GetAttributeSet(UAS_MageAttributeSet::StaticClass())))
			{
				if (MageAS->GetOverheatingStack() >= MageAS->NeedOverheatingStack)
				{
					bSkillEmpowered = true;
				}
			}
		}
	}

	if (FireballMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			FireballMontage,
			1.f,
			NAME_None,
			false
		);
		
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		PlayTask->ReadyForActivation();
	}

	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ShootTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &UGA_Mage_Fireball::OnShootEvent);
	WaitTask->ReadyForActivation();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Mage_Fireball::OnMontageCompleted()
{
	if (!bSkillEmpowered)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}

void UGA_Mage_Fireball::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Mage_Fireball::OnShootEvent(const FGameplayEventData Payload)
{
	AMageCharacter* Mage = Cast<AMageCharacter>(GetAvatarActorFromActorInfo());
	if (!Mage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	UStaticMeshComponent* WandMesh = Mage->GetWandMesh();

	// === 파이어 볼 ===
	if (!bSkillEmpowered)
	{
		if (!ProjectileClass) return;

		//파이어 볼 스폰 위치: 소켓 (없으면 캐릭터)
		FTransform MuzzleXf = Mage->GetActorTransform();
		if (WandMesh && WandMesh->DoesSocketExist(MuzzleSocketName))
		{
			MuzzleXf = WandMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
		}
		const FVector SocketLoc = MuzzleXf.GetLocation();

		FVector EyeLoc;
		FRotator EyeRot;
		if (APlayerController* PC = Cast<APlayerController>(Mage->GetController()))
		{
			PC->GetPlayerViewPoint(EyeLoc, EyeRot);
		}
		else
		{
			Mage->GetActorEyesViewPoint(EyeLoc, EyeRot);
		}
	
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
        {
            FGameplayCueParameters Params;
            Params.Location = SocketLoc;
			
			ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.Fireball.Shoot")), Params);
        }
            
		const FVector AimDir = EyeRot.Vector();
		const FVector TraceEnd = EyeLoc + AimDir * 10000.f;
		FHitResult Hit;
		FCollisionQueryParams Q(SCENE_QUERY_STAT(FB_Aim), true, Mage);
		Q.AddIgnoredActor(Mage);

		if (WandMesh) Q.AddIgnoredComponent(WandMesh);

		Mage->GetWorld()->LineTraceSingleByChannel(Hit, EyeLoc, TraceEnd, ECC_Visibility, Q);

		const FVector AimPoint = Hit.bBlockingHit ? Hit.ImpactPoint : TraceEnd;

		FVector Dir = (AimPoint - SocketLoc).GetSafeNormal();
		if (Dir.IsNearlyZero()) Dir = AimDir;
		
		const FVector Loc = SocketLoc;
		const FRotator Rot = Dir.Rotation();
		
		if (!CurrentActorInfo->IsNetAuthority())
		{
			ServerSpawnProjectile(Loc, Rot);
		}
	}
	// === 강화 파이어 볼 (메테오 레인) ===
	else
	{
		EmpoweredShot(Mage);
	}
}

void UGA_Mage_Fireball::ServerSpawnProjectile_Implementation(const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	SpawnFireball(SpawnLoc, SpawnRot);
}

void UGA_Mage_Fireball::SpawnFireball(const FVector& SpawnLoc, const FRotator& ViewRot)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!Char || !ProjectileClass || !Char->HasAuthority()) return;
	UWorld* World = Char->GetWorld();
	if (!World) return;
	
	FActorSpawnParameters P;
	P.Owner = OwnerActor;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AFireballProjectile* Proj = Char->GetWorld()->SpawnActor<AFireballProjectile>(ProjectileClass, SpawnLoc, ViewRot, P))
	{
		Proj->SetOwner(Char);
		Proj->SetReplicateMovement(true);
		Proj->SetNetUpdateFrequency(120.f);
		Proj->SetMinNetUpdateFrequency(60.f);
		Proj->SetNetDormancy(DORM_Awake);

		Proj->bCountsForOverheat = true;
		Proj->InitVelocity(ViewRot.Vector(), MuzzleSpeed);
		Char->MoveIgnoreActorAdd(Proj);
	}
}

void UGA_Mage_Fireball::EmpoweredShot(AMageCharacter* Mage)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	
	UWorld* World = Char->GetWorld();
	if (!World) return;

	const float ForwardOffset = 1500.f;
	FVector Dir = Char->GetActorForwardVector();
	Dir.Z = 0.f;
	Dir.Normalize();

	const FVector TraceOrigin = Char->GetActorLocation() + Dir * ForwardOffset;
	
	const FVector Start = TraceOrigin + FVector(0.f, 0.f, 50.f);
	const FVector End = TraceOrigin - FVector(0.f, 0.f, 2000.f);
	
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MeteorCenterTrace), false, Char);

	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		MeteorCenter = Hit.ImpactPoint + FVector(0.f, 0.f, 5.f) + Dir * 2;
	}
	
	Mage->ConsumeOverheatingStack();
	
	if (CurrentActorInfo && !CurrentActorInfo->IsNetAuthority())
	{
		ServerStartMeteorTelegraph(MeteorCenter);
	}
	else if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		StartMeteorTelegraph(MeteorCenter);
	}
}

void UGA_Mage_Fireball::ServerStartMeteorTelegraph_Implementation(const FVector& Center)
{
	StartMeteorTelegraph(Center);
}

void UGA_Mage_Fireball::StartMeteorTelegraph(const FVector& Center)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	AMageCharacter* Mage = Cast<AMageCharacter>(Char);
	if (!Mage) return;
	
	if (!MeteorCircleVFX || !Char->HasAuthority()) return;

	Mage->Multicast_SpawnMeteorTelegraph(Center, MeteorCircleVFX);

	MeteorCenter = Center;
	
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, TelegraphTime);
	DelayTask->OnFinish.AddDynamic(this, &UGA_Mage_Fireball::FinishMeteorTelegraph);
	DelayTask->ReadyForActivation();
}

void UGA_Mage_Fireball::SpawnMeteorRainAtPositions()
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!Char || !ProjectileClass || !Char->HasAuthority()) return;

	UWorld* World = Char->GetWorld();
	if (!World) return;
	
	for (int32 i = 0; i < MeteorCount; ++i)
	{
		const float Angle = FMath::FRandRange(0.f, 2.f * PI);
		const float Dist  = FMath::Sqrt(FMath::FRand()) * MeteorRadius;
		const float X     = FMath::Cos(Angle) * Dist;
		const float Y     = FMath::Sin(Angle) * Dist;

		FVector GroundPos = MeteorCenter + FVector(X, Y, 0.f);
		
		const float ExtraHeight = FMath::FRandRange(0.f, MeteorRandomExtraHeight);
		const FVector SpawnLoc  = GroundPos + FVector(0.f, 0.f, MeteorHeight + ExtraHeight);
		const FVector Dir       = (GroundPos - SpawnLoc).GetSafeNormal();
		const FRotator SpawnRot = Dir.Rotation();

		FActorSpawnParameters P;
		P.Owner = OwnerActor;
		P.Instigator = Char;
		P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayCueParameters Params;
			Params.Location = SpawnLoc;
			
			ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.Fireball.Shoot")), Params);
		}
		
		if (AFireballProjectile* Proj = World->SpawnActor<AFireballProjectile>(ProjectileClass, SpawnLoc, SpawnRot, P))
		{
			Proj->SetOwner(Char);
			Proj->SetReplicateMovement(true);
			Proj->SetNetUpdateFrequency(120.f);
			Proj->SetMinNetUpdateFrequency(60.f);
			Proj->SetNetDormancy(DORM_Awake);

			Proj->bCountsForOverheat = false;
			Proj->InitVelocity(Dir, MeteorSpeed);
			Char->MoveIgnoreActorAdd(Proj);
			
		}
	}
}

void UGA_Mage_Fireball::FinishMeteorTelegraph()
{
	SpawnMeteorRainAtPositions();
	bSkillEmpowered = false;
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

