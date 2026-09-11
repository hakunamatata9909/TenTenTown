#include "UI/MVVM/PartyStatusViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "AbilitySystemInterface.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/Texture2D.h"

UPartyStatusViewModel::UPartyStatusViewModel()
{
    
}

void UPartyStatusViewModel::InitializeViewModel()
{
}


void UPartyStatusViewModel::InitializeViewModel(ATTTPlayerState* PartyPlayerState)
{
    CachedPlayerState = PartyPlayerState;
    if (!CachedPlayerState) return;
    
    SetNameText(FText::FromString(CachedPlayerState->GetPlayerName()));
    
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            // Health 구독
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute())
                .AddUObject(this, &UPartyStatusViewModel::OnHealthChanged);

            // MaxHealth 구독
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute())
                .AddUObject(this, &UPartyStatusViewModel::OnMaxHealthChanged);

            
            RecalculateHealthPercentage();
        }
    }

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
            UE_LOG(LogTemp, Warning, TEXT("[PlayerStatusVM] Failed to set Icon Texture: Pawn CDO is not ABaseCharacter."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayerStatusVM] Failed to get Pawn CDO."));
    }
}

void UPartyStatusViewModel::CleanupViewModel()
{
    // GAS 구독 해제
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute()).RemoveAll(this);
        }
    }

    CachedPlayerState = nullptr;
}


void UPartyStatusViewModel::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentMaxHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());
            const float NewHealthPercentage = CurrentMaxHealth > 0 ? Data.NewValue / CurrentMaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage); // Setter 호출 -> UI 브로드캐스트
        }
    }
}

void UPartyStatusViewModel::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
            const float NewMaxHealth = Data.NewValue;
            const float NewHealthPercentage = NewMaxHealth > 0 ? CurrentHealth / NewMaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage); // Setter 호출 -> UI 브로드캐스트
        }
    }
}


void UPartyStatusViewModel::SetIconTexture(UTexture2D* InTexture)
{
    if (IconTexture != InTexture)
    {
        IconTexture = InTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
    }
}
void UPartyStatusViewModel::SetNameText(FText NewText)
{
    if (!NameText.EqualTo(NewText))
    {
        NameText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(NameText);
    }
}

void UPartyStatusViewModel::SetHealthPercentage(float NewValue)
{
    HealthPercentage = NewValue;    
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HealthPercentage);
    
}

void UPartyStatusViewModel::SetHeadTexture(UTexture2D* NewTexture)
{
    if (HeadTexture != NewTexture)
    {
        HeadTexture = NewTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HeadTexture);
    }
}

void UPartyStatusViewModel::RecalculateHealthPercentage()
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
            const float MaxHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());
            
            const float NewHealthPercentage = MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage);
        }
    }
}