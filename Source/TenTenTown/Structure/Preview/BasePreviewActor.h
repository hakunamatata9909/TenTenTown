#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePreviewActor.generated.h"

UCLASS(Blueprintable)
class TENTENTOWN_API ABasePreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePreviewActor();
	virtual void Tick(float DeltaTime) override;

protected:
	// 설치 가능 여부
	UFUNCTION(BlueprintImplementableEvent, Category = "Preview")
	void OnInstallStatusChanged(bool bCanInstall);
	
	// 설치 가능한 최대 거리
	UPROPERTY(EditAnywhere)
	float MaxInstallDistance = 600.f;

	// 최대 스냅 범위
	UPROPERTY(EditAnywhere, Category = "Preview Settings")
	float SnapThreshold = 300.f;

	// 라인 트레이스가 감지할 표면
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
};
