#pragma once 

#include "CoreMinimal.h"
#include "RuptureBaseCharacter.h"
#include "RuptureWeaponBase.h"
#include "RuptureEnemyBase.generated.h"

class UHealthComponent;
class UEnemyHealthBarComponent;

UCLASS()
class RUPTURE_API ARuptureEnemyBase : public ARuptureBaseCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	ARuptureEnemyBase();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARuptureWeaponBase> StartingWeaponClass;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ARuptureWeaponBase* CurrentWeapon;

	UFUNCTION()
	void HandleDeath();

	/** Spawna a arma cedo (antes do BT), para o Rifleman ler munição corretamente. */
	void EnsureWeaponSpawned();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyHealthBarComponent* HealthBarComponent;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireWeapon();

	UFUNCTION(BlueprintPure, Category = "Combat")
	ARuptureWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* EnemyBehaviorTree;
};
