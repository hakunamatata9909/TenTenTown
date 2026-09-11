#include "PlayPCComponent.h"
#include "GameFramework/PlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "AbilitySystemInterface.h"
#include "Components/PrimitiveComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "UI/PlayWidget.h"
#include "UI/TradeMainWidget.h"
#include "UI/Widget/QuickSlotBarWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "TTTGameplayTags.h" 
#include "UI/MVVM/TradeViewModel.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"
#include "UI/MVVM/SkillCoolTimeViewModel.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "UI/Widget/PingWidget.h"
#include "UI/Widget/PingIconWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"



UPlayPCComponent::UPlayPCComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPlayPCComponent::BeginPlay()
{
    Super::BeginPlay();
    
    PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>();
    GameStatusViewModel = NewObject<UGameStatusViewModel>(this);
    PartyManagerViewModel = NewObject<UPartyManagerViewModel>();
    QuickSlotManagerViewModel = NewObject<UQuickSlotManagerViewModel>();
	TradeViewModel = NewObject<UTradeViewModel>();
    SkillCoolTimeViewModel = NewObject<USkillCoolTimeViewModel>();

    
    APlayerController* PC = GetPlayerController();
   

    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle,
        this,
        &UPlayPCComponent::TestFunction,
        0.1f, // 0.1초 뒤에 실행
        false // 반복 안 함 (한 번만 실행)
    );
    
    
}

void UPlayPCComponent::TestFunction()
{
    if (!MyASC)
    {
        FindSetASC();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] BeginPlay: MyASC already set."));
    }
}

void UPlayPCComponent::FindSetASC()
{
    APlayerController* PC = GetPlayerController();
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	
    if (ASC)
    {
        MyASC = ASC;        
        //이벤트 구독
        const FGameplayTag ModeTag = GASTAG::State_Mode_Gameplay; // 가독성을 위해 태그를 변수에 저장

        MyASC->RegisterGameplayTagEvent(ModeTag, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &UPlayPCComponent::OnModeTagChanged);
    
        int32 CurrentCount = MyASC->GetTagCount(ModeTag);

        OnModeTagChanged(ModeTag, CurrentCount);
	}
    else
    {
        GetWorld()->GetTimerManager().SetTimer(
            SetASCTimerHandle,
            this,
            &UPlayPCComponent::FindSetASC,
            0.1f,
            false
        );
    }
}

void UPlayPCComponent::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (Tag == GASTAG::State_Mode_Gameplay)
    {
        if (NewCount > 0)
        {
            OpenHUDUI();
        }
        else
        {
            GetWorld()->GetTimerManager().ClearTimer(OpenReadyTimerHandle);
            CloseHUDUI();
        }
    }
}


