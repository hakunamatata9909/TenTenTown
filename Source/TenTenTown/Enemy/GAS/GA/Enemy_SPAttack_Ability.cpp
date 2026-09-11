// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_SPAttack_Ability.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList//DemonKing.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "TTTGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/SkeletalMeshComponent.h"

void UEnemy_SPAttack_Ability::PlayAttackMontage()
{
	
	if (!Actor||!Actor->HasAuthority())
	{
		return;
	}
	
	ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
	if (!DemonKing)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(DemonKing);
	if (!ASC)
	{
		return;
	}
	
	// 태그로 상태 확인 및 몽타주 교체
	UAnimMontage* MontageToPlay = DemonKing->AttackMontage;
	if (ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Berserk))
	{
		MontageToPlay = DemonKing->SPAttackMontage;
	}

	if (!MontageToPlay)
	{
		return;
	}

	AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
		this,
		NAME_None,
		MontageToPlay,
		AttackSpeed
	);

	if (!Task)
	{
		return;
	}

	Task->OnCompleted.AddDynamic(this, &UEnemy_SPAttack_Ability::OnMontageEnded);
	Task->OnBlendOut.AddDynamic(this, &UEnemy_SPAttack_Ability::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UEnemy_SPAttack_Ability::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UEnemy_SPAttack_Ability::OnMontageEnded);

	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInst = Actor->GetMesh()->GetAnimInstance();
		AnimInst->OnPlayMontageNotifyBegin.RemoveAll(this);
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_SPAttack_Ability::OnNotifyBegin);
	}

	Task->ReadyForActivation();
}
