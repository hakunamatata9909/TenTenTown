#include "Structure/Core/AS/AS_CoreAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_CoreAttributeSet::UAS_CoreAttributeSet()
{
	InitHealth(1000.0f);
	InitMaxHealth(1000.0f);
}

void UAS_CoreAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 어떤 어트리뷰트가 변경되었는지 확인
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Health 값을 0과 MaxHealth 사이로
		float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);

		if (GetOwningActor() && GetOwningActor()->GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("AS_CoreAttributeSet - 코어 체력 변경됨: %f / %f"), NewHealth, GetMaxHealth());
		}
	}
}

void UAS_CoreAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Health와 MaxHealth를 모든 클라이언트에 복제
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_CoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_CoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAS_CoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_CoreAttributeSet, Health, OldHealth);
}

void UAS_CoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_CoreAttributeSet, MaxHealth, OldMaxHealth);
}
