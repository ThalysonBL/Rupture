// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "RuptureDeathScreenWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class RUPTURE_API URuptureDeathScreenWidget : public UUserWidget {
  GENERATED_BODY()

public:
  void SetRoundInfo(int32 RoundNumber);

protected:
  virtual void NativeConstruct() override;

  UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
  UButton *btn_RestartRound;

  UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
  UButton *btn_RestartAll;

  UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
  UTextBlock *txt_Title;

  UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
  UTextBlock *txt_RoundInfo;

  UFUNCTION()
  void OnRestartRoundClicked();

  UFUNCTION()
  void OnRestartAllClicked();

private:
  class ARuptureWaveManager *FindWaveManager() const;
  void HandleRestart(bool bRestartAll);
};
