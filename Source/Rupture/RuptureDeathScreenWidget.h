#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "RuptureDeathScreenWidget.generated.h"

class UButton;
class UImage;
class UOverlay;
class UTextBlock;

UCLASS()
class RUPTURE_API URuptureDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URuptureDeathScreenWidget(const FObjectInitializer& ObjectInitializer);

	void SetRoundInfo(int32 RoundNumber);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> btn_RestartRound;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> btn_RestartAll;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> txt_Title;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> txt_RoundInfo;

	UFUNCTION()
	void OnRestartRoundClicked();

	UFUNCTION()
	void OnRestartAllClicked();

	UFUNCTION()
	void OnRestartRoundHovered();

	UFUNCTION()
	void OnRestartRoundUnhovered();

	UFUNCTION()
	void OnRestartAllHovered();

	UFUNCTION()
	void OnRestartAllUnhovered();

private:
	class ARuptureWaveManager* FindWaveManager() const;
	void HandleRestart(bool bRestartAll);
	void BuildCinematicLayout();
	void BindButtonCallbacks();
	void ApplyGhostButtonStyle(UButton* Button, UTextBlock* Label, bool bPrimary) const;
	void SetAccentVisible(UImage* Accent, bool bHovered) const;
	void StyleLabel(UTextBlock* Label, int32 FontSize, bool bBold, const FLinearColor& Color, int32 LetterSpacing) const;
	UImage* CreateSolidImage(const FName& WidgetName, const FLinearColor& Color);
	UTexture2D* GetWhiteTexture() const;
	UOverlay* FindHostOverlay() const;
	UTextBlock* FindButtonLabel(UButton* Button) const;

	UPROPERTY()
	TObjectPtr<UImage> RestartRoundAccent;

	UPROPERTY()
	TObjectPtr<UImage> RestartAllAccent;

	bool bLayoutReady = false;
};
