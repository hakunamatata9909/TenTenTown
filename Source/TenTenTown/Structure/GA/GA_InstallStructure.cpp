#include "Structure/GA/GA_InstallStructure.h"
#include "Structure/Data/StructureData.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "TTTGamePlayTags.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "Structure/GridSystem/GridFloorActor.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/Crossbow/CrossbowStructure.h"
#include "Character/PS/TTTPlayerState.h"
#include "TTTGamePlayTags.h" 
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_InstallStructure::UGA_InstallStructure()
{
	
}

void UGA_InstallStructure::OnSelectionStarted()
{
	PreviewActor = nullptr;
}

void UGA_InstallStructure::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 트리거된 이벤트 태그 확인
	int32 SlotIndex = 0;
	if (TriggerEventData)
	{
		FGameplayTag TriggerTag = TriggerEventData->EventTag;

		// 태그를 비교해서 슬롯 번호 결정
		if (TriggerTag == GASTAG::Event_Build_SelectStructure_1) SlotIndex = 1;
		else if (TriggerTag == GASTAG::Event_Build_SelectStructure_2) SlotIndex = 2;
		else if (TriggerTag == GASTAG::Event_Build_SelectStructure_3) SlotIndex = 3;
		else if (TriggerTag == GASTAG::Event_Build_SelectStructure_4) SlotIndex = 4;
	}

	UE_LOG(LogTemp, Warning, TEXT("GA_InstallStructure: Triggered by Tag [%s] -> SlotIndex [%d]"), *TriggerEventData->EventTag.ToString(), SlotIndex);

	// 슬롯 번호에 따라 DataTable RowName 결정
	FName TargetRowName = NAME_None;

	switch (SlotIndex)
	{
	case 1:
		TargetRowName = FName("Structure_Crossbow");
		break;
	case 2: 
		TargetRowName = FName("Structure_Ice");
		break;
	case 3: 
		TargetRowName = FName("Structure_Barricade");
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("GA_InstallStructure: Undefined Slot Index %d (Tag mismatch?)"), SlotIndex);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 데이터 테이블 에셋이 연결되어 있는지 확인
	const UDataTable* SourceDataTable = StructureDataRow.DataTable;
	if (SourceDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Install] Critical Error: 'StructureDataRow' has NO DataTable assigned in Blueprint Class Defaults!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 데이터 테이블에서 직접 Row 찾기
	static const FString ContextString(TEXT("GA_InstallStructure::ActivateAbility"));
	FStructureData* RowData = SourceDataTable->FindRow<FStructureData>(TargetRowName, ContextString);

	// 데이터 유효성 검사
	if (!RowData)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Install] Failed: Could not find RowName '%s' in DataTable '%s'"), *TargetRowName.ToString(), *SourceDataTable->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!RowData->PreviewActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Install] Failed: PreviewActorClass is NULL in Row '%s'"), *TargetRowName.ToString());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 멤버 변수 업데이트 (나중에 Server RPC에서 쓰기 위해)
	// 찾은 RowName을 핸들에 저장
	StructureDataRow.RowName = TargetRowName;

	// 프리뷰 액터 스폰
	AActor* OwningActor = ActorInfo->OwnerActor.Get();
	if (!OwningActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // 주인이 없으면 취소
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningActor;
	SpawnParams.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());

	// 일단 캐릭터 위치에 스폰(다음은 틱이 옮김)
	PreviewActor = GetWorld()->SpawnActor<AActor>(
		RowData->PreviewActorClass,
		OwningActor->GetActorLocation(), 
		OwningActor->GetActorRotation(),
		SpawnParams
	);
	
	if (!PreviewActor)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_InstallStructure: 프리뷰 액터 스폰에 실패했습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

// 어빌리티 종료(확정, 취소, 사망 등)
void UGA_InstallStructure::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 확정 이벤트
void UGA_InstallStructure::OnConfirmed(const FGameplayEventData& Payload)
{
	if (!PreviewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Install] Confirm failed: No Preview Actor."));
		// 로컬 프리뷰가 없으면 그냥 취소
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 클라이언트의 로컬 PreviewActor에서 최종 위치와 회전값 가져옴
	const FVector FinalLocation = PreviewActor->GetActorLocation();
	const FRotator FinalRotation = PreviewActor->GetActorRotation();

	// 위치를 서버로 전송
	Server_RequestInstall(FinalLocation, FinalRotation, StructureDataRow.RowName);
}

