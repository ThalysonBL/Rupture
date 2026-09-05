#include "RuptureDeathScreenWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Fonts/SlateFontInfo.h"
#include "Kismet/GameplayStatics.h"
#include "RuptureHUD.h"
#include "RupturePlayerCharacter.h"
#include "RuptureWaveManager.h"
#include "Styling/CoreStyle.h"
#include "Blueprint/WidgetTree.h"

namespace RuptureDeathScreenUI
{
	const FLinearColor ColorTitle(0.97f, 0.95f, 0.91f, 1.f);
	const FLinearColor ColorMuted(0.72f, 0.69f, 0.64f, 0.9f);
	const FLinearColor ColorAccent(0.95f, 0.42f, 0.10f, 1.f);
	const FLinearColor ColorDim(0.01f, 0.01f, 0.012f, 0.78f);
	const FLinearColor ColorGhostHover(0.95f, 0.42f, 0.10f, 0.16f);
	const FLinearColor ColorGhostPress(0.95f, 0.42f, 0.10f, 0.28f);

	void FillOverlay(UOverlaySlot* Slot)
	{
		if (!Slot)
		{
			return;
		}
		Slot->SetHorizontalAlignment(HAlign_Fill);
		Slot->SetVerticalAlignment(VAlign_Fill);
		Slot->SetPadding(FMargin(0.f));
	}
}

using namespace RuptureDeathScreenUI;

URuptureDeathScreenWidget::URuptureDeathScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URuptureDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildCinematicLayout();
	BindButtonCallbacks();
}

void URuptureDeathScreenWidget::SetRoundInfo(int32 RoundNumber)
{
	if (txt_RoundInfo)
	{
		txt_RoundInfo->SetText(
			FText::FromString(FString::Printf(TEXT("Você caiu no round %d"), RoundNumber)));
	}
}

void URuptureDeathScreenWidget::BuildCinematicLayout()
{
	if (bLayoutReady || !WidgetTree)
	{
		return;
	}

	UOverlay* Host = FindHostOverlay();
	if (!Host)
	{
		UE_LOG(LogTemp, Error, TEXT("DeathScreen: overlay hospedeiro não encontrado."));
		return;
	}

	if (UPanelWidget* OldParent = btn_RestartRound ? Cast<UPanelWidget>(btn_RestartRound->GetParent()) : nullptr)
	{
		OldParent->SetVisibility(ESlateVisibility::Collapsed);
	}

	for (int32 Index = Host->GetChildrenCount() - 1; Index >= 0; --Index)
	{
		if (UImage* Background = Cast<UImage>(Host->GetChildAt(Index)))
		{
			Background->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UImage* Dim = CreateSolidImage(TEXT("img_DeathDim"), ColorDim);
	FillOverlay(Host->AddChildToOverlay(Dim));

	UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("vb_DeathContent"));
	UOverlaySlot* ContentSlot = Host->AddChildToOverlay(Content);
	ContentSlot->SetHorizontalAlignment(HAlign_Center);
	ContentSlot->SetVerticalAlignment(VAlign_Center);
	ContentSlot->SetPadding(FMargin(48.f));

	UImage* AccentBar = CreateSolidImage(TEXT("img_DeathAccent"), ColorAccent);
	USizeBox* AccentBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("sb_DeathAccent"));
	AccentBox->SetWidthOverride(56.f);
	AccentBox->SetHeightOverride(3.f);
	AccentBox->AddChild(AccentBar);
	if (UVerticalBoxSlot* AccentSlot = Content->AddChildToVerticalBox(AccentBox))
	{
		AccentSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
		AccentSlot->SetHorizontalAlignment(HAlign_Center);
	}

	if (txt_Title)
	{
		txt_Title->RemoveFromParent();
		txt_Title->SetText(FText::FromString(TEXT("VOCÊ MORREU")));
		StyleLabel(txt_Title, 56, true, ColorTitle, 10);
		if (UVerticalBoxSlot* TitleSlot = Content->AddChildToVerticalBox(txt_Title))
		{
			TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
			TitleSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}

	if (txt_RoundInfo)
	{
		txt_RoundInfo->RemoveFromParent();
		StyleLabel(txt_RoundInfo, 16, false, ColorMuted, 2);
		if (UVerticalBoxSlot* InfoSlot = Content->AddChildToVerticalBox(txt_RoundInfo))
		{
			InfoSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 36.f));
			InfoSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}

	auto AddAccentRow = [this, Content](UButton* Button, UImage*& AccentOut, const FName& AccentName)
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		AccentOut = CreateSolidImage(AccentName, FLinearColor(ColorAccent.R, ColorAccent.G, ColorAccent.B, 0.35f));

		USizeBox* AccentSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		AccentSize->SetWidthOverride(3.f);
		AccentSize->SetHeightOverride(28.f);
		AccentSize->AddChild(AccentOut);

		if (UHorizontalBoxSlot* AccentSlot = Row->AddChildToHorizontalBox(AccentSize))
		{
			AccentSlot->SetPadding(FMargin(0.f, 0.f, 14.f, 0.f));
			AccentSlot->SetVerticalAlignment(VAlign_Center);
		}

		if (Button)
		{
			Button->RemoveFromParent();
			if (UHorizontalBoxSlot* ButtonSlot = Row->AddChildToHorizontalBox(Button))
			{
				ButtonSlot->SetVerticalAlignment(VAlign_Center);
			}
		}

		if (UVerticalBoxSlot* RowSlot = Content->AddChildToVerticalBox(Row))
		{
			RowSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
			RowSlot->SetHorizontalAlignment(HAlign_Center);
		}
	};

	UTextBlock* RestartRoundLabel = FindButtonLabel(btn_RestartRound);
	if (RestartRoundLabel)
	{
		RestartRoundLabel->SetText(FText::FromString(TEXT("REINICIAR ROUND")));
	}
	ApplyGhostButtonStyle(btn_RestartRound, RestartRoundLabel, true);
	UImage* RoundAccentPtr = nullptr;
	AddAccentRow(btn_RestartRound, RoundAccentPtr, TEXT("img_RestartRoundAccent"));
	RestartRoundAccent = RoundAccentPtr;

	UTextBlock* RestartAllLabel = FindButtonLabel(btn_RestartAll);
	if (RestartAllLabel)
	{
		RestartAllLabel->SetText(FText::FromString(TEXT("REINICIAR TODOS")));
	}
	ApplyGhostButtonStyle(btn_RestartAll, RestartAllLabel, false);
	UImage* AllAccentPtr = nullptr;
	AddAccentRow(btn_RestartAll, AllAccentPtr, TEXT("img_RestartAllAccent"));
	RestartAllAccent = AllAccentPtr;

	bLayoutReady = true;
}

