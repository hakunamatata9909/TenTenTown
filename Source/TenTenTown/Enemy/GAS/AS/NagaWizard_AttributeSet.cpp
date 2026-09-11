// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/NagaWizard_AttributeSet.h"

#include "Net/UnrealNetwork.h"

UNagaWizard_AttributeSet::UNagaWizard_AttributeSet()
{
	InitBuffScale(1.f);
	InitBuffDuration(10.f);
	InitBuffCoolDown(10.f);
	InitBuffRadius(500.f);
}

void UNagaWizard_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BuffScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BuffDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BuffCoolDown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BuffRadius, COND_None, REPNOTIFY_Always);

}

void UNagaWizard_AttributeSet::OnRep_BuffScale(const FGameplayAttributeData& OldBuffScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BuffScale,OldBuffScale);
}

void UNagaWizard_AttributeSet::OnRep_BuffDuration(const FGameplayAttributeData& OldBuffDuration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BuffDuration,OldBuffDuration);

}

void UNagaWizard_AttributeSet::OnRep_BuffCoolDown(const FGameplayAttributeData& OldBuffCoolDown)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BuffCoolDown,OldBuffCoolDown);
}

void UNagaWizard_AttributeSet::OnRep_BuffRadius(const FGameplayAttributeData& OldBuffRadius)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BuffCoolDown,OldBuffRadius);
}
