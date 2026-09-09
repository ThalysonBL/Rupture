#include "RuptureMainMenuWidget.h"

#include "RuptureGameInstance.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "Blueprint/WidgetTree.h"

namespace RuptureMainMenuUI
{
	const FLinearColor ColorTitle(0.97f, 0.95f, 0.91f, 1.f);
	const FLinearColor ColorMuted(0.72f, 0.69f, 0.64f, 0.9f);
	const FLinearColor ColorAccent(0.95f, 0.42f, 0.10f, 1.f);
	const FLinearColor ColorDimFull(0.f, 0.f, 0.f, 0.28f);
	const FLinearColor ColorDimLeft(0.02f, 0.02f, 0.025f, 0.72f);
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

URuptureMainMenuWidget::URuptureMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bHasScriptImplementedTick = true;
}

bool URuptureMainMenuWidget::CanBuildRuntimeLayout() const
{
	return !HasAnyFlags(RF_ClassDefaultObject) && GetWorld() != nullptr && !IsRunningCommandlet();
}

void URuptureMainMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URuptureMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!CanBuildRuntimeLayout())
	{
		return;
	}

	BuildCinematicLayout();
	BindButtonCallbacks();

	SetRenderOpacity(0.f);
	bIsFadingIn = true;
	FadeElapsed = 0.f;
}

void URuptureMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsFadingIn)
	{
		return;
	}

	FadeElapsed += InDeltaTime;
	const float Alpha = FadeInDuration > 0.f
		? FMath::Clamp(FadeElapsed / FadeInDuration, 0.f, 1.f)
		: 1.f;
	SetRenderOpacity(FMath::InterpEaseOut(0.f, 1.f, Alpha, 1.6f));

	if (Alpha >= 1.f)
	{
		SetRenderOpacity(1.f);
		bIsFadingIn = false;
	}
}

