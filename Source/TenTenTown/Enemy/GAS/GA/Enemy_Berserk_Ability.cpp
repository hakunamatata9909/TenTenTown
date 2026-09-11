// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_Berserk_Ability.h"
#include "Enemy/EnemyList/DemonKing.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UEnemy_Berserk_Ability::UEnemy_Berserk_Ability()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = GASTAG::Enemy_Ability_Berserk;
    AbilityTriggers.Add(TriggerData);

    ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);
}

void UEnemy_Berserk_Ability::ActivateAbility(
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

    ADemonKing* DemonKing = Cast<ADemonKing>(ActorInfo->AvatarActor.Get());
    if (!DemonKing || !DemonKing->BerserkMontage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilityTask_PlayMontageAndWait* Task =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, NAME_None, DemonKing->BerserkMontage, 1.0f);

    if (!Task)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    Task->OnCompleted.AddDynamic(this, &UEnemy_Berserk_Ability::OnMontageEnded);
    Task->OnBlendOut.AddDynamic(this, &UEnemy_Berserk_Ability::OnMontageEnded);
    Task->OnInterrupted.AddDynamic(this, &UEnemy_Berserk_Ability::OnMontageEnded);
    Task->OnCancelled.AddDynamic(this, &UEnemy_Berserk_Ability::OnMontageEnded);

    Task->ReadyForActivation();
}

void UEnemy_Berserk_Ability::OnMontageEnded()
{
    ADemonKing* DemonKing = Cast<ADemonKing>(GetAvatarActorFromActorInfo());
    if (DemonKing)
    {
        DemonKing->bBerserkPlayed = true; 
    }

    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
