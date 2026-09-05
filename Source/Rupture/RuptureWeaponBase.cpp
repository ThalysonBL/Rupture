#include "RuptureWeaponBase.h"
#include "HealthComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AIController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RuptureProjectile.h"

// Sets default values
ARuptureWeaponBase::ARuptureWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	ProjectileClass = ARuptureProjectile::StaticClass();
	LastFireTime = 0.0f;
}

// Called when the game starts or when spawned
void ARuptureWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	ResetAmmoToFull();
}

void ARuptureWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFire();
	Super::EndPlay(EndPlayReason);
}

void ARuptureWeaponBase::StartFire()
{
	if (CanFire())
	{
		Fire();
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ARuptureWeaponBase::Fire, FireRate, true);
	}
}

void ARuptureWeaponBase::StopFire()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void ARuptureWeaponBase::ResetAmmoToFull()
{
	CurrentAmmo = MaxMagazineAmmo;
	ReserveAmmo = MaxReserveAmmo;
	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}

void ARuptureWeaponBase::Fire()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireRate)
	{
		return;
	}

	if (!CanFire())
	{
		StopFire();
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetController())
	{
		StopFire();
		return;
	}

	// Dono morto (player segurando o gatilho, ou IA) → para o timer de tiro
	if (UHealthComponent* OwnerHealth = OwnerPawn->FindComponentByClass<UHealthComponent>())
	{
		if (OwnerHealth->IsDead())
		{
			StopFire();
			return;
		}
	}

	FVector AimDirection = FVector::ZeroVector;
	if (!TryGetAimDirection(OwnerPawn, AimDirection))
	{
		StopFire();
		return;
	}

	LastFireTime = CurrentTime;
	CurrentAmmo--;
	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);

	if (FireCameraShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PC->ClientStartCameraShake(FireCameraShakeClass);
		}
	}

	OwnerPawn->AddControllerPitchInput(-VerticalRecoil);
	const float RandomYaw = FMath::RandRange(-HorizontalRecoil, HorizontalRecoil);
	OwnerPawn->AddControllerYawInput(RandomYaw);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (MuzzleFlash)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlash,
			WeaponMesh,
			TEXT("MuzzleSocket"),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true);
	}

	SpawnProjectile(OwnerPawn, AimDirection);
}

bool ARuptureWeaponBase::TryGetAimDirection(const APawn* OwnerPawn, FVector& OutAimDirection) const
{
	if (!OwnerPawn || !OwnerPawn->GetController())
	{
		return false;
	}

	FRotator ViewRotation;
	if (OwnerPawn->GetController()->IsPlayerController())
	{
		FVector ViewLocation;
		OwnerPawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else
	{
		const AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
		if (!AIController)
		{
			return false;
		}

		AActor* FocusActor = AIController->GetFocusActor();
		if (!FocusActor)
		{
			return false;
		}

		if (const UHealthComponent* FocusHealth = FocusActor->FindComponentByClass<UHealthComponent>())
		{
			if (FocusHealth->IsDead())
			{
				return false;
			}
		}

		const FVector ViewLocation = OwnerPawn->GetActorLocation() + FVector(0.f, 0.f, 60.f);
		ViewRotation = (FocusActor->GetActorLocation() + FVector(0.f, 0.f, 50.f) - ViewLocation).Rotation();
	}

	const float SpreadInDegrees = OwnerPawn->GetController()->IsPlayerController() ? 0.5f : 4.5f;
	const float SpreadInRadians = FMath::DegreesToRadians(SpreadInDegrees);
	OutAimDirection = FMath::VRandCone(ViewRotation.Vector(), SpreadInRadians);
	return !OutAimDirection.IsNearlyZero();
}

FVector ARuptureWeaponBase::GetMuzzleWorldLocation() const
{
	static const FName MuzzleSocket(TEXT("MuzzleSocket"));
	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocket))
	{
		return WeaponMesh->GetSocketLocation(MuzzleSocket);
	}

	return GetActorLocation();
}

void ARuptureWeaponBase::SpawnProjectile(APawn* OwnerPawn, const FVector& AimDirection)
{
	UWorld* World = GetWorld();
	if (!World || !ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Arma[%s]: ProjectileClass ausente, disparo ignorado."), *GetName());
		return;
	}

	const FVector SpawnLocation = GetMuzzleWorldLocation() + AimDirection * MuzzleSpawnOffset;
	const FRotator SpawnRotation = AimDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARuptureProjectile* Projectile = World->SpawnActor<ARuptureProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Arma[%s]: falha ao spawnar projétil."), *GetName());
		return;
	}

	if (UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		const float Speed = FMath::Max(ProjectileSpeed, 1.f);
		Movement->InitialSpeed = Speed;
		Movement->MaxSpeed = Speed;
		Movement->Velocity = AimDirection * Speed;
	}

	const float SafeSpeed = FMath::Max(ProjectileSpeed, 1.f);
	Projectile->SetLifeSpan(MaxRange / SafeSpeed);
	Projectile->InitializeShot(BaseDamage, ImpactParticle, DecalMaterial);
}

bool ARuptureWeaponBase::CanFire() const
{
	return CurrentAmmo > 0;
}

void ARuptureWeaponBase::Reload()
{
	if (CurrentAmmo < MaxMagazineAmmo && ReserveAmmo > 0)
	{
		int32 AmmoNeeded = MaxMagazineAmmo - CurrentAmmo;
		int32 AmmoToReload = FMath::Min(AmmoNeeded, ReserveAmmo);
		CurrentAmmo += AmmoToReload;
		ReserveAmmo -= AmmoToReload;

		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
		}
	}
}

int32 ARuptureWeaponBase::GetCurrentAmmo() const
{
	return CurrentAmmo; 
}

int32 ARuptureWeaponBase::GetReserveAmmo() const
{
	return ReserveAmmo; 
}
