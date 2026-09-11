#include "ActorArrowRain.h"
#include "Character/Characters/Archer/ArrowRainPooling/Arrow_ObjectPooling.h"
#include "Character/Characters/Archer/ArrowAfterHit_RainPooling/ArrowAfterHit_ObjectPooling.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Net/UnrealNetwork.h"

AActorArrowRain::AActorArrowRain()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    RainSeed = -1;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
    SetRootComponent(StaticMeshComponent);
}

void AActorArrowRain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AActorArrowRain, RainSeed);
}

void AActorArrowRain::InitializeArrowRain(float InDamage, TSubclassOf<UGameplayEffect> InGEClass)
{
    ArrowDamage = InDamage;
    DamageGEClass = InGEClass;
}

void AActorArrowRain::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        RainSeed = FMath::Rand();
        RandStream.Initialize(RainSeed);

        if (ArrowClass)
        {
            GetWorldTimerManager().SetTimer(RainTimerHandle, this, &AActorArrowRain::SpawnSingleArrow, SpawnInterval, true);
        }
    }
}

void AActorArrowRain::OnRep_RainSeed()
{
    RandStream.Initialize(RainSeed);

    if (ArrowClass)
    {
        GetWorldTimerManager().SetTimer(RainTimerHandle, this, &AActorArrowRain::SpawnSingleArrow, SpawnInterval, true);
    }
}

void AActorArrowRain::SpawnSingleArrow()
{
    if (ArrowsSpawnedSoFar >= TotalArrowCount)
    {
        GetWorldTimerManager().ClearTimer(RainTimerHandle);
        CheckRainFinished();
        return;
    }

    float RandomX = RandStream.FRandRange(-RainRadius, RainRadius);
    float RandomY = RandStream.FRandRange(-RainRadius, RainRadius);
    FVector RandomOffset(RandomX, RandomY, 0.0f);
    FVector SpawnLocation = GetActorLocation() + RandomOffset + FVector(0.0f, 0.0f, SpawnHeight);

    FVector BaseDirection = FVector::DownVector;
    float ConeHalfAngleRad = FMath::DegreesToRadians(MaxTiltAngle);
    FVector FallDirection = RandStream.VRandCone(BaseDirection, ConeHalfAngleRad);

    AArrow_ObjectPooling* ArrowToFire = nullptr;

    if (ArrowPool.Num() > 0)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorArrowRain_ArrowActivateFromPool);
        ArrowToFire = ArrowPool.Pop();
    }
    else if (ArrowClass)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorArrowRain_ArrowSpawn);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ArrowToFire = GetWorld()->SpawnActor<AArrow_ObjectPooling>(
            ArrowClass, 
            SpawnLocation, 
            FallDirection.Rotation(), 
            SpawnParams
        );

        if (ArrowToFire)
        {
            ArrowToFire->SetDamage(ArrowDamage);
            if (DamageGEClass)
            {
                ArrowToFire->SetSetByCallerClass(DamageGEClass);
            }
        }
    }

    if (ArrowToFire)
    {
        InFlightArrowCount++;
        ArrowToFire->ActivateFromPool(SpawnLocation, FallDirection, this);
        ArrowsSpawnedSoFar++;
    }
}

void AActorArrowRain::ReturnArrowToPool(AArrow_ObjectPooling* Arrow)
{
    if (Arrow)
    {
        InFlightArrowCount = FMath::Max(0, InFlightArrowCount - 1);
        ArrowPool.Add(Arrow);
        CheckRainFinished();
    }
}

void AActorArrowRain::SpawnAfterHitFromPool(const FVector& Location, const FRotator& Rotation, AActor* TargetActor, FName BoneName)
{
    if (!AfterHitClass) return;

    // 데디케이티드 서버에서는 시각 이펙트를 그릴 필요가 없으므로 스폰 생략
    if (GetNetMode() == NM_DedicatedServer) return;

    AArrowAfterHit_ObjectPooling* HitActor = nullptr;

    if (AfterHitPool.Num() > 0)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorArrowRain_ArrowAFHActivateFromPool);
        HitActor = AfterHitPool.Pop();
    }
    else
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorArrowRain_ArrowAFHSpawn);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        HitActor = GetWorld()->SpawnActor<AArrowAfterHit_ObjectPooling>(
            AfterHitClass, 
            Location, 
            Rotation, 
            SpawnParams
        );
    }

    if (HitActor)
    {
        // 기존 AfterHit_ObjectPooling 활성화 함수 (기존 구조 유지)
        HitActor->ActivateFromPool(Location, Rotation, TargetActor, BoneName, this);
    }
}

void AActorArrowRain::ReturnAfterHitToPool(AArrowAfterHit_ObjectPooling* AfterHit)
{
    if (AfterHit)
    {
        AfterHitPool.Add(AfterHit);
    }
}

void AActorArrowRain::CheckRainFinished()
{
    if (ArrowsSpawnedSoFar >= TotalArrowCount && InFlightArrowCount <= 0)
    {
        SetLifeSpan(RemnantDuration + 0.5f);
    }
}

void AActorArrowRain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (auto& Arrow : ArrowPool)
    {
        if (IsValid(Arrow))
        {
            Arrow->Destroy();
        }
    }
    ArrowPool.Empty();

    for (auto& AfterHit : AfterHitPool)
    {
        if (IsValid(AfterHit))
        {
            AfterHit->Destroy();
        }
    }
    AfterHitPool.Empty();

    Super::EndPlay(EndPlayReason);
}