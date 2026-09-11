// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BaseViewModel.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UBaseViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// **1. 초기화 (Initialize):**
	// PCC에서 NewObject 직후 호출하여 뷰모델을 설정합니다. (데이터 바인딩 및 구독 시작)
	// Blueprint에서도 호출할 수 있도록 BlueprintCallable을 추가합니다.
	UFUNCTION(BlueprintCallable, Category = "MVVM|Core")
	virtual void InitializeViewModel();

	// **2. 정리 (Cleanup):**
	// PCC에서 모드가 종료될 때 호출하여 메모리 누수를 방지합니다. (모든 구독 및 타이머 해제)
	UFUNCTION(BlueprintCallable, Category = "MVVM|Core")
	virtual void CleanupViewModel();
	
};
