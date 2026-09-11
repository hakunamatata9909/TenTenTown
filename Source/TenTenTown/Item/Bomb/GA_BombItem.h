#pragma once

#include "CoreMinimal.h"
#include "Item/Base/GA_ItemBase.h"
#include "GA_BombItem.generated.h"

UCLASS()
class TENTENTOWN_API UGA_BombItem : public UGA_ItemBase
{
	GENERATED_BODY()
	
public:
	UGA_BombItem();

protected:
	virtual void ApplyOnServer(const FGameplayEventData& Payload) override;
};
