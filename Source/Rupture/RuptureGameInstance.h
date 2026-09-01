#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RuptureGameInstance.generated.h"

class URuptureLoadingScreenWidget;

/** Gerencia transições de mapa com tela de carregamento. */
UCLASS()
class RUPTURE_API URuptureGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/** Abre um mapa com tela de carregamento e pré-carregamento assíncrono. */
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void OpenLevelWithLoadingScreen(FName LevelName, const FText& StatusText = FText::GetEmpty());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TSubclassOf<URuptureLoadingScreenWidget> LoadingScreenWidgetClass;

	/** Atraso mínimo para o widget de loading renderizar antes do OpenLevel. */
	UPROPERTY(EditDefaultsOnly, Category = "Loading", meta = (ClampMin = "0.0"))
	float LoadingScreenMinDisplayTime = 0.15f;

private:
	void ShowLoadingScreen(const FText& StatusText);
	void HideLoadingScreen();
	void BeginOpenLevel(FName LevelName);
	void OnMapPreloadComplete(FName LevelName);
	FString BuildMapPackagePath(FName LevelName) const;

	UPROPERTY()
	TObjectPtr<URuptureLoadingScreenWidget> LoadingScreenWidget;

	FName PendingLevelName = NAME_None;
	bool bIsLoadingLevel = false;
	bool bOpenLevelTriggered = false;
	FTimerHandle FallbackLoadTimerHandle;
	FTimerHandle OpenLevelTimerHandle;
};
