#include "UI/MVVM/PlayerStatusViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterMana.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterStamina.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "UI/PCC/PlayPCComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/Texture2D.h"

UPlayerStatusViewModel::UPlayerStatusViewModel()
{
	
}

void UPlayerStatusViewModel::InitializeViewModel(UPlayPCComponent* PlayPCC, ATTTPlayerState* PlayerState, UAbilitySystemComponent* InASC)
{
	CachedPlayerState = PlayerState;	
	if (!CachedPlayerState || !InASC) return;

	CachedPlayPCComponent = PlayPCC;

	// 인수로 받은 ASC를 사용
	UAbilitySystemComponent* ASC = InASC;

	// --- 1. 공통 속성 (Health, Level) 초기화 ---	
	const UAS_CharacterBase* BaseAS = ASC->GetSet<UAS_CharacterBase>();
	if (!BaseAS)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerStatusVM] Failed to find UAS_CharacterBase on ASC. Aborting VM Initialize."));
		return;
	}

	float InitialMaxHealth = ASC->GetNumericAttributeBase(UAS_CharacterBase::GetMaxHealthAttribute());

	// MaxHealth가 유효할 때만 초기값 설정
	if (InitialMaxHealth > 0.0f)
	{
		MaxHealth = InitialMaxHealth;
		CurrentHealth = ASC->GetNumericAttributeBase(UAS_CharacterBase::GetHealthAttribute());
		SetLevel(FMath::RoundToInt(ASC->GetNumericAttributeBase(UAS_CharacterBase::GetLevelAttribute())));
		RecalculateHealthPercentage();
	}
	// else: MaxHealth가 0이면 구독 로직을 통해 OnMaxHealthChanged에서 초기화되도록 위임합니다.

	
	CleanupViewModel(); //기존 구독 해제

	
	const UAS_CharacterMana* ManaAS = ASC->GetSet<UAS_CharacterMana>();

	if (ManaAS) //마법사 Attribute Set이 등록되어 있을 때만 Mana 로직 실행
	{
		MaxMana = ASC->GetNumericAttributeBase(UAS_CharacterMana::GetMaxManaAttribute());
		CurrentMana = ASC->GetNumericAttributeBase(UAS_CharacterMana::GetManaAttribute());
		RecalculateManaPercentage();

		// 3. Mana 구독 설정 (마법사일 때만)
		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterMana::GetManaAttribute())
			.AddUObject(this, &UPlayerStatusViewModel::OnManaChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterMana::GetMaxManaAttribute())
			.AddUObject(this, &UPlayerStatusViewModel::OnMaxManaChanged);

		//중요: SetManaUIVisibility(ESlateVisibility::Visible) 등 UI 활성화 로직 추가
		SetIsManaVisible(ESlateVisibility::Visible);
	}
	else
	{
		//중요: 마법사가 아니면 Mana를 0으로 설정하고 UI를 비활성화 (숨김)
		MaxMana = 0.0f;
		CurrentMana = 0.0f;
		//UI 비활성화 로직 추가
		SetIsManaVisible(ESlateVisibility::Collapsed);
	}

	const UAS_CharacterStamina* StaminaAS = ASC->GetSet<UAS_CharacterStamina>();

	if (StaminaAS)
	{
		MaxStamina = ASC->GetNumericAttributeBase(UAS_CharacterStamina::GetMaxStaminaAttribute());
		CurrentStamina = ASC->GetNumericAttributeBase(UAS_CharacterStamina::GetStaminaAttribute());
		RecalculateStaminaPercentage();

		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterStamina::GetStaminaAttribute())
			.AddUObject(this, &UPlayerStatusViewModel::OnStaminaChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterStamina::GetMaxStaminaAttribute())
			.AddUObject(this, &UPlayerStatusViewModel::OnMaxStaminaChanged);

		SetIsStaminaVisible(ESlateVisibility::Visible);
	}
	else
	{
		MaxStamina = 0.0f;
		CurrentStamina = 0.0f;
		SetIsStaminaVisible(ESlateVisibility::Collapsed);
	}

	// --- 공통 Attribute 변화 구독 설정 (Health, Level) ---
	// 1. Level 구독
	ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetLevelAttribute())
		.AddUObject(this, &UPlayerStatusViewModel::OnLevelChanged);

	// 2. Health 구독
	ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute())
		.AddUObject(this, &UPlayerStatusViewModel::OnHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute())
		.AddUObject(this, &UPlayerStatusViewModel::OnMaxHealthChanged);


	//인벤토리 설정
	APlayerController* PC = Cast<APlayerController>(CachedPlayerState->GetOwner());
	if (!PC) { return; }
	CachedInventory = PC->FindComponentByClass<UInventoryPCComponent>();

	// --- 5. 골드 구독 설정 ---
	CachedPlayerState->OnGoldChangedDelegate.AddDynamic(this, &UPlayerStatusViewModel::SetPlayerGold);
	SetPlayerGold(CachedPlayerState->GetGold());


	// --- 6. 퀵슬롯 구독 설정 ---
	ASC->RegisterGameplayTagEvent(GASTAG::State_BuildMode, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UPlayerStatusViewModel::ChangeQuickSlotWindow);

	int32 CurrentBuildModeCount = ASC->GetTagCount(GASTAG::State_BuildMode);
	ChangeQuickSlotWindow(GASTAG::State_BuildMode, CurrentBuildModeCount);

	//초상화 설정
	const APawn* PawnCDO = CachedPlayerState->SelectedCharacterClass->GetDefaultObject<APawn>();
		
	if (PawnCDO)
	{
		const ABaseCharacter* BaseCharacterCDO = Cast<ABaseCharacter>(PawnCDO);

		if (BaseCharacterCDO)
		{
			SetIconTexture(BaseCharacterCDO->CharacterIconTexture.Get());
		}
		else
		{
			UE_LOG(LogTemp, Warning,TEXT("[PlayerStatusVM] Failed to set Icon Texture: Pawn CDO is not ABaseCharacter."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerStatusVM] Failed to get Pawn CDO."));
	}
	
}

void UPlayerStatusViewModel::InitializeViewModel()
{
}

void UPlayerStatusViewModel::CleanupViewModel()
{
    if (CachedPlayerState)
    {
        if (UAbilitySystemComponent* ASC = CachedPlayerState->GetAbilitySystemComponent())
        {
            // --- 공통 속성 해제 ---
        	if (ASC->GetSet<UAS_CharacterBase>())
        	{
        		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetLevelAttribute()).RemoveAll(this);
        		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute()).RemoveAll(this);
        		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute()).RemoveAll(this);
        	}
        	
        	// --- 스태미나 속성 해제 ---
        	if (ASC->GetSet<UAS_CharacterStamina>())
        	{
        		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterStamina::GetStaminaAttribute()).RemoveAll(this);
        		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterStamina::GetMaxStaminaAttribute()).RemoveAll(this);
        	}
        	
            // --- 마나 속성 해제 ---
            if (ASC->GetSet<UAS_CharacterMana>())
            {
                ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterMana::GetManaAttribute()).RemoveAll(this);
                ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterMana::GetMaxManaAttribute()).RemoveAll(this);
            }            
        }
    }
    //CachedPlayerState = nullptr;
    // Super::CleanupViewModel(); // UBaseViewModel에 있다면 호출
}

