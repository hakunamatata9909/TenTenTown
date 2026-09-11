#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "CoreStructure.generated.h"

// 체력 변화 시
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreHPChanged, float, CurrentHP, float, MaxHP);
// 코어 체력 0일 시
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDead);

class UAbilitySystemComponent;
class UAS_CoreAttributeSet;
class UBoxComponent;
class UGameplayEffect;

UCLASS()
class TENTENTOWN_API ACoreStructure : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ACoreStructure();
	
	// IAbilitySystemInterface 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

	// UI 체력 연결 용
	UPROPERTY(BlueprintAssignable, Category = "Core|Event")
	FOnCoreHPChanged OnHPChanged;
	// 게임모드 코어 체력 0일 시
	UPROPERTY(BlueprintAssignable, Category = "Core|Event")
	FOnCoreDead OnDead;
	// 체력 Get 함수
	UFUNCTION(BlueprintCallable, Category = "Core|Status")
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Core|Status")
	float GetMaxHealth() const;
protected:
	virtual void BeginPlay() override;

	// 몬스터 감지 시 호출될 함수
	UFUNCTION()
	void OnCoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// 체력 변화 감지 함수
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

	// GAS 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	// 코어 어트리뷰트 셋
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAS_CoreAttributeSet> AttributeSet;

	// 몬스터 감지용 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> DetectCollision;

	// 건물의 외형 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 몬스터에게 피격 시 GE
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
