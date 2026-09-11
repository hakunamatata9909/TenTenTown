#include "Structure/Barricade/BarricadeStructure.h"
#include "Components/StaticMeshComponent.h"
#include "Structure/Data/StructureData.h" 
#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Engine/StaticMesh.h"

ABarricadeStructure::ABarricadeStructure()
{
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	SetRootComponent(TurretMesh);
	TurretMesh->SetCollisionProfileName(TEXT("TowerStructure"));
}

void ABarricadeStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABarricadeStructure::UpgradeStructure()
{
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel)
	{
		return;
	}
	
	CurrentUpgradeLevel++;

	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();

		// 업그레이드 큐 실행
		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Upgrade, CueParams);
	}
	
	ApplyStructureStats(CurrentUpgradeLevel);
}

void ABarricadeStructure::ApplyStructureStats(int32 Level)
{
	int32 DataIndex = Level - 1;
	
	// 데이터 유효성 검사
	if (!CachedStructureData.LevelStats.IsValidIndex(DataIndex)) return;
	
	// 현재 레벨에 해당하는 데이터 가져오기 (Level은 1부터 시작하므로 인덱스는 Level-1)
	const FStructureLevelInfo& LevelData = CachedStructureData.LevelStats[DataIndex];

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
                
				UE_LOG(LogTemp, Log, TEXT("[Upgrade] HP Reinforced: %.1f -> %.1f (Diff: +%.1f)"), CurrentHealth, NewHealth, HealthIncrease);
			}
		}
		else if (Level == 1)
		{
			AttributeSet->SetMaxHealth(NewMaxHealth);
		}
	}
}

// 파괴, 사운드 이펙트 작업
void ABarricadeStructure::HandleDestruction()
{
	Super::HandleDestruction();
}
