#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuptureVictoryScreenWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class RUPTURE_API URuptureVictoryScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowVictory();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* txt_Title;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* txt_Message;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* btn_BackToMenu;

	UFUNCTION()
	void OnBackToMenuClicked();

	/** Se > 0, volta ao menu sozinho após X segundos. */
	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	float AutoReturnDelay = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	FName MainMenuLevelName = TEXT("L_MainMenu");

private:
	void ReturnToMainMenu();

	FTimerHandle AutoReturnTimerHandle;
};
