// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WaitHUD.h"
#include "UI/StartWidget.h"
#include "UI/CharSellectWidget.h"
#include "UI/WaitWidget.h"

void AWaitHUD::BeginPlay()
{
	Super::BeginPlay();
	

	if (StartWidgetClass)
	{
		StartWidgetInstance = CreateWidget<UStartWidget>(GetWorld(), StartWidgetClass);
		if (StartWidgetInstance)
		{
			StartWidgetInstance->AddToViewport();
		}
		else
		{
			return;
		}
	}	
	if (CharSellectWidgetClass)
	{
		CharSellectWidgetInstance = CreateWidget<UCharSellectWidget>(GetWorld(), CharSellectWidgetClass);
		if (CharSellectWidgetInstance)
		{
			CharSellectWidgetInstance->AddToViewport();
			//CharSellectWidgetInstance->HideWidget();
			
		}
		else
		{
			return;
		}
	}
	if (WaitWidgetClass)
	{
		WaitWidgetInstance = CreateWidget<UWaitWidget>(GetWorld(), WaitWidgetClass);
		if (WaitWidgetInstance)
		{
			WaitWidgetInstance->AddToViewport();
			WaitWidgetInstance->HideWidget();
		}
		else
		{
			return;
		}
	}



}
