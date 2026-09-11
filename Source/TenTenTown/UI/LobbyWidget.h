#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"


class ULobbyViewModel;
class UButton;
class UImage;


UCLASS()
class TENTENTOWN_API ULobbyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // C++ 외부(LobbyPCComponent)에서 ViewModel을 설정하기 위한 함수
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(ULobbyViewModel* InViewModel);

protected:    
    // BlueprintReadOnly로 설정하여 블루프린트에서 바인딩 소스로 사용 가능하게 합니다.
    UPROPERTY(BlueprintReadOnly, Category = "MVVM")
    TObjectPtr<ULobbyViewModel> LobbyViewModel;

    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    UPROPERTY(meta = (BindWidget))
    UButton* MapButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CharButton;

    UPROPERTY(meta = (BindWidget))
    UImage* MapImage;

    
public:
	

    UFUNCTION()
    void OnConfirmButtonClicked();

    UFUNCTION()
	void OnMapButtonClicked();
    UFUNCTION()
	void OnCharButtonClicked();
};