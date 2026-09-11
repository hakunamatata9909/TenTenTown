#include "Structure/Crossbow/CrossbowBolt.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h" 
#include "Enemy/Base/EnemyBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACrossbowBolt::ACrossbowBolt()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	// 오버랩
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 매시 설정
	BoltMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	BoltMesh->SetupAttachment(Sphere);
	BoltMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Sphere;
	ProjectileMovement->InitialSpeed = 3000.f; // 시작 속도
	ProjectileMovement->MaxSpeed = 3000.f; // 최대 속도
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향 보기
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 중력 제거
	
    // 풀 설정 시 초기화
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    ProjectileMovement->Deactivate(); 
}

// 풀링
void ACrossbowBolt::ActivateProjectile(FVector StartLocation, AActor* TargetActor, float Range)
{
    SetActorLocation(StartLocation);
    
    // 타겟 방향으로 회전
    if (TargetActor)
    {
        FVector Direction = (TargetActor->GetActorLocation() - StartLocation).GetSafeNormal();
    	
        SetActorRotation(Direction.Rotation());
        ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
    }

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    ProjectileMovement->Activate();
    
    // 이벤트 다시 바인딩
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACrossbowBolt::OnOverlap);

	// 범위 + 100 거리 이상 이동 시 풀링 복귀
	if (ProjectileMovement->InitialSpeed > 0.f)
	{
		float ActualRange = Range + 100.0f;
		float Duration = ActualRange / ProjectileMovement->InitialSpeed;
        
		GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, this, &ACrossbowBolt::DeactivateProjectile, Duration, false);
	}
}

// 풀링 복귀
void ACrossbowBolt::DeactivateProjectile()
{
	// 타이머 있음 끄기
	GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
	
	// 액터 숨기고 콜리전 끄기
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    ProjectileMovement->Deactivate();
    ProjectileMovement->Velocity = FVector::ZeroVector;
    
    Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &ACrossbowBolt::OnOverlap);
}

void ACrossbowBolt::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == GetOwner() || OtherActor == this) return;

	static const FName TowerProfileName(TEXT("TowerStructure"));
	if (OtherComp && OtherComp->GetCollisionProfileName() == TowerProfileName)
	{
		return;
	}
	
    // EnemyBase인지 확인
    AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
    if (Enemy)
    {
    	UE_LOG(LogTemp, Warning, TEXT("Hit Enemy: %s"), *Enemy->GetName());
    	// 몬스터 ASC 가져옴
        UAbilitySystemComponent* TargetASC = Enemy->GetAbilitySystemComponent();
        
        // 데미지 적용
        if (TargetASC && DamageEffectClass)
        {
            FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
            Context.AddInstigator(this, this);

        	// 스펙으로 데미지 줄 준비
            FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);
            if (SpecHandle.IsValid())
            {
                // 데미지 태그 전달
                SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, DamageAmount);
            	// 스펙을 몬스터 ASC에 적용
                TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
            }
        }
        
        // 맞았으니 화살 비활성화(복귀)
        DeactivateProjectile();
    }
	// 캐릭터, 같은 구조물 등 관통 설정
    else if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
    {
    	DeactivateProjectile();
    }
}