void UPlayPCComponent::OpenHUDUI()
{
    APlayerController* PC = GetPlayerController();
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    
    //캐릭터 없으면 리턴
    if (!ASC || !ASC->GetAttributeSet(UAS_CharacterBase::StaticClass()))
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: ASC or AttributeSet is null. Retrying..."));
        CallOpenReadyUI();
        return;
    }
    ATTTGameStateBase* GS = GetGameStateRef();
        
    if (!PC || !PC->GetPawn() || !PS || !GS)
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: Essential references are null. Retrying..."));
        CallOpenReadyUI();
        return;
    }
	UWorld* InWorld = GetWorld();
    if (!InWorld)
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: World context is null. Retrying..."));
        CallOpenReadyUI();
        return;
    }
    UTTTGameInstance* TTTGI = InWorld->GetGameInstance<UTTTGameInstance>();
    if (!TTTGI)
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: TTTGameInstance is null. Retrying..."));
        CallOpenReadyUI();
        return; 
    }

    ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(PC->GetPawn());
    if (!BaseCharacter)
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: BaseCharacter is null. Retrying..."));
        CallOpenReadyUI();
        return;
    }


    PlayerStateRef = PS;
    GameStateRef = GS;
    MyASC = ASC;
	MyCharacter = BaseCharacter;
    

    
    //ViewModel 초기화
   
    PlayerStatusViewModel->InitializeViewModel(this, PlayerStateRef, MyASC.Get());
    GameStatusViewModel->InitializeViewModel(GameStateRef, MyASC.Get());
    PartyManagerViewModel->InitializeViewModel(PlayerStateRef, GameStateRef, this);
    QuickSlotManagerViewModel->InitializeViewModel(PlayerStateRef, TTTGI);
    TradeViewModel->InitializeViewModel(this, PlayerStateRef, TTTGI);
    SkillCoolTimeViewModel->InitializeViewModel(MyASC, MyCharacter);
    
    
    
    //카메라
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMiniMapCamera::StaticClass());
        
    if (FoundActor)
    {
        AMiniMapCamera* CachingMinimapCamera = Cast<AMiniMapCamera>(FoundActor);
        if (CachingMinimapCamera && GameStatusViewModel)
        {
            GameStatusViewModel->SetMinimapCamera(CachingMinimapCamera);
            UE_LOG(LogTemp, Log, TEXT("MiniMapCamera Found and Set!"));
        }
        else
        {
			UE_LOG(LogTemp, Error, TEXT("MiniMapCamera Cast FAILED!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MiniMapCamera NOT found in World! 레벨에 배치했는지 확인하세요."));
    }
    

    //PlayWidgetInstance 생성
    if (!PlayWidgetInstance)
    {
        PlayWidgetInstance = CreateWidget<UPlayWidget>(PC, PlayWidgetClass);
    }
    if (!PlayWidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCC] PlayWidgetInstance FAILED TO CREATE. Aborting UI Init."));
        return;
    }

    //ViewModel 주입 및 화면 표시
    PlayWidgetInstance->SetPlayerStatusViewModel(PlayerStatusViewModel);
    PlayWidgetInstance->SetGameStatusViewModel(GameStatusViewModel);
    PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);
    PlayWidgetInstance->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel);
	PlayWidgetInstance->SetSkillCoolTimeViewModel(SkillCoolTimeViewModel);
    PlayWidgetInstance->SetWidgetToMVs();

    //TradeMainWidgetInstance->;//세팅
    if (!TradeMainWidgetInstance)
    {
        TradeMainWidgetInstance = CreateWidget<UTradeMainWidget>(PC, TradeMainWidgetClass);
    }
    if (!TradeMainWidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCC] PlayWidgetInstance FAILED TO CREATE. Aborting UI Init."));
        return;
    }

    TradeMainWidgetInstance->SetPCC(this);
	TradeMainWidgetInstance->SetPlayerStatusViewModel(PlayerStatusViewModel);
	TradeMainWidgetInstance->SetTradeViewModel(TradeViewModel);
	

    // 임시지연    
    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle3,
        this,
        &UPlayPCComponent::TestFunction3,
        2.0f,
        false
    );



    if (!PlayWidgetInstance->IsInViewport())
    {
        PlayWidgetInstance->AddToViewport();
    }
    if (!TradeMainWidgetInstance->IsInViewport())
    {
        TradeMainWidgetInstance->AddToViewport();
	}




    
    //캐릭터 선택 태그 변화 구독 (새로운 로직)
    ASC->RegisterGameplayTagEvent(GASTAG::UI_State_ShopOpen, EGameplayTagEventType::NewOrRemoved)
        .AddUObject(this, &UPlayPCComponent::OnShopOpenTagChanged);
    
    //BeginPlay 시점에 이미 태그가 붙어있을 경우를 처리
    int32 CurrentShopOpenCount = ASC->GetTagCount(GASTAG::UI_State_ShopOpen);
    OnShopOpenTagChanged(GASTAG::UI_State_ShopOpen, CurrentShopOpenCount);
    



    //ui Input
    if (!PingWheelInstance)
    {
        PingWheelInstance = CreateWidget<UPingWidget>(PC, PingWheelWidgetClass);
		PingWheelInstance->SetPCC(this);
    }

    if (!PingWheelInstance->IsInViewport())
    {
        PingWheelInstance->AddToViewport();
        PingWheelInstance->SetVisibility(ESlateVisibility::Hidden);
    }

	ATTTPlayerController* TTTPC = Cast<ATTTPlayerController>(PC);
    if (TTTPC)
    {
        TTTPC->SetInputUI();
    }

 

    //캐릭터 선택 태그 변화 구독 (새로운 로직)
    /*
    ASC->RegisterGameplayTagEvent(GASTAG::UI_State_ShopOpen, EGameplayTagEventType::NewOrRemoved)
        .AddUObject(this, &UPlayPCComponent::OnShopOpenTagChanged);

    //BeginPlay 시점에 이미 태그가 붙어있을 경우를 처리
    int32 CurrentPingOpenCount = ASC->GetTagCount(GASTAG::UI_State_PingOpen);
    OnPingOpenTagChanged(GASTAG::UI_State_PingOpen, CurrentPingOpenCount);
    */





 /*   GetWorld()->GetTimerManager().SetTimer(
        RefreshTimerHandle,
        this,
        &UPlayPCComponent::ForceRefreshList,
        5.1f,
        false
    );*/

    /*GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle2,
        this,
        &UPlayPCComponent::TestFunction2,
        1.0f,
        true
    );*/
}

