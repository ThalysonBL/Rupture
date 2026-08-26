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

	UFUNCTION()
	void OnAmmoChangedCallback(int32 CurrentAmmo, int32 ReserveAmmo);

	void ShowDeathScreen();
	void HideDeathScreen();

	void ShowVictoryScreen();
	void HideVictoryScreen();

	UFUNCTION()
	void OnPlayerDied();

	UFUNCTION()
	void OnAllRoundsCompleted();

	UFUNCTION()
	void OnRoundStarted(int32 RoundNumber);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class URupturePlayerHUDWidget> PlayerHUDClass;

	UPROPERTY()
	class URupturePlayerHUDWidget* PlayerHUDWidget;

	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth, float MaxHealth);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class URuptureDeathScreenWidget> DeathScreenClass;

	UPROPERTY()
	class URuptureDeathScreenWidget* DeathScreenWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class URuptureVictoryScreenWidget> VictoryScreenClass;

	UPROPERTY()
	class URuptureVictoryScreenWidget* VictoryScreenWidget;

	void BindWaveManagerEvents();
};
