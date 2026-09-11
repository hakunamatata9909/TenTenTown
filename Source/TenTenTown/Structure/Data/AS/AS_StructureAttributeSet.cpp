#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_StructureAttributeSet::UAS_StructureAttributeSet()
{
}

void UAS_StructureAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_StructureAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_StructureAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAS_StructureAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UE_LOG(LogTemp, Warning, TEXT("[DEBUG_AS] PostGE Executed! Attribute Name: %s"), *Data.EvaluatedData.Attribute.GetName());

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float Clamped = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		if (!FMath::IsNearlyEqual(Clamped, GetHealth()))
		{
			SetHealth(Clamped);
		}
		return;
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// --- [로그 추가] ---
		float LocalDamage = GetDamage();
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG_AS] Damage Attribute Hit! Value: %.1f"), LocalDamage);
		SetDamage(0.0f);
		
		if (LocalDamage > 0.0f)
		{
			float NewHealth = GetHealth() - LocalDamage;
			// 0 ~ MaxHealth 사이 클램핑
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
            
			UE_LOG(LogTemp, Warning, TEXT("[GAS] Damaged! Received: %.1f, Current Health: %.1f"), LocalDamage, GetHealth());
		}
	}
}

void UAS_StructureAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_StructureAttributeSet, Health, OldHealth);
}

void UAS_StructureAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_StructureAttributeSet, MaxHealth, OldMaxHealth);
}
