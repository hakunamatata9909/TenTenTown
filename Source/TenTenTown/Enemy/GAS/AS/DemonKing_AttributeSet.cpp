// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/DemonKing_AttributeSet.h"
#include "Enemy/EnemyList/DemonKing.h"
#include "GameplayEffectExtension.h"

void UDemonKing_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float IncomingDamage = GetDamage();
		SetDamage(0.f);

		if (IncomingDamage <= 0.f)
		{
			return;
		}
		
		float OldHealth = GetHealth();
		float NewHealth = FMath::Clamp(OldHealth - IncomingDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);
		
		ADemonKing* DemonKing = Cast<ADemonKing>(ASC->GetOwnerActor());

		if (!DemonKing)
		{
			return;
		}
		
		if (NewHealth <= 0.f && OldHealth > 0.f)
		{
			if (ASC && ASC->GetOwnerRole() == ROLE_Authority)
			{
				ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Dead);

				FGameplayEventData Payload;
				Payload.EventTag = GASTAG::Enemy_Ability_Dead;
				ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
			}
		}

		
		float HealthRatio = NewHealth / GetMaxHealth();
		if (HealthRatio <= DemonKing->BerserkHealthThreshold && !bBerserkEffectApplied)
		{
			
			if (ASC->GetOwnerRole() == ROLE_Authority)
			{
				bBerserkEffectApplied = true;
				ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Berserk);

				if (DemonKing->EnemyBerserk)
				{
					FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
					ASC->ApplyGameplayEffectToSelf(
						DemonKing->EnemyBerserk->GetDefaultObject<UGameplayEffect>(),
						1.0f,
						Context
					);
				}
			}
		}
	}

}