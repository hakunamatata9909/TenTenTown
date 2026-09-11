// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/Beholder_AttributeSet.h"

#include "Net/UnrealNetwork.h"

UBeholder_AttributeSet::UBeholder_AttributeSet(): UAS_EnemyAttributeSetBase()
{
	InitExplodeRange(600.f);
}

void UBeholder_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExplodeRange, COND_None, REPNOTIFY_Always);

}

void UBeholder_AttributeSet::OnRep_Explode(const FGameplayAttributeData& OldExplode)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,ExplodeRange,OldExplode);
}
