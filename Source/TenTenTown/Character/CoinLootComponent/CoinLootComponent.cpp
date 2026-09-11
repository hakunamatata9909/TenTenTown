// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinLootComponent.h"

#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/BoxComponent.h"
#include "Enemy/TestEnemy/TestGold.h"
#include "GameFramework/PlayerState.h"

// Sets default values for this component's properties
UCoinLootComponent::UCoinLootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>("OverlapBoxComponent");
	OverlapBox->SetGenerateOverlapEvents(true);
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetBoxExtent(FVector(100,100,75));
}


// Called when the game starts
void UCoinLootComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnBeginOverlap);
}

void UCoinLootComponent::OnRegister()
{
	Super::OnRegister();
	
	AActor* Owner = GetOwner();

	if (Owner)
	{
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			OverlapBox->AttachToComponent(Root,FAttachmentTransformRules::KeepRelativeTransform);
		}

		if (!Owner->GetComponents().Contains(OverlapBox))
		{
			Owner->AddOwnedComponent(OverlapBox);
		}

		if (!OverlapBox->IsRegistered())
		{
			OverlapBox->RegisterComponent();
		}
	}
	
}


void UCoinLootComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!OtherActor || OtherActor == GetOwner() || LootingItems.Contains(OtherActor))
	{
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Start Looting: %s"), *OtherActor->GetName());

	OtherActor->SetActorEnableCollision(false);

	LootingItems.Add(OtherActor);
	
	if (!IsComponentTickEnabled())
	{
		SetComponentTickEnabled(true);
	}
}

// Called every frame
void UCoinLootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LootingItems.IsEmpty())
	{
		return;
	}

	FVector OwnerLoc = GetOwner()->GetActorLocation();

	for (int32 i = LootingItems.Num() - 1; i >= 0; --i)
	{
		AActor* Item = LootingItems[i];

		if (!IsValid(Item))
		{
			LootingItems.RemoveAt(i);
			continue;
		}

		FVector ItemLoc = Item->GetActorLocation();
        
		float DistSq = FVector::DistSquared(OwnerLoc, ItemLoc);

		if (DistSq <= ReachThreshold * ReachThreshold)
		{
			// PS에 골드 추가
			if (ABaseCharacter* Owner = Cast<ABaseCharacter>(GetOwner()))
			{
				if (ATTTPlayerState* PS = Cast<ATTTPlayerState>(Owner->GetPlayerState()))
				{
					ATestGold* GoldItem = Cast<ATestGold>(Item);
					
					PS->AddGold(GoldItem->GetGoldValue());

					UE_LOG(LogTemp, Log, TEXT("Item Looted and Destroyed: %s"), *Item->GetName());

				}
			}

			Item->Destroy();
			LootingItems.RemoveAt(i);
		}
		else
		{
			FVector NewLoc = FMath::VInterpConstantTo(ItemLoc, OwnerLoc, DeltaTime, LootSpeed);
			Item->SetActorLocation(NewLoc);
		}
	}
	
	if (LootingItems.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

