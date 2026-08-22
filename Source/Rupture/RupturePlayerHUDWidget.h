// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RupturePlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class RUPTURE_API URupturePlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* pb_Health;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* txt_CurrentAmmo;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* txt_ReserveAmmo;
    
    // call function in Observer
    void UpdateHealth(float CurrentHealth, float MaxHealth);
	
	void UpdateAmmo(int32 CurrentAmmo, int32 ReserveAmmo);
	
};
