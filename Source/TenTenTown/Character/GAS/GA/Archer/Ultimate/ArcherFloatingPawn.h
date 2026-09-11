#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "ArcherFloatingPawn.generated.h"

class UAbilitySystemComponent;
class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UDecalComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;

UCLASS()
class TENTENTOWN_API AArcherFloatingPawn : public APawn
{
	GENERATED_BODY()

public:
	AArcherFloatingPawn();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	void Move(const FInputActionValue& Value);
	void Ascend(const FInputActionValue& Value);
	void ConfirmLocation(const FInputActionValue& Value);
	
	UFUNCTION(Server, Unreliable)
	void Server_UpdateDecalTransform(FVector NewLocation, FRotator NewRotation, bool bVisible);

	UFUNCTION(Server,Reliable)
	void Server_SendLocationToASC(FVector TargetLocation);
protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> DecalComponent;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> IMCGhost;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AscendAction;
	
	UPROPERTY(EditAnywhere, Category= "Input")
	TObjectPtr<UInputAction> ConfirmAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CrosshairWidget;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(Replicated)
	FVector Rep_TargetLocation;

	UPROPERTY(Replicated)
	FRotator Rep_TargetRotation;

	UPROPERTY(Replicated)
	bool bRep_IsDecalVisible;
	
	virtual void Destroyed() override;
};