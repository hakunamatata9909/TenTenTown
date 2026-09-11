// Fill out your copyright notice in the Description page of Project Settings.


#include "AS_FighterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_FighterAttributeSet::UAS_FighterAttributeSet()
{
	InitHealth(100.f);
	InitStamina(100.f);
	InitMaxHealth(100.f);
	InitMaxStamina(100.f);
	InitLevel(1.f);
}

void UAS_FighterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAS_FighterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f);	
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f);
	}
	else if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,10.f);
	}
	
}

void UAS_FighterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute==GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0,GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute==GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(),0,GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute==GetMaxHealthAttribute())
	{
		
	}
	else if (Data.EvaluatedData.Attribute==GetMaxStaminaAttribute())
	{
		
	}
	else if (Data.EvaluatedData.Attribute==GetLevelAttribute())
	{
		
	}
}

void UAS_FighterAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina,    COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);
	
}

void UAS_FighterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldHealth);
}

void UAS_FighterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Stamina,OldStamina);
}

void UAS_FighterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxHealth,OldMaxHealth);
}

void UAS_FighterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxStamina,OldMaxStamina);
}

void UAS_FighterAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Level,OldLevel);
}
