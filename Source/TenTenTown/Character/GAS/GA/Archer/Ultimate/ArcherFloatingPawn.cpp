#include "ArcherFloatingPawn.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h" // 변수 복제 필수 헤더

AArcherFloatingPawn::AArcherFloatingPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.016f); // 60fps 동기화

	// 멀티플레이어 설정
	bReplicates = true;
	SetReplicateMovement(true);

	// 컨트롤러 회전 사용 안 함 (아처 전방 고정 이동)
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionObjectType(ECC_Pawn);
	SetRootComponent(SphereComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->TargetArmLength = 1.f;
	SpringArmComponent->bUsePawnControlRotation = false; // 마우스로 화면 안 돌림 (필요시 true)
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");

	DecalComponent = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
	DecalComponent->SetupAttachment(RootComponent);
	DecalComponent->DecalSize = FVector(512.f, 450.f, 450.f); // 깊이를 512로 확장
	DecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}

void AArcherFloatingPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArcherFloatingPawn, Rep_TargetLocation);
	DOREPLIFETIME(AArcherFloatingPawn, Rep_TargetRotation);
	DOREPLIFETIME(AArcherFloatingPawn, bRep_IsDecalVisible);
}

void AArcherFloatingPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	
	if (APlayerState* PS = NewController->GetPlayerState<APlayerState>())
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
	}
}

void AArcherFloatingPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		APlayerState* PS = PC->GetPlayerState<APlayerState>();
		
		ASC= UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
	}
}

void AArcherFloatingPawn::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (CrosshairClass)
			{
				CrosshairWidget = CreateWidget<UUserWidget>(PC, CrosshairClass);
				if (CrosshairWidget) CrosshairWidget->AddToViewport();
			}
		}
	}
}

void AArcherFloatingPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		FVector Start = CameraComponent->GetComponentLocation();
		FVector End = Start + (CameraComponent->GetForwardVector() * 10000.f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		if (bHit)
		{
			FVector NewLoc = HitResult.Location;
			FRotator NewRot = UKismetMathLibrary::MakeRotFromX(HitResult.Normal * -1.f);
			Server_UpdateDecalTransform(NewLoc, NewRot, true);
		}
		else
		{
			Server_UpdateDecalTransform(FVector::ZeroVector, FRotator::ZeroRotator, false);
		}
	}

	DecalComponent->SetWorldLocation(Rep_TargetLocation);
	DecalComponent->SetWorldRotation(Rep_TargetRotation);
	DecalComponent->SetVisibility(bRep_IsDecalVisible);
}

void AArcherFloatingPawn::Server_UpdateDecalTransform_Implementation(FVector NewLocation, FRotator NewRotation, bool bVisible)
{
	// 서버에서 값을 변경하면 모든 클라이언트에게 전파됨
	Rep_TargetLocation = NewLocation;
	Rep_TargetRotation = NewRotation;
	bRep_IsDecalVisible = bVisible;
}

void AArcherFloatingPawn::Move(const FInputActionValue& Value)
{
	FVector InputVector = Value.Get<FVector>();
	if (Controller)
	{
		// 스폰 시 결정된 아처의 전방(ActorForward)을 기준으로 고정 이동
		AddMovementInput(GetActorForwardVector(), InputVector.X);
		AddMovementInput(GetActorRightVector(), InputVector.Y);
	}
}

void AArcherFloatingPawn::Ascend(const FInputActionValue& Value)
{
	float Scalar = Value.Get<float>();
	if (Controller && Scalar != 0.f)
	{
		AddMovementInput(FVector::UpVector, Scalar);
	}
}

void AArcherFloatingPawn::ConfirmLocation(const FInputActionValue& Value)
{

	if (!IsLocallyControlled()) return;
	
	if (bRep_IsDecalVisible)
	{
		Server_SendLocationToASC(Rep_TargetLocation);
	}
}

void AArcherFloatingPawn::Destroyed()
{
	if (IsLocallyControlled())
	{
		if (CrosshairWidget)
		{
			CrosshairWidget->RemoveFromParent();
			CrosshairWidget = nullptr;
		}
		
		if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(IMCGhost);
			}
		}
	}
	
	Super::Destroyed();
}

void AArcherFloatingPawn::Server_SendLocationToASC_Implementation(FVector TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Pawn Server: Received Vector from Client: %s"), *TargetLocation.ToString());
	
	if (!ASC)
	{
		APlayerState* PS = Cast<APlayerController>(GetController())->GetPlayerState<APlayerState>();
		if (PS)
		{
			ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
		}
	}
	if (ASC)
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;

		FGameplayAbilityTargetData_LocationInfo* LocData = new FGameplayAbilityTargetData_LocationInfo();
        
		LocData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		LocData->TargetLocation.LiteralTransform = FTransform(TargetLocation);
		LocData->SourceLocation = LocData->TargetLocation;
		
		Payload.TargetData.Add(LocData);
		
		ASC->HandleGameplayEvent(GASTAG::Event_Archer_UltLocationConfirm, &Payload);
		
		UE_LOG(LogTemp, Log, TEXT("ArcherGhost Server: Gameplay Event Sent with Tag"));
	}
}

void AArcherFloatingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMCGhost, 0);
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EIC->BindAction(AscendAction, ETriggerEvent::Triggered, this, &ThisClass::Ascend);
		EIC->BindAction(ConfirmAction,ETriggerEvent::Started,this,&ThisClass::ConfirmLocation);
	}
}

