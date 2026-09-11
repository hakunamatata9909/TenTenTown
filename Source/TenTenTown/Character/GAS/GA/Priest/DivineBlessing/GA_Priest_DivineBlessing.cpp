#include "GA_Priest_DivineBlessing.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"

UGA_Priest_DivineBlessing::UGA_Priest_DivineBlessing()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Priest.DivineBlessing")));
}

void UGA_Priest_DivineBlessing::OnSelectionStarted()
{
	CurrentPreviewTarget = nullptr;
	ClearPreviewVFX();
}
 
void UGA_Priest_DivineBlessing::SetPreviewTarget(ABaseCharacter* NewTarget)
{
	if (NewTarget == CurrentPreviewTarget) return;
	CurrentPreviewTarget = NewTarget;
	
	if (!CurrentActorInfo || !CurrentActorInfo->IsLocallyControlled()) return;
	UpdatePreviewVFX();
}

void UGA_Priest_DivineBlessing::OnConfirmed(const FGameplayEventData& Payload)
{
	if (!CurrentPreviewTarget)
	{
		OnCanceled(Payload);
		return;
	}
	
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		OnCanceled(Payload);
		return;
	}

	if (CastMontage)
	{
		PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			CastMontage,
			1.f,
			NAME_None,
			false
		);
		
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnCastMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCastMontageCancelled);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnCastMontageCancelled);
		PlayTask->ReadyForActivation();
	}

	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ApplyTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &UGA_Priest_DivineBlessing::ApplyBuff);
	WaitTask->ReadyForActivation();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->ForceReplication();
}

void UGA_Priest_DivineBlessing::OnCanceled(const FGameplayEventData& Payload)
{
	CurrentPreviewTarget = nullptr;
	ClearPreviewVFX();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Priest_DivineBlessing::OnCastMontageCompleted()
{

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Priest_DivineBlessing::OnCastMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Priest_DivineBlessing::ApplyBuff(const FGameplayEventData Payload)
{
	Server_ApplyBuff(CurrentPreviewTarget);
}


void UGA_Priest_DivineBlessing::Server_ApplyBuff_Implementation(ABaseCharacter* Target)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentPreviewTarget);
	if (SourceASC && TargetASC && BuffGE)
	{
		const FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(BuffGE, 1, Ctx);
		if (Spec.IsValid())
		{
			const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
			const float BuffValue = BaseAtk * BuffMultiplier;
			Spec.Data->SetSetByCallerMagnitude(Tag_Buff, BuffValue);
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	}
	
	CurrentPreviewTarget = nullptr;
	ClearPreviewVFX();
}

void UGA_Priest_DivineBlessing::UpdatePreviewVFX()
{
	ClearPreviewVFX();
	if (!CurrentPreviewTarget || !PreviewEffect) return;

	USceneComponent* Root= CurrentPreviewTarget->GetRootComponent();
	PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		PreviewEffect,
		Root,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
	);
}

void UGA_Priest_DivineBlessing::ClearPreviewVFX()
{
	if (PreviewComponent)
	{
		PreviewComponent->Deactivate();
		PreviewComponent->DestroyComponent();
		PreviewComponent = nullptr;
	}
}

void UGA_Priest_DivineBlessing::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	CurrentPreviewTarget = nullptr;
	ClearPreviewVFX();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
