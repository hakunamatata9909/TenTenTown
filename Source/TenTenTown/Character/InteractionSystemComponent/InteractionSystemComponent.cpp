#include "InteractionSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

UInteractionSystemComponent::UInteractionSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OverlapCollisionBox = CreateDefaultSubobject<UBoxComponent>("OverlapBoxComponent");
	OverlapCollisionBox->SetGenerateOverlapEvents(true);
	OverlapCollisionBox->SetBoxExtent(FVector(100.f,100,100.f));
	OverlapCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	OverlapCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
}

bool UInteractionSystemComponent::InterAction()
{
	if (!CurrentOverlappedActor) return false;
	else
	{
		//bool bSuccess = Cast</*아이템 인터페이스*/>CurrentOverlappedActor->Active();

		//if (bSuccess) return true;
		//else return false;
	}
	return true;
}

// Called when the game starts
void UInteractionSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	OverlapCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnStart);
	OverlapCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this,&ThisClass::OnEnd);
	
}

void UInteractionSystemComponent::OnRegister()
{
	Super::OnRegister();

	AActor* Owner = GetOwner();

	if (Owner)
	{
		// 콜리전 박스 컴포넌트
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			OverlapCollisionBox->AttachToComponent(Root,FAttachmentTransformRules::KeepRelativeTransform);
		}

		if (!Owner->GetComponents().Contains(OverlapCollisionBox))
		{
			Owner->AddOwnedComponent(OverlapCollisionBox);
		}

		if (!OverlapCollisionBox->IsRegistered())
		{
			OverlapCollisionBox->RegisterComponent();
		}
		OwnerCharacter=Cast<ACharacter>(Owner);
		
	}
}

void UInteractionSystemComponent::OnStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CurrentOverlappedActor)
	{
		CurrentOverlappedActor = OtherActor;
		CurrentOverlappedActor->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnActorDestroyed);

		TArray<UStaticMeshComponent*> StaticMeshComponents;
		CurrentOverlappedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for ( auto Meshs : StaticMeshComponents)
		{
			Meshs->SetOverlayMaterial(OverlayMaterial);
		}

		TArray<UWidgetComponent*> WidgetComponents;
		CurrentOverlappedActor->GetComponents<UWidgetComponent>(WidgetComponents);

		for (auto Widgets:WidgetComponents)
		{
			Widgets->SetVisibility(true);
		}
	}
}

void UInteractionSystemComponent::OnEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor!=CurrentOverlappedActor) return;

	if (CurrentOverlappedActor)
	{
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		CurrentOverlappedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for (auto& Meshs : StaticMeshComponents)
		{
			Meshs->SetOverlayMaterial(nullptr);
		}

		TArray<UWidgetComponent*> WidgetComponents;
		CurrentOverlappedActor->GetComponents<UWidgetComponent>(WidgetComponents);

		for (auto Widgets:WidgetComponents)
		{
			Widgets->SetVisibility(false);
		}
		
		CurrentOverlappedActor->OnDestroyed.RemoveDynamic(this,&ThisClass::OnActorDestroyed);
		CurrentOverlappedActor=nullptr;
	}
}

void UInteractionSystemComponent::OnActorDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor && DestroyedActor==CurrentOverlappedActor) CurrentOverlappedActor=nullptr;
}



