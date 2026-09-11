#include "FireballProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

AFireballProjectile::AFireballProjectile()
{
	bReplicates = true;
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(ProjectileTraceRadius);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->OnComponentHit.AddDynamic(this, &AFireballProjectile::OnHit);
	
	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 1.0f;
	Move->InitialSpeed = Move->MaxSpeed = 2400.f;
	InitialLifeSpan = 5.f;
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		if (APawn* Inst = GetInstigator())
		{
			Collision->IgnoreActorWhenMoving(Inst, true);
		}
	}
}

void AFireballProjectile::InitVelocity(const FVector& Dir, float Speed)
{
	const FVector V = Dir.GetSafeNormal() * Speed;
	Move->Velocity = V;
	Move->InitialSpeed = Move->MaxSpeed = Speed;
}

void AFireballProjectile::OnHit(UPrimitiveComponent* HitComponent,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse,
                                const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	
	const FVector Loc = Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation();
	const FRotator Rot = Hit.bBlockingHit ? FVector(Hit.ImpactNormal).Rotation() : GetActorRotation();

	if (AActor* Inst = GetInstigator())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Inst))
		{
			FGameplayCueParameters Params;
			Params.Location = Loc;
			
			ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Mage.Fireball.Explode")), Params);
		}
	}
	DoExplode_Server(Loc, Rot);
	Destroy();
}

void AFireballProjectile::DoExplode_Server(const FVector& ExplodeLoc, const FRotator& ExplodeRot)
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FireballExplode), false, this);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);
	
	bool bHitAny = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		ExplodeLoc,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,	
		Params
	);
	
	int32 HitNum = 0;
	TSet<AActor*> UniqueHitActors;
	
	if (bHitAny && DamageGE)
	{
		UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		
		if (SourceASC)
		{
			for (const FOverlapResult& Result : Overlaps)
			{
				AActor* HitActor = Result.GetActor();
				
				if (!HitActor || HitActor->ActorHasTag(TEXT("Playable"))) continue;
				if (UniqueHitActors.Contains(HitActor)) continue;
				UniqueHitActors.Add(HitActor);
				
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
				{
					FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
					Ctx.AddSourceObject(this);
			
					FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageGE, 1.f, Ctx);
					if (Spec.IsValid())
					{
						const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
						const float DamageValue = DamageAmount + BaseAtk * DamageMultiplier;
						Spec.Data->SetSetByCallerMagnitude(Tag_Damage, DamageValue);
						SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
						
						HitNum++;
					}
				}
			}
		}
	}
	
	if (HitNum > 0 && bCountsForOverheat)
	{
		if (APawn* Inst = GetInstigator())
		{
			if (AMageCharacter* Mage = Cast<AMageCharacter>(Inst))
			{
				Mage->AddOverheatingStack(HitNum);
			}
		}
	}
}