// LobbyGameState.cpp


#include "GameSystem/GameMode/LobbyGameState.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
	bReplicates = true;
	ConnectedPlayers = 0;
	ReadyPlayers = 0;
	LobbyPhase = ELobbyPhase::Waiting;
	CountdownSeconds  = 0;
	SelectedMapIndex = INDEX_NONE;
}

void ALobbyGameState::OnRep_ReadyPlayers()
{
	OnPlayerCountChanged.Broadcast();
}

void ALobbyGameState::OnRep_ConnectedPlayers()
{
	OnPlayerCountChanged.Broadcast();
}

void ALobbyGameState::OnRep_LobbyPhase()
{
}

void ALobbyGameState::OnRep_CountdownSeconds()
{
	OnCountdownChanged.Broadcast(CountdownSeconds);
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, ConnectedPlayers);
	DOREPLIFETIME(ALobbyGameState, ReadyPlayers);
	DOREPLIFETIME(ALobbyGameState, LobbyPhase);
	DOREPLIFETIME(ALobbyGameState, CountdownSeconds);
	DOREPLIFETIME(ALobbyGameState, SelectedMapIndex);
	DOREPLIFETIME(ALobbyGameState, PlayerResults);
}

void ALobbyGameState::SetConnectedPlayers(int32 NewCount)
{
    if (ConnectedPlayers != NewCount)
    {
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameState] SetConnectedPlayers: %d -> %d"), ConnectedPlayers, NewCount);
        ConnectedPlayers = NewCount;

        OnPlayerCountChanged.Broadcast();
    }
}

void ALobbyGameState::SetReadyPlayers(int32 NewCount)
{
    if (ReadyPlayers != NewCount)
    {
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameState] SetReadyPlayers: %d -> %d"), ReadyPlayers, NewCount);
        ReadyPlayers = NewCount;

        OnPlayerCountChanged.Broadcast();
    }
}

void ALobbyGameState::OnRep_SelectedMapIndex()
{
	OnSelectedMapChanged.Broadcast(SelectedMapIndex);
}

void ALobbyGameState::SetSelectedMapIndex(int32 NewIndex)
{
	if (HasAuthority() && SelectedMapIndex != NewIndex)
	{
		SelectedMapIndex = NewIndex;
		OnSelectedMapChanged.Broadcast(SelectedMapIndex);
	}
}

void ALobbyGameState::OnRep_PlayerResults()
{

}