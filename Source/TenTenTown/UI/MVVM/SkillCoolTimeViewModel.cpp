#include "UI/MVVM/SkillCoolTimeViewModel.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/ENumInputID.h"




void USkillCoolTimeViewModel::InitializeViewModel(UAbilitySystemComponent* InASC, ABaseCharacter* InCharacter)
{
	if (!InASC || !InCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillCoolTimeViewModel] InitializeViewModel: Invalid parameters. InASC or InCharacter is null."));
		return;
	}

	CachedASC = InASC;
	CachedCharacter = InCharacter;
	SetCharSkills();
}

void USkillCoolTimeViewModel::InitializeViewModel()
{
}


void USkillCoolTimeViewModel::SetCharSkills()
{
	if (!CachedASC || !CachedCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillCoolTimeViewModel] SetCharSkills: CachedASC or CachedCharacter is null."));
		return;
	}
	//CachedCharacter의 특정 GA를 가져와서 SlotVMs를 만들면서 채우자
	TArray<ENumInputID> SkillInputIDs = {
		ENumInputID::Dash,
		ENumInputID::SkillA,
		ENumInputID::SkillB,
		ENumInputID::Ult
	};



	//USkillSlotViewModel를 4개 만들어서 SlotVMs에 넣자
    SlotVMs.Empty();

    for (int32 i = 0; i < SkillInputIDs.Num(); ++i)
    {
        ENumInputID InputID = SkillInputIDs[i];
        TSubclassOf<UGameplayAbility> GAClass = CachedCharacter->GetGABasedOnInputID(InputID);

        USkillSlotViewModel* NewSlotVM = NewObject<USkillSlotViewModel>(this);

        if (NewSlotVM)
        {
            if (GAClass)
            {
                NewSlotVM->InitializeSlot(CachedASC, GAClass, InputID);
            }
            else
            {
                NewSlotVM->InitializeEmptySlot();
            }

            SlotVMs.Add(NewSlotVM);
        }
    }
}



TArray<USkillSlotViewModel*> USkillCoolTimeViewModel::GetSlotVMs() const
{
	// 멤버 변수 (TObjectPtr 배열)를 RAW 포인터 배열로 변환하여 반환
	TArray<USkillSlotViewModel*> RawPtrArray;
	for (const TObjectPtr<USkillSlotViewModel>& Member : SlotVMs)
	{
		// TObjectPtr에서 RAW 포인터를 얻습니다.
		RawPtrArray.Add(Member.Get());
	}
	return RawPtrArray;
}
