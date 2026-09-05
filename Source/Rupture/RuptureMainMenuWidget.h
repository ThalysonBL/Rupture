#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuptureMainMenuWidget.generated.h"

class UButton;
class UImage;
class UOverlay;
class UTextBlock;
class UVerticalBox;

UCLASS()
class RUPTURE_API URuptureMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URuptureMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> btn_StartGame;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> btn_Quit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> txt_Title;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> txt_Subtitle;

	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnStartHovered();

	UFUNCTION()
	void OnStartUnhovered();

	UFUNCTION()
	void OnQuitHovered();

	UFUNCTION()
	void OnQuitUnhovered();

	/** Nome do mapa de jogo (sem extensão). */
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FName GameLevelName = TEXT("L_Raids");

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	float FadeInDuration = 1.15f;

private:
	void BuildCinematicLayout();
	void BindButtonCallbacks();
	void ApplyGhostButtonStyle(UButton* Button, UTextBlock* Label, bool bPrimary) const;
	void SetAccentVisible(UImage* Accent, bool bHovered) const;

	UTextBlock* FindOrCreateButtonLabel(UButton* Button, const FText& DefaultText, const FName& WidgetName);
	UButton* CreateGhostButton(const FName& ButtonName, const FText& Label, UTextBlock*& OutLabel);
	UImage* CreateSolidImage(const FName& WidgetName, const FLinearColor& Color);
	UTextBlock* CreateLabel(const FName& WidgetName, const FText& Text, int32 FontSize, bool bBold, const FLinearColor& Color, int32 LetterSpacing);
	UOverlay* EnsureHostOverlay();
	UOverlay* FindHostOverlay() const;
	UTexture2D* GetWhiteTexture() const;

	UPROPERTY()
	TObjectPtr<UImage> StartAccent;

	UPROPERTY()
	TObjectPtr<UImage> QuitAccent;

	bool bLayoutReady = false;
	bool bIsFadingIn = false;
	float FadeElapsed = 0.f;
};