// 취소 이벤트
void UGA_InstallStructure::OnCanceled(const FGameplayEventData& Payload)
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
	
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	// 파괴
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_InstallStructure::Server_RequestInstall_Implementation(FVector Location, FRotator Rotation, FName TargetRowName)
{
	if (StructureDataRow.DataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SERVER: DataTable is NULL in GA_InstallStructure. Check Blueprint Class Defaults."));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	static const FString ContextString(TEXT("GA_InstallStructure::Server_RequestInstall"));
	FStructureData* RowData = StructureDataRow.DataTable->FindRow<FStructureData>(TargetRowName, ContextString);
	
	// 데이터 검사
	if (!RowData || !RowData->ActualStructureClass)
	{
		bool bReplicateEndAbility = true;
		bool bWasCancelled = true;
		// 파괴 및 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	// --- [골드 확인 및 차감] ---
	ATTTPlayerState* PS = nullptr;

	// 오너 액터 가져오기
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	PS = Cast<ATTTPlayerState>(OwnerActor);

	// 만약 Owner가 Pawn이라면, Pawn을 통해 PlayerState 가져오기
	if (!PS)
	{
		if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
		{
			PS = Cast<ATTTPlayerState>(OwnerPawn->GetPlayerState());
		}
	}
    
	// 그래도 없다면 Avatar(캐릭터)를 통해 가져오기
	if (!PS)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (APawn* AvatarPawn = Cast<APawn>(AvatarActor))
		{
			PS = Cast<ATTTPlayerState>(AvatarPawn->GetPlayerState());
		}
	}

	// 결과 확인
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("SERVER: PlayerState Not Found via Owner or Avatar!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	int32 InstallCost = RowData->InstallCost;
	
	// 돈이 부족하면 설치 취소
	if (PS->GetGold() < InstallCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER: Not Enough Gold! (Has: %d, Need: %d)"), PS->GetGold(), InstallCost);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 돈 차감 (음수로 전달)
	PS->Server_AddGold(-InstallCost);
	UE_LOG(LogTemp, Log, TEXT("SERVER: Gold Deducted: -%d"), InstallCost);

	// --- [서버 측 검증 로직] ---
	AGridFloorActor* TargetGridFloor = nullptr;
	bool bInstallSuccess = false;
	const FVector PreviewLocation = Location;

	// 프리뷰 액터 위치에서 바닥을 확인
	FHitResult HitResult;
	FVector TraceStart = PreviewLocation + FVector(0, 0, 50.f); // 프리뷰 살짝 위
	FVector TraceEnd = PreviewLocation - FVector(0, 0, 50.f);   // 프리뷰 살짝 아래

	FCollisionQueryParams QueryParams;

	// 폰 아바타 무시
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		QueryParams.AddIgnoredActor(AvatarActor);
	}
	// 서버에 있는 프리뷰 액터도 무시
	if (PreviewActor)
	{
		QueryParams.AddIgnoredActor(PreviewActor);
	}

	// 트레이스 캐스팅
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel3, QueryParams))
	{
		TargetGridFloor = Cast<AGridFloorActor>(HitResult.GetActor());
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("SERVER: FAILED - Trace did not hit anything."));
	}

	// 캐스팅 성공 확인
	FVector SnappedCenterLocation = FVector::ZeroVector;

	if (TargetGridFloor)
	{
		if (TargetGridFloor->TryInstallStructure(PreviewLocation)) 
		{
			bInstallSuccess = true;

			int32 CellX, CellY;
			// 인덱스 다시 계산
			TargetGridFloor->WorldToCellIndex(PreviewLocation, CellX, CellY);
			SnappedCenterLocation = TargetGridFloor->GetCellCenterWorldLocation(CellX, CellY);
		}
	}
	// --- [검증 로직 끝] ---

	// 스폰
	if (bInstallSuccess)
    {
        const FVector FinalLocation = SnappedCenterLocation; 
        const FRotator FinalRotation = Rotation;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwningActorFromActorInfo();
        SpawnParams.Instigator = GetOwningActorFromActorInfo()->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(
            RowData->ActualStructureClass,
            FTransform(FinalRotation, FinalLocation),
            GetOwningActorFromActorInfo(),
            Cast<APawn>(GetOwningActorFromActorInfo()->GetInstigator()),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        if (NewActor)
        {
            // 데이터 넣기
        	AStructureBase* NewStructure = Cast<AStructureBase>(NewActor);
        	
            if (NewStructure)
            {
            	NewStructure->StructureDataTable = const_cast<UDataTable*>(StructureDataRow.DataTable.Get());
            	NewStructure->StructureRowName = TargetRowName;

            	// 구조물에게 전달
            	NewStructure->SetParentGridFloor(TargetGridFloor);

            	if (PS)
            	{
            		NewStructure->SetBuilder(PS);
            	}
            }

            // 스폰 마무리 -> RefreshStatus() 실행
            UGameplayStatics::FinishSpawningActor(NewActor, FTransform(FinalRotation, FinalLocation));

        	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewActor);

        	if (TargetASC)
        	{
        		FGameplayCueParameters CueParams;
        		CueParams.Location = FinalLocation;

        		TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Structure_Build, CueParams);
        	}
        }

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
    else
    {
    	// 설치 실패 시 환불
    	PS->Server_AddGold(InstallCost);
    	UE_LOG(LogTemp, Warning, TEXT("SERVER: Install Failed on Grid. Refunded %d Gold."), InstallCost);
    	
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}