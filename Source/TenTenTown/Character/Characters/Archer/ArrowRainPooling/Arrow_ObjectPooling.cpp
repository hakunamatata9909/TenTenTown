#include "Arrow_ObjectPooling.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/GAS/GA/Archer/Ultimate/ActorArrowRain.h"

AArrow_ObjectPooling::AArrow_ObjectPooling()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    
    bReplicates = false;
    SetReplicateMovement(false);

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    CollisionComponent->SetSphereRadius(5.f);
    CollisionComponent->SetCollisionProfileName(FName("CharacterProjectile"));
    CollisionComponent->SetGenerateOverlapEvents(false);
    SetRootComponent(CollisionComponent);

    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
    SkeletalMeshComponent->SetupAttachment(RootComponent);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
    NiagaraComponent->bAutoActivate = false;
    NiagaraComponent->SetupAttachment(RootComponent);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    ProjectileMovementComponent->InitialSpeed = BasicSpeed;
    ProjectileMovementComponent->MaxSpeed = BasicSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bAutoActivate = false;

    SetActorHiddenInGame(true);
    if (SkeletalMeshComponent)
    {
        SkeletalMeshComponent->SetHiddenInGame(true);
        SkeletalMeshComponent->SetVisibility(false);
    }
}

void AArrow_ObjectPooling::BeginPlay()
{
    Super::BeginPlay();

    // 클라이언트도 충돌 이벤트를 받아 시각 처리를 해야 하므로 권한 체크 제거
    CollisionComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnHit);

    if (GetOwner())
    {
        CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    }
}

void AArrow_ObjectPooling::ActivateFromPool(const FVector& SpawnLocation, const FVector& Direction, AActorArrowRain* InPoolOwner)
{
    PoolOwner = InPoolOwner;

    // 풀에서 꺼낼 때마다 오너(궁수 캐릭터)와의 충돌 무시 갱신
    if (PoolOwner.IsValid() && PoolOwner->GetOwner())
    {
        CollisionComponent->IgnoreActorWhenMoving(PoolOwner->GetOwner(), true);
    }

    SetActorLocation(SpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
    
    FVector NormalizedDirection = Direction.IsNearlyZero() ? FVector::DownVector : Direction.GetSafeNormal();
    SetActorRotation(NormalizedDirection.Rotation(), ETeleportType::TeleportPhysics);

    SetActorTickEnabled(true);
    SetActorEnableCollision(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->UpdatedComponent = RootComponent;
        ProjectileMovementComponent->Velocity = NormalizedDirection * BasicSpeed;
        ProjectileMovementComponent->Activate(true);
    }

    // 데디케이티드 서버가 아닐 때만 렌더링 활성화
    if (GetNetMode() != NM_DedicatedServer)
    {
        SetActorHiddenInGame(false);
        if (SkeletalMeshComponent)
        {
            SkeletalMeshComponent->SetHiddenInGame(false);
            SkeletalMeshComponent->SetVisibility(true);
        }
        if (NiagaraComponent)
        {
            NiagaraComponent->SetVisibility(true);
            NiagaraComponent->Activate(true);
            NiagaraComponent->ResetSystem();
        }
    }

    GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &ThisClass::DeactivateToPool, 3.5f, false);
}

void AArrow_ObjectPooling::DeactivateToPool()
{
    GetWorldTimerManager().ClearTimer(LifeTimerHandle);

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopSimulating(FHitResult());
        ProjectileMovementComponent->Deactivate();
    }

    // 데디케이티드 서버가 아닐 때만 렌더링 끄기
    if (GetNetMode() != NM_DedicatedServer)
    {
        SetActorHiddenInGame(true);
        if (SkeletalMeshComponent)
        {
            SkeletalMeshComponent->SetHiddenInGame(true);
            SkeletalMeshComponent->SetVisibility(false);
        }
        if (NiagaraComponent)
        {
            NiagaraComponent->DeactivateImmediate();
            NiagaraComponent->SetVisibility(false);
        }
    }

    if (PoolOwner.IsValid())
    {
        PoolOwner->ReturnArrowToPool(this);
    }
}

void AArrow_ObjectPooling::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 클라이언트/서버 역할 분리
    bool bIsServer = PoolOwner.IsValid() && PoolOwner->HasAuthority();

    // 1. 시각적 연출: 클라이언트/리슨 서버가 각자 로컬에서 바닥에 화살을 꽂음
    if (GetNetMode() != NM_DedicatedServer)
    {
        if (PoolOwner.IsValid() && OtherActor)
        {
            FVector FinalSpawnLocation = Hit.Location + GetActorForwardVector() * 25.f;
            PoolOwner->SpawnAfterHitFromPool(FinalSpawnLocation, GetActorRotation(), Hit.GetActor(), Hit.BoneName);
        }
    }

    // 2. 데미지 처리: 오직 서버 권한을 가진 방장 쪽에서만 GAS 로직 실행
    if (bIsServer)
    {
        UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
        if (SetByCallerClass && SourceASC)
        {
            FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(SetByCallerClass, 1.f, SourceASC->MakeEffectContext());
            if (SpecHandle.IsValid())
            {
                SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Damage, Damage);
                if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor()))
                {
                    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
                }
            }
        }
    }

    // 역할 수행 후 로컬 풀로 반환
    DeactivateToPool();
}