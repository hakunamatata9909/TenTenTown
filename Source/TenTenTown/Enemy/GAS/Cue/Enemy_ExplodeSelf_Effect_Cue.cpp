// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_ExplodeSelf_Effect_Cue.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


void AEnemy_ExplodeSelf_Effect_Cue::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(Target, EventType, Parameters);
	FVector Location = Target->GetActorLocation();
	FRotator Rotation = Target->GetActorRotation();

	UE_LOG(LogTemp, Warning, TEXT("Explode Self Effect Cue Executed!"));

	if (EventType == EGameplayCueEvent::Executed)
	{
		if (SoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundCue, Location);
		}
	
		if (ExplodeEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			 Target,                               
			 ExplodeEffect,                     
			 Location,           
			 Rotation,           
			 FVector(1.0f),                        
			 true,                                 
			 true                                  
			 );
			
		}
	}
}
