// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_EnemyAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UAS_EnemyAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAS_EnemyAttributeSetBase();

	//virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attribute", ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ThisClass, Damage);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attribute", ReplicatedUsing= OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(ThisClass, Attack);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_CoreAttack)
	FGameplayAttributeData CoreAttack;
	ATTRIBUTE_ACCESSORS(ThisClass, CoreAttack);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(ThisClass, MovementSpeed);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_MovementSpeedRate)
	FGameplayAttributeData MovementSpeedRate;
	ATTRIBUTE_ACCESSORS(UAS_EnemyAttributeSetBase, MovementSpeedRate);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeed);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(ThisClass, AttackRange);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(ThisClass, Gold);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_Exp)
	FGameplayAttributeData Exp;
	ATTRIBUTE_ACCESSORS(ThisClass, Exp);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_DropPercent)
	FGameplayAttributeData DropPercent;
	ATTRIBUTE_ACCESSORS(ThisClass, DropPercent);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Attribute",ReplicatedUsing= OnRep_Exp)
	FGameplayAttributeData Vulnerable;
	ATTRIBUTE_ACCESSORS(ThisClass, Vulnerable);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldDamage);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack);

	UFUNCTION()
	void OnRep_CoreAttack(const FGameplayAttributeData& OldCoreAttack);
	
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
	
	UFUNCTION()
	void OnRep_MovementSpeedRate(const FGameplayAttributeData& OldMovementSpeedRate);
	
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed);

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange);
	
	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldGold);

	UFUNCTION()
	void OnRep_Exp(const FGameplayAttributeData& OldExp);

	UFUNCTION()
	void OnRep_DropPercent(const FGameplayAttributeData& OldDropPercent);
	
	UFUNCTION()
	void OnRep_Vulnerable(const FGameplayAttributeData& OldVulnerable);
	
};
