#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuptureProjectile.generated.h"

class UNiagaraSystem;
class UPointLightComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;
class UMaterialInterface;

/**
 * Projétil disparado pela arma. Aplica dano no impacto e reproduz FX de hit.
 */
UCLASS()
class RUPTURE_API ARuptureProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARuptureProjectile();

	/**
	 * Configura dano e efeitos de impacto após o spawn.
	 * Ignora a arma e o atirador na colisão.
	 */
	void InitializeShot(float InDamage, UNiagaraSystem* InImpactParticle, UMaterialInterface* InDecalMaterial);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UPointLightComponent> TracerLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UMaterialInterface> DecalMaterial;

private:
	void PlayImpactEffects(const FHitResult& Hit) const;
	void IgnoreShooterCollision();

	bool bConsumed = false;
};
