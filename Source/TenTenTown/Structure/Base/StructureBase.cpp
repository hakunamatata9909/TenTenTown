#include "Structure/Base/StructureBase.h"
#include "Net/UnrealNetwork.h"
#include "Character/PS/TTTPlayerState.h"
#include "Structure/GridSystem/GridFloorActor.h"

AStructureBase::AStructureBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAS_StructureAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AStructureBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				UAS_StructureAttributeSet::GetHealthAttribute()
			).AddUObject(this, &AStructureBase::OnHealthChanged);
		}
	}

	// 시작 시 데이터 로드 및 초기화
	InitializeStructure();
}

void AStructureBase::OnRep_StructureDataTable()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] DataTable Arrived!"));
	InitializeStructure();
}

void AStructureBase::OnRep_StructureRowName()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] RowName Arrived: %s"), *StructureRowName.ToString());
	InitializeStructure();
}

void AStructureBase::InitializeStructure()
{
	if (!StructureDataTable || StructureRowName.IsNone()) return;

	static const FString ContextString(TEXT("Structure Initialization"));
	FStructureData* RowData = StructureDataTable->FindRow<FStructureData>(StructureRowName, ContextString);

	if (RowData)
	{
		// 찾은 데이터를 캐싱 변수에 복사
		CachedStructureData = *RowData;
		UE_LOG(LogTemp, Log, TEXT("[StructureBase] Data Loaded Successfully: %s"), *StructureRowName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[StructureBase] Row Not Found: %s"), *StructureRowName.ToString());
		return;
	}

	// 데이터 적용
	if (CachedStructureData.LevelStats.Num() > 0)
	{
		// 1레벨(인덱스 0) 데이터 가져오기
		const FStructureLevelInfo& InitData = CachedStructureData.LevelStats[0];

		// GAS 속성값 초기화
		if (AttributeSet)
		{
			AttributeSet->InitMaxHealth(InitData.Health);
			AttributeSet->InitHealth(InitData.Health);
		}
        
		// 스탯 적용 함수 호출 (현재 레벨 1)
		CurrentUpgradeLevel = 1;
		ApplyStructureStats(CurrentUpgradeLevel);
	}
}

void AStructureBase::SetBuilder(ATTTPlayerState* InPlayerState)
{
	BuilderPlayerState = InPlayerState;
}

bool AStructureBase::IsBuilder(ATTTPlayerState* InPlayerState) const
{
	if (!BuilderPlayerState) return false;

	return BuilderPlayerState == InPlayerState;
}

int32 AStructureBase::GetUpgradeCost() const
{
	// 최대 레벨이거나 그 이상이면 비용 0
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel) return 0;

	// 현재 레벨의 인덱스 계산 (1레벨 = 0번 인덱스)
	int32 DataIndex = CurrentUpgradeLevel - 1;

	// 배열 범위 확인
	if (CachedStructureData.LevelStats.IsValidIndex(DataIndex))
	{
		return CachedStructureData.LevelStats[DataIndex].UpgradeCost;
	}

	UE_LOG(LogTemp, Warning, TEXT("GetUpgradeCost: Invalid Level Index!"));
	return 0;
}

// 판매 비용 로직
int32 AStructureBase::GetSellReturnAmount() const
{
	//return CachedStructureData.InstallCost;
	
	int32 TotalSpent = CachedStructureData.InstallCost;

	// 업그레이드에 쓴 돈도 계산에 포함
	for (int32 i = 0; i < CurrentUpgradeLevel - 1; i++)
	{
		// 데이터 테이블 설정 실수로 배열 범위를 벗어나는 것을 방지
		if (CachedStructureData.LevelStats.IsValidIndex(i))
		{
			TotalSpent += CachedStructureData.LevelStats[i].UpgradeCost;
		}
	}

	// 70% 환급
	return FMath::FloorToInt(TotalSpent * 0.7f);
}

void AStructureBase::UpgradeStructure()
{
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel) return;

	CurrentUpgradeLevel++;

	OnRep_UpgradeLevel();
	
	UE_LOG(LogTemp, Log, TEXT("[StructureBase] Upgraded to Level %d"), CurrentUpgradeLevel);
}

void AStructureBase::SellStructure()
{
	if (AbilitySystemComponent)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();

		AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Destroy, CueParams);
	}
	
	SetActorHiddenInGame(true);       // 안 보이게 숨김
	SetActorEnableCollision(false);   // 충돌 끔
	SetLifeSpan(0.1f);
}

void AStructureBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		HandleDestruction();
	}
}

void AStructureBase::HandleDestruction()
{
	if (IsPendingKillPending()) return;

	// 이벤트 브로드캐스트
	if (OnStructureDestroyed.IsBound())
	{
		OnStructureDestroyed.Broadcast(this);
	}

	// 서버 체크
	if (HasAuthority())
	{
		if (ParentGridFloor.IsValid())
		{
			// 그리드 점유 해제
			if (ParentGridFloor.IsValid())
			{
				ParentGridFloor->TryRemoveStructure(GetActorLocation());
			}
		}
		if (AbilitySystemComponent)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
            
			AbilitySystemComponent->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Destroy, CueParams);
		}
		
		SetActorEnableCollision(false); // 충돌 끄기
		SetActorHiddenInGame(true);     // 숨김
		SetLifeSpan(0.1f);
	}
}

void AStructureBase::ApplyStructureStats(int32 Level)
{
}

void AStructureBase::SetParentGridFloor(AGridFloorActor* InGridFloor)
{
	ParentGridFloor = InGridFloor;
}

void AStructureBase::OnRep_UpgradeLevel()
{
	ApplyStructureStats(CurrentUpgradeLevel);
}

void AStructureBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DT 동기화
	DOREPLIFETIME(AStructureBase, CurrentUpgradeLevel);
	DOREPLIFETIME(AStructureBase, StructureRowName);
	DOREPLIFETIME(AStructureBase, StructureDataTable);
	
	// 설치자 정보 동기화
	DOREPLIFETIME(AStructureBase, BuilderPlayerState);
}
