// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TTTPlayerState.generated.h"




DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChangedDelegate, int32, NewGoldAmount);

enum class EGameplayEffectReplicationMode : uint8;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API ATTTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATTTPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; };
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/** 선택한 캐릭터 클래스 (Fighter, Mage 등) */
	UPROPERTY(ReplicatedUsing=OnRep_SelectedCharacterClass, BlueprintReadOnly, Category="Lobby")
	TSubclassOf<APawn> SelectedCharacterClass;
	
	/* 로비 프리뷰 폰 포인터 */
	UPROPERTY(Replicated)
	TObjectPtr<APawn> LobbyPreviewPawn = nullptr;
	
	/** 준비 완료 여부 */
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category="Lobby")
	bool bIsReady = false;

	UFUNCTION()
	void OnRep_IsReady();

	/** Ready 토글 (UI에서 버튼으로 호출) */
	UFUNCTION(BlueprintCallable, Category="Lobby")
	void ToggleReady();

	/** 서버에서 Ready 값 설정 */
	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bNewReady);

	/** 편의용 Getter */
	UFUNCTION(BlueprintPure, Category="Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION()
	void OnRep_SelectedCharacterClass();
	
	UFUNCTION(BlueprintCallable)
	int32 GetKillcount();
	UFUNCTION(BlueprintCallable)
	void SetKillcount(int32 NewKillcount);
	UFUNCTION(BlueprintCallable)
	void SetKillcountZero();
	UFUNCTION(BlueprintCallable)
	void AddKillcount(int32 Plus);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void ResetAllGASData();
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS")
	EGameplayEffectReplicationMode ReplicationMode;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<class UAS_MageAttributeSet> MageAttributes;


#pragma region UI Using

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Gold, meta = (AllowPrivateAccess = true))
	int32 Gold;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_KillCount, meta = (AllowPrivateAccess = true))
	int32 KillCount;
	
	UFUNCTION()
	void OnRep_Gold();

	UFUNCTION()
	void OnRep_KillCount();

public:
	UPROPERTY()
	FOnGoldChangedDelegate OnGoldChangedDelegate;

	void AddGold(int32 Amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddGold(int32 Amount);

	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetGold() const { return Gold; }

	UPROPERTY(ReplicatedUsing = OnRep_CharacterIndex, BlueprintReadOnly, Category = "Lobby")
	int32 CharacterIndex = -1;


public:
	void OnAbilitySystemInitialized();
	
	UFUNCTION(Server, Reliable)
	void Server_NotifyReady();

	UFUNCTION()
	void OnRep_CharacterIndex();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCharacterIndex(int32 NewIndex);
	int32 GetCharacterIndex() const { return CharacterIndex; }

	UPROPERTY(ReplicatedUsing = OnRep_CharIndexNeed, BlueprintReadOnly, Category = "Lobby")
	int32 CharIndexNeed = -1;

	UFUNCTION()
	void OnRep_CharIndexNeed();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCharIndexNeed(int32 NewIndex);


#pragma endregion





};
