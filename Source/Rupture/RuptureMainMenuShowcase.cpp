#include "RuptureMainMenuShowcase.h"

#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Animation/AnimSequence.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"

ARuptureMainMenuShowcase::ARuptureMainMenuShowcase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add(TEXT("MainMenuShowcase"));

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(Root);
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CharacterMesh->SetCastShadow(true);
	CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	CharacterMesh->SetRelativeRotation(FRotator(0.f, 150.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(
		TEXT("/Game/QuantumCharacter/Mesh/SKM_QuantumCharacter.SKM_QuantumCharacter"));
	if (MeshFinder.Succeeded())
	{
		CharacterMesh->SetSkeletalMesh(MeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleFinder(
		TEXT("/Game/QuantumCharacter/Demo/Animations/A_MM_Idle.A_MM_Idle"));
	if (IdleFinder.Succeeded())
	{
		CharacterMesh->SetAnimation(IdleFinder.Object);
	}

	RifleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleMesh"));
	RifleMesh->SetupAttachment(CharacterMesh);
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RifleMesh->SetCastShadow(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RifleFinder(
		TEXT("/Game/QuantumCharacter/Mesh/Rifle/SM_Rifle_Olive.SM_Rifle_Olive"));
	if (RifleFinder.Succeeded())
	{
		RifleMesh->SetStaticMesh(RifleFinder.Object);
	}

	PortalVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalVfx"));
	PortalVfx->SetupAttachment(Root);
	PortalVfx->SetRelativeLocation(FVector(-40.f, 210.f, 70.f));
	PortalVfx->SetRelativeScale3D(FVector(1.6f));
	PortalVfx->bAutoActivate = true;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PortalFinder(
		TEXT("/Game/Portals/Rounded/LightningPortal/NS_Lightning.NS_Lightning"));
	if (PortalFinder.Succeeded())
	{
		PortalVfx->SetAsset(PortalFinder.Object);
	}

	CinematicCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CinematicCamera"));
	CinematicCamera->SetupAttachment(Root);
	CinematicCamera->SetFieldOfView(48.f);
	CinematicCamera->bConstrainAspectRatio = false;
	CinematicCamera->PostProcessBlendWeight = 1.f;
	CinematicCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
	CinematicCamera->PostProcessSettings.VignetteIntensity = 0.65f;
	CinematicCamera->PostProcessSettings.bOverride_AutoExposureBias = true;
	CinematicCamera->PostProcessSettings.AutoExposureBias = -0.15f;
	CinematicCamera->PostProcessSettings.bOverride_ColorSaturation = true;
	CinematicCamera->PostProcessSettings.ColorSaturation = FVector4(0.9f, 0.88f, 0.92f, 1.f);

	KeyLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("KeyLight"));
	KeyLight->SetupAttachment(Root);
	KeyLight->SetRelativeLocation(FVector(140.f, -90.f, 210.f));
	KeyLight->SetIntensity(9000.f);
	KeyLight->SetAttenuationRadius(900.f);
	KeyLight->SetLightColor(FLinearColor(1.f, 0.52f, 0.22f));
	KeyLight->SetCastShadows(true);

	RimLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RimLight"));
	RimLight->SetupAttachment(Root);
	RimLight->SetRelativeLocation(FVector(-70.f, 170.f, 150.f));
	RimLight->SetIntensity(12000.f);
	RimLight->SetAttenuationRadius(800.f);
	RimLight->SetLightColor(FLinearColor(0.28f, 0.72f, 1.f));
	RimLight->SetCastShadows(false);
}

void ARuptureMainMenuShowcase::BeginPlay()
{
	Super::BeginPlay();

	SnapToFloor();
	AttachRifleToHand();
	FrameCinematicCamera();

	if (CharacterMesh)
	{
		CharacterMesh->Play(true);
	}

	if (PortalVfx && !PortalVfx->IsActive())
	{
		PortalVfx->Activate(true);
	}

	UE_LOG(LogTemp, Log, TEXT("MenuShowcase: cena cinematográfica pronta em %s"), *GetActorLocation().ToString());
}

void ARuptureMainMenuShowcase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CinematicCamera)
	{
		return;
	}

	CameraTime += DeltaTime;
	const float YawSway = FMath::Sin(CameraTime * 0.18f) * CameraSwayYawDegrees;
	const float HeightSway = FMath::Sin(CameraTime * 0.23f) * CameraSwayHeight;

	CinematicCamera->SetRelativeLocation(BaseCameraLocation + FVector(0.f, 0.f, HeightSway));
	CinematicCamera->SetRelativeRotation(BaseCameraRotation + FRotator(0.f, YawSway, 0.f));
}

void ARuptureMainMenuShowcase::SnapToFloor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = GetActorLocation() + FVector(0.f, 0.f, 200.f);
	const FVector End = GetActorLocation() - FVector(0.f, 0.f, 600.f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MenuShowcaseFloor), false, this);
	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		SetActorLocation(Hit.ImpactPoint);
	}
}

void ARuptureMainMenuShowcase::AttachRifleToHand()
{
	if (!RifleMesh || !CharacterMesh)
	{
		return;
	}

	static const FName CandidateSockets[] = {
		TEXT("WeaponSocket"),
		TEXT("weapon_r"),
		TEXT("hand_r"),
		TEXT("Hand_R"),
		TEXT("ik_hand_gun"),
		TEXT("socket_r_hand")
	};

	for (const FName SocketName : CandidateSockets)
	{
		if (CharacterMesh->DoesSocketExist(SocketName))
		{
			RifleMesh->AttachToComponent(
				CharacterMesh,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName);
			UE_LOG(LogTemp, Log, TEXT("MenuShowcase: rifle anexado ao socket %s"), *SocketName.ToString());
			return;
		}
	}

	RifleMesh->SetRelativeLocation(FVector(8.f, 12.f, 108.f));
	RifleMesh->SetRelativeRotation(FRotator(-12.f, 90.f, 8.f));
	UE_LOG(LogTemp, Warning, TEXT("MenuShowcase: nenhum socket de arma encontrado; rifle em pose aproximada."));
}

void ARuptureMainMenuShowcase::FrameCinematicCamera()
{
	if (!CinematicCamera)
	{
		return;
	}

	BaseCameraLocation = FVector(290.f, -230.f, 148.f);
	const FVector LookAtTarget(10.f, 35.f, 98.f);
	BaseCameraRotation = (LookAtTarget - BaseCameraLocation).Rotation();

	CinematicCamera->SetRelativeLocation(BaseCameraLocation);
	CinematicCamera->SetRelativeRotation(BaseCameraRotation);
}
