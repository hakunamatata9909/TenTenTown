#include "UI/MVVM/ResultSlotViewModel.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Character/Characters/Base/BaseCharacter.h"



void UResultSlotViewModel::InitializeSlot(ATTTPlayerController* InPlayerController, const FPlayerResultData& InPlayerResult)
{
	CachedPlayerController = InPlayerController;

	if (!CachedPlayerController) { return; }

	UTTTGameInstance* TTTGI = CachedPlayerController->GetGameInstance<UTTTGameInstance>();
	PlayerName = InPlayerResult.PlayerName;
	KillCountText = FText::AsNumber(InPlayerResult.Kills);
	ScoreText = FText::AsNumber(InPlayerResult.Score);

	if (InPlayerResult.bIsWin)
	{
		IconTexture = TTTGI->WinCharacterIcons[InPlayerResult.CharacterIndex];
	}
	else
	{
		IconTexture = TTTGI->LoseCharacterIcons[InPlayerResult.CharacterIndex];
	}


	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(KillCountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ScoreText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}

void UResultSlotViewModel::ReCharge()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(KillCountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ScoreText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}
