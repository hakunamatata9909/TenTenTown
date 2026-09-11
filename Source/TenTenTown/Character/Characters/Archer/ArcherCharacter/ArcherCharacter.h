// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "ArcherCharacter.generated.h"

class AArcherBow;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API AArcherCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AArcherCharacter();
	AArcherBow* GetEquippedBow();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_JumpToSection(UAnimMontage* Montage, FName SectionName);
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="BowActor")
	TSubclassOf<AArcherBow> BowClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="BowActor",Replicated)
	TObjectPtr<AArcherBow> EquippedBow;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS|GE")
	TSubclassOf<UGameplayEffect> GE_ArcherRegen;
	
private:
	void EquipBow();
};
