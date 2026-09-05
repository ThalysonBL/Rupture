#include "RuptureMainMenuPlayerController.h"

#include "RuptureMainMenuShowcase.h"
#include "RuptureMainMenuWidget.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ARuptureMainMenuPlayerController::ARuptureMainMenuPlayerController()
{
	MainMenuWidgetClass = URuptureMainMenuWidget::StaticClass();
}

void ARuptureMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	SpawnShowcaseIfNeeded();
	PossessCinematicCamera();

	if (!MainMenuWidgetClass)
	{
		MainMenuWidgetClass = URuptureMainMenuWidget::StaticClass();
	}

	MainMenuWidget = CreateWidget<URuptureMainMenuWidget>(this, MainMenuWidgetClass);
	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport(0);

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		UE_LOG(LogTemp, Log, TEXT("MainMenuPC: menu cinematográfico adicionado ao viewport."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuPC: falha ao criar o widget do menu."));
	}
}

void ARuptureMainMenuPlayerController::SpawnShowcaseIfNeeded()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> Existing;
	UGameplayStatics::GetAllActorsOfClass(World, ARuptureMainMenuShowcase::StaticClass(), Existing);
	if (Existing.Num() > 0)
	{
		return;
	}

	FVector MenuActorLocation(-200.f, 0.f, 92.f);
	FRotator MenuActorRotation(0.f, 180.f, 0.f);
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()))
	{
		MenuActorLocation = PlayerStart->GetActorLocation();
		MenuActorRotation = PlayerStart->GetActorRotation();
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	World->SpawnActor<ARuptureMainMenuShowcase>(ARuptureMainMenuShowcase::StaticClass(), MenuActorLocation, MenuActorRotation, Params);
}

void ARuptureMainMenuPlayerController::PossessCinematicCamera()
{
	TArray<AActor*> Showcases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuptureMainMenuShowcase::StaticClass(), Showcases);
	if (Showcases.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuPC: showcase não encontrado; câmera padrão será usada."));
		return;
	}

	SetViewTargetWithBlend(Showcases[0], 0.f);
}