// ----------------------------------------------------------------------
// GAS 콜백 함수 구현
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::OnLevelChanged(const FOnAttributeChangeData& Data)
{
	SetLevel(FMath::RoundToInt(Data.NewValue));
}

void UPlayerStatusViewModel::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	RecalculateHealthPercentage();
}

void UPlayerStatusViewModel::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	RecalculateHealthPercentage();
}

void UPlayerStatusViewModel::OnManaChanged(const FOnAttributeChangeData& Data)
{
	CurrentMana = Data.NewValue;
	RecalculateManaPercentage();
}

void UPlayerStatusViewModel::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	MaxMana = Data.NewValue;
	RecalculateManaPercentage();
}

void UPlayerStatusViewModel::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	CurrentStamina = Data.NewValue;
	RecalculateStaminaPercentage();
}

void UPlayerStatusViewModel::OnMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	MaxStamina = Data.NewValue;
	RecalculateStaminaPercentage();
}






// ----------------------------------------------------------------------
// 백분율 계산 로직 (ViewModel의 핵심 책임)
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::RecalculateHealthPercentage()
{
	float NewPercentage = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;

	SetHealthPercentage(FMath::Clamp(NewPercentage, 0.0f, 1.0f));
}

void UPlayerStatusViewModel::RecalculateManaPercentage()
{
	float NewPercentage = (MaxMana > 0.0f) ? (CurrentMana / MaxMana) : 0.0f;
	SetManaPercentage(FMath::Clamp(NewPercentage, 0.0f, 1.0f));
}

