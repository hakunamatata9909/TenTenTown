// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSubsystem.h"
#include "AbilitySystemGlobals.h"
#include "EngineUtils.h"
#include "PreloadSubsystem.h"
#include "Enemy/Base/EnemyBase.h"
#include "SpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Enemy/Data/WaveData.h"
#include "Engine/World.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"

void USpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CacheSpawnPoints();
}

void USpawnSubsystem::StartWave(int32 WaveIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    UPreloadSubsystem* PreloadSubsystem = World->GetSubsystem<UPreloadSubsystem>();
    if (!PreloadSubsystem)
    {
        return;
    }
    if (!PreloadSubsystem->EnemyWaveData.Contains(WaveIndex))
    {
        return;
    }
    const FEnemyWave& Wave = PreloadSubsystem->EnemyWaveData[WaveIndex];

    bool bIsBossWave = false;
    
    if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(World)))
    {
        bIsBossWave = GM->IsBossWave(WaveIndex); //보스 웨이브 확인
    }
    // =========================
    // ★ 1) 이번 웨이브 총 스폰 수 계산
    // =========================
    int32 TotalSpawnCount = 0;
    bool bHasInfinite = false;
    
    for (const FEnemySpawnInfo& Info : Wave.EnemyInfos)
    {
        if (Info.bInfiniteSpawn)
        {
            bHasInfinite = true;
        }
        else if (!Info.bIsBoss) // 보스는 카운트 제외
        {
            TotalSpawnCount += Info.SpawnCount;
        }
    }

    // =========================
    // ★ 2) GameMode에 Target 세팅 (이게 없으면 페이즈 절대 안 넘어감)
    // =========================
    if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(World)))
    {
        if (bHasInfinite)
        {
            UE_LOG(LogTemp, Error, TEXT("[SpawnSubsystem] Wave %d has bInfiniteSpawn=true. KillCount clear cannot work on this wave."), WaveIndex);
            // 그냥 0이면 TryClearPhaseByKillCount가 비교 자체를 안 함 → 끝나지 않는 게 정상
            GM->SetPhaseTargetKillCount(0);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[SpawnSubsystem] Wave %d TotalSpawnCount=%d -> SetPhaseTargetKillCount"), WaveIndex, TotalSpawnCount);
            GM->SetPhaseTargetKillCount(TotalSpawnCount);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnSubsystem] GameMode is not ATTTGameModeBase. Cannot set target kill count."));
    }
    
    // =========================
    // 3) 기존 스폰 타이머 세팅
    // =========================
    for (const FEnemySpawnInfo& Info : Wave.EnemyInfos)
    {
        if (Info.bIsBoss) // 컴뱃 페이즈에서만 일반 몬스터 스폰
        {
            continue;
        }

        const int32 TaskIndex = ActiveSpawnTasks.Emplace(WaveIndex, Info);

        World->GetTimerManager().SetTimer(
              ActiveSpawnTasks[TaskIndex].TimerHandle,
              FTimerDelegate::CreateUObject(
                  this,
                  &USpawnSubsystem::HandleSpawnTick,
                  TaskIndex
              ),
              Info.SpawnInterval,
              true,
              Info.SpawnDelay
          );
    }
}

void USpawnSubsystem::SpawnBoss(int32 WaveIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    UPreloadSubsystem* PreloadSubsystem = World->GetSubsystem<UPreloadSubsystem>();
    if (!PreloadSubsystem)
    {
        return;
    }
    if (!PreloadSubsystem->EnemyWaveData.Contains(WaveIndex))
    {
        return;
    }
    const FEnemyWave& Wave = PreloadSubsystem->EnemyWaveData[WaveIndex];

    // 보스 몬스터만 스폰
    for (const FEnemySpawnInfo& Info : Wave.EnemyInfos)
    {
        if (Info.bIsBoss)
        {
            SpawnEnemy(WaveIndex, Info);
        }
    }
}

