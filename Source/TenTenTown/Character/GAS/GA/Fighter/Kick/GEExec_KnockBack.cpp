// Fill out your copyright notice in the Description page of Project Settings.


#include "GEExec_KnockBack.h"

#include "AbilitySystemComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameFramework/Character.h"

void UGEExec_KnockBack::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                               FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetASC->GetAvatarActor());

	const FGameplayEffectSpec& Spec =ExecutionParams.GetOwningSpec();
	const FGameplayEffectContext* Context = Spec.GetContext().Get();

	FVector SourceActorForward = Context->GetEffectCauser()->GetActorForwardVector();

	FVector KnockBackVelocity = SourceActorForward*300.f+FVector(0,0,300);
	
	TargetCharacter->LaunchCharacter(KnockBackVelocity,true,true);
	float Damage = Spec.GetSetByCallerMagnitude(GASTAG::Data_Damage);

	FGameplayModifierEvaluatedData HealthMod(
		UAS_EnemyAttributeSetBase::GetDamageAttribute(),
		EGameplayModOp::AddFinal,
		Damage);

	OutExecutionOutput.AddOutputModifier(HealthMod);
}
