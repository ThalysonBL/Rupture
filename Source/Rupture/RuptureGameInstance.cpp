#include "RuptureGameInstance.h"

#include "RuptureLoadingScreenWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void URuptureGameInstance::OpenLevelWithLoadingScreen(FName LevelName, const FText& StatusText)
{
	if (LevelName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance: LevelName inválido para carregamento."));
		return;
	}

	if (bIsLoadingLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance: carregamento já em andamento (%s)."), *PendingLevelName.ToString());
		return;
	}

	bIsLoadingLevel = true;
	bOpenLevelTriggered = false;
	PendingLevelName = LevelName;

	ShowLoadingScreen(StatusText.IsEmpty()
		? FText::FromString(TEXT("Carregando..."))
		: StatusText);

	const FString MapPackagePath = BuildMapPackagePath(LevelName);
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(
		FSoftObjectPath(MapPackagePath),
		FStreamableDelegate::CreateUObject(this, &URuptureGameInstance::OnMapPreloadComplete, LevelName),
		FStreamableManager::AsyncLoadHighPriority
	);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FallbackLoadTimerHandle,
			FTimerDelegate::CreateUObject(this, &URuptureGameInstance::BeginOpenLevel, LevelName),
			5.f,
			false
		);
	}
}

void URuptureGameInstance::ShowLoadingScreen(const FText& StatusText)
{
	if (!LoadingScreenWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance: LoadingScreenWidgetClass não definida. Defina no BP_GameInstance ou Class Defaults."));
		return;
	}

	if (!LoadingScreenWidget)
	{
		LoadingScreenWidget = CreateWidget<URuptureLoadingScreenWidget>(this, LoadingScreenWidgetClass);
	}

	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->SetStatusText(StatusText);
		if (!LoadingScreenWidget->IsInViewport())
		{
			LoadingScreenWidget->AddToViewport(1000);
		}
	}
}

void URuptureGameInstance::HideLoadingScreen()
{
	if (LoadingScreenWidget && LoadingScreenWidget->IsInViewport())
	{
		LoadingScreenWidget->RemoveFromParent();
	}
}

void URuptureGameInstance::OnMapPreloadComplete(FName LevelName)
{
	if (!bIsLoadingLevel || PendingLevelName != LevelName || bOpenLevelTriggered)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FallbackLoadTimerHandle);

		World->GetTimerManager().SetTimer(
			OpenLevelTimerHandle,
			FTimerDelegate::CreateUObject(this, &URuptureGameInstance::BeginOpenLevel, LevelName),
			LoadingScreenMinDisplayTime,
			false
		);
	}
	else
	{
		BeginOpenLevel(LevelName);
	}
}

void URuptureGameInstance::BeginOpenLevel(FName LevelName)
{
	if (!bIsLoadingLevel || PendingLevelName != LevelName || bOpenLevelTriggered)
	{
		return;
	}

	bOpenLevelTriggered = true;
	bIsLoadingLevel = false;
	PendingLevelName = NAME_None;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FallbackLoadTimerHandle);
		World->GetTimerManager().ClearTimer(OpenLevelTimerHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("GameInstance: abrindo mapa %s"), *LevelName.ToString());

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, LevelName);
	}
	else
	{
		UGameplayStatics::OpenLevel(this, LevelName);
	}

	HideLoadingScreen();
}

FString URuptureGameInstance::BuildMapPackagePath(FName LevelName) const
{
	const FString LevelString = LevelName.ToString();
	return FString::Printf(TEXT("/Game/Levels/%s.%s"), *LevelString, *LevelString);
}
