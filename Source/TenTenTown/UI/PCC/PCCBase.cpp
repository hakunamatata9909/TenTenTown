#include "UI/PCC/PCCBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Engine/World.h"


UPCCBase::UPCCBase()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPCCBase::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* UPCCBase::GetAbilitySystemComponent() const
{
    const APlayerController* PC = Cast<APlayerController>(GetOwner());

    if (PC)
    {
        if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
        {
            IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PS);

            if (ASCInterface)
            {
                return ASCInterface->GetAbilitySystemComponent();
            }
        }
    }
    return nullptr;
}

void UPCCBase::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
}

APlayerController* UPCCBase::GetPlayerController() const
{
    // 오너를 APlayerController로 캐스팅하여 가져옵니다.
    return Cast<APlayerController>(GetOwner());
}

ATTTPlayerState* UPCCBase::GetPlayerStateRef() const
{
    APlayerController* PC = GetPlayerController();
    if (PC)
    {
        return Cast<ATTTPlayerState>(PC->PlayerState);
    }
    return nullptr;
}

ATTTGameStateBase* UPCCBase::GetGameStateRef() const
{
    if (GetWorld())
    {
        return Cast<ATTTGameStateBase>(GetWorld()->GetGameState());
    }
    return nullptr;
}
