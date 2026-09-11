#include "BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TTTGamePlayTags.h"
#include "Character/CharacterDataTables.h"
#include "Character/ENumInputID.h"
#include "Character/CoinLootComponent/CoinLootComponent.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterMana.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterStamina.h"
#include "Engine/LocalPlayer.h"
#include "Character/InteractionSystemComponent/InteractionSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/Engine.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "Structure/Crossbow/CrossbowStructure.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"

ABaseCharacter::ABaseCharacter()
{
	bUseControllerRotationYaw=true;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(70.f);
	
	GetCharacterMovement()->bOrientRotationToMovement=false;
	GetCharacterMovement()->MaxWalkSpeed=600.f;

	SpringArmComponent= CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->bUsePawnControlRotation=true;
	SpringArmComponent->TargetArmLength=300.f;
	SpringArmComponent->SocketOffset = FVector(0.0f,115.f,65.f);
	SpringArmComponent->bDoCollisionTest=true;
	SpringArmComponent->ProbeSize = 10.f;
	SpringArmComponent->ProbeChannel =ECC_Camera;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);

	if (UCapsuleComponent* Capsule = this->GetCapsuleComponent())
	{
		Capsule->SetCollisionProfileName(TEXT("CharacterMesh"));
	}
	
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->VisibilityBasedAnimTickOption =
			EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		
		MeshComp->SetComponentTickEnabled(true);
		MeshComp->PrimaryComponentTick.bCanEverTick = true;
		MeshComp->PrimaryComponentTick.bStartWithTickEnabled = true;	
		
		MeshComp->PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	}

	BuildComponent = CreateDefaultSubobject<UBuildSystemComponent>(TEXT("BuildSystemComponent"));
	BuildComponent->SetIsReplicated(true);
	
	//점프 횟수
	JumpMaxCount = 1;

	GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;
	
	PrimaryActorTick.bCanEverTick = true;
	ISC = CreateDefaultSubobject<UInteractionSystemComponent>("ISC");
	CoinLootComponent = CreateDefaultSubobject<UCoinLootComponent>("CoinLootComponent");
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
	}

	if (!bAbilityInit)
	{
		for (const auto& IDnAbility : InputIDGAMap)
		{
			const auto& [InputID, Ability] = IDnAbility;
			FGameplayAbilitySpec Spec(Ability,1,static_cast<int32>(InputID), this);
			ASC->GiveAbility(Spec);
		}
	
		for (const auto& NonIDAbility : GAArray)
		{
			FGameplayAbilitySpec Spec(NonIDAbility,1);
			ASC->GiveAbility(Spec);
		}
	
		for (const auto& AS : AttributeSets)
		{
			UAttributeSet* AttributeSet = NewObject<UAttributeSet>(PS, AS);
			ASC->AddAttributeSetSubobject(AttributeSet);
		}
		
		for (const TSubclassOf<UGameplayAbility> Passive : PassiveAbilities)
		{
			if (*Passive)
			{
				FGameplayAbilitySpec Spec(Passive, 1, INDEX_NONE, this);
				FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
			
				ASC->TryActivateAbility(Handle);
			}
		}
		
		LevelUP();
		
		bAbilityInit = true;
	}
	
	CharacterBaseAS = ASC ? Cast<UAS_CharacterBase>(ASC->GetAttributeSet(UAS_CharacterBase::StaticClass())) : nullptr;
	StaminaAS = ASC ? Cast<UAS_CharacterStamina>(ASC->GetAttributeSet(UAS_CharacterStamina::StaticClass())) : nullptr;
	ManaAS = ASC ? Cast<UAS_CharacterMana>(ASC->GetAttributeSet(UAS_CharacterMana::StaticClass())) : nullptr;
	
	
	if (HasAuthority() && ASC && CharacterBaseAS)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(CharacterBaseAS->GetMoveSpeedRateAttribute()).AddUObject(this, &ABaseCharacter::OnMoveSpeedRateChanged);
		const float Rate = CharacterBaseAS->GetMoveSpeedRate();
		GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed * (1.f + Rate);
		
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Buff.Shield")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ABaseCharacter::OnShieldBuffTagChanged);
	}
	
	// 리슨 서버 전용
	if (IsLocallyControlled())
	{
		AddDefaultMappingContext();
	}
	
	ASC->InitAbilityActorInfo(PS,this);
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	PS = Cast<ATTTPlayerState>(GetPlayerState());

	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->InitAbilityActorInfo(PS, this);
            
			CharacterBaseAS = Cast<UAS_CharacterBase>(ASC->GetAttributeSet(UAS_CharacterBase::StaticClass()));
			StaminaAS = Cast<UAS_CharacterStamina>(ASC->GetAttributeSet(UAS_CharacterStamina::StaticClass()));
			ManaAS = Cast<UAS_CharacterMana>(ASC->GetAttributeSet(UAS_CharacterMana::StaticClass()));

			if (CharacterBaseAS)
			{
				ASC->GetGameplayAttributeValueChangeDelegate(CharacterBaseAS->GetMoveSpeedRateAttribute()).RemoveAll(this);
				ASC->GetGameplayAttributeValueChangeDelegate(CharacterBaseAS->GetMoveSpeedRateAttribute()).AddUObject(this, &ABaseCharacter::OnMoveSpeedRateChanged);

				const float Rate = CharacterBaseAS->GetMoveSpeedRate();
				if (GetCharacterMovement())
				{
					GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed * (1.f + Rate);
				}
			}
		}
	}
	else
	{
		ASC = nullptr;
		CharacterBaseAS = nullptr;
		StaminaAS = nullptr;
		ManaAS = nullptr;
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ABaseCharacter::GiveDefaultAbility()
{
	if (!ASC || GetLocalRole() != ROLE_Authority) return;

	for (const auto& Pair : InputIDGAMap)
	{
		if (TSubclassOf<UGameplayAbility> GA = Pair.Value)
		{
			const int32 InputID = static_cast<int32>(Pair.Key);
			ASC->GiveAbility(FGameplayAbilitySpec(GA, 1, InputID, this));
		}
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		ULocalPlayer* LP = PC->GetLocalPlayer();
		if (LP)
		{
			UEnhancedInputLocalPlayerSubsystem* EILPS = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			EILPS->AddMappingContext(IMC,0);
		}
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EIC)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EIC->BindAction(SprintAction,ETriggerEvent::Triggered,this,&ThisClass::ActivateGAByInputID,ENumInputID::Sprint);
		EIC->BindAction(SprintAction,ETriggerEvent::Completed,this,&ThisClass::ActivateGAByInputID,ENumInputID::Sprint);
		
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Jump);
		EIC->BindAction(DashAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Dash);
		
		EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::NormalAttack);
		EIC->BindAction(AttackAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::NormalAttack);
		
		EIC->BindAction(RightChargeAction,ETriggerEvent::Started,this,&ThisClass::ActivateGAByInputID,ENumInputID::RightChargeAttack);
		EIC->BindAction(RightChargeAction,ETriggerEvent::Completed,this,&ThisClass::ActivateGAByInputID,ENumInputID::RightChargeAttack);
		
		EIC->BindAction(SkillAAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
		EIC->BindAction(SkillAAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
		
		EIC->BindAction(SkillBAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
		EIC->BindAction(SkillBAction,ETriggerEvent::Completed,this,&ThisClass::ActivateGAByInputID,ENumInputID::SkillB);
		
		EIC->BindAction(UltAction,ETriggerEvent::Started,this,&ThisClass::ActivateGAByInputID,ENumInputID::Ult);

		EIC->BindAction(LevelUpAction, ETriggerEvent::Started, this, &ThisClass::OnLevelUpInput);

		EIC->BindAction(ItemQuickSlotAction1, ETriggerEvent::Started, this, &ThisClass::OnQuickSlot1);
		EIC->BindAction(ItemQuickSlotAction2, ETriggerEvent::Started, this, &ThisClass::OnQuickSlot2);
		EIC->BindAction(ItemQuickSlotAction3, ETriggerEvent::Started, this, &ThisClass::OnQuickSlot3);
		EIC->BindAction(ItemQuickSlotAction4, ETriggerEvent::Started, this, &ThisClass::OnQuickSlot4);
	}

	// ----- [빌드 모드] -----
	if (ToggleBuildModeAction)
	{
		EIC->BindAction(ToggleBuildModeAction, ETriggerEvent::Started, this, &ThisClass::ToggleBuildMode);
	}
	
	
	if(SelectStructureAction1) EIC->BindAction(SelectStructureAction1, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 1);
	if(SelectStructureAction2) EIC->BindAction(SelectStructureAction2, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 2);
	if(SelectStructureAction3) EIC->BindAction(SelectStructureAction3, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 3);
	if(SelectStructureAction4) EIC->BindAction(SelectStructureAction4, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 4);
	if(SelectStructureAction4) EIC->BindAction(SelectStructureAction5, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 5);
	if(SelectStructureAction4) EIC->BindAction(SelectStructureAction6, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 6);
	if(SelectStructureAction4) EIC->BindAction(SelectStructureAction7, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 7);
	if(SelectStructureAction4) EIC->BindAction(SelectStructureAction8, ETriggerEvent::Started, this, &ThisClass::SelectStructure, 8);
	
	if(ConfirmAction) EIC->BindAction(ConfirmAction, ETriggerEvent::Started, this, &ThisClass::ConfirmActionLogic);
	if(CancelAction) EIC->BindAction(CancelAction, ETriggerEvent::Started, this, &ThisClass::CancelActionLogic);
	if(RepairAction) EIC->BindAction(RepairAction, ETriggerEvent::Started, this, &ThisClass::RepairActionLogic);
	// -----------------------
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	
	if (IsLocallyControlled()&&CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(),CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}
}

void ABaseCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	if (IsLocallyControlled())
	{
		AddDefaultMappingContext();
	}
}

