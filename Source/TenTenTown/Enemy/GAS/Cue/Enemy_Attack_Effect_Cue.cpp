// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Attack_Effect_Cue.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void AEnemy_Attack_Effect_Cue::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(Target, EventType, Parameters);

	UE_LOG(LogTemp, Warning, TEXT("Attack Cue Executed"));
	
	switch (EventType)
	{
	case EGameplayCueEvent::Executed:
		{
			const FGameplayTagContainer& SourceTags = Parameters.AggregatedSourceTags;
    
			TArray<FGameplayTag> SoundMapKeys;
			SoundMap.GetKeys(SoundMapKeys);
    
			const FGameplayTag* SoundTagPtr = SoundMapKeys.FindByPredicate(
			[&SourceTags](const FGameplayTag& Tag)
			{
				return SourceTags.HasTagExact(Tag);	
			});
    
			if (SoundTagPtr)
			{
				if (USoundCue* const* SoundToPlayPtr = SoundMap.Find(*SoundTagPtr))
				{
					if (*SoundToPlayPtr)
					{
						UGameplayStatics::PlaySoundAtLocation(this, *SoundToPlayPtr, Target->GetActorLocation());
					}
				}
			}
	
			TArray<FGameplayTag> EffectMapKeys;
			EffectMap.GetKeys(EffectMapKeys);
    
			const FGameplayTag* EffectTagPtr = EffectMapKeys.FindByPredicate(
			[&SourceTags](const FGameplayTag& Tag)
			{
				return SourceTags.HasTagExact(Tag);
			});
    
			if (EffectTagPtr)
			{
				if (UNiagaraSystem* const* EffectToPlayPtr = EffectMap.Find(*EffectTagPtr))
				{
					if (*EffectToPlayPtr)
					{
						float RandX = FMath::RandRange(-Offset, Offset);
						float RandY = FMath::RandRange(-Offset, Offset);
						float RandZ = FMath::RandRange(-Offset, Offset);
             
						FVector RandomOffsetVector(RandX, RandY, RandZ);

						FVector SpawnLocation = Target->GetActorLocation() + RandomOffsetVector;
             
						FRotator SpawnRotation = (Target) ? Target->GetActorRotation() : FRotator::ZeroRotator;
				
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						 Target,                               
						 *EffectToPlayPtr,                     
						 SpawnLocation,           
						 SpawnRotation,           
						 FVector(1.0f),                        
						 true,                                 
						 true                                  
					 );
					}
				}
			}
			break;
		}

	case EGameplayCueEvent::Removed:
		{
			
			break;
		}

	default:
	break;
}
}
