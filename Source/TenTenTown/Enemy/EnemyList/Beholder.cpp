// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/Beholder.h"

void ABeholder::InitializeEnemy()
{
	Super::InitializeEnemy();

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_Beholder);
	}
	bIsFly = true;
}
