#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuptureMainMenuWidget.generated.h"

class UButton;

UCLASS()
class RUPTURE_API URuptureMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* btn_StartGame;

	UFUNCTION()
	void OnStartGameClicked();

	/** Nome do mapa de jogo (sem extensão). */
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FName GameLevelName = TEXT("L_Raids");
};
