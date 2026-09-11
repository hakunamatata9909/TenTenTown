#include "GA_Mage_ComboAttack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

UGA_Mage_ComboAttack::UGA_Mage_ComboAttack()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.Attack")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Combo")));
}

void UGA_Mage_ComboAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !ComboAttackAM || ComboSections.Num() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AMageCharacter* Mage = Cast<AMageCharacter>(GetAvatarActorFromActorInfo());
	if (Mage)
	{
		if (USkeletalMeshComponent* Mesh = Mage->GetMesh())
		{
			AnimInstance = Mesh->GetAnimInstance();
		}
	}

	if (!AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ComboIdx = 0;
	bWindowOpen = false;
	bComboInput = false;

	WaitOpen = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OpenTag);
	WaitClose = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CloseTag);
	WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitTag);

	WaitOpen->EventReceived.AddUniqueDynamic(this, &ThisClass::OnOpen);
	WaitClose->EventReceived.AddUniqueDynamic(this, &ThisClass::OnClose);
	WaitHit->EventReceived.AddUniqueDynamic(this, &ThisClass::OnHit);

	WaitOpen->ReadyForActivation();
	WaitClose->ReadyForActivation();
	WaitHit->ReadyForActivation();
	
	PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(TEXT("MageCombo")),
		ComboAttackAM,
		1.f,
		NAME_None,
		true
	);
	PlayTask->OnCompleted.AddUniqueDynamic(this, &ThisClass::OnMontageCompleted);
	PlayTask->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnMontageInterrupted);
	PlayTask->ReadyForActivation();
	
	ASC->ForceReplication();
}

void UGA_Mage_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	  Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
    if (!IsActive()) return;

    if (!ASC) ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;
	
    if (ComboIdx == 0 && bWindowOpen && !bComboInput)
    {
        if (ComboSections.Num() > 1)
        {
            ASC->CurrentMontageSetNextSectionName(
                ComboSections[0],
                ComboSections[1]
            );
        }
    	
        ComboIdx    = 1;
        bComboInput = true;
    }
	
	//ASC->ForceReplication();
}

void UGA_Mage_ComboAttack::OnOpen(FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsLocallyControlled()) return;
	
	bWindowOpen = true;
}

void UGA_Mage_ComboAttack::OnClose(FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsLocallyControlled()) return;
	
	bWindowOpen = false;
}

void UGA_Mage_ComboAttack::OnHit(FGameplayEventData Payload)
{
	AMageCharacter* Mage = Cast<AMageCharacter>(GetAvatarActorFromActorInfo());
	if (Mage && ASC)
	{
		FGameplayCueParameters Params;
		Params.Location = Mage->GetActorLocation();
		
		FGameplayTag CueTag = (ComboIdx == 0) ?
		FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.ComboAttack.Attack1")):
		FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.ComboAttack.Attack2"));
		
		ASC->ExecuteGameplayCue(CueTag, Params);
	}
	
	DoTraceAndApply();
}

void UGA_Mage_ComboAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_ComboAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Mage_ComboAttack::DoTraceAndApply()
{
	
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;

	AMageCharacter* Mage = Cast<AMageCharacter>(GetAvatarActorFromActorInfo());
	if (!Mage) return;

	UStaticMeshComponent* WandMesh = Mage->GetWandMesh();
	if (!WandMesh) return;

	const FVector Start   = WandMesh->GetSocketLocation(WandSocketName);
	const FVector Forward = Mage->GetActorForwardVector();
	const FVector End     = Start + Forward * HitRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MageComboTrace), false, Mage);
	Params.bReturnPhysicalMaterial = false;
	Params.AddIgnoredActor(Mage);

	TArray<FHitResult> HitResults;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(HitRadius);

	const bool bHit = Mage->GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		Sphere,
		Params
	);
	
	if (!bHit) return;
	
	UAbilitySystemComponent* SourceASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Mage->GetOwner());
	if (!SourceASC) return;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();
		if (!TargetActor || TargetActor == Mage) continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor, true);
		if (!TargetASC) continue;

		FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
		Ctx.AddInstigator(Mage, Mage);
		Ctx.AddHitResult(Hit);

		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageGE, 1.f, Ctx);
		if (!Spec.IsValid()) continue;

		const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
		DamageAmount = BaseAtk;
		Spec.Data->SetSetByCallerMagnitude(Tag_Damage, -DamageAmount);
		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	}
}


void UGA_Mage_ComboAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	bWindowOpen = false;
	bComboInput = false;
	ComboIdx = 0;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
