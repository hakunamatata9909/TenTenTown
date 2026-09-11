#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Structure/Data/StructureData.h"
#include "Character/PS/TTTPlayerState.h"
#include "StructureBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureDestroyed, AStructureBase*, DestroyedStructure);

class AGridFloorActor;

UCLASS()
class TENTENTOWN_API AStructureBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AStructureBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStructureDestroyed OnStructureDestroyed;

protected:
	virtual void BeginPlay() override;

	// [GAS] 공통 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAS_StructureAttributeSet> AttributeSet;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TObjectPtr<ATTTPlayerState> BuilderPlayerState;

	UPROPERTY(ReplicatedUsing=OnRep_UpgradeLevel, VisibleAnywhere, Category="State")
	int32 CurrentUpgradeLevel = 1;
	
	UFUNCTION()
	void OnRep_UpgradeLevel();

	UPROPERTY()
	TWeakObjectPtr<AGridFloorActor> ParentGridFloor;
	
public:
	// --- 데이터 테이블 정보 (자식에서 설정) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", ReplicatedUsing = OnRep_StructureDataTable)
	UDataTable* StructureDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", ReplicatedUsing = OnRep_StructureRowName)
	FName StructureRowName;

	UFUNCTION()
	void OnRep_StructureDataTable();
	UFUNCTION()
	void OnRep_StructureRowName();

	// 캐싱된 데이터 (매번 테이블 찾지 않도록 저장)
	FStructureData CachedStructureData;

	// 초기화 (설치 시 호출)
	virtual void InitializeStructure();

	// 설치자 설정 함수
	void SetBuilder(ATTTPlayerState* InPlayerState);
	// 설치자인지 확인하는 함수
	bool IsBuilder(ATTTPlayerState* InPlayerState) const;
	
	// 업그레이드 비용 반환 (데이터 테이블 기반)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual int32 GetUpgradeCost() const;
	// 판매 반환금 반환 (설치비 + 업글비의 일부)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual int32 GetSellReturnAmount() const;
	// 업그레이드
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void UpgradeStructure();
	// 판매
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void SellStructure();
	// 체력 변경 감지
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	// 파괴 처리
	virtual void HandleDestruction();

	virtual void ApplyStructureStats(int32 Level);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Test")
	TSubclassOf<class UGameplayEffect> TestDamageEffectClass;

	void SetParentGridFloor(AGridFloorActor* InGridFloor);
};