void URuptureMainMenuWidget::BuildCinematicLayout()
{
	if (bLayoutReady || !WidgetTree)
	{
		return;
	}

	UOverlay* Host = EnsureHostOverlay();
	if (!Host)
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenu: overlay hospedeiro não encontrado."));
		return;
	}

	UImage* FullDim = CreateSolidImage(TEXT("img_FullDim"), RuptureMainMenuUI::ColorDimFull);
	RuptureMainMenuUI::FillOverlay(Host->AddChildToOverlay(FullDim));
	FullDim->SetVisibility(ESlateVisibility::HitTestInvisible);

	UHorizontalBox* Columns = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("hb_Columns"));
	RuptureMainMenuUI::FillOverlay(Host->AddChildToOverlay(Columns));
	Columns->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	UOverlay* LeftOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ov_LeftPanel"));
	UHorizontalBoxSlot* LeftSlot = Columns->AddChildToHorizontalBox(LeftOverlay);
	FSlateChildSize LeftSize(ESlateSizeRule::Fill);
	LeftSize.Value = 0.42f;
	LeftSlot->SetSize(LeftSize);
	LeftSlot->SetVerticalAlignment(VAlign_Fill);
	LeftSlot->SetHorizontalAlignment(HAlign_Fill);

	UImage* LeftDim = CreateSolidImage(TEXT("img_LeftDim"), RuptureMainMenuUI::ColorDimLeft);
	RuptureMainMenuUI::FillOverlay(LeftOverlay->AddChildToOverlay(LeftDim));
	LeftDim->SetVisibility(ESlateVisibility::HitTestInvisible);

	USizeBox* ContentBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("sb_Content"));
	UOverlaySlot* ContentSlot = LeftOverlay->AddChildToOverlay(ContentBox);
	ContentSlot->SetHorizontalAlignment(HAlign_Left);
	ContentSlot->SetVerticalAlignment(VAlign_Center);
	ContentSlot->SetPadding(FMargin(88.f, 40.f, 32.f, 40.f));

	UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("vb_Content"));
	ContentBox->AddChild(Content);

	UImage* AccentBar = CreateSolidImage(TEXT("img_AccentBar"), RuptureMainMenuUI::ColorAccent);
	USizeBox* AccentBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("sb_AccentBar"));
	AccentBox->SetWidthOverride(56.f);
	AccentBox->SetHeightOverride(3.f);
	AccentBox->AddChild(AccentBar);
	if (UVerticalBoxSlot* AccentSlot = Content->AddChildToVerticalBox(AccentBox))
	{
		AccentSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
		AccentSlot->SetHorizontalAlignment(HAlign_Left);
	}

	if (!txt_Title)
	{
		txt_Title = CreateLabel(
			TEXT("txt_Title"),
			FText::FromString(TEXT("RUPTURE")),
			78,
			true,
			RuptureMainMenuUI::ColorTitle,
			16);
	}
	else
	{
		txt_Title->SetText(FText::FromString(TEXT("RUPTURE")));
	}
	if (UVerticalBoxSlot* TitleSlot = Content->AddChildToVerticalBox(txt_Title))
	{
		TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		TitleSlot->SetHorizontalAlignment(HAlign_Left);
	}

	if (!txt_Subtitle)
	{
		txt_Subtitle = CreateLabel(
			TEXT("txt_Subtitle"),
			FText::FromString(TEXT("SOBREVIVA A 10 ROUNDS")),
			15,
			false,
			RuptureMainMenuUI::ColorMuted,
			6);
	}
	else
	{
		txt_Subtitle->SetText(FText::FromString(TEXT("SOBREVIVA A 10 ROUNDS")));
	}
	if (UVerticalBoxSlot* SubSlot = Content->AddChildToVerticalBox(txt_Subtitle))
	{
		SubSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 42.f));
		SubSlot->SetHorizontalAlignment(HAlign_Left);
	}

	UVerticalBox* ButtonStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("vb_Buttons"));
	if (UVerticalBoxSlot* StackSlot = Content->AddChildToVerticalBox(ButtonStack))
	{
		StackSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 24.f));
		StackSlot->SetHorizontalAlignment(HAlign_Left);
	}

	auto AddAccentRow = [this, ButtonStack](UButton* Button, UImage*& AccentOut, const FName& AccentName)
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		AccentOut = CreateSolidImage(AccentName, FLinearColor(RuptureMainMenuUI::ColorAccent.R, RuptureMainMenuUI::ColorAccent.G, RuptureMainMenuUI::ColorAccent.B, 0.35f));

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

		if (UVerticalBoxSlot* RowSlot = ButtonStack->AddChildToVerticalBox(Row))
		{
			RowSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
			RowSlot->SetHorizontalAlignment(HAlign_Left);
		}
	};

	UTextBlock* StartLabel = nullptr;
	if (!btn_StartGame)
	{
		btn_StartGame = CreateGhostButton(TEXT("btn_StartGame"), FText::FromString(TEXT("INICIAR JOGO")), StartLabel);
	}
	else
	{
		StartLabel = FindOrCreateButtonLabel(btn_StartGame, FText::FromString(TEXT("INICIAR JOGO")), TEXT("txt_StartLabel"));
	}
	ApplyGhostButtonStyle(btn_StartGame, StartLabel, true);
	UImage* StartAccentPtr = nullptr;
	AddAccentRow(btn_StartGame, StartAccentPtr, TEXT("img_StartAccent"));
	StartAccent = StartAccentPtr;

	UTextBlock* QuitLabel = nullptr;
	if (!btn_Quit)
	{
		btn_Quit = CreateGhostButton(TEXT("btn_Quit"), FText::FromString(TEXT("SAIR")), QuitLabel);
	}
	else
	{
		QuitLabel = FindOrCreateButtonLabel(btn_Quit, FText::FromString(TEXT("SAIR")), TEXT("txt_QuitLabel"));
	}
	ApplyGhostButtonStyle(btn_Quit, QuitLabel, false);
	UImage* QuitAccentPtr = nullptr;
	AddAccentRow(btn_Quit, QuitAccentPtr, TEXT("img_QuitAccent"));
	QuitAccent = QuitAccentPtr;

	USpacer* RightSpace = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("spc_World"));
	if (UHorizontalBoxSlot* RightSlot = Columns->AddChildToHorizontalBox(RightSpace))
	{
		FSlateChildSize RightSize(ESlateSizeRule::Fill);
		RightSize.Value = 0.58f;
		RightSlot->SetSize(RightSize);
	}

	bLayoutReady = true;
}

void URuptureMainMenuWidget::BindButtonCallbacks()
{
	if (btn_StartGame)
	{
		btn_StartGame->OnClicked.RemoveDynamic(this, &URuptureMainMenuWidget::OnStartGameClicked);
		btn_StartGame->OnClicked.AddDynamic(this, &URuptureMainMenuWidget::OnStartGameClicked);
		btn_StartGame->OnHovered.RemoveDynamic(this, &URuptureMainMenuWidget::OnStartHovered);
		btn_StartGame->OnHovered.AddDynamic(this, &URuptureMainMenuWidget::OnStartHovered);
		btn_StartGame->OnUnhovered.RemoveDynamic(this, &URuptureMainMenuWidget::OnStartUnhovered);
		btn_StartGame->OnUnhovered.AddDynamic(this, &URuptureMainMenuWidget::OnStartUnhovered);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenu: btn_StartGame não pôde ser criado."));
	}

	if (btn_Quit)
	{
		btn_Quit->OnClicked.RemoveDynamic(this, &URuptureMainMenuWidget::OnQuitClicked);
		btn_Quit->OnClicked.AddDynamic(this, &URuptureMainMenuWidget::OnQuitClicked);
		btn_Quit->OnHovered.RemoveDynamic(this, &URuptureMainMenuWidget::OnQuitHovered);
		btn_Quit->OnHovered.AddDynamic(this, &URuptureMainMenuWidget::OnQuitHovered);
		btn_Quit->OnUnhovered.RemoveDynamic(this, &URuptureMainMenuWidget::OnQuitUnhovered);
		btn_Quit->OnUnhovered.AddDynamic(this, &URuptureMainMenuWidget::OnQuitUnhovered);
	}
}

