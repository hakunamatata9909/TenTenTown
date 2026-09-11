// Fill out your copyright notice in the Description page of Project Settings.

#include "PreloadSubsystem.h"
#include "Engine/AssetManager.h"

void UPreloadSubsystem::SetupTable(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
    WaveTable = InWaveTable;
    PreloadWaveEnemies(WaveTable);
}

void UPreloadSubsystem::PreloadWaveEnemies(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
    
    TArray<FSoftObjectPath> AssetsToLoad;
    TArray<FName> RowNames = InWaveTable->GetRowNames();
    
    for (int32 WaveIdx = 0; WaveIdx < RowNames.Num(); ++WaveIdx)
    {
        const FWaveData* WaveDataRow = InWaveTable->FindRow<FWaveData>(RowNames[WaveIdx], TEXT("Preload"));
        if (!WaveDataRow)
        {
            continue;
        }
        FEnemyWave& Wave = EnemyWaveData.FindOrAdd(WaveIdx);
        Wave.EnemyInfos = WaveDataRow->EnemyGroups;
        

        for (const FEnemySpawnInfo& Info : WaveDataRow->EnemyGroups)
        {
            if (!Info.EnemyBP.IsNull())
            {
                AssetsToLoad.Add(Info.EnemyBP.ToSoftObjectPath());
            }
        }
    }
    if (AssetsToLoad.Num() > 0)
    {
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        
        // 에셋 비동기 로드
        PreloadHandle = Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateLambda([]()
        {
            UE_LOG(LogTemp, Log, TEXT("[PreloadSubsystem] Preload Complete."));
        }));
    }
}

