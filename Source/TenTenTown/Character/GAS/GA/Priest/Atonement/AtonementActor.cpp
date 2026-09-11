#include "AtonementActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Enemy/Base/EnemyBase.h"

AAtonementActor::AAtonementActor()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	AreaVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	AreaVFX->SetupAttachment(Root);
	AreaVFX->SetAutoActivate(true);
	
	Area = CreateDefaultSubobject<USphereComponent>(TEXT("AtonementArea"));
	Area->SetupAttachment(Root);
	Area->SetCollisionProfileName(TEXT("AoE"));
	Area->SetGenerateOverlapEvents(true);
	Area->SetSphereRadius(AoERadius);
	
	Area->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAreaBeginOverlap);
	Area->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnAreaEndOverlap);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AAtonementActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) return;

	if (!SourceASC)
	{
		AActor* Inst = GetInstigator();
		SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Inst);
	}
	if (AoELifeTime > 0.f)
	{
		SetLifeSpan(AoELifeTime);
	}
	
	if (HasAuthority() && RefreshGEInterval > 0.f)
	{
		if (SpeedUpGE || SlowGE || VulnGE)
		{
			GetWorldTimerManager().SetTimer(
			RefreshGETimerHandle,
			this,
			&ThisClass::RefreshGE,
			RefreshGEInterval,
			true
		);
		}
	}
}

void AAtonementActor::OnAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
	
	ABaseCharacter* Char = Cast<ABaseCharacter>(OtherActor);
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Char && !Enemy) return;
	
	const FVector SourceLoc = GetActorLocation();
	const FVector TargetLoc = OtherActor->GetActorLocation();

	const float ZDiff = FMath::Abs(TargetLoc.Z - SourceLoc.Z);
	if (ZDiff > AoEHalfHeight) return;
	
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	
	if (!ASC) return;
	
	if (Char)
	{
		CharsInArea.Add(Char);
		
		if (ShieldGE && !ShieldedASCs.Contains(ASC))
		{
			const float BaseAtk = SourceASC->GetNumericAttribute(UAS_CharacterBase::GetBaseAtkAttribute());
			float ShieldValue = ShieldAmount + BaseAtk * ShieldMultiplier;
			
			ApplyGEToASC(ASC, ShieldGE, 1.f, ShieldTag, ShieldValue);
			ApplyGEToASC(ASC, ShieldActiveGE, 1.f, ShieldTag, 0.f);
			ShieldedASCs.Add(ASC);

			int32& Cnt = ShieldApplyCount.FindOrAdd(ASC);
			Cnt++;
			UE_LOG(LogTemp, Warning, TEXT("[ShieldApply] Count=%d Target=%s ASC=%p"), Cnt, *GetNameSafe(OtherActor), ASC);
		}
		
		if (SpeedUpGE)
		{
			ApplyGEToASC(ASC, SpeedUpGE, 1.f, SpeedUpTag, SpeedUpRate);
		}
	}
	if (Enemy)
	{
		CharsInArea.Add(Enemy);
		
		if (SlowGE)
		{
			ApplyGEToASC(ASC, SlowGE, 1.f, SlowTag, SlowRate);
		}
		if (VulnGE)
		{
			ApplyGEToASC(ASC, VulnGE, 1.f, VulnTag, VulnerabilityRate);
		}
	}
}

void AAtonementActor::OnAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
	
	ABaseCharacter* Char = Cast<ABaseCharacter>(OtherActor);
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Char && !Enemy) return;
	
	CharsInArea.Remove(Char);
	CharsInArea.Remove(Enemy);
}

void AAtonementActor::ApplyGEToASC(
	UAbilitySystemComponent* TargetASC,
	TSubclassOf<UGameplayEffect> GEClass,
	float Level,
	FGameplayTag  SetByCallerTag,
	float SetByCallerValue) const
{
	if (!TargetASC || !*GEClass) return;
	
	FGameplayEffectContextHandle Ctx = TargetASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GEClass, Level, Ctx);
	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec && SetByCallerTag.IsValid())
		{
			Spec->SetSetByCallerMagnitude(SetByCallerTag, SetByCallerValue);
		}
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AAtonementActor::RefreshGE()
{
	if (!HasAuthority()) return;
	
	for (auto It = CharsInArea.CreateIterator(); It; ++It)
	{
		TWeakObjectPtr<ACharacter> WeakChar = *It;
		if (!WeakChar.IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		ACharacter* C = WeakChar.Get();
		ABaseCharacter* Char = Cast<ABaseCharacter>(C);
		AEnemyBase* Enemy = Cast<AEnemyBase>(C);
		
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(C);
		if (!ASC) continue;

		if (Char)
		{
			if (SpeedUpGE)
			{
				ApplyGEToASC(ASC, SpeedUpGE, 1.f, SpeedUpTag, SpeedUpRate);
			}
		}

		if (Enemy)
		{
			if (SlowGE)
			{
				ApplyGEToASC(ASC, SlowGE, 1.f, SlowTag, SlowRate);
			}

			if (VulnGE)
			{
				ApplyGEToASC(ASC, VulnGE, 1.f, VulnTag, VulnerabilityRate);
			}
		}
	}
}
