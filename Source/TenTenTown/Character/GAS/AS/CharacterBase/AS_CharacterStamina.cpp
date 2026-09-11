// Fill out your copyright notice in the Description page of Project Settings.

#include "AS_CharacterStamina.h"
#include "Net/UnrealNetwork.h"

UAS_CharacterStamina::UAS_CharacterStamina()
{
	InitMaxStamina(100.f);
	InitStamina(100.f);
}

void UAS_CharacterStamina::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAS_CharacterStamina::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute==GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxStamina());
	}
	else if (Attribute==GetMaxStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f); 
	}
}

void UAS_CharacterStamina::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAS_CharacterStamina::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass,Stamina,COND_None,REPNOTIFY_Always);
}

void UAS_CharacterStamina::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Stamina,OldStamina);
}

void UAS_CharacterStamina::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxStamina,OldMaxStamina);
}
