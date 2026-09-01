#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuptureLoadingScreenWidget.generated.h"

class UTextBlock;

/** Tela de carregamento exibida durante transições de mapa. */
UCLASS()
class RUPTURE_API URuptureLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void SetStatusText(const FText& InText);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* txt_Status;
};
