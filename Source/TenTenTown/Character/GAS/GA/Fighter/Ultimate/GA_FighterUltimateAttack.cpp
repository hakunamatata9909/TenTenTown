// GA_FighterUltimateAttack.cpp

#include "GA_FighterUltimateAttack.h"
#include "AbilitySystemComponent.h"
#include "CollisionQueryParams.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GAS/GA/Fighter/Fighter_Fireball/Fireball_Projectile.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

void UGA_FighterUltimateAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    // 1) 몽타주 태스크
    UAbilityTask_PlayMontageAndWait* PlayAMTask =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            FName("AMTask"),
            AnimMontage,
            /*Rate*/ 1.f,
            /*StartSectionName*/ NAME_None,
            /*bStopWhenAbilityEnds*/ true,
            /*AnimRootMotionTranslationScale*/ 1.f,
            /*StartSectionIndex*/ 0,
            true
        );
    // 정상 종료 / 인터럽트 / 취소 모두 바인딩
    PlayAMTask->OnCompleted  .AddUniqueDynamic(this, &ThisClass::OnMontageCompleted);
    PlayAMTask->OnInterrupted.AddUniqueDynamic(this, &ThisClass::OnMontageInterrupted);
    PlayAMTask->OnCancelled  .AddUniqueDynamic(this, &ThisClass::OnMontageInterrupted);

    // 2) 이벤트 대기 태스크
    UAbilityTask_WaitGameplayEvent* WaitReleaseEvent =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::Event_Fighter_UltimateRelease,    nullptr, /*OnlyTriggerOnce*/ true);
    UAbilityTask_WaitGameplayEvent* WaitReleaseEndEvent =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::Event_Fighter_UltimateReleaseEnd, nullptr, /*OnlyTriggerOnce*/ true);

    WaitReleaseEvent    ->EventReceived.AddUniqueDynamic(this, &ThisClass::OnRelease);
    WaitReleaseEndEvent ->EventReceived.AddUniqueDynamic(this, &ThisClass::OnReleaseEnd);

    // 3) 활성화
    WaitReleaseEvent   ->ReadyForActivation();
    WaitReleaseEndEvent->ReadyForActivation();
    PlayAMTask         ->ReadyForActivation();
}

void UGA_FighterUltimateAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    // 방어적 정리: 애니메이션 끼임 방지
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (ASC->IsAnimatingAbility(this))
        {
            ASC->CurrentMontageStop();
            ASC->ClearAnimatingAbility(this);
        }
        // (필요 시) 여기서 임시 태그/GE 정리
        // ASC->RemoveLooseGameplayTag(...);
        // ASC->RemoveActiveGameplayEffect(...);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FighterUltimateAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateCancelAbility)
{
    // Cancel 경로에서도 동일 방어 정리
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (ASC->IsAnimatingAbility(this))
        {
            ASC->CurrentMontageStop();
            ASC->ClearAnimatingAbility(this);
        }
    }

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_FighterUltimateAttack::OnRelease(const FGameplayEventData Data)
{
	
    APlayerController* PC = Cast<APlayerController>(AvatarCharacter->GetController());
	FVector Start;
	FRotator Rotation;
	PC->GetPlayerViewPoint(Start,Rotation);

	FVector End = Rotation.Vector()*10000.f+Start;
	
	FHitResult HitResult;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AvatarCharacter);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	
	bool bSuccessLineTrace = GetWorld()->LineTraceSingleByObjectType(HitResult,Start,End,ObjectQueryParams,CollisionQueryParams);

	FTransform SpawnTransform;
	FVector SpawnLocation = AvatarCharacter->GetActorLocation()+AvatarCharacter->GetActorForwardVector()*100.f;
	FRotator SpawnRotator = AvatarCharacter->GetActorRotation();
	float ScaleSize = 1.f;
	
	FVector SpawnScale = FVector(ScaleSize,ScaleSize,ScaleSize);
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotator.Quaternion());
	SpawnTransform.SetScale3D(SpawnScale);

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GASTAG::GameplayCue_Fireball_Cast);
	if (bSuccessLineTrace)
	{
		UE_LOG(LogTemp,Log,TEXT("%f %f"),Start.X,End.X);

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);
		
		if (!Projectile) UE_LOG(LogTemp,Log,TEXT("no projectile spawn"));
		
		Proj->SetSetbyCallerGameplayEffectClass(GEDamage);
		
		Proj->FinishSpawning(SpawnTransform);
		FVector Direction = HitResult.Location-SpawnLocation;
		Proj->FireProjectile(Direction,AvatarCharacter);
	}
	else
	{
		UE_LOG(LogTemp,Log,TEXT("%f %f"),Start.X,End.X);

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);
		if (!Projectile) UE_LOG(LogTemp,Log,TEXT("no projectile spawn"));
		
		Proj->FinishSpawning(SpawnTransform);
		
		Proj->FireProjectile(End-SpawnLocation,AvatarCharacter);
	}
}

void UGA_FighterUltimateAttack::OnReleaseEnd(const FGameplayEventData Data)
{
    // 정상 종료
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}

void UGA_FighterUltimateAttack::OnMontageCompleted()
{
    // 몽타주가 정상적으로 끝났을 때도 정상 종료
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}

void UGA_FighterUltimateAttack::OnMontageInterrupted()
{
    // 다른 몽타주가 덮어쓴 경우 등: 취소 경로로 종료
    // 방어적 정리 + CancelAbility 호출(취소 플로우 보장)
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (ASC->IsAnimatingAbility(this))
        {
            ASC->CurrentMontageStop();
            ASC->ClearAnimatingAbility(this);
        }
    }
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateCancelAbility*/ true);
}
