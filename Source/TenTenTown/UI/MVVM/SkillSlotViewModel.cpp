#include "UI/MVVM/SkillSlotViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "TimerManager.h"
#include "GameSystem/GAS/TTTASComponent.h"
#include "GameplayTagContainer.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "Character/ENumInputID.h"

void USkillSlotViewModel::InitializeViewModel()
{
}

void USkillSlotViewModel::InitializeSlot(UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> InGAClass, ENumInputID InInputID)
{
    UTTTASComponent* MyASC = Cast<UTTTASComponent>(InASC);
    if (!MyASC || !InGAClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SkillSlotVM Init Failed: Missing ASC or GAClass (or ASC not UTTTASComponent)."));
        return;
    }

    CachedASC = MyASC;
    GAClass = InGAClass;


    //GA 클래스로부터 Cooldown GE 클래스 추출 및 캐싱
    const UGameplayAbility* GA_CDO = Cast<UGameplayAbility>(GAClass->GetDefaultObject());
    if (!GA_CDO)
    {
        UE_LOG(LogTemp, Error, TEXT("SkillSlotVM Init Failed: Invalid GA CDO for %s"), *GAClass->GetName());
        return;
    }
    
    const UBaseGameplayAbility* BaseGA_CDO = Cast<UBaseGameplayAbility>(GA_CDO);

    if (BaseGA_CDO && BaseGA_CDO->SkillIconTexture)
    {
        //아이콘을 가져와 ViewModel에 설정합니다.
        SetIconTexture(BaseGA_CDO->SkillIconTexture);
    }
    else
    {
        // 아이콘이 설정되지 않은 경우 처리 (예: 기본 아이콘 설정)
        // SetIconTexture(DefaultEmptyIcon);
        UE_LOG(LogTemp, Warning, TEXT("GA %s has no SkillIconTexture set."), *GAClass->GetName());
    }

    
    // CooldownGameplayEffectClass가 있는 경우
    if (GA_CDO->GetClass()->FindPropertyByName(FName(TEXT("CooldownGameplayEffectClass"))))
    {
        CooldownGEClass = *GA_CDO->GetClass()->FindPropertyByName(FName(TEXT("CooldownGameplayEffectClass")))->ContainerPtrToValuePtr<TSubclassOf<UGameplayEffect>>(GA_CDO);
    }
    else if (GA_CDO->GetClass()->FindPropertyByName(FName(TEXT("CooldownGameplayEffect"))))
    {
        // UPROPERTY가 UGameplayEffect* 타입일 경우
        UGameplayEffect* CooldownGE = *GA_CDO->GetClass()->FindPropertyByName(FName(TEXT("CooldownGameplayEffect")))->ContainerPtrToValuePtr<UGameplayEffect*>(GA_CDO);
        if (CooldownGE)
        {
            CooldownGEClass = CooldownGE->GetClass();
        }
    }


    if (!CooldownGEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GA %s has no Cooldown GE Class."), *GAClass->GetName());        
        return;
    }



    //Cooldown GE 클래스에서 쿨타임 태그 추출
    const UGameplayEffect* GE_CDO = Cast<UGameplayEffect>(CooldownGEClass->GetDefaultObject());
    
    if (GE_CDO)
    {
        // 우선적으로 GE가 대상에게 부여하는 태그들(GetGrantedTags)을 확인
        const FGameplayTagContainer& GrantedTags = GE_CDO->GetGrantedTags();

        if (GrantedTags.Num() > 0)
        {
            for (const FGameplayTag& Tag : GrantedTags)
            {
                CooldownTag = Tag;
                break; // 첫 번째 태그를 사용
            }
        }
        else
        {
            // GrantedTags가 비어있다면 자산 태그(GetAssetTags)도 확인
            const FGameplayTagContainer& AssetTags = GE_CDO->GetAssetTags();
            if (AssetTags.Num() > 0)
            {
                for (const FGameplayTag& Tag : AssetTags)
                {
                    CooldownTag = Tag;
                    break;
                }
            }
        }
    }


    if (!CooldownTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Cooldown Tag for GE: %s. Check GE configuration."), *CooldownGEClass->GetName());
        return;
    }
    // 4. ASC에 쿨타임 태그 이벤트 구독 — 핸들을 저장해서 나중에 해제
    auto& TagEventRef = CachedASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved);
    CooldownTagDelegateHandle = TagEventRef.AddUObject(this, &USkillSlotViewModel::OnCooldownTagChanged);


    if (InInputID == ENumInputID::Dash)
    {
        SetKeyName(FText::FromString("F"));
    }
    else if (InInputID == ENumInputID::SkillA)
    {
        SetKeyName(FText::FromString("Q"));
    }
    else if (InInputID == ENumInputID::SkillB)
    {
        SetKeyName(FText::FromString("E"));
    }
    else if (InInputID == ENumInputID::Ult)
    {
		SetKeyName(FText::FromString("X"));
    }
    
}

