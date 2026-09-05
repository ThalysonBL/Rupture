#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuptureMainMenuShowcase.generated.h"

class UCameraComponent;
class UNiagaraComponent;
class UPointLightComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

/**
 * Cena cinematográfica do menu: personagem, portal, luzes e câmera.
 * Spawna em runtime a partir do PlayerStart de L_MainMenu.
 */
UCLASS()
class RUPTURE_API ARuptureMainMenuShowcase : public AActor
{
	GENERATED_BODY()

public:
	ARuptureMainMenuShowcase();

	UCameraComponent* GetCinematicCamera() const { return CinematicCamera; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UStaticMeshComponent> RifleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UNiagaraComponent> PortalVfx;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UCameraComponent> CinematicCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UPointLightComponent> KeyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UPointLightComponent> RimLight;

	UPROPERTY(EditAnywhere, Category = "Menu|Câmera")
	float CameraSwayYawDegrees = 2.2f;

	UPROPERTY(EditAnywhere, Category = "Menu|Câmera")
	float CameraSwayHeight = 5.0f;

private:
	void SnapToFloor();
	void AttachRifleToHand();
	void FrameCinematicCamera();

	FVector BaseCameraLocation = FVector::ZeroVector;
	FRotator BaseCameraRotation = FRotator::ZeroRotator;
	float CameraTime = 0.f;
};
