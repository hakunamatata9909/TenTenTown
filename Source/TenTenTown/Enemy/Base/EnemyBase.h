// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystemInterface.h"
#include "EnemyBase.generated.h"

class ASplineActor;
class USoundCue;
class AEnemyProjectileBase;
class ATestGold;
class UAnimInstance;
class UAnimMontage;
class USphereComponent;
class UAbilitySystemComponent;
class UStateTreeComponent;
class UCapsuleComponent;
class UAttributeSet;
class UGameplayAbility;
class UAS_EnemyAttributeSetBase;

DECLARE_DYNAMIC_DELEGATE_OneParam(FMontageEnded, UAnimMontage*, Montage);

UCLASS()
class TENTENTOWN_API AEnemyBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FMontageEnded OnMontageEndedDelegate;

	UPROPERTY()
	TObjectPtr<UAS_EnemyAttributeSetBase> AttributeSet;
	
	AEnemyBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	float MovedDistance = 0.f;

	UPROPERTY(Replicated)
	float DistanceOffset = 0.f;

	UPROPERTY()
	int32 SpawnWaveIndex = -1;

	UPROPERTY()
	bool bIsFly = false;
	
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category="State")
	bool bIsMoving = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<ATestGold> GoldItem;

	virtual void InitializeEnemy();

	void OnMoveTagChanged(FGameplayTag Tag, int32 NewCount);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	
	void SpeedChanged(const FOnAttributeChangeData& Data);

	//Event
	UFUNCTION()
	void OnDetection(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndDetection(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void SetCombatTagStatus(bool IsCombat);

	TArray<TWeakObjectPtr<AActor>> OverlappedPawns;

	void AddDefaultAbility();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeComponent> StateTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection")
	TObjectPtr<USphereComponent> DetectComponent;

public:
	// Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeadMontage;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMontage(UAnimMontage* MontageToPlay, float InPlayRate);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	float PlayMontage(UAnimMontage* MontageToPlay, FMontageEnded Delegate, float InPlayRate = 1.f);

	// ItemDrop
	
	void DropGoldItem();

	//distance와 offset복제
	UPROPERTY(Replicated)
	TObjectPtr<ASplineActor> SplineActor;
	
	UFUNCTION()
	void OnRep_MovedDistance();

	UFUNCTION()
	void OnRep_DistanceOffset();

	UFUNCTION()
	void OnRep_SplineActor();

	void ApplySplineMovementCorrection();
	
	// Range Only
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	TSubclassOf<AEnemyProjectileBase> RangedProjectileClass;

	TSubclassOf<AEnemyProjectileBase> GetRangedProjectileClass() const { return RangedProjectileClass; }

	TObjectPtr<USphereComponent> GetDetectComponent() const { return DetectComponent;};
public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	const TArray<TWeakObjectPtr<AActor>>& GetOverlappedPawns() const { return OverlappedPawns; }

	UFUNCTION(BlueprintCallable)
	void StartTree();

private:
	/** 3초 로그 출력을 위한 타이머 */
	float LogTimer = 0.0f;

	void LogAttributeAndTags();

	FDelegateHandle SpeedChangeDelegateHandle;

#pragma region UI_Region
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidgetComponent;

	void HealthChanged(const FOnAttributeChangeData& Data);
	void UpdateHealthBar_Initial();
private:
	// ASC 변경 이벤트 핸들러 저장용
	FDelegateHandle HealthChangeDelegateHandle;
#pragma endregion

};
