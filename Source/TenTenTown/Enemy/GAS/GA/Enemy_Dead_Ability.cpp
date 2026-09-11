// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Dead_Ability.h"

#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/System/PreloadSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"

UEnemy_Dead_Ability::UEnemy_Dead_Ability()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Dead;
	AbilityTriggers.Add(TriggerData);
}

void UEnemy_Dead_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("DeadAbility FINALLY Activated!"));

	AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
   this,
   NAME_None,
   Actor->DeadMontage,
   1.0f);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnDeathMontageFinished);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnDeathMontageFinished);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnDeathMontageFinished);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnDeathMontageFinished);
		MontageTask->ReadyForActivation();
	}
	else
	{
		OnDeathMontageFinished();
	}
}

void UEnemy_Dead_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Dead_Ability::DropItem(AEnemyBase* Actor)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	//float DropPercent = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetDropPercentAttribute());

	float DropPercent = 0.1f;
	float RandomValue = FMath::FRand();

	if (RandomValue > DropPercent) return;
	
	Actor->DropGoldItem(); 

}

void UEnemy_Dead_Ability::OnDeathMontageFinished()
{
	AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());
	if (!Actor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if (Actor && Actor->HasAuthority())
	{
		if (UWorld* World = Actor->GetWorld())
		{
			if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(World)))
			{
				GM->NotifyEnemyDead(Actor);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[DeadAbility] GameMode is not ATTTGameModeBase. NotifyEnemyDead skipped."));
			}
		}
		
		//Actor->DropGoldItem();
		DropItem(Actor);

		Actor->Destroy();
		/*if (UWorld* World = Actor->GetWorld())
		{
			
			if (UPoolSubsystem* PoolSubsystem = World->GetSubsystem<UPoolSubsystem>())
			{
				PoolSubsystem->ReleaseEnemy(Actor->SpawnWaveIndex,Actor);

				if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
				{	
					ASC->RemoveLooseGameplayTag(GASTAG::Enemy_Ability_Dead);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UDeadTask:Release Failed"));
			}
		}*/
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
