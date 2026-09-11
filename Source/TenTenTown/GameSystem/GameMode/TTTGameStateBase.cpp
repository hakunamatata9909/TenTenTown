// TTTGameStateBase.cpp


#include "GameSystem/GameMode/TTTGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "Character/PS/TTTPlayerState.h"

ATTTGameStateBase::ATTTGameStateBase()
{
	bReplicates = true;
	Wave=0;
}

void ATTTGameStateBase::OnRep_Phase()
{
    const UEnum* EnumPtr = StaticEnum<ETTTGamePhase>();
    const FString PhaseName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)Phase) : TEXT("Unknown");

    UE_LOG(LogTemp, Warning, TEXT("[GameState] Phase Changed -> %s (%d)"), *PhaseName, (int32)Phase);

    OnPhaseChanged.Broadcast(Phase);
}

void ATTTGameStateBase::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void ATTTGameStateBase::OnRep_Wave()
{    
    OnWaveLevelChangedDelegate.Broadcast(Wave);
}

void ATTTGameStateBase::OnRep_RemainEnemy()
{
    // C++ delegate
    OnRemainEnemyChangedDelegate.Broadcast(RemainEnemy);

    // BP delegate
    OnRemainEnemyChanged.Broadcast(RemainEnemy);
}
void ATTTGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTGameStateBase, Phase);
	DOREPLIFETIME(ATTTGameStateBase, RemainingTime);
	DOREPLIFETIME(ATTTGameStateBase, Wave);
    
    DOREPLIFETIME(ATTTGameStateBase, CoreHealth);
	DOREPLIFETIME(ATTTGameStateBase, CoreMaxHealth);
    DOREPLIFETIME(ATTTGameStateBase, WaveLevel);
    DOREPLIFETIME(ATTTGameStateBase, RemainEnemy);
}


#pragma region UI_Region
void ATTTGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        //OnPlayerJoinedDelegate.Broadcast(TTTPlayerState);
    }
}

void ATTTGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        //OnPlayerLeftDelegate.Broadcast(TTTPlayerState);
    }

    Super::RemovePlayerState(PlayerState);
}



void ATTTGameStateBase::NotifyPlayerJoined(ATTTPlayerState* NewPlayerState)
{
    //OnPlayerJoinedDelegate.Broadcast(NewPlayerState);
}

void ATTTGameStateBase::NotifyPlayerLeft(ATTTPlayerState* LeavingPlayerState)
{
    //OnPlayerLeftDelegate.Broadcast(LeavingPlayerState);
}

void ATTTGameStateBase::SetWaveLevel(int32 NewWaveLevel)
{
    if (WaveLevel != NewWaveLevel)
    {
        WaveLevel = NewWaveLevel;
        OnWaveLevelChangedDelegate.Broadcast(WaveLevel);
    }
}

void ATTTGameStateBase::SetRemainEnemy(int32 NewRemainEnemy)
{
    if (RemainEnemy != NewRemainEnemy)
    {
        RemainEnemy = NewRemainEnemy;
        OnRemainEnemyChangedDelegate.Broadcast(RemainEnemy);
        OnRemainEnemyChanged.Broadcast(RemainEnemy); 
    }
}

void ATTTGameStateBase::NotifyPlayerReady()
{
    if (!HasAuthority())
    {
        return;
    }

    Multicast_PlayerJoinedBroadcast();
}
void ATTTGameStateBase::Multicast_PlayerJoinedBroadcast_Implementation()
{
    OnPlayerJoinedDelegate.Broadcast();
}

void ATTTGameStateBase::UpdateCoreHealthUI(float NewHealth, float NewMaxHealth)
{
	CoreHealth = static_cast<int32>(NewHealth);
	CoreMaxHealth = static_cast<int32>(NewMaxHealth);
}
void ATTTGameStateBase::OnRep_CoreHealth()
{
    OnCoreHealthUpdated.Broadcast(CoreHealth, CoreMaxHealth);
}
void ATTTGameStateBase::OnRep_CoreMaxHealth()
{
    OnCoreHealthUpdated.Broadcast(CoreHealth, CoreMaxHealth);
}
#pragma endregion

