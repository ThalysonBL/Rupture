#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RuptureAnimInstance.generated.h"

/**
 * Classe pai do ABP_Rupture.
 * Calcula locomoção e dados de Animation Warping (Stride / Orientation) em C++.
 */
UCLASS()
class RUPTURE_API URuptureAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Velocidade horizontal do personagem (unidades/s). */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Speed = 0.f;

	/** Ângulo entre a frente do ator e a direção do movimento (-180 a 180). */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Direction = 0.f;

	/** Escala de passada para Stride Warping. */
	UPROPERTY(BlueprintReadOnly, Category = "Animation Warping")
	float StrideScale = 1.f;

	/** Normal do chão para Stride / Slope Warping. */
	UPROPERTY(BlueprintReadOnly, Category = "Animation Warping")
	FVector FloorNormal = FVector::UpVector;

	/** Ângulo de locomoção para Orientation Warping (mesma base do Direction). */
	UPROPERTY(BlueprintReadOnly, Category = "Animation Warping")
	float LocomotionAngle = 0.f;

	/** Pitch suavizado enviado ao Aim Offset (AOBS). */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|Aim")
	float AimPitch = 0.f;

	/** Intensidade do aim offset (0 = desligado, 1 = total). */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|Aim")
	float AimOffsetAlpha = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsCrouched = false;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsFalling = false;

protected:
	/** Velocidade de referência da animação de caminhada. */
	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleWalkReferenceSpeed = 300.f;

	/** Velocidade de referência da animação de corrida. */
	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleRunReferenceSpeed = 600.f;

	/** Acima deste valor usa referência de corrida no StrideScale. */
	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleWalkRunThreshold = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleMin = 0.75f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleMax = 1.25f;

	/** Abaixo desta velocidade, StrideScale volta para 1 (idle). */
	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Stride")
	float StrideScaleIdleSpeedThreshold = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Floor")
	bool bEnableFloorTrace = true;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Warping|Floor", meta = (ClampMin = "10.0"))
	float FloorTraceDistance = 100.f;

	/** Limites do pitch enviados ao Aim Offset (devem bater com o range do AOBS). */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Aim")
	float AimPitchMin = -60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Aim")
	float AimPitchMax = 60.f;

	/** Multiplicador do pitch (ex.: 0.7 = inclina um pouco menos). */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Aim", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float AimPitchMultiplier = 1.f;

	/** Inverte o pitch. Ative se CU estiver no topo (+) do AOBS; desative se CU estiver embaixo (-). */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Aim")
	bool bInvertAimPitch = true;

	/** Suavização do pitch (0 = instantâneo). */
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Aim", meta = (ClampMin = "0.0"))
	float AimPitchInterpSpeed = 12.f;

private:
	void UpdateLocomotionData(const class APawn* Pawn, const class UCharacterMovementComponent* MoveComp);
	void UpdateStrideScale(float GroundSpeed);
	void UpdateFloorNormal(const class APawn* Pawn);
	void UpdateAimPitch(const class APawn* Pawn, float DeltaSeconds);

	float SmoothedAimPitch = 0.f;
};