void URuptureMainMenuWidget::ApplyGhostButtonStyle(UButton* Button, UTextBlock* Label, bool bPrimary) const
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
	Hovered.TintColor = FSlateColor(RuptureMainMenuUI::ColorGhostHover);

	FSlateBrush Pressed = Normal;
	Pressed.TintColor = FSlateColor(RuptureMainMenuUI::ColorGhostPress);

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
		const int32 Size = bPrimary ? 20 : 18;
		FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle(bPrimary ? TEXT("Bold") : TEXT("Regular"), Size);
		Font.LetterSpacing = 4;
		Label->SetFont(Font);
		Label->SetColorAndOpacity(FSlateColor(RuptureMainMenuUI::ColorTitle));
	}
}

void URuptureMainMenuWidget::SetAccentVisible(UImage* Accent, bool bHovered) const
{
	if (!Accent)
	{
		return;
	}

	const float Alpha = bHovered ? 1.f : 0.35f;
	Accent->SetColorAndOpacity(FLinearColor(RuptureMainMenuUI::ColorAccent.R, RuptureMainMenuUI::ColorAccent.G, RuptureMainMenuUI::ColorAccent.B, Alpha));
}

UTextBlock* URuptureMainMenuWidget::FindOrCreateButtonLabel(UButton* Button, const FText& DefaultText, const FName& WidgetName)
{
	if (!Button)
	{
		return nullptr;
	}

	if (UTextBlock* Existing = Cast<UTextBlock>(Button->GetChildAt(0)))
	{
		Existing->SetText(DefaultText);
		return Existing;
	}

	UTextBlock* Label = CreateLabel(WidgetName, DefaultText, 20, true, RuptureMainMenuUI::ColorTitle, 4);
	Button->AddChild(Label);
	return Label;
}

UButton* URuptureMainMenuWidget::CreateGhostButton(const FName& ButtonName, const FText& Label, UTextBlock*& OutLabel)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), ButtonName);
	OutLabel = CreateLabel(*FString::Printf(TEXT("%s_Label"), *ButtonName.ToString()), Label, 20, true, RuptureMainMenuUI::ColorTitle, 4);
	Button->AddChild(OutLabel);
	return Button;
}

UImage* URuptureMainMenuWidget::CreateSolidImage(const FName& WidgetName, const FLinearColor& Color)
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

UTextBlock* URuptureMainMenuWidget::CreateLabel(
	const FName& WidgetName,
	const FText& Text,
	int32 FontSize,
	bool bBold,
	const FLinearColor& Color,
	int32 LetterSpacing)
{
	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName);
	Label->SetText(Text);
	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle(bBold ? TEXT("Bold") : TEXT("Regular"), FontSize);
	Font.LetterSpacing = LetterSpacing;
	Label->SetFont(Font);
	Label->SetColorAndOpacity(FSlateColor(Color));
	Label->SetVisibility(ESlateVisibility::HitTestInvisible);
	return Label;
}

UOverlay* URuptureMainMenuWidget::EnsureHostOverlay()
{
	if (UOverlay* Existing = FindHostOverlay())
	{
		return Existing;
	}

	if (!WidgetTree)
	{
		return nullptr;
	}

	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("CinematicRoot"));
	WidgetTree->RootWidget = Root;
	return Root;
}

UOverlay* URuptureMainMenuWidget::FindHostOverlay() const
{
	if (btn_StartGame)
	{
		if (UOverlay* Overlay = Cast<UOverlay>(btn_StartGame->GetParent()))
		{
			return Overlay;
		}
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

UTexture2D* URuptureMainMenuWidget::GetWhiteTexture() const
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
}

void URuptureMainMenuWidget::OnStartGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("MainMenu: Iniciar jogo -> %s"), *GameLevelName.ToString());

	if (URuptureGameInstance* GameInstance = Cast<URuptureGameInstance>(GetGameInstance()))
	{
		GameInstance->OpenLevelWithLoadingScreen(
			GameLevelName,
			FText::FromString(TEXT("Carregando missão...")));
		return;
	}

	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void URuptureMainMenuWidget::OnQuitClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("MainMenu: saindo do jogo."));
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void URuptureMainMenuWidget::OnStartHovered()
{
	SetAccentVisible(StartAccent, true);
}

void URuptureMainMenuWidget::OnStartUnhovered()
{
	SetAccentVisible(StartAccent, false);
}

void URuptureMainMenuWidget::OnQuitHovered()
{
	SetAccentVisible(QuitAccent, true);
}

void URuptureMainMenuWidget::OnQuitUnhovered()
{
	SetAccentVisible(QuitAccent, false);
}
