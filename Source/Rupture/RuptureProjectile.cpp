#include "RuptureProjectile.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"

ARuptureProjectile::ARuptureProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	InitialLifeSpan = 3.f;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->InitSphereRadius(6.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->SetGenerateOverlapEvents(false);
	CollisionSphere->BodyInstance.bUseCCD = true;
	CollisionSphere->OnComponentHit.AddDynamic(this, &ARuptureProjectile::OnHit);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(CollisionSphere);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletMesh->SetCastShadow(false);
	BulletMesh->SetRelativeScale3D(FVector(6.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletMeshFinder(
		TEXT("/Game/NW_MuzzleFX/Meshes/Static_Mesh/NS3A1_Bullet_02_editedPivot.NS3A1_Bullet_02_editedPivot"));
	if (BulletMeshFinder.Succeeded())
	{
		BulletMesh->SetStaticMesh(BulletMeshFinder.Object);
	}

	TracerLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TracerLight"));
	TracerLight->SetupAttachment(CollisionSphere);
	TracerLight->SetIntensity(2500.f);
	TracerLight->SetAttenuationRadius(120.f);
	TracerLight->SetLightColor(FLinearColor(1.f, 0.55f, 0.18f));
	TracerLight->SetCastShadows(false);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 7500.f;
	ProjectileMovement->MaxSpeed = 7500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bForceSubStepping = true;
	ProjectileMovement->MaxSimulationTimeStep = 0.0167f;
}

void ARuptureProjectile::BeginPlay()
{
	Super::BeginPlay();
	IgnoreShooterCollision();
}

void ARuptureProjectile::InitializeShot(const float InDamage, UNiagaraSystem* InImpactParticle, UMaterialInterface* InDecalMaterial)
{
	Damage = InDamage;
	ImpactParticle = InImpactParticle;
	DecalMaterial = InDecalMaterial;
	IgnoreShooterCollision();
}

void ARuptureProjectile::IgnoreShooterCollision()
{
	if (!CollisionSphere)
	{
		return;
	}

	if (AActor* Weapon = GetOwner())
	{
		CollisionSphere->IgnoreActorWhenMoving(Weapon, true);
		if (AActor* Shooter = Weapon->GetOwner())
		{
			CollisionSphere->IgnoreActorWhenMoving(Shooter, true);
		}
	}

	if (APawn* ShooterPawn = GetInstigator())
	{
		CollisionSphere->IgnoreActorWhenMoving(ShooterPawn, true);
	}
}

void ARuptureProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bConsumed)
	{
		return;
	}

	AActor* Weapon = GetOwner();
	APawn* Shooter = GetInstigator();
	if (!OtherActor || OtherActor == this || OtherActor == Weapon || OtherActor == Shooter)
	{
		return;
	}

	bConsumed = true;

	if (CollisionSphere)
	{
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}

	const FVector ShotDirection = GetVelocity().GetSafeNormal();
	AActor* DamageCauser = Weapon ? Weapon : this;
	UGameplayStatics::ApplyPointDamage(
		OtherActor,
		Damage,
		ShotDirection,
		Hit,
		GetInstigatorController(),
		DamageCauser,
		UDamageType::StaticClass());

	PlayImpactEffects(Hit);
	Destroy();
}

void ARuptureProjectile::PlayImpactEffects(const FHitResult& Hit) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			ImpactParticle,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation());
	}

	if (DecalMaterial)
	{
		UGameplayStatics::SpawnDecalAtLocation(
			World,
			DecalMaterial,
			FVector(8.f, 8.f, 8.f),
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation(),
			10.f);
	}
}
