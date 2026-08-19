// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuptureWeaponBase.generated.h"

UCLASS()
class RUPTURE_API ARuptureWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARuptureWeaponBase();

	void StartFire();
	void StopFire();
	void Reload();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//SKELETAL MESH COMPONENT
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float BaseDamage = 25.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float MaxRange = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float FireRate = 0.2f;

	float LastFireTime;

	void Fire();

	FTimerHandle FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 MaxMagazineAmmo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 MaxReserveAmmo = 90;

	bool CanFire() const;

	//FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	class UNiagaraSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ImpactParticle;

	//SOUND
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	class USoundBase* FireSound;

};


