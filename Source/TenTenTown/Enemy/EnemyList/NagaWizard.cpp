// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/NagaWizard.h"

void ANagaWizard::InitializeEnemy()
{
	Super::InitializeEnemy();

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_NagaWizard);
	}
}
