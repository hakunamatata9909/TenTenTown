#include "Structure/IceTrap/IceTrapStructure.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "TTTGamePlayTags.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Engine/StaticMesh.h"

AIceTrapStructure::AIceTrapStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);
	BaseMesh->SetCollisionProfileName(TEXT("TowerStructure"));
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BaseMesh);
	TurretMesh->SetCollisionProfileName(TEXT("TowerStructure"));

	SlowArea = CreateDefaultSubobject<USphereComponent>(TEXT("SlowArea"));
	SlowArea->SetupAttachment(RootComponent);
	
	// 적만 감지하도록 설정
	SlowArea->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SlowArea->SetSphereRadius(AttackRange);

	// DetectSphere 설치 트레이스에 감지 x
	SlowArea->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore); // GridFloor 채널 무시
}

void AIceTrapStructure::BeginPlay()
{
	if (SlowArea)
	{
		SlowArea->OnComponentBeginOverlap.AddDynamic(this, &AIceTrapStructure::OnEnemyEntered);
		SlowArea->OnComponentEndOverlap.AddDynamic(this, &AIceTrapStructure::OnEnemyExited);
	}
	
	Super::BeginPlay();
}

void AIceTrapStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;

	// 범위 내에 적이 있을 때만 타이머가 작동
	if (EnemyCountInRange > 0)
	{
		FireTimer += DeltaTime;
		float AttackInterval = FMath::Max(0.1f, AttackSpeed);

		if (FireTimer >= AttackInterval)
		{
			//UE_LOG(LogTemp, Warning, TEXT("[IceTrap] Pulse Attack Executed! Targets nearby: %d"), EnemyCountInRange);
			PulseAttack();
			FireTimer = 0.0f;
		}
	}
}

void AIceTrapStructure::PulseAttack()
{
	TArray<AActor*> OverlappingEnemies;
	SlowArea->GetOverlappingActors(OverlappingEnemies, AEnemyBase::StaticClass());

	for (AActor* EnemyActor : OverlappingEnemies)
	{
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(EnemyActor);
		if (ASCInterface) // 인터페이스가 있는지 먼저 확인
		{
			UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
			if (TargetASC) // ASC가 유효한지 확인
			{
				FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
				EffectContext.AddInstigator(GetInstigator(), this);

				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(SlowGameplayEffectClass, CurrentUpgradeLevel, EffectContext);
				if (SpecHandle.IsValid())
				{
					float CalculatedMultiplier = (100.0f - SlowMagnitude) / 100.0f;
					
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(
						GASTAG::Data_Structure_SlowMagnitude, 
						CalculatedMultiplier
					);
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}

	// 큐
	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_IceTrap_Active, CueParams);
		//UE_LOG(LogTemp, Log, TEXT("[IceTrap] GameplayCue Executed"));
	}
}

void AIceTrapStructure::OnEnemyEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<AEnemyBase>() && OtherComp == OtherActor->GetRootComponent())
	{
		EnemyCountInRange++;
	}
}

void AIceTrapStructure::OnEnemyExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<AEnemyBase>() && OtherComp == OtherActor->GetRootComponent())
	{
		EnemyCountInRange = FMath::Max(0, EnemyCountInRange - 1);
		
		if (EnemyCountInRange == 0)
		{
			FireTimer = 0.0f;
		}
	}
}

void AIceTrapStructure::ApplyStructureStats(int32 Level)
{
	int32 DataIndex = Level - 1;
	if (!CachedStructureData.LevelStats.IsValidIndex(DataIndex))
	{
		return;
	}

	const FStructureLevelInfo& LevelData = CachedStructureData.LevelStats[DataIndex];
	
	AttackRange = LevelData.AttackRange;
	AttackSpeed = LevelData.AttackSpeed;
	SlowMagnitude = LevelData.SlowMagnitude;

	if (SlowArea)
	{
		SlowArea->SetSphereRadius(LevelData.AttackRange);
		SlowArea->UpdateOverlaps();
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
			}
		}
		else if (Level == 1)
		{
			AttributeSet->SetMaxHealth(NewMaxHealth);
			AttributeSet->SetHealth(NewMaxHealth);
		}
	}
}

void AIceTrapStructure::UpgradeStructure()
{
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel) return;

	CurrentUpgradeLevel++;
	
	// 시각 효과 실행
	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Upgrade, CueParams);
	}

	ApplyStructureStats(CurrentUpgradeLevel);
}

void AIceTrapStructure::HandleDestruction()
{
	Super::HandleDestruction();
}
