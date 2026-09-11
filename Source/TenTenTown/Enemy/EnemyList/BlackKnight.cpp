// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/BlackKnight.h"

#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"


void ABlackKnight::InitializeEnemy()
{
	Super::InitializeEnemy();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_BlackKnight);
	}
}


