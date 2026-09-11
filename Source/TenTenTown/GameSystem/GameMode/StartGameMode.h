// StartGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StartGameMode.generated.h"

class UUserWidget;

UCLASS()
class TENTENTOWN_API AStartGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AStartGameMode();

protected:
	virtual void BeginPlay() override;
};
