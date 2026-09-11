// Fill out your copyright notice in the Description page of Project Settings.


#include "GEExec_Pull.h"

#include "AbilitySystemComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGEExec_Pull::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                          FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetASC->GetAvatarActor());

	const FGameplayEffectSpec& Spec =ExecutionParams.GetOwningSpec();
	const FGameplayEffectContext* Context = Spec.GetContext().Get();

	FVector SourceActorForward = Context->GetEffectCauser()->GetActorForwardVector();

	FVector KnockBackVelocity = -SourceActorForward*100.f+FVector(0,0,100);
	TargetCharacter->LaunchCharacter(KnockBackVelocity,true,true);
	float Damage = Spec.GetSetByCallerMagnitude(GASTAG::Data_Damage);

	FGameplayModifierEvaluatedData HealthMod(
		UAS_EnemyAttributeSetBase::GetDamageAttribute(),
		EGameplayModOp::AddFinal,
		Damage);

	OutExecutionOutput.AddOutputModifier(HealthMod);
}
