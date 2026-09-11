#pragma once

#include "CoreMinimal.h"
#include "Item/Base/GA_ItemBase.h"
#include "GA_PotionItem.generated.h"

UCLASS()
class TENTENTOWN_API UGA_PotionItem : public UGA_ItemBase
{
	GENERATED_BODY()
	
public:
	UGA_PotionItem();

protected:
	virtual void ApplyOnServer(const FGameplayEventData& Payload) override;
};