void URuptureDeathScreenWidget::BindButtonCallbacks()
{
	if (btn_RestartRound)
	{
		btn_RestartRound->OnClicked.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundClicked);
		btn_RestartRound->OnClicked.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundClicked);
		btn_RestartRound->OnHovered.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundHovered);
		btn_RestartRound->OnHovered.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundHovered);
		btn_RestartRound->OnUnhovered.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundUnhovered);
		btn_RestartRound->OnUnhovered.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartRoundUnhovered);
	}

	if (btn_RestartAll)
	{
		btn_RestartAll->OnClicked.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartAllClicked);
		btn_RestartAll->OnClicked.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartAllClicked);
		btn_RestartAll->OnHovered.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartAllHovered);
		btn_RestartAll->OnHovered.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartAllHovered);
		btn_RestartAll->OnUnhovered.RemoveDynamic(this, &URuptureDeathScreenWidget::OnRestartAllUnhovered);
		btn_RestartAll->OnUnhovered.AddDynamic(this, &URuptureDeathScreenWidget::OnRestartAllUnhovered);
	}
}

void URuptureDeathScreenWidget::ApplyGhostButtonStyle(UButton* Button, UTextBlock* Label, bool bPrimary) const
{
	if (!Button)
	{
		return;
	}

	UTexture2D* White = GetWhiteTexture();

	FButtonStyle Style;
	FSlateBrush Normal;
	Normal.DrawAs = ESlateBrushDrawType::Image;
	Normal.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	Normal.SetResourceObject(White);

	FSlateBrush Hovered = Normal;
	Hovered.TintColor = FSlateColor(ColorGhostHover);

	FSlateBrush Pressed = Normal;
	Pressed.TintColor = FSlateColor(ColorGhostPress);

	Style.SetNormal(Normal);
	Style.SetHovered(Hovered);
	Style.SetPressed(Pressed);
	Style.SetDisabled(Normal);
	Style.NormalPadding = FMargin(4.f, 8.f, 18.f, 8.f);
	Style.PressedPadding = FMargin(4.f, 9.f, 18.f, 7.f);
	Button->SetStyle(Style);
	Button->SetBackgroundColor(FLinearColor::White);

	if (Label)
	{
		StyleLabel(Label, bPrimary ? 18 : 16, bPrimary, ColorTitle, 4);
	}
}

