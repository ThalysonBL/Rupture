#include "RuptureAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

void URuptureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
	{
		return;
	}

	const UCharacterMovementComponent* MoveComp = Pawn->FindComponentByClass<UCharacterMovementComponent>();
	if (!MoveComp)
	{
		return;
	}

	UpdateLocomotionData(Pawn, MoveComp);
	UpdateStrideScale(Speed);
	UpdateFloorNormal(Pawn);

	bIsCrouched = MoveComp->IsCrouching();
	bIsFalling = MoveComp->IsFalling();

	UpdateAimPitch(Pawn, DeltaSeconds);
}

void URuptureAnimInstance::UpdateAimPitch(const APawn* Pawn, const float DeltaSeconds)
{
	if (!Pawn)
	{
		AimPitch = 0.f;
		AimOffsetAlpha = 0.f;
		SmoothedAimPitch = 0.f;
		return;
	}

	const AController* Controller = Pawn->GetController();
	if (!Controller)
	{
		AimPitch = 0.f;
		AimOffsetAlpha = 0.f;
		return;
	}

	// UE: olhar para cima = pitch negativo; para baixo = positivo.
	// AOBS (CU em -60, CD em +60) usa o mesmo sinal — sem inversão.
	float TargetPitch = FRotator::NormalizeAxis(Controller->GetControlRotation().Pitch);

	TargetPitch *= AimPitchMultiplier;
	TargetPitch = FMath::Clamp(TargetPitch, AimPitchMin, AimPitchMax);

	if (AimPitchInterpSpeed <= 0.f)
	{
		SmoothedAimPitch = TargetPitch;
	}
	else
	{
		SmoothedAimPitch = FMath::FInterpTo(SmoothedAimPitch, TargetPitch, DeltaSeconds, AimPitchInterpSpeed);
	}

	AimPitch = SmoothedAimPitch;
	AimOffsetAlpha = 1.f;
}

void URuptureAnimInstance::UpdateLocomotionData(const APawn* Pawn, const UCharacterMovementComponent* MoveComp)
{
	const FVector Velocity = MoveComp->Velocity;
	Speed = Velocity.Size2D();

	if (Speed > KINDA_SMALL_NUMBER)
	{
		const float VelocityYaw = Velocity.ToOrientationRotator().Yaw;
		const float ActorYaw = Pawn->GetActorRotation().Yaw;
		Direction = FMath::FindDeltaAngleDegrees(ActorYaw, VelocityYaw);
		LocomotionAngle = Direction;
	}
	else
	{
		Direction = 0.f;
		LocomotionAngle = 0.f;
	}
}

void URuptureAnimInstance::UpdateStrideScale(const float GroundSpeed)
{
	if (GroundSpeed < StrideScaleIdleSpeedThreshold)
	{
		StrideScale = 1.f;
		return;
	}

	const float ReferenceSpeed = (GroundSpeed < StrideScaleWalkRunThreshold)
		? StrideScaleWalkReferenceSpeed
		: StrideScaleRunReferenceSpeed;

	const float SafeReferenceSpeed = FMath::Max(ReferenceSpeed, 1.f);
	StrideScale = FMath::Clamp(GroundSpeed / SafeReferenceSpeed, StrideScaleMin, StrideScaleMax);
}

void URuptureAnimInstance::UpdateFloorNormal(const APawn* Pawn)
{
	if (!bEnableFloorTrace)
	{
		FloorNormal = FVector::UpVector;
		return;
	}

	// Parado: assume chão plano sem gastar line trace.
	if (Speed < StrideScaleIdleSpeedThreshold)
	{
		FloorNormal = FVector::UpVector;
		return;
	}

	++FloorTraceFrameCounter;
	if (FloorTraceFrameCounter % FloorTraceIntervalFrames != 0)
	{
		return;
	}

	const UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		FloorNormal = FVector::UpVector;
		return;
	}

	const FVector TraceStart = Pawn->GetActorLocation();
	const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, FloorTraceDistance);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RuptureFloorTrace), false, Pawn);
	QueryParams.AddIgnoredActor(Pawn);

	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		FloorNormal = Hit.ImpactNormal.GetSafeNormal();
	}
	else
	{
		FloorNormal = FVector::UpVector;
	}
}
