#include "Structure/Crossbow/CrossbowStructure.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"

ACrossbowStructure::ACrossbowStructure()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 컴포넌트 세팅
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);
	BaseMesh->SetCollisionProfileName(TEXT("TowerStructure"));
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BaseMesh);
	TurretMesh->SetCollisionProfileName(TEXT("TowerStructure"));
	
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(TurretMesh); // 회전체에 붙어있어야 같이 돔
	MuzzleLocation->SetRelativeLocation(FVector(50.f, 0.f, 0.f)); // 약간 앞으로

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->SetSphereRadius(AttackRange);
	
	// DetectSphere 설치 트레이스에 감지 x
	DetectSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 겹침 전용
	DetectSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore); // GridFloor 채널 무시
}

void ACrossbowStructure::BeginPlay()
{
	Super::BeginPlay();

	// GAS 체력 변경 바인딩
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &ACrossbowStructure::OnEnemyExit);

	InitializePool();
}

void ACrossbowStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟이 유효한지 검사
	if (CurrentTarget && (CurrentTarget->IsPendingKillPending() || !IsValid(CurrentTarget)))
	{
		CurrentTarget = nullptr;
	}

	// 타겟이 없다면 새로운 타겟 탐색
	if (!CurrentTarget)
	{
		// 0.2초마다 한 번씩만 탐색 (최적화)
		TargetSearchTimer += DeltaTime;
		if (TargetSearchTimer >= 0.2f)
		{
			FindBestTarget();
			TargetSearchTimer = 0.0f;
		}
	}

	// 타겟이 있다면 회전 및 공격
	if (CurrentTarget)
	{
		// 멀어지면 안쏨
		float Dist = GetDistanceTo(CurrentTarget);
		if (Dist > AttackRange + 100.0f)
		{
			CurrentTarget = nullptr;
			return;
		}
		
		// 회전
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		ACharacter* EnemyChar = Cast<ACharacter>(CurrentTarget);
		
		if (EnemyChar && EnemyChar->GetMesh())
		{
			// 메시의 정중앙
			TargetLoc = EnemyChar->GetMesh()->Bounds.Origin;
		}
		FVector MyLoc = TurretMesh->GetComponentLocation();
		// 정중앙에서 Z + 50
		TargetLoc.Z += 50.0f;
		
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
		float AdjustYaw = -90.0f;
		
		// 좌우 회전만
		FRotator TargetRot = FRotator(0.f, LookAtRot.Yaw + AdjustYaw, 0.f);

		FRotator CurrentRot = TurretMesh->GetComponentRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 10.0f);
        
		TurretMesh->SetWorldRotation(NewRot); // TurretMesh만 돌림 (받침대는 가만히)

		// 공격 타이머
		FireTimer += DeltaTime;
		if (FireTimer >= (1.0f / AttackSpeed))
		{
			Fire();
			FireTimer = 0.f;
		}
	}
	else
	{
		// 적이 없으면 방향 복귀
		FRotator ReturnRot = FMath::RInterpTo(TurretMesh->GetRelativeRotation(), FRotator::ZeroRotator, DeltaTime, 2.0f);
		TurretMesh->SetRelativeRotation(ReturnRot);
	}
}

// 풀링으로 화살 미리 만들기
void ACrossbowStructure::InitializePool()
{
	if (!BoltClass) return;

	for (int32 i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 안 보이는 곳에 생성
		ACrossbowBolt* NewBolt = GetWorld()->SpawnActor<ACrossbowBolt>(BoltClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewBolt)
		{
			BoltPool.Add(NewBolt);
		}
	}
}

// 풀에서 화살 꺼내기
ACrossbowBolt* ACrossbowStructure::GetBoltFromPool()
{
	// Hidden 화살 찾기
	for (ACrossbowBolt* Bolt : BoltPool)
	{
		if (Bolt && Bolt->IsHidden())
		{
			return Bolt;
		}
	}

	// 다 쓰고 없으면 새로 만듬
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ACrossbowBolt* NewBolt = GetWorld()->SpawnActor<ACrossbowBolt>(BoltClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (NewBolt)
	{
		BoltPool.Add(NewBolt);
		return NewBolt;
	}

	return nullptr;
}

// 타겟이 나가면 타겟 해제
void ACrossbowStructure::OnEnemyExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentTarget)
	{
		CurrentTarget = nullptr;
	}
}

