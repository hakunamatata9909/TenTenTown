#include "LobbyPCComponent.h"
#include "AbilitySystemComponent.h"
#include "TTTGameplayTags.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UI/CharSellectWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "UI/LobbyWidget.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "TenTenTown/GameSystem/Player/TTTPlayerController.h"
#include "TimerManager.h"
#include "UI/Widget/MapSelectWidget.h"
#include "UI/Widget/ResultWidget.h"
#include "GameSystem/GameMode/LobbyGameMode.h"


ULobbyPCComponent::ULobbyPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULobbyPCComponent::BeginPlay()
{
	Super::BeginPlay();

    ReBeginPlay();
}

void ULobbyPCComponent::ReBeginPlay()
{
    if (GetWorld() && InitCheckTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(InitCheckTimerHandle);
    }

    // 이전 레벨의 UI나 데이터가 남아있다면 싹 정리 (Clean up)
    CloseLobbyUI();

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        //1. State.Mode.Lobby 태그 변화를 구독하고 OnLobbyTagChanged 함수를 연결합니다.
        ASC->RegisterGameplayTagEvent(GASTAG::State_Mode_Lobby, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ULobbyPCComponent::OnLobbyTagChanged);

        //2. 캐릭터 선택 태그 변화 구독 (새로운 로직)
        ASC->RegisterGameplayTagEvent(GASTAG::UI_State_CharacterSelectOpen, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ULobbyPCComponent::OnCharacterSelectionTagChanged);
        ASC->RegisterGameplayTagEvent(GASTAG::UI_State_MapSelectOpen, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ULobbyPCComponent::OnMapSelectionTagChanged);
        ASC->RegisterGameplayTagEvent(GASTAG::State_Role_Host, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ULobbyPCComponent::OnRoleHostTagChanged);
        ASC->RegisterGameplayTagEvent(GASTAG::UI_State_ResultOpen, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ULobbyPCComponent::OnResultOpenTagChanged);


        // 2. BeginPlay 시점에 이미 태그가 붙어있을 경우를 처리합니다.
        int32 CurrentCount = ASC->GetTagCount(GASTAG::State_Mode_Lobby);
        OnLobbyTagChanged(GASTAG::State_Mode_Lobby, CurrentCount);
        int32 CurrentCharacterSelectCount = ASC->GetTagCount(GASTAG::UI_State_CharacterSelectOpen);
        OnCharacterSelectionTagChanged(GASTAG::UI_State_CharacterSelectOpen, CurrentCharacterSelectCount);
		int32 CurrentMapSelectCount = ASC->GetTagCount(GASTAG::UI_State_MapSelectOpen);
		OnMapSelectionTagChanged(GASTAG::UI_State_MapSelectOpen, CurrentMapSelectCount);
		int32 CurrentRoleHostCount = ASC->GetTagCount(GASTAG::State_Role_Host);
        OnRoleHostTagChanged(GASTAG::State_Role_Host, CurrentRoleHostCount);
		int32 CurrentResultOpenCount = ASC->GetTagCount(GASTAG::UI_State_ResultOpen);
		OnResultOpenTagChanged(GASTAG::UI_State_ResultOpen, CurrentResultOpenCount);
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(
            this,
            &ULobbyPCComponent::ReBeginPlay
        );
    }
}



// OnModeTagChanged는 Base 클래스에서 정의된 가상 함수입니다.
void ULobbyPCComponent::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// Base 함수 호출 (필요 시)
	// Super::OnModeTagChanged(Tag, NewCount); 

	// OnLobbyTagChanged에서 이미 로직을 처리하므로 여기는 비워두거나,
	// 여러 모드 태그를 처리하도록 확장할 수 있습니다. 
}

void ULobbyPCComponent::OnLobbyTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OpenLobbyUI();
	}
	else
	{
		CloseLobbyUI();
	}
}


// 이 함수들이 기존에 ATTTPlayerController에 있던 위젯 생성/파괴 역할을 대체합니다.

