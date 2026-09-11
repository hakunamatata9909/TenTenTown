#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "PCCBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UPCCBase : public UActorComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPCCBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// ASC를 안전하게 가져오는 함수 (PlayerState에서 찾음)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// UI 활성화/비활성화 로직을 여기에 구현합니다.
	virtual void OnModeTagChanged(const FGameplayTag Tag, int32 NewCount);

public:

	APlayerController* GetPlayerController() const;
	ATTTPlayerState* GetPlayerStateRef() const;
	ATTTGameStateBase* GetGameStateRef() const;

		
};
