// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Berserk_Effect_Cue.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void AEnemy_Berserk_Effect_Cue::HandleGameplayCue(
	AActor* Target,
	EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	if (!Target)
	{
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(Target);
	USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;

	if ((EventType == EGameplayCueEvent::OnActive))
	{
		
		UNiagaraFunctionLibrary::SpawnSystemAttached(
		BerserkAuraEffect,
		Mesh,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		true);
	}
}
