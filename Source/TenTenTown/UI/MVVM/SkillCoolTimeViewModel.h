#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "UI/MVVM/SkillSlotViewModel.h"
#include "SkillCoolTimeViewModel.generated.h"


UCLASS()
class TENTENTOWN_API USkillCoolTimeViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	//某教
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	UPROPERTY()
	TObjectPtr<ABaseCharacter> CachedCharacter;

	//轰葛胆
	UPROPERTY(BlueprintReadOnly, FieldNotify) // Setter客 Getter 加己 力芭
	TArray<TObjectPtr<USkillSlotViewModel>> SlotVMs;

public:	
	void InitializeViewModel(UAbilitySystemComponent* InASC, ABaseCharacter* InCharacter);
	virtual void InitializeViewModel() override;
	void SetCharSkills();

	TArray<USkillSlotViewModel*> GetSlotVMs() const;

};