// 발사
void ACrossbowStructure::Fire()
{
	if (!CurrentTarget) return;

	FVector FireLocation = MuzzleLocation->GetComponentLocation();
	
	ACrossbowBolt* Bolt = GetBoltFromPool();
	if (Bolt)
	{
		Bolt->SetOwner(this);
		Bolt->SetInstigator(GetInstigator());
		Bolt->DamageAmount = this->AttackDamage;
		Bolt->ActivateProjectile(FireLocation, CurrentTarget, AttackRange);
	}
	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = FireLocation; // 총구 위치에서 소리 재생
    
		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Crossbow_Fire, CueParams);
	}
}

// 가장 가까운 적 찾기
void ACrossbowStructure::FindBestTarget()
{
	TArray<AActor*> OverlappingActors;
	DetectSphere->GetOverlappingActors(OverlappingActors, AEnemyBase::StaticClass());

	float MinDist = 999999.0f;
	AActor* NearestEnemy = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);
		if (Enemy && !Enemy->IsPendingKillPending()) // 살아있는 적만
		{
			float Dist = GetDistanceTo(Enemy);
			if (Dist < MinDist)
			{
				MinDist = Dist;
				NearestEnemy = Enemy;
			}
		}
	}
	CurrentTarget = NearestEnemy;
}

void ACrossbowStructure::UpgradeStructure()
{
	// 최대 레벨 체크 (3레벨 이상이면 중단)
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max Level Reached!"));
		return;
	}

	// 레벨 증가
	CurrentUpgradeLevel++;

	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
        
		// 업그레이드 큐 실행
		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Upgrade, CueParams);
	}

	// 스탯 적용 (서버)
	ApplyStructureStats(CurrentUpgradeLevel);
}

void ACrossbowStructure::ApplyStructureStats(int32 Level)
{
	int32 DataIndex = Level - 1;

	// 데이터 유효성 검사
	if (!CachedStructureData.LevelStats.IsValidIndex(DataIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[Crossbow] Invalid Level Data Index: %d"), DataIndex);
		return;
	}

	const FStructureLevelInfo& LevelData = CachedStructureData.LevelStats[DataIndex];

	// 스탯 적용
	AttackDamage = LevelData.AttackDamage;
	AttackSpeed = LevelData.AttackSpeed;
	AttackRange = LevelData.AttackRange;

	// 사거리 변경 시 감지 범위 업데이트
	if (DetectSphere)
	{
		DetectSphere->SetSphereRadius(AttackRange);
	}

	// 메시 변경
	UStaticMesh* NewMesh = LevelData.TurretMesh.LoadSynchronous();
	if (NewMesh && TurretMesh)
	{
		TurretMesh->SetStaticMesh(NewMesh);
	}

	// 업그레이드 시 체력 회복
	if (HasAuthority() && AttributeSet)
	{
		float NewMaxHealth = LevelData.Health;
		
		if (Level > 1)
		{
			float OldMaxHealth = AttributeSet->GetMaxHealth();
			float CurrentHealth = AttributeSet->GetHealth();
			
			float HealthIncrease = NewMaxHealth - OldMaxHealth;

			// 최대 체력 업데이트
			AttributeSet->SetMaxHealth(NewMaxHealth);

			// 최대 체력 증가량 만큼만 증가
			if (HealthIncrease > 0.0f)
			{
				float NewHealth = FMath::Clamp(CurrentHealth + HealthIncrease, 0.0f, NewMaxHealth);
				AttributeSet->SetHealth(NewHealth);
                
				//UE_LOG(LogTemp, Log, TEXT("[Upgrade] HP Reinforced: %.1f -> %.1f (Diff: +%.1f)"), CurrentHealth, NewHealth, HealthIncrease);
			}
		}
		else if (Level == 1)
		{
			AttributeSet->SetMaxHealth(NewMaxHealth);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("[Crossbow] Applied Level %d Stats. Dmg: %.1f"), Level, AttackDamage);
}

// 파괴 이펙트, 사운드 등 여기서 작업
void ACrossbowStructure::HandleDestruction()
{
	// 부모 함수 호출
	Super::HandleDestruction();
}

void ACrossbowStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACrossbowStructure, CurrentTarget);
}
