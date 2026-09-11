#include "Item/Base/BaseItem.h"

#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABaseItem::ABaseItem()
{
	bReplicates = true;
	SetReplicateMovement(true);
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetNotifyRigidBodyCollision(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->UpdatedComponent = Collision;
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 1.0f;
	Move->InitialSpeed = 1000.f;
	Move->MaxSpeed = 1000.f;
	
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner()) Collision->IgnoreActorWhenMoving(OwnerActor, true);
	if (HasAuthority()) Collision->OnComponentHit.AddDynamic(this, &ThisClass::OnBombHit);
}

void ABaseItem::InitItemData(FName InItemID, const FItemData& InItemData)
{
	ItemID = InItemID;
	ItemData = InItemData;
}

void ABaseItem::Throw(const FVector& InDir)
{
	if (Move)
	{
		Move->Velocity = InDir.GetSafeNormal() * Power;
	}
}

void ABaseItem::OnBombHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	
	Explode();
}

void ABaseItem::Explode()
{
	const FVector Origin = GetActorLocation();
	const float Radius = ItemData.EffectRadius;
	float Magnitude = ItemData.Magnitude;

	DrawDebugSphere(GetWorld(), Origin, Radius, 16, FColor::Red, false, 1.0f);

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	if (ItemData.PassiveEffect && SourceASC)
	{
		FGameplayTag CueTag;

		if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Bomb.Normal"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Bomb.Normal.Explode"));
		}
		else if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Bomb.Ice"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Bomb.Ice.Explode"));
			Magnitude = (1 - (Magnitude / 100));
		}
		else
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Apply"));
		}
		
		FGameplayCueParameters CueParams;
		CueParams.Location = Origin;
		SourceASC->ExecuteGameplayCue(CueTag, CueParams);
			
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = false;

		const FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
		const bool bHitAny = GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, Sphere, Params);
		if (bHitAny)
		{
			for (const FOverlapResult& Result : Overlaps)
			{
				AEnemyBase* Enemy = Cast<AEnemyBase>(Result.GetActor());
				if (!Enemy) continue;

				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy))
				{
					FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
					Ctx.AddSourceObject(this);

					FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(ItemData.PassiveEffect, 1.f, Ctx);
					if (Spec.IsValid())
					{
						Spec.Data->SetSetByCallerMagnitude(ItemData.ItemTag, Magnitude);
						SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
					}
					
				}
			}
		}
	}
	Destroy();
}

