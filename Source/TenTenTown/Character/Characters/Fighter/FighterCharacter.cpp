// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/ENumInputID.h"
#include "TTTGameplayTags.h" 

AFighterCharacter::AFighterCharacter()
{
    JumpMaxCount = 2;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
    PrimaryActorTick.bCanEverTick = true;
}

void AFighterCharacter::BeginPlay()
{
    Super::BeginPlay();
    
}

void AFighterCharacter::PossessedBy(AController* NewController)
{
    // 부모 클래스가 ASC 설정, AttributeSet 생성(배열 기반), 기본 Ability 지급을 처리
    Super::PossessedBy(NewController);

    if (ASC)
    {
        ASC->ApplyGameplayEffectToSelf(GE_FighterRegen->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());
    }
}

void AFighterCharacter::OnRep_PlayerState()
{
    // 부모 클래스가 ASC, PS, ActorInfo 초기화를 처리
    Super::OnRep_PlayerState();
    
}

void AFighterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // 부모 클래스가 Move, Look, Jump, Dash 등을 바인딩
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    // --- Fighter 고유 액션 바인딩 ---
    
    EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::UltimateNormalAttack);
    
}