void ULobbyPCComponent::OpenLobbyUI()
{
    // APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr; // 기존 코드에서 이미 가져옴
    APlayerController* PC = GetOwner<APlayerController>();

    if (!PC || !PC->GetLocalPlayer())
    {
        UE_LOG(LogTemp, Warning, TEXT("OpenLobbyUI skipped: PC is not linked to a Local Player.z"));
        return;
    }

    // 1. GameState 가져오기
    ALobbyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    
    ATTTPlayerController* TTT_PC = Cast<ATTTPlayerController>(PC);

    // 2. 뷰모델 생성 및 초기화
    // ULocalPlayer 체크 로직 대신 PC 체크만으로 충분할 수 있습니다.
    if (PC && TTT_PC && !LobbyRootViewModel)
    {
        // 뷰모델 생성 (PCC가 소유)
        LobbyRootViewModel = NewObject<ULobbyViewModel>(this);

        // GameState만 전달하여 초기화
        LobbyRootViewModel->Initialize(GS, TTT_PC, this);
    }

    // 3. 위젯 생성 및 뷰모델 연결
    // CharSellectWidgetClass, CharSellectWidgetInstance 대신 
    // LobbyWidgetClass, LobbyWidgetInstance 변수를 사용한다고 가정합니다.
    if (LobbyWidgetClass && GetWorld() && PC)
    {
        // LobbyWidget 인스턴스 생성
        // UCharSellectWidget 대신 ULobbyWidget을 사용한다고 가정
        LobbyWidgetInstance = CreateWidget<ULobbyWidget>(PC, LobbyWidgetClass);

        if (LobbyWidgetInstance)
        {
            //SetViewModel 호출: ViewModel 인스턴스를 Widget에 주입 (핵심 연결)
            LobbyWidgetInstance->SetViewModel(LobbyRootViewModel);

            LobbyWidgetInstance->AddToViewport();
        }
    }

    // 4. 캐릭터 선택 위젯 생성 및 뷰모델 연결 (CharSellectWidget)
    if (CharSellectWidgetClass && GetWorld() && PC)
    {
        // CharSellectWidget 인스턴스 생성
        CharSellectWidgetInstance = CreateWidget<UCharSellectWidget>(PC, CharSellectWidgetClass);

        if (CharSellectWidgetInstance)
        {
            // SetViewModel 호출: ViewModel 인스턴스를 Widget에 주입
            CharSellectWidgetInstance->SetViewModel(LobbyRootViewModel);

            // 팝업이므로 바로 띄울 수도 있고, 숨겨진 상태로 둘 수도 있습니다.
            CharSellectWidgetInstance->AddToViewport(1); // ZOrder를 높여 메인 위젯 위에 표시
            //CharSellectWidgetInstance->HideWidget(); // 초기에는 숨김 처리 가정
        }
    }

    if (MapSelectWidgetClass && GetWorld()&& PC)
    {
        MapSelectWidgetInstance = CreateWidget<UMapSelectWidget>(PC, MapSelectWidgetClass);
        if (MapSelectWidgetInstance)
        {
			MapSelectWidgetInstance->SetViewModel(LobbyRootViewModel);
			MapSelectWidgetInstance->AddToViewport(2); // ZOrder를 더 높여서 가장 위에 표시
        }
    }

    if (ResultWidgetClass && GetWorld() && PC)
    {
        ResultWidgetInstance = CreateWidget<UResultWidget>(PC, ResultWidgetClass);
        if (ResultWidgetInstance)
        {
            ResultWidgetInstance->SetLobbyViewModel(LobbyRootViewModel);
            ResultWidgetInstance->AddToViewport(10); // ZOrder를 더 높여서 가장 위에 표시
        }
	}

    // WaitWidget 등 다른 UI도 여기서 생성/관리합니다.
}

void ULobbyPCComponent::CloseLobbyUI()
{
    // 1. 위젯 제거
    if (CharSellectWidgetInstance)
    {
        CharSellectWidgetInstance->RemoveFromParent();
        CharSellectWidgetInstance = nullptr;
    }
    if (LobbyWidgetInstance)
    {
        LobbyWidgetInstance->RemoveFromParent();
        LobbyWidgetInstance = nullptr;
    }
    if( MapSelectWidgetInstance)
    {
        MapSelectWidgetInstance->RemoveFromParent();
        MapSelectWidgetInstance = nullptr;
	}
    if (ResultWidgetInstance)
    {
        ResultWidgetInstance->RemoveFromParent();
        ResultWidgetInstance = nullptr;
	}

    // 2. 뷰모델 정리
    if (LobbyRootViewModel)
    {
        LobbyRootViewModel->CleanupViewModel(); // 구독 해제 등 정리 로직
        LobbyRootViewModel = nullptr; // GC 대상
    }
    
}


