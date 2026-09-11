// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "GameFramework/Character.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TTTGamePlayTags.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Enemy/Route/SplineActor.h"
#include "Enemy/TestEnemy/TestGold.h"
#include "Net/UnrealNetwork.h"
#include "Structure/Crossbow/CrossbowStructure.h"
#include "Structure/Base/StructureBase.h"
#include "UI/Enemy/EnemyHealthBarWidget.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Structure/IceTrap/IceTrapStructure.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetNetUpdateFrequency(30.f);

	AttributeSet = CreateDefaultSubobject<UAS_EnemyAttributeSetBase>(TEXT("AttributeSet"));
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	if (ASC)
	{
		ASC->SetIsReplicated(true);
		ASC->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}

	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));
	StateTree->SetAutoActivate(false);

	DetectComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectComponent"));
	if (RootComponent && DetectComponent)
	{
		DetectComponent->SetupAttachment(RootComponent);
	}

	AutoPossessAI = EAutoPossessAI::Disabled;


	// UWidgetComponent
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComponent"));
	HealthWidgetComponent->SetupAttachment(RootComponent);

	
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));

}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, MovedDistance);
	DOREPLIFETIME(AEnemyBase, DistanceOffset);
	DOREPLIFETIME(AEnemyBase, SplineActor);
	DOREPLIFETIME(AEnemyBase, bIsMoving);
}

void AEnemyBase::InitializeEnemy()
{
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		GetMesh()->SetIsReplicated(true);

		DetectComponent->SetSphereRadius(
			ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute())
		);
		DetectComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		DetectComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
		if (UCapsuleComponent* MyCapsule = GetCapsuleComponent())
		{
			MyCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
		if (USkeletalMeshComponent* MyMesh = GetMesh())
		{
			MyMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
		if (UCapsuleComponent* MyCapsule = GetCapsuleComponent())
		{
			MyCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
		ASC->RegisterGameplayTagEvent(
		GASTAG::Enemy_State_Move,
		EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &AEnemyBase::OnMoveTagChanged);

		AddDefaultAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null"));
	}
}


void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (ASC) // ����: ���͵� ASC�� ����
	{
		// ü�� (Health) Attribute ���� �� HealthChanged �Լ� ȣ���ϵ��� ���ε�
		HealthChangeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
			UAS_EnemyAttributeSetBase::GetHealthAttribute() // ����: UAS_CharacterBase::Health() ��� ���� Getter ���
		).AddUObject(this, &AEnemyBase::HealthChanged);
		SpeedChangeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
			UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute()
		).AddUObject(this, &AEnemyBase::SpeedChanged);
		UpdateHealthBar_Initial();
	}
}


void AEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	LogTimer += DeltaSeconds;

	if (LogTimer >= 3.0f)
	{
		LogAttributeAndTags();
        
		LogTimer = 0.0f;
	}
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	DetectComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnDetection);
	DetectComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::EndDetection);

}

void AEnemyBase::SpeedChanged(const FOnAttributeChangeData& Data)
{
	float NewSpeed = Data.NewValue;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = NewSpeed;
        
		// 여기서 로그를 확인하세요!
		UE_LOG(LogTemp, Warning, TEXT("[GAS] Speed Changed: %.1f"), NewSpeed);
	}
}
void AEnemyBase::OnMoveTagChanged(FGameplayTag Tag, int32 NewCount)
{
	bIsMoving = (NewCount > 0);
}

void AEnemyBase::OnDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ASC || ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Dead))
	{
		return; 
	}

	bool bIsTargetType = false;
	
	if (OtherActor && OtherActor != this)
	{
		if(OtherActor->IsA<ABaseCharacter>())
		{
			UAbilitySystemComponent* PlayerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
			if (!PlayerASC || PlayerASC->HasMatchingGameplayTag(GASTAG::State_Character_Dead))
			{
				return;
			}

			bIsTargetType = true;
		}
		else if (OtherActor->IsA<AStructureBase>() && !OtherActor->IsA<AIceTrapStructure>())
		{
			if (OtherComp == OtherActor->GetRootComponent())
			{
				bIsTargetType = true;
			}
		}
	}

	if (bIsTargetType)
	{
		if (!OverlappedPawns.Contains(OtherActor))
		{
			OverlappedPawns.Add(OtherActor);
			
			SetCombatTagStatus(true);
		}
	}
	
}

