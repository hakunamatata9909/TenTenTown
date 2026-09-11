// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Radius_Buff.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList/NagaWizard.h"
#include "Enemy/GAS/AS/NagaWizard_AttributeSet.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemy_Radius_Buff::UEnemy_Radius_Buff()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(GASTAG::Enemy_Ability_BuffNearBy);
	SetAssetTags(Tags);
}

bool UEnemy_Radius_Buff::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}


void UEnemy_Radius_Buff::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ANagaWizard* Actor = Cast<ANagaWizard>(GetAvatarActorFromActorInfo());

	if (!ASC || !Actor)
	{
		
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SkillAbility Activate"));

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
		this,
		NAME_None,
		Actor->BuffMontage,
		1.0f
	);

	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &UEnemy_Radius_Buff::OnMontageEnded);
		Task->OnBlendOut.AddDynamic(this, &UEnemy_Radius_Buff::OnMontageEnded);
		Task->OnInterrupted.AddDynamic(this, &UEnemy_Radius_Buff::OnMontageEnded);
		Task->OnCancelled.AddDynamic(this, &UEnemy_Radius_Buff::OnMontageEnded);
       
		Task->ReadyForActivation();
	}

	TArray<AActor*> OverlapActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; 
	TArray<AActor*> IgnoreActors;
	

	bool bResult = UKismetSystemLibrary::SphereOverlapActors(
		Actor->GetWorld(),
		Actor->GetActorLocation(),
		ASC->GetNumericAttribute(UNagaWizard_AttributeSet::GetBuffRadiusAttribute()),
		ObjectTypes, 
		AEnemyBase::StaticClass(),
		IgnoreActors,
		OverlapActors
	);

	if (bResult)
	{
		for (AActor* TargetActor : OverlapActors)
		{
			if (AEnemyBase* TargetEnemy = Cast<AEnemyBase>(TargetActor))
			{
				if (UAbilitySystemComponent* TargetASC = TargetEnemy->GetAbilitySystemComponent())
				{
					FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
					EffectContext.AddInstigator(Actor, Actor);
					
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
					   BuffEffect, 
					   GetAbilityLevel(), 
					   EffectContext
					);

					if (SpecHandle.IsValid())
					{
						const float BuffScale = ASC->GetNumericAttributeBase(UNagaWizard_AttributeSet::GetBuffScaleAttribute());
						const float BuffDuration = ASC->GetNumericAttributeBase(UNagaWizard_AttributeSet::GetBuffDurationAttribute());
						
						SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Enemy_State_Buffed, BuffScale);
						SpecHandle.Data->SetDuration(BuffDuration, true);
						
						ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

						UE_LOG(LogTemp, Warning, TEXT("Buff Applied"));

						
						//TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Buff, Params);
					}
				}
			}
		}
	}

	if (CastingEffect)
	{
		FGameplayEffectContextHandle CastingEffectContext = ASC->MakeEffectContext();
		CastingEffectContext.AddInstigator(Actor, Actor);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CastingEffect, GetAbilityLevel(), CastingEffectContext);
		CastingEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	if (CoolDownEffect)
	{
		FGameplayEffectContextHandle CoolDownContext = ASC->MakeEffectContext();
		CoolDownContext.AddInstigator(Actor, Actor);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CoolDownEffect, GetAbilityLevel(), CoolDownContext);

		float CoolDown = ASC->GetNumericAttribute(UNagaWizard_AttributeSet::GetBuffCoolDownAttribute());
		
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Cooldown_Enemy_Skill, CoolDown);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	

}

void UEnemy_Radius_Buff::OnMontageEnded()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && CastingEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(CastingEffectHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("Buff Ended"));
	
	OnAbilityCompleted.Broadcast();
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UEnemy_Radius_Buff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
