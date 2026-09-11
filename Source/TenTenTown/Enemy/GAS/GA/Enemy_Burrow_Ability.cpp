// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Burrow_Ability.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList/Worm.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

UEnemy_Burrow_Ability::UEnemy_Burrow_Ability()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    
    FGameplayTagContainer Tags = GetAssetTags();
    Tags.AddTag(GASTAG::Enemy_Ability_Burrow);
    SetAssetTags(Tags);
}

void UEnemy_Burrow_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AWorm* Worm = Cast<AWorm>(GetAvatarActorFromActorInfo());
    if (!Worm || !Worm->BurrowMontage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (UAnimInstance* AnimInst = Worm->GetMesh()->GetAnimInstance())
    {
        AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::OnNotifyBegin);
        AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnNotifyBegin);
    }


    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        Worm->BurrowMontage,
        0.8f);

    if (MontageTask)
    {
        MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnBurrowMontageFinished);
        MontageTask->Activate();
    }
    else
    {
        OnBurrowMontageFinished(); 
    }
}

void UEnemy_Burrow_Ability::OnBurrowMontageFinished()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    AActor* Actor = GetAvatarActorFromActorInfo();
    if (!ASC || !Actor) return;

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddInstigator(Actor, Actor);

    if (InvulnerableEffect)
    {
        FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(InvulnerableEffect, GetAbilityLevel(), EffectContext);
        ActiveInvulnerableGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }

    if (BurrowEffect)
    {
        FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(BurrowEffect, GetAbilityLevel(), EffectContext);
        ActiveBurrowGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }

}

void UEnemy_Burrow_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
    AWorm* Worm = Cast<AWorm>(GetAvatarActorFromActorInfo());

    UE_LOG(LogTemp, Warning, TEXT("EndAbility Activated!"));



    if (Worm && Worm->UnBurrowMontage) 
    {
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            Worm->UnBurrowMontage,
            1.0f);

        UE_LOG(LogTemp, Warning, TEXT("UnBurrow Montage Play!"));
        
        MontageTask->Activate();

    }

    CleanupState();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UEnemy_Burrow_Ability::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateCancelAbility)
{
    EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, false);
    
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UEnemy_Burrow_Ability::CleanupState()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    AActor* Actor = GetAvatarActorFromActorInfo();

    if (AWorm* Worm = Cast<AWorm>(Actor))
    {
        if (USkeletalMeshComponent* Mesh = Worm->GetMesh())
        {
            if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
            {
                AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::OnNotifyBegin);
            }
        }
    }

    if (ASC)
    {
        if (ActiveInvulnerableGEHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(ActiveInvulnerableGEHandle);
            ActiveInvulnerableGEHandle.Invalidate();
        }
        if (ActiveBurrowGEHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(ActiveBurrowGEHandle);
            ActiveBurrowGEHandle.Invalidate();
        }
    }

    if (AWorm* Worm = Cast<AWorm>(GetAvatarActorFromActorInfo()))
    {
        if (USkeletalMeshComponent* Mesh = Worm->GetMesh())
        {
            Mesh->SetVisibility(true);

            UE_LOG(LogTemp, Warning, TEXT("Invisible End"))
        }
        if (UCapsuleComponent* Capsule = Worm->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}

void UEnemy_Burrow_Ability::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());

    UE_LOG(LogTemp, Warning, TEXT("Notify Name: %s"), *NotifyName.ToString());

    if (!Actor) return;

    if (NotifyName == FName("BurrowStart"))
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            Mesh->SetVisibility(false);

            UE_LOG(LogTemp, Warning, TEXT("Invisible Start"));

        }
        if (UCapsuleComponent* Capsule = Actor->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
    }
    else if (NotifyName == FName("BurrowEnd"))
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            Mesh->SetVisibility(true);

            UE_LOG(LogTemp, Warning, TEXT("Invisible End"))
        }
        if (UCapsuleComponent* Capsule = Actor->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}