void UPlayerStatusViewModel::RecalculateStaminaPercentage()
{
	float NewPercentage = (MaxStamina > 0.0f) ? (CurrentStamina / MaxStamina) : 0.0f;
	SetStaminaPercentage(FMath::Clamp(NewPercentage, 0.0f, 1.0f));
}

// ----------------------------------------------------------------------
// UPROPERTY Setter 구현 (FieldNotify 브로드캐스트)
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::SetIconTexture(UTexture2D* InTexture)
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerStatusVM] SetIconTexture called."));
	if (IconTexture != InTexture)
	{
		UE_LOG(LogTemp, Log, TEXT("[PlayerStatusVM] IconTexture changed, broadcasting update."));
		IconTexture = InTexture;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	}
}

void UPlayerStatusViewModel::SetLevel(int32 NewLevel)
{
	if (Level != NewLevel)
	{
		Level = NewLevel;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Level);
	}
}

void UPlayerStatusViewModel::SetExpPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(ExpPercentage, NewPercentage))
	{
		ExpPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ExpPercentage);
	}
}

void UPlayerStatusViewModel::SetHealthPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(HealthPercentage, NewPercentage))
	{
		HealthPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HealthPercentage);
	}
}

void UPlayerStatusViewModel::SetManaPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(ManaPercentage, NewPercentage))
	{
		ManaPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ManaPercentage);
	}
}

void UPlayerStatusViewModel::SetStaminaPercentage(float NewPercentage)
{	
	if (!FMath::IsNearlyEqual(StaminaPercentage, NewPercentage))
	{
		StaminaPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StaminaPercentage);
	}
}

void UPlayerStatusViewModel::SetIsManaVisible(ESlateVisibility bNewVisible)
{
	if (IsManaVisible != bNewVisible)
	{
		IsManaVisible = bNewVisible;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsManaVisible);
	}
}

void UPlayerStatusViewModel::SetIsStaminaVisible(ESlateVisibility bNewVisible)
{
	if (IsStaminaVisible != bNewVisible)
	{
		IsStaminaVisible = bNewVisible;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsStaminaVisible);
	}
}

void UPlayerStatusViewModel::SetIsItemQuickSlotVisible(ESlateVisibility NewVisibility)
{
	if (IsItemQuickSlotVisible != NewVisibility)
	{
		IsItemQuickSlotVisible = NewVisibility;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsItemQuickSlotVisible);
	}
}



void UPlayerStatusViewModel::SetIsStructureQuickSlotVisible(ESlateVisibility NewVisibility)
{
	if (IsStructureQuickSlotVisible != NewVisibility)
	{
		IsStructureQuickSlotVisible = NewVisibility;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsStructureQuickSlotVisible);
	}
}

void UPlayerStatusViewModel::SetPlayerGold(int32 NewGold)
{
	if (PlayerGold != NewGold)
	{
		PlayerGold = NewGold;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerGold);
	}
}


void UPlayerStatusViewModel::OnOffTraderWindow(bool OnOff)
{
	if (CachedPlayPCComponent)
	{
		// 3. 컴포넌트를 찾았으면 함수를 실행합니다.
		CachedPlayPCComponent->Server_ControllTradeOpenEffect(OnOff);
	}
}

void UPlayerStatusViewModel::ChangeQuickSlotWindow(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		SetIsItemQuickSlotVisible(ESlateVisibility::Hidden);
		SetIsStructureQuickSlotVisible(ESlateVisibility::Visible);
	}
	else
	{
		SetIsItemQuickSlotVisible(ESlateVisibility::Visible);
		SetIsStructureQuickSlotVisible(ESlateVisibility::Hidden);
	}
}
