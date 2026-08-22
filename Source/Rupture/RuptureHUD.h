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
	UFUNCTION()
	void OnWeaponEquippedCallback(class ARuptureWeaponBase* NewWeapon);

	// Callback que escuta os tiros e reloads (OBRIGATÓRIO ter UFUNCTION)
	UFUNCTION()
	void OnAmmoChangedCallback(int32 CurrentAmmo, int32 ReserveAmmo);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class URupturePlayerHUDWidget> PlayerHUDClass;

	UPROPERTY()
	class URupturePlayerHUDWidget* PlayerHUDWidget;

	// Callback do evento de vida (Obrigatório UFUNCTION)
	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth);
};