void AEnemyBase::EndDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA<APawn>())
	{
		OverlappedPawns.Remove(OtherActor);

		if (OverlappedPawns.Num() == 0)
		{
			SetCombatTagStatus(false);
		}
	}
}

void AEnemyBase::SetCombatTagStatus(bool IsCombat)
{
	if (ASC)
	{
		FGameplayTag CombatTag = GASTAG::Enemy_State_Combat; 

		if (IsCombat)
		{
			if (!ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->AddLooseGameplayTag(CombatTag);
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->RemoveLooseGameplayTag(CombatTag);
			}
		}
	}
}




UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return ASC;
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::StartTree()
{
	if (StateTree)
	{
		StateTree->StartLogic();
	}
}

void AEnemyBase::LogAttributeAndTags()
{

	float CurrentHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());
	float Attack = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute());

	UE_LOG(LogTemp, Display, TEXT("===== Enemy Log: %s ====="), *GetName());
	UE_LOG(LogTemp, Display, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	UE_LOG(LogTemp, Display, TEXT("Attack: %.1f"), Attack);
	
	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	FString TagsString;
	for (const FGameplayTag& Tag : OwnedTags)
	{
		TagsString.Appendf(TEXT("%s, "), *Tag.GetTagName().ToString());
	}

	if (TagsString.IsEmpty())
	{
		TagsString = TEXT("NONE");
	}
	else
	{
		TagsString.RemoveAt(TagsString.Len() - 2); 
	}

	UE_LOG(LogTemp, Display, TEXT("Current Tags: %s"), *TagsString);
	UE_LOG(LogTemp, Display, TEXT("=========================="));
}


void AEnemyBase::AddDefaultAbility()
{
	for (TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability, 1, INDEX_NONE, this);
		ASC->GiveAbility(AbilitySpec);
	}
}


float AEnemyBase::PlayMontage(UAnimMontage* MontageToPlay, FMontageEnded Delegate, float InPlayRate)
{

	if (!MontageToPlay || !GetMesh())
	{
		return 0.0f;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return 0.0f;
	}

	const float Duration = AnimInstance->Montage_Play(MontageToPlay, InPlayRate);
	if (Duration <= 0.f)
	{
		return 0.f;
	}

	FOnMontageEnded MontageEndedDelegate;
	if (Delegate.IsBound())
	{
		MontageEndedDelegate.BindUFunction(Delegate.GetUObject(), Delegate.GetFunctionName());
	}
	
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
	
	return Duration;
}

void AEnemyBase::DropGoldItem()
{
	if (this->HasAuthority())
	{
				
		const float GoldAmount = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetGoldAttribute());

		const float NumOf50 = FMath::FloorToInt(GoldAmount / 50.0f);
		const float NumOf10 = GoldAmount - (NumOf50 * 50.0f);
		
		const float MinImpulse = 100.0f;              
		const float MaxImpulse = 200.0f;

		if (!GoldItem) return;
		
		ATestGold* GoldItemCDO = Cast<ATestGold>(GoldItem->GetDefaultObject());

		if (!GoldItemCDO) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Template = GoldItemCDO;

		for (int32 i = 0; i < NumOf50; ++i)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 20.f);
			
			ATestGold* SpawnedGold = GetWorld()->SpawnActor<ATestGold>(GoldItem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedGold)
			{
				if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnedGold->GetRootComponent()))
				{
					PrimitiveComp->SetSimulatePhysics(true);

					SpawnedGold->SetGoldValue(50.0f);
					
					FVector RandomDirection = FMath::VRand(); 
					RandomDirection.Z = FMath::Max(0.2f, RandomDirection.Z);
					RandomDirection.Normalize();

					float RandomImpulse = FMath::RandRange(MinImpulse, MaxImpulse);
                
					PrimitiveComp->AddImpulse(RandomDirection * RandomImpulse * PrimitiveComp->GetMass(), NAME_None, true);
				}
			}
		}

		for (int32 i = 0; i < NumOf10; ++i)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 20.f);
			
			ATestGold* SpawnedGold = GetWorld()->SpawnActor<ATestGold>(GoldItem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedGold)
			{
				if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnedGold->GetRootComponent()))
				{
					PrimitiveComp->SetSimulatePhysics(true);

					SpawnedGold->SetGoldValue(10.0f);
					
					FVector RandomDirection = FMath::VRand(); 
					RandomDirection.Z = FMath::Max(0.2f, RandomDirection.Z);
					RandomDirection.Normalize();

					float RandomImpulse = FMath::RandRange(MinImpulse, MaxImpulse);
                
					PrimitiveComp->AddImpulse(RandomDirection * RandomImpulse * PrimitiveComp->GetMass(), NAME_None, true);
				}
			}
		}
	}
}

