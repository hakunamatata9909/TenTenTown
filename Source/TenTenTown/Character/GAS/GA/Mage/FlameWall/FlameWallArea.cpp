#include "FlameWallArea.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/BoxComponent.h"

AFlameWallArea::AFlameWallArea()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	DamageZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageZone"));
	DamageZone->SetupAttachment(Root);
	DamageZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageZone->SetGenerateOverlapEvents(true);
	DamageZone->SetCollisionObjectType(ECC_WorldDynamic);
	DamageZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	DamageZone->OnComponentBeginOverlap.AddDynamic(this, &AFlameWallArea::OnDamageZoneBeginOverlap);
	DamageZone->OnComponentEndOverlap.AddDynamic(this, &AFlameWallArea::OnDamageZoneEndOverlap);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFlameWallArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DamageZone)
	{
		DamageZone->SetBoxExtent(DamageZoneHalfExtent, true);
		DamageZone->SetRelativeLocation(DamageZoneOffset);
	}
}

void AFlameWallArea::BeginPlay()
{
	Super::BeginPlay();

	if (!FlameWallVFX)
	{
		FlameWallVFX = FindComponentByClass<UNiagaraComponent>();
	}
	if (FlameWallVFX)
	{
		FlameWallVFX->SetVariableFloat(SpawnScaleParamName, 1.0f);
	}
	
	GetWorldTimerManager().SetTimer(
		LifetimeHandle,
		this,
		&AFlameWallArea::EndWallVFX,
		Lifetime,
		false
	);
}

void AFlameWallArea::Init(float InLifeTime)
{
	Lifetime = InLifeTime;
}

void AFlameWallArea::OnDamageZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || !DotGE) return;
	if (OtherActor == GetOwner()) return;
	
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner(), true);
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor, true);
	if (!SourceASC || !TargetASC) return;

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DotGE, 1.f, Ctx);
	if (!Spec.IsValid()) return;

	const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
	const float DamageValue = DamagePerTick + BaseAtk * DamageMultiplier;
	Spec.Data->SetSetByCallerMagnitude(Tag_DoT, DamageValue);
	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

	if (OtherActor->ActorHasTag(TEXT("Playable"))) return;
	if (OverheatGivenActors.Contains(OtherActor)) return;
	OverheatGivenActors.Add(OtherActor);
	
    AMageCharacter* Mage = nullptr;
    if (AActor* Avatar = SourceASC->GetAvatarActor())
        Mage = Cast<AMageCharacter>(Avatar);
    if (Mage)
        Mage->AddOverheatingStack(1);
}

void AFlameWallArea::OnDamageZoneEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (!TargetASC) return;

	if (!DotGrantedTags.IsEmpty())
	{
		TargetASC->RemoveActiveEffectsWithGrantedTags(DotGrantedTags);
	}
}

void AFlameWallArea::EndWallVFX()
{
	if (FlameWallVFX)
	{
		FlameWallVFX->SetVariableFloat(SpawnScaleParamName, 0.0f);
	}
	
	GetWorldTimerManager().SetTimer(
		DestroyHandle,
		this,
		&AFlameWallArea::DestroySelf,
		FadeOutTime,
		false
	); 
}
 
void AFlameWallArea::DestroySelf()
{
	Destroy();
}

