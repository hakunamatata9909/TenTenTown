#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/PS/TTTPlayerState.h"
#include "Enemy/Base/EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

UAS_EnemyAttributeSetBase::UAS_EnemyAttributeSetBase()
{
	InitHealth(7.f);
	InitMaxHealth(777.f);
	InitAttack(777.f);
    InitCoreAttack(77.f);
	InitAttackSpeed(7.f);
	InitMovementSpeed(777.f);
	InitMovementSpeedRate(1.f);
	InitAttackRange(777.f);
	InitGold(777.f);
	InitExp(7.f);
    
    InitDropPercent(0.f);
    InitDamage(0.f);
    InitVulnerable(0.f);
}

void UAS_EnemyAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        const float Rate = FMath::Max(GetVulnerable(), 0.f);
        const float IncomingDamage = GetDamage() * (1 + Rate);
        SetDamage(0.f);

        if (IncomingDamage <= 0.f)
            return;
        
        float OldHealth = GetHealth();
        float NewHealth = FMath::Clamp(OldHealth - IncomingDamage, 0.f, GetMaxHealth());

        SetHealth(NewHealth);
        
        if (NewHealth <= 0.f && OldHealth > 0.f)
        {
            if (ASC && ASC->GetOwnerRole() == ROLE_Authority)
            {
                // 킬카운트 올림
                const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
                if (Context.IsValid())
                {
                    AActor* InstigatorActor = Context.GetInstigator();
                    
                    if (InstigatorActor->IsA<ABaseCharacter>())
                    {
                        ABaseCharacter *BaseCharacter = Cast<ABaseCharacter>(InstigatorActor);
                        ATTTPlayerState* PS = Cast<ATTTPlayerState>(BaseCharacter->GetPlayerState());

                        //킬 카운트 함수 호출
                        PS->AddKillcount(1);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Instigator is not BaseCharacter"));
                    }
                }
              
                // Dead어빌리티 호출
                ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Dead);

                FGameplayEventData Payload;
                Payload.EventTag = GASTAG::Enemy_Ability_Dead;
                ASC->HandleGameplayEvent(Payload.EventTag, &Payload);

            }
        }
    }
    if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
    {
        // 이 속성을 가진 액터
        AActor* TargetActor = Data.Target.GetAvatarActor();
        if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
        {
            // MaxWalkSpeed에 변경된 속성값을 적용
            UCharacterMovementComponent* MoveComp = TargetCharacter->GetCharacterMovement();
            if (MoveComp)
            {
                MoveComp->MaxWalkSpeed = GetMovementSpeed();
                
                UE_LOG(LogTemp, Log, TEXT("[GAS] Speed Changed: %.1f"), GetMovementSpeed());
            }
        }
    }
}

void UAS_EnemyAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);

    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Attack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, CoreAttack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, MovementSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, MovementSpeedRate, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, AttackSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, AttackRange, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Gold, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Exp, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, DropPercent, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAS_EnemyAttributeSetBase, Vulnerable, COND_None, REPNOTIFY_Always);
}

void UAS_EnemyAttributeSetBase::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Damage, OldDamage);
}

void UAS_EnemyAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Health, OldHealth);
}

void UAS_EnemyAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UAS_EnemyAttributeSetBase::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Attack, OldAttack);
}

void UAS_EnemyAttributeSetBase::OnRep_CoreAttack(const FGameplayAttributeData& OldCoreAttack)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, CoreAttack, OldCoreAttack);
}

void UAS_EnemyAttributeSetBase::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, MovementSpeed, OldMovementSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_MovementSpeedRate(const FGameplayAttributeData& OldMovementSpeedRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, MovementSpeedRate, OldMovementSpeedRate);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, AttackSpeed, OldAttackSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, AttackRange, OldAttackRange);
}

void UAS_EnemyAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Gold, OldGold);
}

void UAS_EnemyAttributeSetBase::OnRep_Exp(const FGameplayAttributeData& OldExp)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Exp, OldExp);
}

void UAS_EnemyAttributeSetBase::OnRep_DropPercent(const FGameplayAttributeData& OldDropPercent)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Exp, OldDropPercent);
}

void UAS_EnemyAttributeSetBase::OnRep_Vulnerable(const FGameplayAttributeData& OldVulnerable)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_EnemyAttributeSetBase, Vulnerable, OldVulnerable);
}
