// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Gameplayevent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"


void UAnimNotify_Gameplayevent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	if (!Animation) return;

	AActor* Owner = MeshComp->GetOwner();
	
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = Owner;
	EventData.Target = Owner;
	EventData.EventMagnitude = Magnitude;
	EventData.ContextHandle = FGameplayEffectContextHandle();

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner,EventTag,EventData);
}
