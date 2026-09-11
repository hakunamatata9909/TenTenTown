#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "SkillSlotViewModel.generated.h"


class UTTTASComponent;

UCLASS()
class TENTENTOWN_API USkillSlotViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	virtual void InitializeViewModel() override;
	//캐싱
	UPROPERTY()
	TObjectPtr<UTTTASComponent> CachedASC;
	UPROPERTY()
	TSubclassOf<UGameplayAbility> GAClass;
	UPROPERTY()
	UWorld* CachedWorld = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> CooldownGEClass;
	UPROPERTY()
	FGameplayTag CooldownTag;

	//필드로 지정할것  스킬이미지, 쿨타임, 사용가능여부, 게이지, 사용키
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	TObjectPtr<UTexture2D> IconTexture;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	FText CoolTime;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	bool bIsUsable;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	float CoolTimePercent;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	//visible설정
	ESlateVisibility CoolTimeVisible = ESlateVisibility::Collapsed;
	//사용키..흠.. 텍스트로 표기해줘야하나?
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	FKey InputKey;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "SkillSlot")
	FText KeyName;


	//필드 보조
	float CurrentCoolTime;
	float MaxCoolTime;
public:
	void InitializeSlot(UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> InGAClass, ENumInputID InInputID);
	void InitializeEmptySlot();

	void OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount);

	void CoolTimeDelete();

	//set함수
	void SetIconTexture(UTexture2D* InTexture);
	void SetCoolTime(float InCoolTime);
	void SetIsUsable(bool bInIsUsable);
	void SetCoolTimePercent(float InPercent);
	void SetCoolTimeVisible(ESlateVisibility InVisibility);
	void SetKeyName(const FText& InKeyName);

protected:	
	FTimerHandle CooldownUpdateTimerHandle;

	//저장할 델리게이트 핸들 (구독 해제용)
	FDelegateHandle CooldownTagDelegateHandle;

	UFUNCTION()
	void CoolTimeTick();
	
};
