// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_Gameplayevent.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UAnimNotify_Gameplayevent : public UAnimNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="TAG")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Magnitude")
	float Magnitude = 1.f;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