void URuptureDeathScreenWidget::SetAccentVisible(UImage* Accent, bool bHovered) const
{
	if (!Accent)
	{
		return;
	}

	const float Alpha = bHovered ? 1.f : 0.35f;
	Accent->SetColorAndOpacity(FLinearColor(ColorAccent.R, ColorAccent.G, ColorAccent.B, Alpha));
}

void URuptureDeathScreenWidget::StyleLabel(
	UTextBlock* Label,
	int32 FontSize,
	bool bBold,
	const FLinearColor& Color,
	int32 LetterSpacing) const
{
	if (!Label)
	{
		return;
	}

	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle(bBold ? TEXT("Bold") : TEXT("Regular"), FontSize);
	Font.LetterSpacing = LetterSpacing;
	Label->SetFont(Font);
	Label->SetColorAndOpacity(FSlateColor(Color));
}

UImage* URuptureDeathScreenWidget::CreateSolidImage(const FName& WidgetName, const FLinearColor& Color)
{
	UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), WidgetName);
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.TintColor = FSlateColor(Color);
	Brush.SetResourceObject(GetWhiteTexture());
	Image->SetBrush(Brush);
	Image->SetColorAndOpacity(FLinearColor::White);
	Image->SetVisibility(ESlateVisibility::HitTestInvisible);
	return Image;
}

UTexture2D* URuptureDeathScreenWidget::GetWhiteTexture() const
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
}

UOverlay* URuptureDeathScreenWidget::FindHostOverlay() const
{
	UWidget* WidgetIt = btn_RestartRound;
	while (WidgetIt)
	{
		if (UOverlay* Overlay = Cast<UOverlay>(WidgetIt))
		{
			return Overlay;
		}
		WidgetIt = WidgetIt->GetParent();
	}

	if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget()))
	{
		for (int32 Index = 0; Index < Canvas->GetChildrenCount(); ++Index)
		{
			if (UOverlay* Overlay = Cast<UOverlay>(Canvas->GetChildAt(Index)))
			{
				return Overlay;
			}
		}
	}

	return Cast<UOverlay>(GetRootWidget());
}

UTextBlock* URuptureDeathScreenWidget::FindButtonLabel(UButton* Button) const
{
	if (!Button)
	{
		return nullptr;
	}
	return Cast<UTextBlock>(Button->GetChildAt(0));
}

void URuptureDeathScreenWidget::OnRestartRoundClicked()
{
	HandleRestart(false);
}

void URuptureDeathScreenWidget::OnRestartAllClicked()
{
	HandleRestart(true);
}

void URuptureDeathScreenWidget::OnRestartRoundHovered()
{
	SetAccentVisible(RestartRoundAccent, true);
}

void URuptureDeathScreenWidget::OnRestartRoundUnhovered()
{
	SetAccentVisible(RestartRoundAccent, false);
}

void URuptureDeathScreenWidget::OnRestartAllHovered()
{
	SetAccentVisible(RestartAllAccent, true);
}

void URuptureDeathScreenWidget::OnRestartAllUnhovered()
{
	SetAccentVisible(RestartAllAccent, false);
}

ARuptureWaveManager* URuptureDeathScreenWidget::FindWaveManager() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuptureWaveManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		return Cast<ARuptureWaveManager>(FoundActors[0]);
	}
	return nullptr;
}

void URuptureDeathScreenWidget::HandleRestart(bool bRestartAll)
{
	ARuptureWaveManager* WaveManager = FindWaveManager();
	if (WaveManager)
	{
		if (bRestartAll)
		{
			WaveManager->RestartAllRounds();
		}
		else
		{
			WaveManager->RestartCurrentRound();
		}
	}

	if (ARupturePlayerCharacter* Player = Cast<ARupturePlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->Revive();
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ARuptureHUD* HUD = Cast<ARuptureHUD>(PC->GetHUD()))
		{
			HUD->HideDeathScreen();
		}
	}
}
