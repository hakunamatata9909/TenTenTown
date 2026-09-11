#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h" // 추가 예정

class UStaticMesh;

#include "StructureData.generated.h"

USTRUCT(BlueprintType)
struct FStructureLevelInfo
{
	GENERATED_BODY()

public:
	// 현재 레벨에서 사용할 터렛 메시 (없으면 이전 단계 유지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> TurretMesh;

	// 현재 레벨의 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage = 10.0f;
	// 현재 레벨의 공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float SlowMagnitude = 0.3f;
	// 현재 레벨의 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 1000.0f;
	// 현재 레벨의 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Health = 1000.0f;
	// 업그레이드 비용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UpgradeCost = 500;
};

USTRUCT(BlueprintType)
struct FStructureData: public FTableRowBase
{
	GENERATED_BODY()

public:
	// [UI] 구조물 이름(한글 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText DisplayName;
	// [UI] 구조물 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText Description;
	// [UI] 구조물 이미지(TSoftObjectPtr = 실제 필요할 때 로딩)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftObjectPtr<UTexture2D> StructureImage;
	// [UI, 인게임] 최대 설치 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI | Ingame")
	int32 MaxInstallCount = 10;
	// [UI, 인게임] 최대 업글 단계
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI | Ingame")
	int32 MaxUpgradeLevel = 3;
	// [UI, 인게임] 설치 비용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	float InstallCost = 300;
	
	// [인게임] 프리뷰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingame")
	TSubclassOf<AActor> PreviewActorClass;
	// [인게임] 실제 스폰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingame")
	TSubclassOf<AActor> ActualStructureClass;
	
	// ----- [스탯] -----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	TArray<FStructureLevelInfo> LevelStats;
};