void ULobbyPCComponent::OnCharacterSelectionTagChanged(const FGameplayTag Tag, int32 NewCount)
{    
    if (CharSellectWidgetInstance)
    {
        if (NewCount > 0)
        {
            // 태그가 부여됨: 위젯 표시 (Visible)
            CharSellectWidgetInstance->SetVisibility(ESlateVisibility::Visible);            
        }
        else
        {
            // 태그가 제거됨: 위젯 숨기기 (Hidden)
            CharSellectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);            
        }
        UpdateInputMode();
    }
}

void ULobbyPCComponent::OnMapSelectionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (MapSelectWidgetInstance)
    {
        if (NewCount > 0)
        {
            // 태그가 부여됨: 위젯 표시 (Visible)
            MapSelectWidgetInstance->SetVisibility(ESlateVisibility::Visible);            
        }
        else
        {
            // 태그가 제거됨: 위젯 숨기기 (Hidden)
            MapSelectWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
        UpdateInputMode();
    }

}

void ULobbyPCComponent::UpdateInputMode()
{
    APlayerController* PC = GetOwner<APlayerController>();
    if (!PC)
    {
        return;
    }

    // 캐릭터 선택 위젯 또는 맵 선택 위젯 중 하나라도 보이는지 확인
    bool bIsAnyUIOpen = (CharSellectWidgetInstance && CharSellectWidgetInstance->GetVisibility() == ESlateVisibility::Visible) ||
        (MapSelectWidgetInstance && MapSelectWidgetInstance->GetVisibility() == ESlateVisibility::Visible);

    if (bIsAnyUIOpen)
    {
        // **하나라도 열려 있음:** UI 전용 입력 모드 설정
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
        LobbyRootViewModel->SetMapButtonVisibility(ESlateVisibility::Hidden);
        LobbyRootViewModel->SetCharButtonVisibility(ESlateVisibility::Hidden);

    }
    else
    {
        // **모두 닫혀 있음:** 게임 전용 입력 모드 설정
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
        LobbyRootViewModel->SetMapButtonVisibility(ESlateVisibility::Visible);
		LobbyRootViewModel->SetCharButtonVisibility(ESlateVisibility::Visible);        
    }
}

void ULobbyPCComponent::OnRoleHostTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (LobbyRootViewModel)
    {
        if (NewCount > 0)
        {
            // 호스트 역할이 부여됨
            LobbyRootViewModel->SetIsHost(true);
        }
        else
        {
            // 호스트 역할이 제거됨
            LobbyRootViewModel->SetIsHost(false);
        }
    }
}

void ULobbyPCComponent::OnResultOpenTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    //서버실행 차단

    if (ResultWidgetInstance)
    {
        if (NewCount > 0)
        {
            //result vms 세팅
            LobbyRootViewModel->SetResultVMs();
            ResultWidgetInstance->SetResultListView();
            ResultWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            ResultWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
        UpdateInputMode();
    }
}


void ULobbyPCComponent::Server_ResultWindowCloseEffect_Implementation()
{
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

    UWorld* World = GetWorld();
    ALobbyGameMode* LobbyGameMode = World ? Cast<ALobbyGameMode>(World->GetAuthGameMode()) : nullptr;

    if (!PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("not PS"));
    }
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("not ASC"));
    }
    if (!LobbyGameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("not LobbyGameMode"));
    }
    if (!LobbyGameMode->ResultGEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("not LobbyGameMode Eft"));
    }

    if (PS && ASC && LobbyGameMode && LobbyGameMode->ResultGEClass)
    {
        ASC->RemoveActiveGameplayEffectBySourceEffect(LobbyGameMode->ResultGEClass, ASC);
    }
}
