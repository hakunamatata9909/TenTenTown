#include "AS_MageAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_MageAttributeSet::UAS_MageAttributeSet()
{
	InitOverheatingStack(0.f);
}

void UAS_MageAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetOverheatingStackAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxOverheatingStack);
	}
}

void UAS_MageAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAS_MageAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UAS_MageAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, OverheatingStack, COND_None, REPNOTIFY_Always);
}

void UAS_MageAttributeSet::OnRep_OverheatingStack(const FGameplayAttributeData& OldOverheatingStack )
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, OverheatingStack, OldOverheatingStack);
}
