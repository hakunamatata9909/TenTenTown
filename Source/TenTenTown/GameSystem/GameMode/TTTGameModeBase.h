//TTTGameModeBase.h


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h" 
#include "TTTGameStateBase.h"
#include "TTTGameModeBase.generated.h"

class ACoreStructure;
class UTTTGameInstance;
class USoundBase;
class UDataTable;
class UGameplayEffect;
class APlayerStart;

UCLASS()
class TENTENTOWN_API ATTTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATTTGameModeBase();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TTT|Phase")
	void StartPhase(ETTTGamePhase NewPhase, int32 DurationSeconds);
	
	UPROPERTY(EditAnywhere, Category="TTT|Game") int32 MaxWaves = 1;
	void EndGame(bool bVictory);

	void ReturnToLobby();

	// --- Combat/Boss 페이즈용 보조 함수들 ---
	void StartCombatMonitoring();      // Combat에서 몬스터 감시 시작
	void UpdateCombatMonitoring();     // 1초마다 살아있는 몬스터 수 체크

	bool IsBossWave(int32 WaveIndex) const;

	void StartBossPhase();             // Boss 페이즈 진입
	void FinishBossPhaseTemp();        // (임시) 5초 후 Reward로 이동
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable*  WaveDataTableAsset;

	void SetupDataTables();

	// ===== Kill Count 기반 Combat/Boss 종료 =====
	// ※ SpawnSubsystem(웨이브 테이블)에서 "이번 페이즈 총 스폰 수"를 계산한 뒤 Combat/Boss 진입 직후 서버에서 호출
	UFUNCTION(BlueprintCallable, Category = "TTT|Wave")
	void SetPhaseTargetKillCount(int32 TargetKillCount);

	// ※ 몬스터/보스가 죽을 때(DeadAbility가 호출되는 시점) 서버에서 호출
	UFUNCTION(BlueprintCallable, Category = "TTT|Wave")
	void NotifyEnemyDead(AActor* DeadEnemy);

	// 스폰될 때 서버에서 호출 
	UFUNCTION(BlueprintCallable, Category = "TTT|Wave")
	void NotifyEnemySpawned(AActor* SpawnedEnemy);

	// ====== 네트워크/플레이어 관련 ======
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float EnemyBaseStatMultiplier = 1.0f;

	// 킬 1개당 추가 XP
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	float BonusXPPerKill = 10.f;

	// 보상 지급 후 킬카운트 초기화 여부
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	bool bResetKillCountAfterReward = true;

	// 웨이브당 지급 XP 
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	float RewardXPPerWave = 150.f;

	// BGM
	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TObjectPtr<USoundBase> InGameBGM = nullptr;   // Village_Day 기본

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TObjectPtr<USoundBase> BossBGM = nullptr;     // 보스 페이즈

	void BroadcastBGM(USoundBase* NewBGM);
	
protected:
	APawn* SpawnSelectedCharacter(AController* NewPlayer);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Core")
	ACoreStructure* CoreStructure = nullptr;

	
	FTimerHandle TimerHandle_Tick1s;

	void TickPhaseTimer();
	void AdvancePhase();
	int32 GetDefaultDurationFor(ETTTGamePhase Phase) const;

	UFUNCTION(Exec)
	void PM_SetPhase(const FString& Name);

	void CheckAllCharactersSpawnedAndStartBuild();

	void BindCoreEvents();

	UFUNCTION()
	void HandleCoreDead();   // OnDead 델리게이트용

	void GrantRewardPhaseRewards();
	// 중복지급 방지
	int32 LastRewardedWave = -1;

	// XP 지급용 GE 
	UPROPERTY(EditDefaultsOnly, Category="Rewards|GAS")
	TSubclassOf<UGameplayEffect> RewardXPGEClass;

	// “EXP 100당 1레벨” 규칙값
	UPROPERTY(EditDefaultsOnly, Category="Rewards")
	float ExpPerLevel = 100.f;

	// SetByCaller 태그
	FGameplayTag RewardXPSetByCallerTag;

private:
	bool GetSpawnCapsuleForPlayer(AController* Player, float& OutRadius, float& OutHalfHeight) const;
	bool IsStartSpotFree(const AActor* Start, float Radius, float HalfHeight) const;
	APlayerStart* FindStartByTag(const FName& TagName) const;

	// 같은 프레임/흐름에서 중복 선택 방지용 “예약”
	TMap<TWeakObjectPtr<AController>, TWeakObjectPtr<AActor>> ReservedStartByController;
	TSet<TWeakObjectPtr<AActor>> ReservedStarts;
	ATTTGameStateBase* GS() const { return GetGameState<ATTTGameStateBase>(); };

	bool bHasReturnedToLobby = false;

	// ================================
	// Combat / Boss 상태 관리용
	// ================================
	// Combat 중 "살아있는 몬스터 수" 체크 타이머
	FTimerHandle TimerHandle_CombatMonitor;

	// Combat 경과 시간(초) - 너무 빨리 0마리라고 끝내지 않도록 최소 시간 두기용
	float CombatElapsedTime = 0.0f;

	// Combat 시작 후 최소 몇 초가 지난 뒤부터 "0마리면 종료"로 볼지
	UPROPERTY(EditAnywhere, Category = "TTT|Game")
	float MinCombatTimeBeforeClearCheck = 3.0f;

	// Boss 페이즈 유지 타이머 (임시 5초)
	FTimerHandle TimerHandle_BossPhase;

	// Boss 페이즈에서 머무는 시간(임시) - 나중에 보스 처치 이벤트로 대체 예정
	UPROPERTY(EditAnywhere, Category = "TTT|Game")
	float BossPhaseDuration = 5.0f;
	// Boss가 아직 미완성일 때만 임시 타이머로 Reward로 넘길지 (기본: false)
	UPROPERTY(EditAnywhere, Category = "TTT|Game")
	float bUseTempBossTimer = false;

	// Boss 페이즈에서 Kill Count 목표치 (보통 1 = 보스 1마리)
	UPROPERTY(EditAnywhere, Category = "TTT|Game")
	int32 DefaultBossKillTarget = 1;

	// ===== Kill Count 기반 종료용 카운터 =====
	int32 PhaseTargetKillCount = 0;   // 이번 페이즈에서 죽여야 하는 총 수(테이블 기반)
	int32 PhaseDeadKillCount = 0;     // DeadAbility(NotifyEnemyDead) 호출 누적
	int32 PhaseSpawnedCount = 0;      // (선택) 스폰 누적(디버그)
	bool bPhaseClearProcessed = false;

	void ResetPhaseKillTracking();
	void TryClearPhaseByKillCount();
	TSet<TWeakObjectPtr<AActor>> PhaseCountedEnemies;

#pragma region UI_Region
public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void InitializeAllPlayerStructureLists();

protected:
	/*TArray<FInventoryItemData> CreateInitialStructureList(UDataTable* DataTable);*/
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> PlayStateGEClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ShopOpenGEClass;


protected:
	UFUNCTION()
	void HandleCoreHealthChanged(float NewHealth, float NewMaxHealth);

	void PlayerStateData(ATTTGameStateBase* GameSat, UTTTGameInstance* GameIns, bool bIsWinlose, int32 WaveLevels);

#pragma endregion



};