void AEnemyBase::ApplySplineMovementCorrection()
{
	if (ASC && ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Teleporting)) 
	{
		return; 
	}
	
	if (!SplineActor || !SplineActor->SplineActor) return;

	USplineComponent* SplineComp = SplineActor->SplineActor;

	float ClampedDistance = FMath::Clamp(MovedDistance, 0.f, SplineComp->GetSplineLength());

	FVector SplineLocation = SplineComp->GetLocationAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	FVector Direction = SplineComp->GetDirectionAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	Direction.Normalize();
	const FVector RightVector = FVector::CrossProduct(Direction, FVector::UpVector);

	FVector OffsetVector = RightVector * DistanceOffset;

	FVector NewLocation = SplineLocation + OffsetVector;
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AEnemyBase::OnRep_MovedDistance()
{
	ApplySplineMovementCorrection();
}

void AEnemyBase::OnRep_DistanceOffset()
{
	ApplySplineMovementCorrection();
}

void AEnemyBase::OnRep_SplineActor()
{
	ApplySplineMovementCorrection();
}


void AEnemyBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		PlayMontage(MontageToPlay, FMontageEnded(), InPlayRate);
	}
}

#pragma region UI_Region
void AEnemyBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	// 1. ���ο� ü�� �� ��������
	float NewHealth = Data.NewValue;
	float MaxHealth = GetAbilitySystemComponent()->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());

	// 2. ���� �ν��Ͻ� �������� (WBP_EnemyHealthBar�� C++ �θ� Ŭ���� �ʿ�)
	if (HealthWidgetComponent->GetUserWidgetObject())
	{
		// WBP_EnemyHealthBar�� C++ �θ� Ŭ���� (��: UEnemyHealthBarWidget)�� ĳ����
		if (UEnemyHealthBarWidget* HealthBar = Cast<UEnemyHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
		{
			// 3. ������ C++ �Լ��� ȣ���Ͽ� �� ����
			HealthBar->UpdateHealth(NewHealth, MaxHealth);
		}
	}

	// 4. (������) ü���� 0 ���ϸ� ���� ����
	if (NewHealth <= 0.0f)
	{
		HealthWidgetComponent->SetVisibility(false);
	}
}

void AEnemyBase::UpdateHealthBar_Initial()
{
	if (!ASC) return;

	// ���� Health�� Max Health ���� ASC���� ���� �����ɴϴ�.
	float CurrentHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());

	// UWidgetComponent�� ���� ���� �ν��Ͻ��� ������ C++ �Լ��� ȣ���մϴ�.
	if (HealthWidgetComponent->GetUserWidgetObject())
	{
		if (UEnemyHealthBarWidget* HealthBar = Cast<UEnemyHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
		{
			// �� ������ ������ �ʱ� ���� �����մϴ�. (��: 100/100)
			HealthBar->UpdateHealth(CurrentHealth, MaxHealth);
		}
	}

	// (���� ����: ���� Health�� 0�� ���·� �����Ѵٸ�)
	if (CurrentHealth <= 0.0f)
	{
		HealthWidgetComponent->SetVisibility(false);
	}
}
#pragma endregion
