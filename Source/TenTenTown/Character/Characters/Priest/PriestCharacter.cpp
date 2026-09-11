#include "PriestCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Character/ENumInputID.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GAS/AS/PriestAttributeSet/AS_PriestAttributeSet.h"
#include "Character/GAS/GA/Priest/DivineBlessing/GA_Priest_DivineBlessing.h"
#include "Engine/LocalPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

APriestCharacter::APriestCharacter()
{
	WandMesh=CreateDefaultSubobject<UStaticMeshComponent>("WandMesh");
	WandMesh->SetupAttachment(GetMesh(), WandAttachSocket);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);

	SetWeaponMeshComp(WandMesh);
	
	//점프 횟수
	JumpMaxCount = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void APriestCharacter::BeginPlay()
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

void APriestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PriestAS = ASC ? Cast<UAS_PriestAttributeSet>(ASC->GetAttributeSet(UAS_PriestAttributeSet::StaticClass())) : nullptr;
}

void APriestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDivineBlessingTargetPreview();
}

void APriestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	EIC->BindAction(SkillConfirmAction, ETriggerEvent::Started, this, &ThisClass::ConfirmSelection);
	EIC->BindAction(SkillCancelAction, ETriggerEvent::Started, this, &ThisClass::CancelSelection);
}

void APriestCharacter::UpdateDivineBlessingTargetPreview()
{
	if (!ASC || !ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting)) return;
	
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(ENumInputID::SkillA));
	if (!Spec || !Spec->IsActive()) return;
	
	UGA_Priest_DivineBlessing* BlessingGA = Cast<UGA_Priest_DivineBlessing>(Spec->GetPrimaryInstance());
	if (!BlessingGA) return;
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	
	FVector EyeLoc;
	FRotator EyeRot;
	PC->GetPlayerViewPoint(EyeLoc, EyeRot);

	float MaxRange = 1500.f;
	const FVector Start = EyeLoc + EyeRot.Vector() * 350.f;
	const FVector End = Start + EyeRot.Vector() * MaxRange;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DivinBlessingTrace), false, this);

	float TraceRadius = 100;
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);
	
	ABaseCharacter* NewTarget = nullptr;
	if (bHit)
	{
		float BestDistSq = TNumericLimits<float>::Max();
		
		for (const FHitResult& H : Hits)
		{
			ABaseCharacter* HitChar = Cast<ABaseCharacter>(H.GetActor());
			if (!HitChar || HitChar == this) continue;
			if (!HitChar->ActorHasTag(TEXT("Playable"))) continue;
			
			const FVector ActorLoc = HitChar->GetActorLocation();
			const FVector ClosestOnLine =
				FMath::ClosestPointOnSegment(ActorLoc, Start, End);

			const float DistSq =
				FVector::DistSquared(ActorLoc, ClosestOnLine);

			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				NewTarget = HitChar;
			}
		}
	}
	BlessingGA->SetPreviewTarget(NewTarget);
}

void ABaseCharacter::ConfirmSelection(const FInputActionInstance& FInputActionInstance)
{
	if (!ASC) return;
	
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			GASTAG::Event_Confirm,
			Payload
		);
		
		Server_ConfirmSelection();
	}
}

void ABaseCharacter::CancelSelection(const FInputActionInstance& FInputActionInstance)
{
	if (!ASC) return;
	
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			GASTAG::Event_Cancel,
			Payload
		);
		
		Server_CancelSelection();
	}
}

void ABaseCharacter::Server_ConfirmSelection_Implementation()
{
	FGameplayEventData Payload;
	Payload.Instigator = this;
		
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		GASTAG::Event_Confirm,
		Payload
	);
}

void ABaseCharacter::Server_CancelSelection_Implementation()
{
	FGameplayEventData Payload;
	Payload.Instigator = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		GASTAG::Event_Cancel,
		Payload
	);
}