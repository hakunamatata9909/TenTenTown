
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/GA/GA_ConfirmableAbility.h"
#include "GA_Priest_DivineBlessing.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UNiagaraComponent;
class UNiagaraSystem;
class UAbilityTask_WaitGameplayEvent;
class ABaseCharacter;

UCLASS()
class TENTENTOWN_API UGA_Priest_DivineBlessing : public UGA_ConfirmableAbility
{
	GENERATED_BODY()

public:
	UGA_Priest_DivineBlessing();

	void SetPreviewTarget(ABaseCharacter* NewTarget);
	
protected:
	virtual void OnSelectionStarted() override;
	virtual void OnConfirmed(const FGameplayEventData& Payload) override;
	virtual void OnCanceled(const FGameplayEventData& Payload) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnCastMontageCompleted();
	UFUNCTION()
	void OnCastMontageCancelled();
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyBuff(ABaseCharacter* Target);
	UFUNCTION()
	void ApplyBuff(const FGameplayEventData Payload);
	
	UPROPERTY()
	ABaseCharacter* CurrentPreviewTarget = nullptr;

	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> BuffGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_Buff;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float BuffMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category="DivineBlessing|VFX")
	UNiagaraSystem* PreviewEffect = nullptr;
	UPROPERTY()
	UNiagaraComponent* PreviewComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="DivineBlessing|Anim")
	UAnimMontage* CastMontage;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag ApplyTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Priest.DivineBlessing"));
	
	// 내부 헬퍼
	void UpdatePreviewVFX();
	void ClearPreviewVFX();
	
};
