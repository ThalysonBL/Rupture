// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RupturePlayerHUDWidget.generated.h"

class UProgressBar;
UCLASS()
class RUPTURE_API URupturePlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    	UProgressBar* pb_Health;
    
    	// Função chamada pelo Observer
    	void UpdateHealth(float CurrentHealth, float MaxHealth);
	
};
