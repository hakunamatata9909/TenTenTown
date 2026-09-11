// Enemy_Guard_Effect_Cue.cpp

#include "Enemy_Guard_Effect_Cue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void AEnemy_Guard_Effect_Cue::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(Target, EventType, Parameters);

	if (!Target)
		return;

	ACharacter* Character = Cast<ACharacter>(Target);
	USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;

	if (!Mesh || !GuardEffect)
		return;

	if (EventType == EGameplayCueEvent::OnActive)
	{
		UNiagaraComponent* Guard = UNiagaraFunctionLibrary::SpawnSystemAttached(
			GuardEffect,
			Mesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			false
		);

		GuardStack.Add(Guard);
	}
	
	if (EventType == EGameplayCueEvent::Removed)
	{
		ClearGuardStack();
	}

}

void AEnemy_Guard_Effect_Cue::ClearGuardStack()
{
	for (UNiagaraComponent* Guard : GuardStack)
	{
		if (Guard)
		{
			Guard->Deactivate();
			Guard->DestroyComponent();
		}
	}
	GuardStack.Empty();
}