void ABaseCharacter::AddDefaultMappingContext()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(IMC); 
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}

void ABaseCharacter::ToggleBuildMode(const FInputActionInstance& Instance)
{
	if (BuildComponent)
	{
		BuildComponent->ToggleBuildMode();
	}
}

void ABaseCharacter::SelectStructure(int32 SlotIndex)
{
	if (BuildComponent)
	{
		BuildComponent->SelectStructure(SlotIndex);
	}
}

void ABaseCharacter::ConfirmActionLogic(const FInputActionInstance& Instance)
{
	if (BuildComponent)
	{
		BuildComponent->HandleConfirmAction();
	}
}

void ABaseCharacter::CancelActionLogic(const FInputActionInstance& Instance)
{
	if (BuildComponent)
	{
		BuildComponent->HandleCancelAction();
	}
}

void ABaseCharacter::RepairActionLogic(const FInputActionInstance& Instance)
{
	if (BuildComponent)
	{
		BuildComponent->HandleRepairAction();
	}
}

void ABaseCharacter::Move(const FInputActionInstance& FInputActionInstance)
{
	const FVector Vector = FInputActionInstance.GetValue().Get<FVector>();
	const float ForwardScalar = Vector.X;
	const float RightScalar = Vector.Y;

	FRotator ControllerRotation = GetController()->GetControlRotation();
	ControllerRotation.Pitch=0;

	const FRotationMatrix RotationMatrix(ControllerRotation);
	const FVector ForwardVector = RotationMatrix.GetUnitAxis(EAxis::X);
	const FVector RightVector = RotationMatrix.GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector,ForwardScalar);
	AddMovementInput(RightVector,RightScalar);
}