void USkillSlotViewModel::InitializeEmptySlot()
{
	//음..
}


void USkillSlotViewModel::OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount/*, TSubclassOf<UGameplayAbility> InGAClass*/)
{
    if (!CachedASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnCooldownTagChanged: Invalid CachedASC."));
        CoolTimeDelete();
        return;
    }

    CachedWorld = CachedASC->GetWorld();
    if (!CachedWorld)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnCooldownTagChanged: Invalid World."));
        CoolTimeDelete();
        return;
    }

    if (NewCount > 0)
    {
        // ASC의 제공 함수로 남은시간/길이 확인 (예측/복제 안전)
        float Remaining = CachedASC->GetCooldownRemainingByTag(CooldownTag);
        float Duration = CachedASC->GetCooldownDurationByTag(CooldownTag);

        if (Remaining > 0.f && Duration > 0.f)
        {
            MaxCoolTime = Duration;
            CurrentCoolTime = Remaining;

            // 타이머 시작 (중복 설정 방지)
            if (!CachedWorld->GetTimerManager().IsTimerActive(CooldownUpdateTimerHandle))
            {
                CachedWorld->GetTimerManager().SetTimer(
                    CooldownUpdateTimerHandle,
                    this,
                    &USkillSlotViewModel::CoolTimeTick,
                    0.05f,
                    true
                );
            }
            // 즉시 UI 업데이트
            SetCoolTimePercent(1 - (Remaining / Duration));
            SetCoolTime(Remaining);
            return;
        }
    }

    // 쿨타임이 끝났거나 찾지 못함 -> 정리
    CoolTimeDelete();

}

void USkillSlotViewModel::CoolTimeDelete()
{
    if (CachedWorld)
    {
        CachedWorld->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
    }
    SetCoolTime(-0.1f);
    SetCoolTimePercent(0.0f);
}

void USkillSlotViewModel::SetIconTexture(UTexture2D* InTexture)
{
    IconTexture = InTexture;
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}

void USkillSlotViewModel::SetCoolTime(float InCoolTime)
{
    FNumberFormattingOptions Options;
    Options.SetMaximumFractionalDigits(1); // 소수점 최대 1자리
    Options.SetMinimumFractionalDigits(1); // 소수점 최소 1자리
    Options.SetUseGrouping(false);       // 천 단위 구분 기호 사용 안함

    FText NewCoolTimeText;

    if (InCoolTime > 0.0f)
    {
        NewCoolTimeText = FText::AsNumber(InCoolTime, &Options);

        SetCoolTimeVisible(ESlateVisibility::Visible);
    }
    else
    {
        NewCoolTimeText = FText::GetEmpty();

        SetCoolTimeVisible(ESlateVisibility::Collapsed);
    }

    CoolTime = NewCoolTimeText;
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CoolTime);
}

void USkillSlotViewModel::SetIsUsable(bool bInIsUsable)
{
	bIsUsable = bInIsUsable;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsUsable);
}

void USkillSlotViewModel::SetCoolTimePercent(float InPercent)
{
	CoolTimePercent = InPercent;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CoolTimePercent);
}

void USkillSlotViewModel::SetCoolTimeVisible(ESlateVisibility InVisibility)
{
    CoolTimeVisible = InVisibility;
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CoolTimeVisible);
}

void USkillSlotViewModel::SetKeyName(const FText& InKeyName)
{
    KeyName = InKeyName;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(KeyName);
}


void USkillSlotViewModel::CoolTimeTick()
{
    if (!CachedASC || !CachedWorld)
    {
        CoolTimeDelete();
        return;
    }

    // 매 tick마다 ASC에서 최신값을 가져온다 (Prediction-safe)
    float Remaining = CachedASC->GetCooldownRemainingByTag(CooldownTag);
    float Duration = CachedASC->GetCooldownDurationByTag(CooldownTag);

    if (Duration > 0.f && Remaining > 0.f)
    {
        SetCoolTimePercent(Remaining / Duration);
        SetCoolTime(Remaining);
    }
    else
    {
        CoolTimeDelete();
    }
}


