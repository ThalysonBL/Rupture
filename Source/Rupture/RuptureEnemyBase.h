// Fill out your copyright notice in the Description page of Project Settings.

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
	ARuptureEnemyBase();
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARuptureWeaponBase> StartingWeaponClass;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ARuptureWeaponBase* CurrentWeapon;

	UFUNCTION()
	void HandleDeath();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyHealthBarComponent* HealthBarComponent;
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* EnemyBehaviorTree;
};
