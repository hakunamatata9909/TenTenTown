//MageCharacter.cpp

#include "MageCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Character/ENumInputID.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"
#include "Engine/LocalPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

AMageCharacter::AMageCharacter()
{
	WandMesh=CreateDefaultSubobject<UStaticMeshComponent>("WandMesh");
	WandMesh->SetupAttachment(GetMesh(), WandAttachSocket);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);

 	SetWeaponMeshComp(WandMesh);
	
	//점프 횟수
	JumpMaxCount = 1;

	PrimaryActorTick.bCanEverTick = false;
}

void AMageCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	WandMesh->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		WandAttachSocket
	);
	WandMesh->SetSimulatePhysics(false);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);
}

void AMageCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MageAS = ASC ? Cast<UAS_MageAttributeSet>(ASC->GetAttributeSet(UAS_MageAttributeSet::StaticClass())) : nullptr;
}

void AMageCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMageCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;
    
	EIC->BindAction(UltAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
	EIC->BindAction(UltAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
	EIC->BindAction(UltAction, ETriggerEvent::Canceled, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
}

void AMageCharacter::Multicast_SpawnMeteorTelegraph_Implementation(const FVector& Center, UNiagaraSystem* CircleVFX)
{
	UWorld* World = GetWorld();
	if (!World || World->IsNetMode(NM_DedicatedServer)) return;

	if (!CircleVFX) return;
	
	const FVector  VfxLoc = Center;
	const FRotator VfxRot = FRotator::ZeroRotator;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		CircleVFX,
		VfxLoc,
		VfxRot,
		FVector(1.f),
		true,
		true
	);
}

void AMageCharacter::AddOverheatingStack(int32 HitNum)
{
	if (HitNum <= 0 || !HasAuthority() || !ASC || !MageAS) return;

	const float CurrentStacks = MageAS->GetOverheatingStack();
	const float MaxStacks = MageAS->MaxOverheatingStack;

	const float NewStacks = FMath::Clamp(CurrentStacks + HitNum, 0.f, MaxStacks);

	ASC->SetNumericAttributeBase(
		UAS_MageAttributeSet::GetOverheatingStackAttribute(),
		NewStacks
	);
}

void AMageCharacter::ConsumeOverheatingStack()
{
	if (!HasAuthority() || !ASC || !MageAS) return;

	const float CurrentStacks = MageAS->GetOverheatingStack();
	float NewStacks = CurrentStacks - ConsumeStacks;
	ASC->SetNumericAttributeBase(
		UAS_MageAttributeSet::GetOverheatingStackAttribute(),
		NewStacks
	);
}