void ABaseCharacter::Look(const FInputActionInstance& FInputActionInstance)
{
	const FVector InputVector = FInputActionInstance.GetValue().Get<FVector>();
	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(InputVector.Y);
}

void ABaseCharacter::OnQuickSlot1(const FInputActionInstance& FInputActionInstance) { UseQuickSlot(0); }
void ABaseCharacter::OnQuickSlot2(const FInputActionInstance& FInputActionInstance) { UseQuickSlot(1); }
void ABaseCharacter::OnQuickSlot3(const FInputActionInstance& FInputActionInstance) { UseQuickSlot(2); }
void ABaseCharacter::OnQuickSlot4(const FInputActionInstance& FInputActionInstance) { UseQuickSlot(3); }

void ABaseCharacter::UseQuickSlot(int32 Index)
{
	if (!IsLocallyControlled()) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	UInventoryPCComponent* InventoryComp = PC->FindComponentByClass<UInventoryPCComponent>();
	if (!InventoryComp) return; 
	
	InventoryComp->UseItem(Index);
}

void ABaseCharacter::SetWeaponMeshComp(UStaticMeshComponent* InWeapon)
{
	WeaponMeshComp = InWeapon;
}

static void SetCompVisible(USceneComponent* Comp, bool bVisible)
{
	if (!Comp) return;
	Comp->SetHiddenInGame(!bVisible, true);
	Comp->SetVisibility(bVisible, true);
}

