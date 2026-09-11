// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Buffed_Effect_Cue.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"

AEnemy_Buffed_Effect_Cue::AEnemy_Buffed_Effect_Cue()
{
	bAutoDestroyOnRemove = true;

}

void AEnemy_Buffed_Effect_Cue::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType, 
							const FGameplayCueParameters& Parameters)
{
	// 타겟이 유효한 적 액터인지 확인합니다.
	AEnemyBase* Enemy = Cast<AEnemyBase>(Target);

	if (!Enemy || !BuffedEffect)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = Enemy->GetMesh();
	if (!Mesh)
	{
		return;
	}

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
		{
			// 💡 1. OnActive: 버프가 처음 적용될 때 이펙트를 재생합니다.
            
			// Attached: 이펙트가 타겟의 Mesh에 붙어 함께 움직이도록 합니다.
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				BuffedEffect, 
				Mesh, 
				NAME_None, // 특정 소켓이 없다면 NAME_None
				FVector::ZeroVector, 
				FRotator::ZeroRotator, 
				EAttachLocation::KeepRelativeOffset, 
				true // AutoDestroy: 컴포넌트가 파괴될 때 함께 파괴
			);

			UE_LOG(LogTemp, Warning, TEXT("Buffed Effect Spawned"));
            
			break;
		}
	case EGameplayCueEvent::Removed:
		{
			// 💡 2. Removed: 이펙트가 제거될 때 (bAutoDestroyOnRemove=true 이므로 자동 처리됩니다)
			// AGameplayCueNotify_Actor를 사용하고 bAutoDestroyOnRemove = true이므로, 
			// 별도의 제거 로직 없이 액터 자체가 파괴됩니다.
            
			// 만약 UNiagaraComponent 핸들을 저장했다면 여기서 수동으로 DestroyComponent를 호출할 수 있습니다.
			break;
		}

	default:
		break;
	}
}