void UPlayPCComponent::CallOpenReadyUI()
{
    GetWorld()->GetTimerManager().SetTimer(
        OpenReadyTimerHandle,
        this,
        &UPlayPCComponent::OpenHUDUI,
        0.1f,
        false
    );
}

void UPlayPCComponent::ForceRefreshList()
{
    if (!GameStateRef)
    {
        return;
    }

    GetPartyManagerViewModel()->ResetAndRefreshAll();
}




void UPlayPCComponent::TestFunction3()
{
    PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);
    //PlayWidgetInstance->SetsPartyListView();
    //트레이드인스턴스 위젯이 제대로 생성됫는지 로그로 확인
    //UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] TestFunction3: TradeMainWidgetInstance: %p"), TradeMainWidgetInstance.Get());
    
    TradeViewModel->CallSlotDelegate();
}




void UPlayPCComponent::RemoveStartWidget()
{
    // PlayWidgetInstance를 제거하는 로직 (필요 시 구현)
}

void UPlayPCComponent::OpenTrader(bool BIsOpen)
{
    // TradeMainWidget을 열고 닫는 로직 (필요 시 구현)
}
UTradeMainWidget* UPlayPCComponent::GetTradeMainWidgetInstance() const
{
    return TradeMainWidgetInstance.Get();    
}




void UPlayPCComponent::CloseHUDUI()
{
    if (PlayWidgetInstance && PlayWidgetInstance->IsInViewport())
    {
        PlayWidgetInstance->RemoveFromParent();
    }
    if (TradeMainWidgetInstance && TradeMainWidgetInstance->IsInViewport())
    {
        TradeMainWidgetInstance->RemoveFromParent();
    }
}


void UPlayPCComponent::HandlePhaseChanged(ETTTGamePhase NewPhase)
{
    // GameState 델리게이트 핸들러 (필요 시 구현)
}

void UPlayPCComponent::HandleRemainingTimeChanged(int32 NewRemainingTime)
{
    // GameState 델리게이트 핸들러 (필요 시 구현)
}

void UPlayPCComponent::TestFunction2()
{   
    if (!MyASC)
    {
        UE_LOG(LogTemp, Error, TEXT("[TestFunction2] Failed to get ASC. Cannot apply damage GE."));
        return;
    }

    if (!DebugDamageGEClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[TestFunction2] DebugDamageGEClass is not set in editor. Cannot apply damage."));
        return;
    }
    APlayerController* PC = GetPlayerController();
    if (!PC->IsLocalController())
    {
        // 서버에서 실행
        
    }
    ServerApplyDamageGE();
}


void UPlayPCComponent::ServerApplyDamageGE_Implementation()
{
    if (!MyASC || !DebugDamageGEClass) return;

    FGameplayEffectContextHandle Context = MyASC->MakeEffectContext();

    FGameplayEffectSpecHandle SpecHandle = MyASC->MakeOutgoingSpec(DebugDamageGEClass, 1.f, Context);

    if (SpecHandle.IsValid())
    {        
        MyASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}


void UPlayPCComponent::OnShopOpenTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (TradeMainWidgetInstance)
    {
        if (NewCount > 0)
        {
            if (TradeMainWidgetInstance->GetVisibility() != (ESlateVisibility::Visible))
            {
                TradeMainWidgetInstance->SetVisibility(ESlateVisibility::Visible);
            }
            PingWheelInstance->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            if (TradeMainWidgetInstance->GetVisibility() != (ESlateVisibility::Hidden))
            {
                TradeMainWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        UpdateInputMode();
    }

}

void UPlayPCComponent::UpdateInputMode()
{
    APlayerController* PC = GetOwner<APlayerController>();
    if (!PC)
    {
        return;
    }

    bool bIsAnyUIOpen = (TradeMainWidgetInstance && TradeMainWidgetInstance->GetVisibility() == ESlateVisibility::Visible);

    if (bIsAnyUIOpen)
    {
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
    }
    else
    {
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
    }
}



void UPlayPCComponent::Server_ControllTradeOpenEffect_Implementation(bool OnOff)
{
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

    UWorld* World = GetWorld();
    ATTTGameModeBase* TTTGameMode = World ? Cast<ATTTGameModeBase>(World->GetAuthGameMode()) : nullptr;

    if (!PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("not PS"));
    }
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("not ASC"));
    }
    if (!TTTGameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("not TTTGameMode"));
    }
    if (!TTTGameMode->ShopOpenGEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("not TTTGameMode Eft"));
    }

    if (PS && ASC && TTTGameMode && TTTGameMode->ShopOpenGEClass)
    {
        int32 CurrentShopOpenCount = ASC->GetTagCount(GASTAG::UI_State_ShopOpen);

        if (OnOff)
        {
            if (CurrentShopOpenCount > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("Shop is already open. Skipping GE application."));
                return;
            }

            FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
            ContextHandle.AddSourceObject(this);

            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(TTTGameMode->ShopOpenGEClass, 1.0f, ContextHandle);

            if (SpecHandle.IsValid())
            {
                ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);                
            }
        }
        else
        {   
            if (CurrentShopOpenCount <= 0)
            {
				UE_LOG(LogTemp, Log, TEXT("Shop is already closed. Skipping GE removal."));
				return;
            }
            // 3. 이펙트 제거
            ASC->RemoveActiveGameplayEffectBySourceEffect(TTTGameMode->ShopOpenGEClass, ASC);
        }
    }
}




