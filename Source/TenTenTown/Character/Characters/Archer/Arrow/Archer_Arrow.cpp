// Fill out your copyright notice in the Description page of Project Settings.


#include "Archer_Arrow.h"

#include "NiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Archer/ArrowAfterHit/ArrowAfterHit.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Elements/Framework/TypedElementSelectionSet.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArcher_Arrow::AArcher_Arrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates=true;
	SetReplicateMovement(true);
	
	MaxSpeed = 2000.f;
	BasicSpeed = 1000.f;
	GravityScale = 0.5;
	bAutoActivate = false;
	bRotationFollowVelocity = true;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetSphereRadius(5.f);
	CollisionComponent->SetCollisionProfileName(FName("CharacterProjectile"));
	SetRootComponent(CollisionComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArrowSkeletalMesh");
	SkeletalMeshComponent -> SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->bAutoActivate=false;
	NiagaraComponent->SetupAttachment(RootComponent);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AArcher_Arrow::Multicast_PlayEffects_Implementation()
{
	if (NiagaraComponent)
	{
		NiagaraComponent->Activate(true);

	}
}

void AArcher_Arrow::FireArrow(FVector Direction, float SpeedRatio)
{
	SetActorEnableCollision(true);
	
	FVector NormalizedDirection;
	
	if (Direction.IsNearlyZero())
	{
		NormalizedDirection = GetActorForwardVector();
	}
	else
	{
		NormalizedDirection = Direction.GetSafeNormal();
	}
	
	SetActorRotation(NormalizedDirection.Rotation());
	
	if (ProjectileMovementComponent)
	{
		SpeedRatio = FMath::Clamp(SpeedRatio,0.3f,1.f);
		Damage = Damage*SpeedRatio;
		
		ProjectileMovementComponent->Velocity = NormalizedDirection* ProjectileMovementComponent->InitialSpeed*SpeedRatio;
		ProjectileMovementComponent->Activate();
	}
}

void AArcher_Arrow::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->MaxSpeed = MaxSpeed;
		ProjectileMovementComponent->InitialSpeed = BasicSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
		ProjectileMovementComponent->bAutoActivate = bAutoActivate;
		ProjectileMovementComponent->bRotationFollowsVelocity = bRotationFollowVelocity;
		
		ProjectileMovementComponent->UpdateComponentToWorld();
	}
}

void AArcher_Arrow::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority() || !IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner() || IsActorBeingDestroyed()) return;

	SetActorEnableCollision(false);
	
	if (AfterArrowHitClass)
	{
		const FVector ArrowDirection = GetActorForwardVector();
		FVector FinalArrowSpawnLocation = Hit.Location + ArrowDirection * 10.f;
		const FRotator FinalArrowRotation = GetActorRotation();
		FName FinalBoneName = Hit.BoneName;
		USkeletalMeshComponent* TargetMesh = nullptr;

		if (ACharacter* TargetCharacter = Cast<ACharacter>(OtherActor))
		{
			TargetMesh = TargetCharacter->GetMesh();
			if (TargetMesh)
			{
				// 적중한 캐릭터의 메시만 검사하여 다른 액터의 본을 선택하지 않는다.
				FHitResult MeshHit;
				FCollisionQueryParams Params(SCENE_QUERY_STAT(ArrowBoneTrace), false);
				Params.AddIgnoredComponent(TargetCharacter->GetCapsuleComponent());
				const bool bHitMesh = TargetMesh->LineTraceComponent(
					MeshHit, Hit.Location - ArrowDirection * 10.f,
					Hit.Location + ArrowDirection * 150.f, Params);

				if (bHitMesh)
				{
					FinalArrowSpawnLocation = MeshHit.ImpactPoint + ArrowDirection * 10.f;
					FinalBoneName = MeshHit.BoneName;
				}

				// 트레이스 실패 또는 유효한 본이 없는 경우 가장 가까운 본으로 보완한다.
				if (!bHitMesh || FinalBoneName == NAME_None || TargetMesh->GetBoneIndex(FinalBoneName) == INDEX_NONE)
				{
					FinalBoneName = TargetMesh->FindClosestBone(Hit.Location);
				}
			}
		}
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.Instigator = this->GetInstigator();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* AfterArrowHitActor = GetWorld()->SpawnActor<AActor>(AfterArrowHitClass,FinalArrowSpawnLocation,FinalArrowRotation,SpawnParameters);
		if (AfterArrowHitActor)
		{
			AfterArrowHitActor->SetActorEnableCollision(false);
			if (TargetMesh)
			{
				AfterArrowHitActor->AttachToComponent(TargetMesh, FAttachmentTransformRules::KeepWorldTransform, FinalBoneName);
			}
			else if (IsValid(OtherComp))
			{
				AfterArrowHitActor->AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
			}
			else
			{
				AfterArrowHitActor->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
	
	// 데미지 적용
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (SetByCallerClass && SourceASC && TargetASC)
	{
		FGameplayEffectSpecHandle DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(SetByCallerClass, 1.f, SourceASC->MakeEffectContext());
		if (DamageEffectSpecHandle.IsValid())
		{
			DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Damage, Damage);
			SourceASC->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetASC);
		}
	}
	
	Destroy();
}

// Called when the game starts or when spawned
void AArcher_Arrow::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(false);

	if (AActor* ProjectileOwner = GetOwner())
		if (ProjectileOwner)
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(ProjectileOwner))
				if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(OwnerCharacter->GetPlayerState()))
				{
					ASC=TTTPS->GetAbilitySystemComponent();
				}
	
	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
        
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			CollisionComponent->IgnoreComponentWhenMoving(OwnerCharacter->GetCapsuleComponent(), true);
		}
	}
	
	CollisionComponent->OnComponentHit.AddUniqueDynamic(this,&ThisClass::OnHit);
	
}

void AArcher_Arrow::SetSetByCallerClass(TSubclassOf<UGameplayEffect> Class)
{
	SetByCallerClass = Class;
}

void AArcher_Arrow::SetDamage(float NewDamage)
{
	this->Damage = NewDamage;
}

void AArcher_Arrow::SetIgnoreActor(AActor* ActorToIgnore)
{
	if (CollisionComponent&&ActorToIgnore)
	{
		CollisionComponent->IgnoreActorWhenMoving(ActorToIgnore,true);
		CollisionComponent->MoveIgnoreActors.Add(ActorToIgnore);
	}
}

void AArcher_Arrow::SetASC(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}

