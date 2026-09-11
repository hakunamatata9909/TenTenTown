// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterDataTables.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBaseDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float BaseAtk = 1.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float MaxHP = 100.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float HP = 100.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float Level = 1.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float EXP = 0.f;
	
};

USTRUCT(BlueprintType)
struct FCharacterStaminaDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float MaxStamina = 100.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float Stamina = 100.f;
};

USTRUCT(BlueprintType)
struct FCharacterManaDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float MaxMana = 100.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
	float Mana = 100.f;
};