#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Structure/BuildSystem/BuildSystemComponent.h"
#include "BaseCharacter.generated.h"

class UUserWidget;
class UCoinLootComponent;
class UAS_CharacterStamina;
class UAS_CharacterMana;
class UAS_CharacterBase;
class UAttributeSet;
class UInteractionSystemComponent;
struct FInputActionInstance;
class UCameraComponent;
class USpringArmComponent;
class ATTTPlayerState;
class UInputAction;
class UInputMappingContext;
class UGameplayAbility;
enum class ENumInputID : uint8;
class UAbilitySystemComponent;

UCLASS()
class TENTENTOWN_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}
	
	virtual void LevelUP();
	
	UAnimMontage* GetDeathMontage() {return DeathMontage;}
	UAnimMontage* GetReviveMontage() {return ReviveMontage;}
	
	void OnMoveSpeedRateChanged(const FOnAttributeChangeData& Data);
	void OnShieldBuffTagChanged(FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetWeaponMeshComp(UStaticMeshComponent* InWeapon);
	
	UFUNCTION(Server, UnReliable) void Server_ItemEquip(UStaticMesh* ItemMesh);
	UFUNCTION(Server, UnReliable) void Server_ItemHide();
	UFUNCTION(Server, UnReliable) void Server_RestoreWeapon();
	UFUNCTION(NetMulticast, UnReliable) void Multicast_ItemEquip(UStaticMesh* ItemMesh);
	UFUNCTION(NetMulticast, UnReliable) void Multicast_ItemHide();
	UFUNCTION(NetMulticast, UnReliable) void Multicast_RestoreWeapon();
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void OnRep_Controller() override;
	
	void AddDefaultMappingContext();
	void GiveDefaultAbility();

	UPROPERTY()
	bool bAbilityInit = false;
	UFUNCTION()
	void OnLevelUpInput(const FInputActionInstance& InputActionInstance);
	UFUNCTION(Server, Reliable)
	void Server_LevelUp();
	
	//인풋 액션
	//이동 및 공격
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputMappingContext> IMC;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> DashAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SkillAAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SkillBAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> RightChargeAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> UltAction;
	

	// ------ [빌드 모드] ------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBuildSystemComponent> BuildComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> ToggleBuildModeAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Build")
	TObjectPtr<UInputAction> ConfirmAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Build")
	TObjectPtr<UInputAction> CancelAction;
	
	//타워 설치
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> InstallAction;

	//타워 수리
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Build")
	TObjectPtr<UInputAction> RepairAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction6;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction7;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Build")
	TObjectPtr<UInputAction> SelectStructureAction8;

	void ToggleBuildMode(const FInputActionInstance& Instance);
	void SelectStructure(int32 SlotIndex);
	void ConfirmActionLogic(const FInputActionInstance& Instance);
	void CancelActionLogic(const FInputActionInstance& Instance);
	void RepairActionLogic(const FInputActionInstance& Instance);
	// ------------------------------

	//디버깅용 레벨업
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LevelUpAction;

	//아이템 퀵슬롯
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> ItemQuickSlotAction1;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> ItemQuickSlotAction2;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> ItemQuickSlotAction3;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> ItemQuickSlotAction4;
	
	//IA 바인딩 함수
	void Move(const FInputActionInstance& FInputActionInstance);
	void Look(const FInputActionInstance& FInputActionInstance);
	virtual void ActivateGAByInputID(const FInputActionInstance& FInputActionInstance,ENumInputID InputID);
	void ConfirmSelection(const FInputActionInstance& FInputActionInstance);
	void CancelSelection(const FInputActionInstance& FInputActionInstance);
	
	UFUNCTION(Server, Reliable)
	void Server_ConfirmSelection();
	UFUNCTION(Server, Reliable)
	void Server_CancelSelection();

	void OnQuickSlot1(const FInputActionInstance& FInputActionInstance);
	void OnQuickSlot2(const FInputActionInstance& FInputActionInstance);
	void OnQuickSlot3(const FInputActionInstance& FInputActionInstance);
	void OnQuickSlot4(const FInputActionInstance& FInputActionInstance);
	void UseQuickSlot(int32 Index);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Move")
	float BaseMoveSpeed = 300.f;
	
	//InputID, GA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS|EnputIDGAMap")
	TMap <ENumInputID,TSubclassOf<UGameplayAbility>> InputIDGAMap;
	
	//InputID가 없는 GA만 등록합니다.
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "GAS|GAArray")
	TArray<TSubclassOf<UGameplayAbility>> GAArray;
	
	UPROPERTY(EditAnywhere, Category="GAS|Passive")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	
	//기본 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UInteractionSystemComponent> ISC;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UCoinLootComponent> CoinLootComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComp = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> InHandItemMeshComp = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="ItemVisual")
	FName HandSocketName = TEXT("hand_r");
	
	//주요 캐싱
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerState")
	TObjectPtr<ATTTPlayerState> PS;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|ASC")
	TObjectPtr<UAbilitySystemComponent> ASC = nullptr;

	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="BaseDataTable")
	TObjectPtr<UDataTable> BaseDataTable;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="StaminaDataTable")
	TObjectPtr<UDataTable> StaminaDataTable;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ManaDataTable")
	TObjectPtr<UDataTable> ManaDataTable;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS|Attributeset")
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
	UPROPERTY()
	const UAS_CharacterBase* CharacterBaseAS;
	UPROPERTY()
	const UAS_CharacterStamina* StaminaAS;
	UPROPERTY()
	const UAS_CharacterMana* ManaAS;

	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anim")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anim")
	TObjectPtr<UAnimMontage> ReviveMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CrossHair")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> CrosshairWidget;
	
public:
	TSubclassOf<UGameplayAbility> GetGABasedOnInputID(ENumInputID InputID) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UTexture2D> CharacterIconTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	int32 CharacterID = -1;
};
