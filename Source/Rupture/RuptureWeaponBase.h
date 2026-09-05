// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuptureWeaponBase.generated.h"

class ARuptureProjectile;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, ReserveAmmo);

UCLASS()
class RUPTURE_API ARuptureWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARuptureWeaponBase();

	void StartFire();
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon Properties")
	void Reload();

	/** Restaura magazine e reserva ao máximo (novo round / revive). */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void ResetAmmoToFull();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;
	
	void Fire();
	
	UFUNCTION(BlueprintPure, Category = "Ammo")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure, Category = "Ammo")
	int32 GetReserveAmmo() const;

	/** Ancora a arma no WeaponSocket e corrige o eixo do mesh (KA47 aponta +X). */
	void AttachToCharacterMesh(USkeletalMeshComponent* CharacterMesh);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Recoil")
	float VerticalRecoil = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Recoil")
	float HorizontalRecoil = 0.05f;

	FTimerHandle FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 MaxMagazineAmmo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 MaxReserveAmmo = 90;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 ReserveAmmo;

	bool CanFire() const;
	bool TryGetAimDirection(const APawn* OwnerPawn, FVector& OutAimDirection) const;
	FVector GetMuzzleWorldLocation() const;
	void SpawnProjectile(APawn* OwnerPawn, const FVector& AimDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties|Projectile")
	TSubclassOf<ARuptureProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties|Projectile")
	float ProjectileSpeed = 7500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties|Projectile")
	float MuzzleSpawnOffset = 18.f;

	/**
	 * Correção do eixo da KA47 no socket da mão.
	 * Identity deixa o cano apontando para a direita do personagem.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	FRotator SocketAttachRotation = FRotator(0.f, -90.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	FVector SocketAttachLocation = FVector::ZeroVector;

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

	//Shake Cam
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Feedback")
	TSubclassOf<class UCameraShakeBase> FireCameraShakeClass;



};


