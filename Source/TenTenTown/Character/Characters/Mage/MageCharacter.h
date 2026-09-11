//MageCharacter.cpp

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "MageCharacter.generated.h"

class UNiagaraSystem;
class UAS_MageAttributeSet;

UCLASS()
class TENTENTOWN_API AMageCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMageCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WandAttachSocket = TEXT("hand_r");

	//Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnMeteorTelegraph(const FVector& Center, UNiagaraSystem* CircleVFX);

	//Getter 함수
	UFUNCTION(BlueprintPure, Category="Mage|Weapon")
	UStaticMeshComponent* GetWandMesh() const { return WandMesh; };
	
	//Helper 함수
	void AddOverheatingStack(int32 HitNum);
	void ConsumeOverheatingStack();
	
	UPROPERTY(EditDefaultsOnly, Category="Meteor")
	float ConsumeStacks = 10;
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Mage|Weapon")
	TObjectPtr<class UStaticMeshComponent> WandMesh;
	
	UPROPERTY()
	const UAS_MageAttributeSet* MageAS;
};
