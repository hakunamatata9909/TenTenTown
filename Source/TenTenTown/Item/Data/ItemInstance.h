#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct TENTENTOWN_API FItemInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
	

	FItemInstance() = default;

	FItemInstance(FName InItemID, int32 InCount)
		: ItemID(InItemID)
		, Count(InCount)
	{}
};
