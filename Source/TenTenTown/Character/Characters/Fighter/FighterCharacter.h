// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "FighterCharacter.generated.h"

class UAS_FighterAttributeSet;
class UInputAction;
struct FInputActionInstance;

/**
 * ABaseCharacter를 상속받아 Fighter 고유의 기능을 구현하는 클래스
 */
UCLASS()
class TENTENTOWN_API AFighterCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AFighterCharacter();

protected:
	// --- Life Cycle & Overrides ---
    
	virtual void BeginPlay() override;
    
	// 매 프레임 호출 (Fighter 전용 디버그 HUD - 스태미나 포함)
	virtual void Tick(float DeltaTime) override;

	// 서버: 컨트롤러 빙의 시 호출 (FighterAttributeSet 캐싱)
	virtual void PossessedBy(AController* NewController) override;

	// 클라이언트: PlayerState 복제 시 호출 (FighterAttributeSet 캐싱)
	virtual void OnRep_PlayerState() override;

	// 입력 바인딩 설정 (Fighter 전용 키 바인딩)
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	// --- Fighter 전용 리젠 GE ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS|GE")
	TSubclassOf<UGameplayEffect> GE_FighterRegen;
};