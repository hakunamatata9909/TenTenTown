// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TestEnemy/TestEnemy.h"

#include "Components/StateTreeComponent.h"


void ATestEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitializeEnemy();
	StartTree();
}

void ATestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute()) <= 0.f)
	{
		StateTree->StopLogic("");
		Destroy();
	}
}
