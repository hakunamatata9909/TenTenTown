// TTTGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Character/PS/TTTPlayerState.h"
#include "TTTGameStateBase.generated.h"

UENUM(BlueprintType)
enum class ETTTGamePhase:uint8
{
	Waiting,
	Build,
	Combat,
	Boss,
	Reward,
	Victory,
	GameOver,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTTTOnPhaseChanged,ETTTGamePhase,NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTTTOnRemainingTimeChanged,int32,NewReamaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTTTOnRemainEnemyChanged, int32, NewRemainEnemy);

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedSignature, ATTTPlayerState* /* NewPlayerState */);
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLeftSignature, ATTTPlayerState* /* LeavingPlayerState */);
DECLARE_MULTICAST_DELEGATE(FOnPlayerJoinedDelegate);

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoreHealthChangedSignature, int32 /* NewCoreHealth */);
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChangedSignature, int32 /* NewRemainingTime */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWaveLevelChangedSignature, int32 /* NewWaveLevel */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemainEnemyChangedSignature, int32 /* NewRemainEnemy */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreHealthUpdated, float, NewHealth, float, NewMaxHealth);


UCLASS()
class TENTENTOWN_API ATTTGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATTTGameStateBase();

	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly, Category="TTT|Phase")
	ETTTGamePhase Phase = ETTTGamePhase::Waiting;

	UPROPERTY(ReplicatedUsing=OnRep_RemainingTime, BlueprintReadOnly, Category="TTT|Phase")
	int32 RemainingTime = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Wave, BlueprintReadOnly, Category = "TTT|Phase")
	int32 Wave = 1;
	
	UPROPERTY(BlueprintAssignable, Category="TTT|Phase")
	FTTTOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category="TTT|Phase")
	FTTTOnRemainingTimeChanged OnRemainingTimeChanged;
	
	UFUNCTION()
	void OnRep_Phase();

	UFUNCTION()
	void OnRep_RemainingTime();
	
	UFUNCTION()
	void OnRep_Wave();

	UFUNCTION()
	void OnRep_RemainEnemy();

	UPROPERTY(BlueprintAssignable)
	FTTTOnRemainEnemyChanged OnRemainEnemyChanged;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;




#pragma region UI_Region
protected:
	UPROPERTY(ReplicatedUsing = OnRep_CoreHealth, BlueprintReadOnly)
	int32 CoreHealth = 10;
	UPROPERTY(ReplicatedUsing = OnRep_CoreMaxHealth, BlueprintReadOnly)
	int32 CoreMaxHealth = 10;

	UPROPERTY(Replicated)
	int32 WaveLevel = 1;

	UPROPERTY(ReplicatedUsing=OnRep_RemainEnemy)
	int32 RemainEnemy = 0;

public:

	//FOnCoreHealthChangedSignature OnCoreHealthChangedDelegate;
	//FOnRemainingTimeChangedSignature OnRemainingTimeChangedDelegate;
	FOnWaveLevelChangedSignature OnWaveLevelChangedDelegate;
	FOnRemainEnemyChangedSignature OnRemainEnemyChangedDelegate;

	void NotifyPlayerJoined(ATTTPlayerState* NewPlayerState);
	void NotifyPlayerLeft(ATTTPlayerState* LeavingPlayerState);

	int32 GetCoreHealth() { return CoreHealth; }
	int32 GetWaveLevel() { return WaveLevel; }
	int32 GetRemainingTime() { return RemainingTime; }
	int32 GetRemainEnemy() { return RemainEnemy; }

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	
	//void SetRemainingTime(int32 NewRemainingTime);
	void SetWaveLevel(int32 NewWaveLevel);
	void SetRemainEnemy(int32 NewRemainEnemy);

	

	UFUNCTION(BlueprintCallable, Category = "Party")
	TArray<ATTTPlayerState*> GetAllCurrentPartyMembers(ATTTPlayerState* ExcludePlayerState = nullptr) const
	{
		TArray<ATTTPlayerState*> Result;
		for (APlayerState* PS : PlayerArray)
		{
			ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS);
			if (TTTPS && TTTPS != ExcludePlayerState)
			{
				Result.Add(TTTPS);
			}
		}
		return Result;
	}
	FOnPlayerJoinedDelegate OnPlayerJoinedDelegate;
	void NotifyPlayerReady();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerJoinedBroadcast();


public:
	UPROPERTY(BlueprintAssignable, Category = "TTT|Core")
	FOnCoreHealthUpdated OnCoreHealthUpdated;

	void UpdateCoreHealthUI(float NewHealth, float NewMaxHealth);
	
	UFUNCTION()
	void OnRep_CoreHealth();
	UFUNCTION()
	void OnRep_CoreMaxHealth();

#pragma endregion

};
