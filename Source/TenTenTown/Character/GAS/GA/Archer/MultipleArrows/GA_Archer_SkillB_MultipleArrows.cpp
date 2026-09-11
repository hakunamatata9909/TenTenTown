// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Archer_SkillB_MultipleArrows.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Archer/ArcherCharacter/ArcherCharacter.h"
#include "Character/Characters/Archer/Arrow/Archer_Arrow.h"
#include "Character/Characters/Archer/Bow/ArcherBow.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Archer_SkillB_MultipleArrows::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo);
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	AvatarCharacter = Cast<AArcherCharacter>(GetAvatarActorFromActorInfo());
	EquippedBow = AvatarCharacter->GetEquippedBow();


	// 1. 5발의 화살 미리 생성 및 부착 (서버 권한 확인)
	if (HasAuthority(&ActivationInfo) && ArrowClass && EquippedBow)
	{
		SpawnedArrows.Empty();
		FVector SpawnPos = EquippedBow->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
		FRotator SpawnRot = EquippedBow->GetMesh()->GetSocketRotation(FName("ArrowSocket"));
		FTransform SpawnTransform(SpawnRot, SpawnPos);

		for (int32 i = 0; i < 5; ++i)
		{
			AArcher_Arrow* NewArrow = GetWorld()->SpawnActorDeferred<AArcher_Arrow>(
				ArrowClass, SpawnTransform, AvatarCharacter, AvatarCharacter, 
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (NewArrow)
			{
				NewArrow->SetIgnoreActor(AvatarCharacter);
				NewArrow->SetIgnoreActor(EquippedBow);
				NewArrow->SetSetByCallerClass(SetByCallerClass);
                
				float BaseAtk = ASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
				NewArrow->SetDamage(BaseAtk * DamageMultiplier);
				NewArrow->FinishSpawning(SpawnTransform);
				NewArrow->AttachToComponent(EquippedBow->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ArrowSocket"));
                
				SpawnedArrows.Add(NewArrow);
			}
		}
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("None"),AttackMontage);
	
	MontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	MontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	MontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);

	UAbilityTask_WaitGameplayEvent* WaitFireGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Archer_SkillBFire,nullptr,true);

	WaitFireGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::FireArrows);
	
	WaitFireGameplayEvent->ReadyForActivation();
	MontageTask->ReadyForActivation();
	
	EquippedBow->PlayNormalAttackMontage_Start();
	
}

void UGA_Archer_SkillB_MultipleArrows::FireArrows(FGameplayEventData Payload)
{
    // 디버그: 실제로 배열에 몇 개가 들어있는지 확인
    UE_LOG(LogTemp, Log, TEXT("FireArrows Called. SpawnedArrows Count: %d"), SpawnedArrows.Num());

    if (!HasAuthority(&CurrentActivationInfo)) return;
    if (SpawnedArrows.Num() == 0) return;

    // 1. 조준점 계산 (기존 로직 유지)
    FVector TargetLocation;
    APlayerController* PC = Cast<APlayerController>(AvatarCharacter->GetController());
    if (PC)
    {
        FVector CamLoc; FRotator CamRot;
        PC->GetPlayerViewPoint(CamLoc, CamRot);
        FVector TraceEnd = CamLoc + (CamRot.Vector() * 20000.f);
        TargetLocation = TraceEnd;

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(AvatarCharacter);
        // 생성된 모든 화살을 트레이스에서 무시 (선택 사항)
        for (auto Arrow : SpawnedArrows) { if (Arrow) Params.AddIgnoredActor(Arrow); }

        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
        {
            TargetLocation = Hit.ImpactPoint;
        }
    }
    else
    {
        TargetLocation = AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * 1000.f;
    }

    // 2. 부채꼴 방향 계산
    FVector BowStart = EquippedBow->GetMesh()->GetSocketLocation(FName("ArrowSocket"));
    FVector CombinedDir = (TargetLocation - BowStart).GetSafeNormal();
    FRotator CenterRot = CombinedDir.Rotation();

    float HalfSpread = SpreadAngle / 2.f;
    // 화살이 1개일 경우를 대비한 안전 코드
    float AngleStep = (SpawnedArrows.Num() > 1) ? SpreadAngle / (SpawnedArrows.Num() - 1) : 0.f;

    // 3. 발사 루프
    for (int32 i = 0; i < SpawnedArrows.Num(); ++i)
    {
        AArcher_Arrow* CurrentArrow = SpawnedArrows[i];
        if (CurrentArrow)
        {
            // [중요] 화살끼리 부딪히지 않도록 서로를 무시 설정
            for (auto OtherArrow : SpawnedArrows)
            {
                if (CurrentArrow != OtherArrow)
                {
                    CurrentArrow->SetIgnoreActor(OtherArrow);
                }
            }

            float CurrentYaw = -HalfSpread + (AngleStep * i);
            FRotator LaunchRot = CenterRot;
            LaunchRot.Yaw += CurrentYaw;

            // 부착 해제 및 발사
            CurrentArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            CurrentArrow->SetLifeSpan(5.f);
            
            // 방향 벡터를 명확히 계산해서 전달
            FVector FinalDir = LaunchRot.Vector();
            CurrentArrow->FireArrow(FinalDir, 1.0f); 
            
            CurrentArrow->Multicast_PlayEffects();
            
            UE_LOG(LogTemp, Log, TEXT("Arrow %d Fired in Direction: %s"), i, *FinalDir.ToString());
        }
    }

    SpawnedArrows.Empty(); 
}

void UGA_Archer_SkillB_MultipleArrows::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Archer_SkillB_MultipleArrows::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