void ABaseCharacter::Multicast_ItemEquip_Implementation(UStaticMesh* ItemMesh)
{
	if (GetNetMode() == NM_DedicatedServer) return;
	
	if (!GetMesh()) return;

	if (WeaponMeshComp)
	{
		SetCompVisible(WeaponMeshComp, false);
	}
	
	if (!InHandItemMeshComp)
	{
		InHandItemMeshComp = NewObject<UStaticMeshComponent>(this);
		InHandItemMeshComp->RegisterComponent();
		InHandItemMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InHandItemMeshComp->SetGenerateOverlapEvents(false);
	}

	InHandItemMeshComp->SetStaticMesh(ItemMesh);
	InHandItemMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
	SetCompVisible(InHandItemMeshComp, true);
}

void ABaseCharacter::Multicast_ItemHide_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer) return;
	
	SetCompVisible(InHandItemMeshComp, false);
	//if (InHandItemMeshComp) InHandItemMeshComp->SetStaticMesh(nullptr);
}

void ABaseCharacter::Multicast_RestoreWeapon_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer) return;
	
	Multicast_ItemHide();
	SetCompVisible(WeaponMeshComp, true);
}

void ABaseCharacter::Server_ItemEquip_Implementation(UStaticMesh* ItemMesh)
{
	Multicast_ItemEquip(ItemMesh);
}

void ABaseCharacter::Server_ItemHide_Implementation()
{
	Multicast_ItemHide();
}

void ABaseCharacter::Server_RestoreWeapon_Implementation()
{
	Multicast_RestoreWeapon();
}

TSubclassOf<UGameplayAbility> ABaseCharacter::GetGABasedOnInputID(ENumInputID InputID) const
{
	const TSubclassOf<UGameplayAbility>* GAClassPtr = InputIDGAMap.Find(InputID);

	if (GAClassPtr)
	{
		return *GAClassPtr;
	}
	return TSubclassOf<UGameplayAbility>();

}

void ABaseCharacter::ActivateGAByInputID(const FInputActionInstance& FInputActionInstance, ENumInputID InputID)
{
	bool bIsBuildMode = (ASC && ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode));
	
	if (bIsBuildMode)
	{
		bool bIsRestrictedInput = (
			InputID == ENumInputID::NormalAttack || 
			InputID == ENumInputID::SkillA || 
			InputID == ENumInputID::SkillB || 
			InputID == ENumInputID::Ult || 
			InputID == ENumInputID::ChargeAttack || 
			InputID == ENumInputID::ComboAttack || 
			InputID == ENumInputID::UltimateNormalAttack || 
			InputID == ENumInputID::RightChargeAttack
		);
		
		if (bIsRestrictedInput)
		{
			return; 
		}
	}
	
#if WITH_EDITOR
	if (!ASC)
	{
		UE_LOG(LogTemp,Log,TEXT("NO ASC IN ActivateGAByInputID"));
		return;
	}
#endif
	
	const ETriggerEvent TriggerEvent = FInputActionInstance.GetTriggerEvent();

	if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID)))
	{
		switch (TriggerEvent)
		{
		case ETriggerEvent::Started:
		case ETriggerEvent::Triggered:
			if (Spec->IsActive()) ASC->AbilityLocalInputPressed(static_cast<int32>(InputID));
			else ASC->TryActivateAbility(Spec->Handle,true);
			break;
		case ETriggerEvent::Canceled:
		case ETriggerEvent::Completed:
			ASC->AbilityLocalInputReleased(static_cast<int32>(InputID));
			break;
		case ETriggerEvent::Ongoing:
			break;
		case ETriggerEvent::None:
			break;
		}
	}
}

