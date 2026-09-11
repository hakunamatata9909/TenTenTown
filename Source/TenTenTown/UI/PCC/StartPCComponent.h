// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StartPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UStartPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStartPCComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UStartWidget> StartWidgetClass;

	UPROPERTY()
	class UStartWidget* StartWidgetInstance;

public:
	void RemoveStartWidget();

		
};