void UPlayPCComponent::OnPingPressed()
{
    bool bIsShopOpen = (TradeMainWidgetInstance && TradeMainWidgetInstance->GetVisibility() == ESlateVisibility::Visible);
    if (bIsShopOpen) return;

    PingWheelInstance->SetVisibility(ESlateVisibility::Visible);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(PingWheelInstance->TakeWidget());
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    int32 SizeX, SizeY;
    PC->GetViewportSize(SizeX, SizeY);
    PC->SetMouseLocation(SizeX / 2, SizeY / 2);
}

void UPlayPCComponent::OnPingReleased()
{
    ATTTPlayerController* PC = Cast<ATTTPlayerController>(GetOwner());
    if (!PC || !PingWheelInstance) return;

    if (PingWheelInstance->GetVisibility() == ESlateVisibility::Hidden) return;

    if (PingTypeSelected != -1)
    {
        Server_RequestPing(PingTypeSelected);
        
        PingTypeSelected = -1;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Ping Canceled (No button hovered)"));
    }

    PingWheelInstance->SetVisibility(ESlateVisibility::Hidden);

    FInputModeGameOnly GameMode;
    PC->SetInputMode(GameMode);
    PC->bShowMouseCursor = false;

    
}


void UPlayPCComponent::OnPingOpenTagChanged(const FGameplayTag Tag, int32 NewCount)
{   
    if (PingWheelInstance)
    {
        if (NewCount > 0)
        {
            PingWheelInstance->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            PingWheelInstance->SetVisibility(ESlateVisibility::Hidden);
        }
        UpdateInputMode();
    }
}

void UPlayPCComponent::OnShopPressed()
{
    if (!TradeMainWidgetInstance)
    {
        return;
    }
    if (TradeMainWidgetInstance && TradeMainWidgetInstance->GetVisibility() == ESlateVisibility::Visible)
    {
        Server_ControllTradeOpenEffect(false);
        OnShopOpenTagChanged(FGameplayTag(), 0);
    }
    else if (TradeMainWidgetInstance && TradeMainWidgetInstance->GetVisibility() == ESlateVisibility::Hidden)
    {
		Server_ControllTradeOpenEffect(true);
        OnShopOpenTagChanged(FGameplayTag(), 1);
    }
}

void UPlayPCComponent::Server_RequestPing_Implementation(int32 PingType)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC)
        {
            UPlayPCComponent* TargetComp = PC->FindComponentByClass<UPlayPCComponent>();
            if (TargetComp)
            {
                TargetComp->Client_SpawnPingWidget(PlayerStateRef, PingType);
            }
        }
    }
}

void UPlayPCComponent::Client_SpawnPingWidget_Implementation(APlayerState* TargetPS, int32 PingType)
{
    if (!TargetPS || !GameStatusViewModel) return;

    UPingIconWidget* NewPingWidget = PlayWidgetInstance->CreatePingIconWidget(PingType);

    NewPingWidget->SetLocation(GameStatusViewModel->CreatePingVM(TargetPS));
}


void UPlayPCComponent::SetPartyWidgets()
{
    if (PlayWidgetInstance)
    {
		PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);
    }
}