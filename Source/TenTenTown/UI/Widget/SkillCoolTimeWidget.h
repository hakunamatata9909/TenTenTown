#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SkillCoolTimeWidget.generated.h"



UCLASS()
class TENTENTOWN_API USkillCoolTimeWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<class USkillSlotViewModel> SkillSlotViewModel;

	

public:
	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetSkillSlotViewModel(class USkillSlotViewModel* InViewModel);
};
