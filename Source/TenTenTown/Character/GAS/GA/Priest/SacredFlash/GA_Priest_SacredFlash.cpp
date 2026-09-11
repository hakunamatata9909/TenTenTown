#include "GA_Priest_SacredFlash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Priest/PriestCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Enemy/Base/EnemyBase.h"


UGA_Priest_SacredFlash::UGA_Priest_SacredFlash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Priest.SacredFlash")));
}

void UGA_Priest_SacredFlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (FlashMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			FlashMontage,
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
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnShootEvent);
	WaitTask->ReadyForActivation();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Priest_SacredFlash::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Priest_SacredFlash::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Priest_SacredFlash::OnShootEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;

	APriestCharacter* Priest = Cast<APriestCharacter>(GetAvatarActorFromActorInfo());
	if (!Priest) return;

	UStaticMeshComponent* WandMesh = Priest->GetWandMesh();
	if (!WandMesh) return;
	
	if (UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Priest))
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = Priest;
		CueParams.EffectCauser = Priest;
		CueParams.SourceObject = this;

		SourceASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Priest.SacredFlash.Shoot")), CueParams);
	}
	
	const FVector Start = WandMesh->GetSocketLocation(Priest->GetWandSocketName());
	const FRotator ControlRot = Priest->GetControlRotation();
	const FVector Dir = ControlRot.Vector().GetSafeNormal();
	const FVector End = Start + Dir * HitRange;

	FRotator YawOnlyRot(0.f, ControlRot.Yaw, 0.f);
	const FQuat Rot = YawOnlyRot.Quaternion();
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PriestSacredFlashTrace), false, Priest);
	Params.bReturnPhysicalMaterial = false;
	Params.AddIgnoredActor(Priest);

	TArray<FHitResult> HitResults;
	const FCollisionShape HitBox = FCollisionShape::MakeBox(HitBoxHalfSize);

	const bool bHit = Priest->GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		Rot,
		ECollisionChannel::ECC_Pawn,
		HitBox,
		Params
	);
	
	if (!bHit) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Priest);
	if (!SourceASC) return;

	TSet<TWeakObjectPtr<AActor>> UniqueActors;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		if (UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
		if (!TargetASC) continue;
		
		ABaseCharacter* Char = Cast<ABaseCharacter>(HitActor);
		AEnemyBase* Enemy = Cast<AEnemyBase>(HitActor);
		
		if (Char && ShieldGE)
		{
			ApplyGEToASC(SourceASC, TargetASC, ShieldGE, 1.f, ShieldTag, ShieldAmount, ShieldMultiplier);
			ApplyGEToASC(SourceASC, TargetASC, ShieldActiveGE, 1.f, ShieldTag, 0.f, 0.f);
		}
		
		if (Enemy)
		{
			ApplyGEToASC(SourceASC, TargetASC, DamageGE, 1.f, DamageTag, DamageAmount, DamageMultiplier);
		}
	}
}

void UGA_Priest_SacredFlash::ApplyGEToASC(
	UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC,
	TSubclassOf<UGameplayEffect> GEClass,
	float Level,
	FGameplayTag  SetByCallerTag,
	float SetByCallerAmount,
	float SetByCallerMultiplier) const
{
	if (!SourceASC || !TargetASC || !*GEClass) return;
	if (!SetByCallerTag.IsValid()) return;
	
	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GEClass, Level, Ctx);
	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec && SetByCallerTag.IsValid())
		{
			const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
			const float SetByCallerValue = SetByCallerAmount + BaseAtk * SetByCallerMultiplier;
			Spec->SetSetByCallerMagnitude(SetByCallerTag, SetByCallerValue);
		}
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void UGA_Priest_SacredFlash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


