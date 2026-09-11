#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "ItemData.generated.h"

class ABaseItem;

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None UMETA(DisplayName = "None"),
	Drink UMETA(DisplayName = "Drink"),
	Throw UMETA(DisplayName = "Throw"),
};

USTRUCT(BlueprintType)
struct FItemData: public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// [UI] 아이템 이름(한글 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText ItemName;
	// [UI] 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Description;
	// [UI] 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> ItemImage;
	// [UI, 인게임] 최대 중첩 갯수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	int32 MaxStackCount = 3;
	// [UI, 인게임] 판매 가격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	int32 SellPrice = 0;
	
	// [GAS] 아이템이 적용할 효과 GE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|GAS")
	TSubclassOf<UGameplayEffect> PassiveEffect = nullptr;
	
	// [GAS] 효과 강도(회복량, 데미지, 슬로우 등의 수치로 활용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|GAS")
	float Magnitude = 0.0f;
	// [GAS] 효과 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|GAS")
	float EffectRadius = 0.0f;
	// [GAS] 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|GAS")
	float Duration = 0.0f;
	// [GAS] 아이템 분류 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|GAS")
	FGameplayTag ItemTag;

	// 아이템 사용 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemUseType UseType = EItemUseType::None;
	// 아이템 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseItem> ItemActorClass = nullptr;
	// 아이템 메시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	TObjectPtr<UStaticMesh> ItemMesh = nullptr;
	// 아이템 사용 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> UseMontage = nullptr;
};
