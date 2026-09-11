// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Base/EnemyProjectileBase.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnemyBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structure/Crossbow/CrossbowStructure.h"

AEnemyProjectileBase::AEnemyProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetIsReplicated(true);
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent); 
	
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
    
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	
}

void AEnemyProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Sphere->OnComponentHit.AddDynamic(this, &AEnemyProjectileBase::OnHit);
}

void AEnemyProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyProjectileBase::SetProjectileSpeed(float Speed)
{
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

float AEnemyProjectileBase::GetProjectileSpeed()
{
	return ProjectileMovement->InitialSpeed;
}

void AEnemyProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}

	if (OtherActor->IsA<AEnemyBase>())
	{
		return;
	}
	
	if (HasAuthority())
	{
		if (OtherActor->IsA<ABaseCharacter>() || OtherActor->IsA<AStructureBase>())
		{
			UE_LOG(LogTemp, Display, TEXT("Projectile Hit Can Damage"));
		
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
		
			//if (ProjectileEffect)
			//{
			//	UNiagaraFunctionLibrary::SpawnSystemAttached(
			//		ProjectileEffect, 
			//		Mesh, 
			//		NAME_None,
			//		Hit.ImpactPoint, 
			//		FRotator::ZeroRotator, 
			//		EAttachLocation::KeepRelativeOffset, 
			//		true
			//	);
			//}
		
			if (TargetASC && DamageEffect)
			{
			
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
				Context.AddInstigator(GetInstigator(), this);
        
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, EffectLevel, Context);
				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, AttackDamage);
				
					FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				
					HitComp->OnComponentHit.RemoveDynamic(this, &AEnemyProjectileBase::OnHit);
				}
			
			}

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			
			// (충돌 시 시각 효과(Particle/Sound) 재생)
			Destroy();

		}
    
	}
	
}





