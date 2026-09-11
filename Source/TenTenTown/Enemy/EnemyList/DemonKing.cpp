#include "DemonKing.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

void ADemonKing::InitializeEnemy()
{
	Super::InitializeEnemy();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_DemonKing);

		ASC->RegisterGameplayTagEvent(
		GASTAG::Enemy_State_Berserk,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADemonKing::OnBerserkTagChanged);
	}
}
void ADemonKing::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemonKing, bIsBerserk);
}

void ADemonKing::OnBerserkTagChanged(FGameplayTag Tag, int32 NewCount)
{
	bIsBerserk = (NewCount > 0);
}

