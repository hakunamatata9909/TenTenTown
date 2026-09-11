// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GAS/TTTASComponent.h"

float UTTTASComponent::GetCooldownRemainingByTag(FGameplayTag CooldownTag) const
{
	if (!CooldownTag.IsValid()) return 0.f; // 안전성 체크

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
		FGameplayTagContainer(CooldownTag)
	);

	// 1. 함수를 호출하고 반환 값(TArray<TPair<float, float>>)을 Times 변수에 저장합니다.
	TArray<TPair<float, float>> Times = GetActiveEffectsTimeRemainingAndDuration(Query);

	float MaxRemaining = 0.f;

	// 2. Times 배열을 순회하며 가장 큰 남은 시간(Pair.Key)을 찾습니다.
	for (const auto& Pair : Times)
		MaxRemaining = FMath::Max(MaxRemaining, Pair.Key);

	return MaxRemaining;
}

float UTTTASComponent::GetCooldownDurationByTag(FGameplayTag CooldownTag) const
{
	if (!CooldownTag.IsValid()) return 0.f; // 안전성 체크

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
		FGameplayTagContainer(CooldownTag)
	);

	// 1. 함수를 호출하고 반환 값(TArray<TPair<float, float>>)을 Times 변수에 저장합니다.
	TArray<TPair<float, float>> Times = GetActiveEffectsTimeRemainingAndDuration(Query);

	float MaxDuration = 0.f;

	// 2. Times 배열을 순회하며 가장 큰 총 시간(Pair.Value)을 찾습니다.
	for (const auto& Pair : Times)
		MaxDuration = FMath::Max(MaxDuration, Pair.Value);

	return MaxDuration;
}