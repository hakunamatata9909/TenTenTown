#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MiniMapCamera.generated.h"

UCLASS()
class TENTENTOWN_API AMiniMapCamera : public AActor
{
	GENERATED_BODY()
	
public:
	AMiniMapCamera();
	

	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	FVector2D GetPlayerMiniMapPosition(FVector PlayerLocation);

protected:
	virtual void BeginPlay() override;

	void CaptureMap();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MiniMap")
	USceneCaptureComponent2D* MiniMapCaptureComponent;

	//º¸°ü
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	UTextureRenderTarget2D* MiniMapRenderTarget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	FVector MiniMapLocation = FVector(0.0f, 0.0f, 7000.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	FRotator MiniMapRotation = FRotator(-90.0f, 0.0f, 0.0f);


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	float OrthoWidth = 5000.0f;


};
