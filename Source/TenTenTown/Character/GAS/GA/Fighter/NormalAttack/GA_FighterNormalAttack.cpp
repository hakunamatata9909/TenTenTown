// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_FighterNormalAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"
#include "TA_FighterSquare.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

void UGA_FighterNormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
	
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	
	AnimInstance = Cast<ACharacter>(GetAvatarActorFromActorInfo())->GetMesh()->GetAnimInstance();
	if (!AnimInstance) EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	
	SectionNames = {"First","Second"};
	bIsComboSectionStart = false;
	bIsComboInputPressed = false;
	CurrentComboCount = 0;
	
	auto* ComboStartWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_ComboStart);
	auto* ComboEndWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_ComboEnd);
	auto* AttackWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_Attack);
	auto* FirstSecondComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("FirstSecondComboMontageTask"),FirstSecondComboMontage,1.f,NAME_None,true
		,1.f,0.f,true);
	
	ComboStartWaitTask->EventReceived.AddUniqueDynamic(this,&ThisClass::ComboStart);
	ComboEndWaitTask->EventReceived.AddUniqueDynamic(this,&ThisClass::ComboEnd);
	AttackWaitTask->EventReceived.AddUniqueDynamic(this,&ThisClass::OnAttack);
	FirstSecondComboMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnFirstSecondMontageEnd);
	FirstSecondComboMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnInterrupted);
	
	ComboStartWaitTask->ReadyForActivation();
	ComboEndWaitTask->ReadyForActivation();
	FirstSecondComboMontageTask->ReadyForActivation();
	AttackWaitTask->ReadyForActivation();
	
	ASC->ForceReplication();
}

void UGA_FighterNormalAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FighterNormalAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (!bIsComboSectionStart) return;

	if (!bIsComboInputPressed)
	{
		if (CurrentComboCount==0)
		{
			ASC->CurrentMontageSetNextSectionName(SectionNames[CurrentComboCount],SectionNames[CurrentComboCount+1]);
		}
		CurrentComboCount++;
		bIsComboInputPressed = true;
	}

	ASC->ForceReplication();
}

void UGA_FighterNormalAttack::ComboStart(const FGameplayEventData Data)
{
	bIsComboSectionStart=true;
}

void UGA_FighterNormalAttack::ComboEnd(const FGameplayEventData Data)
{
	bIsComboSectionStart=false;
	bIsComboInputPressed=false;
}

void UGA_FighterNormalAttack::OnAttack(const FGameplayEventData Data)
{
	if (CurrentComboCount<2)
	{
		auto* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this,FName("WaitTargetDataTask"),EGameplayTargetingConfirmation::Instant,TargetActor);
		WaitTargetDataTask->ValidData.AddUniqueDynamic(this,&ThisClass::OnTargetDataCome);
		WaitTargetDataTask->ReadyForActivation();

		AGameplayAbilityTargetActor* SpawnedTA;
		WaitTargetDataTask->BeginSpawningActor(this,TargetActor,SpawnedTA);
		WaitTargetDataTask->FinishSpawningActor(this,SpawnedTA);
	}
	else
	{
		AGameplayAbilityTargetActor* SpawnedTA = GetWorld()->SpawnActorDeferred<ATA_FighterSquare>(
			   ATA_FighterSquare::StaticClass(),
			   FTransform::Identity,
			   nullptr,
			   nullptr,
			   ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		   );

		ATA_FighterSquare* Square = Cast<ATA_FighterSquare>(SpawnedTA);
		if (Square)
		{
			Square->Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
			Square->ASC = GetAbilitySystemComponentFromActorInfo();
			Square->ShapePos = 0.f;
			Square->Extent = FVector(200, 200, 200);
		}

		SpawnedTA->FinishSpawning(FTransform::Identity);

		auto* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
			this, 
			FName("WaitTargetDataTask"), 
			EGameplayTargetingConfirmation::Instant, 
			SpawnedTA 
		);
        
		WaitTargetDataTask->ValidData.AddUniqueDynamic(this, &ThisClass::OnTargetDataCome);
		WaitTargetDataTask->ReadyForActivation();
	}
}

void UGA_FighterNormalAttack::OnFirstSecondMontageEnd()
{
	if (CurrentComboCount==2)
	{
		auto* PlayLastAMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,FName("LastComboMontageTask"),LastComboMontage,0.75f,NAME_None,true
			,1.f,0.f,true);
		PlayLastAMTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnLastMontageEnd);
		PlayLastAMTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnInterrupted);
		PlayLastAMTask->ReadyForActivation();
		ASC->ForceReplication();
	}
	else
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
	}
}

void UGA_FighterNormalAttack::OnLastMontageEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_FighterNormalAttack::OnInterrupted()
{
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
}

void UGA_FighterNormalAttack::OnTargetDataCome(const FGameplayAbilityTargetDataHandle& Data)
{
	if (Data.Num()==0) return;
	TArray<AActor*> Actors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(Data,0);

	const FGameplayEffectSpecHandle& SpecHandle = MakeOutgoingGameplayEffectSpec(GE,1.f);
	FGameplayEffectSpec* Spec= SpecHandle.Data.Get();
	
	float Damage = ASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
	if (CurrentComboCount!=2)
	{
		Spec->SetSetByCallerMagnitude(GASTAG::Data_Damage,Damage*3.f);
	}
	else
	{
		Spec->SetSetByCallerMagnitude(GASTAG::Data_Damage,Damage*4.5f);
	}
	
	
	for (const auto& A : Actors)
	{
		if (A && A!=GetAvatarActorFromActorInfo())
		{
			if (UAbilitySystemComponent*TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(A))
			{
				if (CurrentComboCount==2)
				{
					TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fighter_PunchWhirlWindHit);
				}
				else
				{
					TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fighter_PunchHit);
				}
				ASC->ApplyGameplayEffectSpecToTarget(*Spec,TargetASC);
			}
		}
	}
}
