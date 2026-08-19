// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RuptureHUD.generated.h"

UCLASS()
class RUPTURE_API ARuptureHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerHUDClass;

	//ponteiro que vai guardar o widget criado em memoria
	UPROPERTY()
	class UUserWidget* PlayerHUDWidget;;
	
};
