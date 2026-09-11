// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/DataTable.h"
#include "WaveData.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AEnemyBase> EnemyBP;//스폰될 적

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnPoint = "Point1";//스폰 지점

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount = 5;//스폰되는 전체 개체 수

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 1.0f;//스폰 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDelay = 0.0f;//일정 시간 이후 스폰 시작

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInfiniteSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBoss = false;
};


USTRUCT()
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Wave = "0";//웨이브 이름

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpawnInfo> EnemyGroups;//웨이브마다 스폰 될 적의 정보
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RewardGold = 10;//웨이브 클리어 시 골드 보상

	
};