void USpawnSubsystem::SpawnEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo)
{
    UPreloadSubsystem* PreloadSubsystem = GetWorld()->GetSubsystem<UPreloadSubsystem>();
    if (!PreloadSubsystem)
    {
        return;
    }
    
    ASpawnPoint* SpawnPoint = FindSpawnPointByName(EnemyInfo.SpawnPoint);
    if (!SpawnPoint)
    {
        return;
    }
    FTransform SpawnTransform = SpawnPoint->GetSpawnTransform();


    UClass* EnemyClass = EnemyInfo.EnemyBP.Get();

    if (!EnemyClass)
    {
        EnemyClass = EnemyInfo.EnemyBP.LoadSynchronous();
    }

    if (!EnemyClass)
    {
        return;
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(
        EnemyClass,
        SpawnTransform.GetLocation(),
        SpawnTransform.GetRotation().Rotator(),
        SpawnParams
    );

    if (Enemy)
    {
        Enemy->SpawnWaveIndex = WaveIndex;
        Enemy->InitializeEnemy();

       
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy))
        {
            if (const UAS_EnemyAttributeSetBase* AttrSet = ASC->GetSet<UAS_EnemyAttributeSetBase>())
            {
                if (!EnemyInfo.bIsBoss)
                {
                    float BaseMult = 1.0f;
                    if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
                    {
                        BaseMult = GM->EnemyBaseStatMultiplier;
                    }
                    
                    float Mult = BaseMult + WaveIndex * 0.1f;
                    float NewMaxHealth = AttrSet->GetMaxHealth() * Mult;
                    float NewAttack = AttrSet->GetAttack() * Mult;
                    ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute(), NewMaxHealth);
                    ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetHealthAttribute(), NewMaxHealth);
                    ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute(), NewAttack);
                }
            }
        }

        Enemy->StartTree();
        FSpawnedEnemyWave& SpawnedWave = WaveSpawnedEnemies.FindOrAdd(WaveIndex);
        SpawnedWave.SpawnedEnemies.Add(Enemy);
    }
}

void USpawnSubsystem::CacheSpawnPoints()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (ASpawnPoint* Point = Cast<ASpawnPoint>(Actor))
        {
            SpawnPointMap.Add(Point->PointName, Point);
        }
    }
}

ASpawnPoint* USpawnSubsystem::FindSpawnPointByName(FName PointName)
{
    if (TWeakObjectPtr<ASpawnPoint>* Found = SpawnPointMap.Find(PointName))
    {
        return Found->Get();
    }
    
    return nullptr;
}

void USpawnSubsystem::HandleSpawnTick(int32 TaskIndex)
{
    if (!ActiveSpawnTasks.IsValidIndex(TaskIndex))
    {
        return;
    }
    
    FSpawnTask& Task = ActiveSpawnTasks[TaskIndex];

    if (Task.Info.bInfiniteSpawn)
    {
        SpawnEnemy(Task.WaveIndex, Task.Info);
        return;
    }

    if (Task.SpawnedCount < Task.Info.SpawnCount)
    {
        SpawnEnemy(Task.WaveIndex, Task.Info);
        Task.SpawnedCount++;
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(Task.TimerHandle);
    }
}


void USpawnSubsystem::EndWave(int32 WaveIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    for (FSpawnTask& Task : ActiveSpawnTasks)
    {
        World->GetTimerManager().ClearTimer(Task.TimerHandle);
    }
   
    ActiveSpawnTasks.Empty();

   
    if (FSpawnedEnemyWave* Wave = WaveSpawnedEnemies.Find(WaveIndex))
    {
        for (TWeakObjectPtr<AEnemyBase>& EnemyPtr : Wave->SpawnedEnemies)
        {
            if (EnemyPtr.IsValid())
            {
                EnemyPtr->Destroy();
            }
        }
        Wave->SpawnedEnemies.Empty();
    }
}