void ABaseCharacter::OnLevelUpInput(const FInputActionInstance& InputActionInstance)
{
	if (!IsLocallyControlled()) return;
	Server_LevelUp();
}

void ABaseCharacter::Server_LevelUp_Implementation()
{
	if (!ASC || !CharacterBaseAS) return;

	float CurLevel = CharacterBaseAS->GetLevel();
	if (CurLevel >= 10.f) return;

	float NewLevel = CurLevel + 1.f;
	
	ASC->SetNumericAttributeBase(
		UAS_CharacterBase::GetLevelAttribute(),
		NewLevel
	);
}

void ABaseCharacter::LevelUP()
{
	if (!ASC) return;
	
	float CurrentLevelFloat = ASC->GetNumericAttribute(UAS_CharacterBase::GetLevelAttribute());
	int32 CurrentLevel = FMath::RoundToInt32(CurrentLevelFloat);
	
	FName RowName = *FString::FromInt(CurrentLevel);
	
	static const FString ContextString("LevelUp In BaseCharacter");
	
	if (BaseDataTable)
	{
		FCharacterBaseDataTable* BaseDataTableRow = BaseDataTable->FindRow<FCharacterBaseDataTable>(RowName,ContextString);
		
		if (BaseDataTableRow)
		{
			// 최대 체력 설정
			ASC->SetNumericAttributeBase(UAS_CharacterBase::GetMaxHealthAttribute(),BaseDataTableRow->MaxHP);
			// 체력 설정
			ASC->SetNumericAttributeBase(UAS_CharacterBase::GetHealthAttribute(),BaseDataTableRow->HP);
			// 공격력 설정
			ASC->SetNumericAttributeBase(UAS_CharacterBase::GetBaseAtkAttribute(),BaseDataTableRow->BaseAtk);
			// exp 설정
			ASC->SetNumericAttributeBase(UAS_CharacterBase::GetEXPAttribute(),BaseDataTableRow->EXP);
		}
	}
	
	if (StaminaDataTable)
	{
		FCharacterStaminaDataTable* StaminaDataTableRow = StaminaDataTable->FindRow<FCharacterStaminaDataTable>(RowName,ContextString);
		
		if (StaminaDataTableRow)
		{
			// 최대 스태미너 설정
			ASC->SetNumericAttributeBase(UAS_CharacterStamina::GetMaxStaminaAttribute(),StaminaDataTableRow->MaxStamina);
			// 스태미너 설정
			ASC->SetNumericAttributeBase(UAS_CharacterStamina::GetStaminaAttribute(),StaminaDataTableRow->Stamina);
		}
	}

	if (ManaDataTable)
	{
		FCharacterManaDataTable* ManaDataTableRow = ManaDataTable->FindRow<FCharacterManaDataTable>(RowName,ContextString);
		
		if (ManaDataTableRow)
		{
			// 최대 마나 설정
			ASC->SetNumericAttributeBase(UAS_CharacterMana::GetMaxManaAttribute(),ManaDataTableRow->MaxMana);
			// 마나 설정
			ASC->SetNumericAttributeBase(UAS_CharacterMana::GetManaAttribute(),ManaDataTableRow->Mana);
		}
	}
}

void ABaseCharacter::OnMoveSpeedRateChanged(const FOnAttributeChangeData& Data)
{
	const float Rate = Data.NewValue;
	GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed * (1.f + Rate);
}

void ABaseCharacter::OnShieldBuffTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!ASC || ASC->GetOwnerRole() != ROLE_Authority) return;

	if (NewCount > 0)
	{
		ASC->AddGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Buff.Shield")));
	}
	else
	{
		if (const UAS_CharacterBase* AS = Cast<UAS_CharacterBase>(ASC->GetAttributeSet(UAS_CharacterBase::StaticClass())))
		{
			const float OldShield = AS->GetShield();

			if (OldShield > 0.f)
			{
				ASC->SetNumericAttributeBase(UAS_CharacterBase::GetShieldAttribute(), 0.f);
			}
		}
		ASC->RemoveGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Buff.Shield")));
	}
}