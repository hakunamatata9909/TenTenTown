#include "Fireball_Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "GameplayEffect.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Enemy/Base/EnemyBase.h"

AFireball_Projectile::AFireball_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates=true;
	SetReplicateMovement(true);
	
	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;
	AlignActorWithVelocity = true;
	GravityScale=0.f;
	CollisionRadius=10.f;
	LifeSpan=3.f;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(CollisionRadius);
	
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	
	SetRootComponent(CollisionComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed=InitialSpeed;
	ProjectileMovementComponent->MaxSpeed=MaxSpeed;
	ProjectileMovementComponent->bRotationFollowsVelocity=AlignActorWithVelocity;
	ProjectileMovementComponent->ProjectileGravityScale=GravityScale;
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->bAutoActivate=false;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(CollisionComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
}

void AFireball_Projectile::SetNiagaraScale(float Secs)
{
	float Scale= FMath::Clamp(Secs/3.f,0.1f,1.f);
	NiagaraComponent->SetFloatParameter("Scale Overall",Scale);
}

void AFireball_Projectile::SetSetbyCallerGameplayEffectClass(TSubclassOf<UGameplayEffect> GameplayEffect)
{
	SetByCallerGameplayEffectClass = GameplayEffect;
}

void AFireball_Projectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp,Log,TEXT("ChargeSecFromProjectile : %f"),ChargeSecFromAbility);
	
	const FVector3d ActorScale = GetActorScale3D();

	const USceneComponent* Root = GetRootComponent();
	const FVector3d RootRelScale = Root ? Root->GetRelativeScale3D() : FVector3d(1);
	const FVector3d RootWorldScale = Root ? Root->GetComponentTransform().GetScale3D() : FVector3d(1);

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Scale Actor:%g %g %g  Root(Rel):%g %g %g  Root(World):%g %g %g"),
		ActorScale.X, ActorScale.Y, ActorScale.Z,
		RootRelScale.X, RootRelScale.Y, RootRelScale.Z,
		RootWorldScale.X, RootWorldScale.Y, RootWorldScale.Z);
	
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnHit);
	ProjectileMovementComponent->OnProjectileStop.AddUniqueDynamic(this,&ThisClass::OnStop);
	OnDestroyed.AddUniqueDynamic(this,&ThisClass::DestroyBinding);
	
	GetWorldTimerManager().SetTimer(OnTimeOut,[this]()
		{Destroy();},LifeSpan,false,-1);
	
	if (AActor* ProjectileOwner = GetOwner())
		if (ProjectileOwner)
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(ProjectileOwner))
				if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(OwnerCharacter->GetPlayerState()))
				{
					ASC=TTTPS->GetAbilitySystemComponent();
				}

	if (ProjectileSound)
	{
		AudioComponent->SetSound(ProjectileSound);
		AudioComponent->Play();
	}
}

void AFireball_Projectile::FireProjectile(const FVector& Direction, AActor* IgnoreActor)
{

	CollisionComponent->IgnoreActorWhenMoving(IgnoreActor,true);
	
	ProjectileMovementComponent->Velocity=Direction.GetSafeNormal()*InitialSpeed;
	ProjectileMovementComponent->Activate(true);
	
}

void AFireball_Projectile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Explode();
	Destroy();
}

void AFireball_Projectile::OnStop(const FHitResult& HitResult)
{
	Explode();
	Destroy();
}

void AFireball_Projectile::DestroyBinding(AActor* DestroyedActor)
{
	FGameplayCueParameters Params;
	Params.Location = GetActorLocation();
	Params.Normal = GetActorForwardVector();
	Params.SourceObject = this;
	Params.Instigator = Owner;
	Params.EffectCauser = this;
	Params.RawMagnitude =ChargeSecFromAbility;
	
	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fireball_Explode,Params);
}

void AFireball_Projectile::Explode()
{
    if (!ASC)
    {
        if (AFighterCharacter* OwnerCharacter = Cast<AFighterCharacter>(GetOwner()))
        {
            ASC = OwnerCharacter->GetAbilitySystemComponent();
        }
    }

    if (!ASC) return; 
	
    const FVector Center = GetActorLocation();
 
    const float ChargeMultiplier = FMath::Clamp(ChargeSecFromAbility, 1.f, 3.f);
    const float Radius = 120.f * ChargeMultiplier;

    FCollisionObjectQueryParams ObjParams;
    ObjParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ProjectileOverlap), false, this);
    TArray<FOverlapResult> Overlaps;

    const bool bAny = GetWorld()->OverlapMultiByObjectType(
       Overlaps,
       Center,
       FQuat::Identity,
       ObjParams,
       FCollisionShape::MakeSphere(Radius),
       QueryParams
    );

    if (bAny)
    {
       TSet<AActor*> HittedActor;
       FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SetByCallerGameplayEffectClass, 1.f, ASC->MakeEffectContext());
    	
       float CurrentBaseAtk = ASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
       float SkillCoefficient = 1.5f; 
       float FinalDamage = CurrentBaseAtk * SkillCoefficient * ChargeMultiplier*7.f; // 1레벨 기준 700 데미지 가량
       if (FinalDamage <= 0.f) FinalDamage = 10.f; // 최소 데미지 보정
       
    
       SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Damage, FinalDamage);
       
       // -----------------------------------------------------------------------

       for (const FOverlapResult& R : Overlaps)
       {
          if (AActor* A = R.GetActor())
          {
             // 자기 자신(시전자)은 맞지 않게 처리
             if (A == GetOwner()) continue;
             
             if (!Cast<AEnemyBase>(A)) continue;
             HittedActor.Add(A);
          }
       }

       for (auto* A : HittedActor)
       {
          UAbilitySystemComponent* TargetASC = Cast<AEnemyBase>(A)->GetAbilitySystemComponent();
          if (TargetASC) // TargetASC 체크 추가
          {
              ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
          }
       }
    }
    else
    {
       // UE_LOG(LogTemp, Log, TEXT("[Projectile Overlap] None"));
    }
    
    // GameplayCue 파라미터 설정 및 실행
    FGameplayCueParameters Params;
    Params.Location = GetActorLocation();
    Params.Normal = GetActorForwardVector();
    Params.SourceObject = this;
    Params.Instigator = GetOwner();
    Params.EffectCauser = this;
    Params.RawMagnitude = ChargeSecFromAbility;
    
    ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fireball_Explode, Params);
}
