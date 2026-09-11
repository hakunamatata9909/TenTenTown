#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "PriestCharacter.generated.h"

class UAS_PriestAttributeSet;

UCLASS()
class TENTENTOWN_API APriestCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APriestCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}
	
	void UpdateDivineBlessingTargetPreview();

	//Getter 함수
	UFUNCTION(BlueprintPure, Category="Mage|Weapon")
	UStaticMeshComponent* GetWandMesh() const { return WandMesh; };
	UFUNCTION(BlueprintPure, Category="Mage|Weapon")
	FName GetWandSocketName() const { return WandAttachSocket; };
	
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY()
	const UAS_PriestAttributeSet* PriestAS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WandMesh;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WandAttachSocket = TEXT("WandAttach");

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Priest")
	TObjectPtr<UInputAction> SkillConfirmAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Priest")
	TObjectPtr<UInputAction> SkillCancelAction;
};
