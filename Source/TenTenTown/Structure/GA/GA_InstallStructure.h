#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/GA/GA_ConfirmableAbility.h"
#include "Engine/DataTable.h"
#include "GA_InstallStructure.generated.h"

UCLASS(Blueprintable)
class TENTENTOWN_API UGA_InstallStructure : public UGA_ConfirmableAbility
{
	GENERATED_BODY()
	
public:
	UGA_InstallStructure();

protected:
	// 데이터 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Structure")
	FDataTableRowHandle StructureDataRow;

	// 현재 떠 있는 프리뷰 액터 저장
	UPROPERTY()
	TObjectPtr<AActor> PreviewActor;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 이벤트
	virtual void OnSelectionStarted() override;
	virtual void OnConfirmed(const FGameplayEventData& Payload) override;
	virtual void OnCanceled(const FGameplayEventData& Payload) override;

	// 서버 요청
	UFUNCTION(Server, Reliable)
	void Server_RequestInstall(FVector Location, FRotator Rotation, FName TargetRowName);
};
