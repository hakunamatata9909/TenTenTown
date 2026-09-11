// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Enemy/Data/WaveData.h"
#include "Engine/StreamableManager.h"
#include "PreloadSubsystem.generated.h"

USTRUCT()
struct FEnemyWave
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FEnemySpawnInfo> EnemyInfos;//스폰할 몬스터 정보
};

UCLASS()
class TENTENTOWN_API UPreloadSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    //게임모드에서 데이터 테이블 받아오기
    void SetupTable(UDataTable* InWaveTable);

    //데이터 테이블에서 웨이브별 몬스터 데이터 저장
    void PreloadWaveEnemies(UDataTable* InWaveTable);
    
    UPROPERTY()
    TMap<int32, FEnemyWave> EnemyWaveData;

private:
    UPROPERTY()
    UDataTable* WaveTable = nullptr;

    //비동기 로드된 에셋 저장
    TSharedPtr<FStreamableHandle> PreloadHandle;
};
