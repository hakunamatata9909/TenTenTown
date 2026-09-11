#include "ArrowAfterHit_ObjectPooling.h"
#include "Character/GAS/GA/Archer/Ultimate/ActorArrowRain.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

AArrowAfterHit_ObjectPooling::AArrowAfterHit_ObjectPooling()
{
    PrimaryActorTick.bCanEverTick = false;

    // 네트워크 동기화 완전 해제
    bReplicates = false;
    bAlwaysRelevant = false;
    SetReplicateMovement(false); 

    SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
    SetRootComponent(SceneComponent);

    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
    SkeletalMeshComponent->SetupAttachment(RootComponent);
}

void AArrowAfterHit_ObjectPooling::BeginPlay()
{
    Super::BeginPlay();

    SetActorHiddenInGame(true);
    if (SkeletalMeshComponent)
    {
        SkeletalMeshComponent->SetHiddenInGame(true);
        SkeletalMeshComponent->SetVisibility(false);
    }
}

void AArrowAfterHit_ObjectPooling::ActivateFromPool(const FVector& Location, const FRotator& Rotation, AActor* AttachTarget, FName BoneName, AActorArrowRain* InPoolOwner)
{
    PoolOwner = InPoolOwner;

    // 타겟이 존재하면 부착, 아니면 월드 좌표에 배치
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    if (AttachTarget)
    {
        AttachToActor(AttachTarget, FAttachmentTransformRules::KeepWorldTransform, BoneName);
    }

    SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
    SetActorHiddenInGame(false);

    if (SkeletalMeshComponent)
    {
        SkeletalMeshComponent->SetHiddenInGame(false);
        SkeletalMeshComponent->SetVisibility(true);
    }

    // 일정 시간 뒤 풀로 반환
    GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &ThisClass::DeactivateToPool, DisplayDuration, false);
}

void AArrowAfterHit_ObjectPooling::DeactivateToPool()
{
    GetWorldTimerManager().ClearTimer(LifeTimerHandle);

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorHiddenInGame(true);

    if (SkeletalMeshComponent)
    {
        SkeletalMeshComponent->SetHiddenInGame(true);
        SkeletalMeshComponent->SetVisibility(false);
    }

    if (PoolOwner.IsValid())
    {
        PoolOwner->ReturnAfterHitToPool(this);
    }
}