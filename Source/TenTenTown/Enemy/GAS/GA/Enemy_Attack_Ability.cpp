// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_Attack_Ability.h"
#include "Enemy/Base/EnemyBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Attack_Ability::UEnemy_Attack_Ability()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = GASTAG::Enemy_Ability_Attack;
    AbilityTriggers.Add(TriggerData);

    ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);
}

bool UEnemy_Attack_Ability::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UEnemy_Attack_Ability::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }

    if (TriggerEventData->Target == nullptr)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }

    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        ASC->AddLooseGameplayTag(GASTAG::Enemy_Ability_Attack); 
    }

    if (TriggerEventData)
    {
        Actor = const_cast<AEnemyBase*>(Cast<AEnemyBase>(TriggerEventData->Instigator.Get()));
        CurrentTarget = const_cast<AActor*>(TriggerEventData->Target.Get());
        
        FVector TargetLocation = CurrentTarget->GetActorLocation();
        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
        Actor->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
        
        PlayAttackMontage();
    }
}

//공격속도를 재생속도로 적용하여 Montage 재생
void UEnemy_Attack_Ability::PlayAttackMontage()
{
    if (!Actor || !Actor->AttackMontage)
    {
        return;
    }
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
    {
        AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
    }

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
    (
        this,
        NAME_None,
        Actor->AttackMontage,
        AttackSpeed
    );

    if (!Task)
    {
        return;
    }
    Task->OnCompleted.AddDynamic(this, &UEnemy_Attack_Ability::OnMontageEnded);
    Task->OnBlendOut.AddDynamic(this, &UEnemy_Attack_Ability::OnMontageEnded);
    Task->OnInterrupted.AddDynamic(this, &UEnemy_Attack_Ability::OnMontageEnded);
    Task->OnCancelled.AddDynamic(this, &UEnemy_Attack_Ability::OnMontageEnded);


    if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
    {
        UAnimInstance* AnimInst = Actor->GetMesh()->GetAnimInstance();
        AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_Attack_Ability::OnNotifyBegin);
    }
    Task->ReadyForActivation();
}

//공격력만큼 데미지 적용
void UEnemy_Attack_Ability::ApplyDamageToTarget(AActor* TargetActor)
{    
    if (!TargetActor || !Actor)
    {
        return;
    }
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

    if (!ASC || !TargetASC)
    {
        return;
    }
    
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddInstigator(Actor, Actor);
    EffectContext.AddOrigin(Actor->GetActorLocation());

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);

    if (SpecHandle.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackAttribute()));
        
        ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

        UE_LOG(LogTemp, Warning, TEXT("Attack Effect Executed"));

    }
    
}

//Montage 종료 시 Ability 종료
void UEnemy_Attack_Ability::OnMontageEnded()
{
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

//Notify 시작 시 DetectComponent에 감지된 액터에게 데미지 적용
void UEnemy_Attack_Ability::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == FName("AttackHit") && Actor && Actor->HasAuthority())
    {
        if (CurrentTarget)
        {
            USphereComponent* Detect = Actor->GetDetectComponent();
            if (Detect->IsOverlappingActor(CurrentTarget))
            {
                ApplyDamageToTarget(CurrentTarget);
            }
        }
    }
    
}

void UEnemy_Attack_Ability::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
    {
        Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UEnemy_Attack_Ability::OnNotifyBegin);
    }

    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        ASC->RemoveLooseGameplayTag(GASTAG::Enemy_Ability_Attack);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
