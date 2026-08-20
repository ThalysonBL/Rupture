#include "RuptureWeaponBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

// Sets default values
ARuptureWeaponBase::ARuptureWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	LastFireTime = 0.0f;
}

// Called when the game starts or when spawned
void ARuptureWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxMagazineAmmo;
	
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxMagazineAmmo);
	}
}

void ARuptureWeaponBase::StartFire()
{
	if (CanFire())
	{
		Fire();
		// 2. Liga o cronômetro para ficar repetindo a função Fire() baseado no seu FireRate
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ARuptureWeaponBase::Fire, FireRate, true);
	}
}

void ARuptureWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ARuptureWeaponBase::Fire()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireRate)
	{
		return; // Ainda não passou o tempo suficiente desde o último disparo
	}

	if (!CanFire())
	{
		StopFire(); // Para de atirar se não houver munição
		return;
	}

	LastFireTime = CurrentTime;

	// Extrai quem é o dono da arma e busca a câmera direto do cérebro (Controller) dele
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetController()) return;

	FVector CameraLocation;
	FRotator CameraRotation;

	OwnerPawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraForward = CameraRotation.Vector();

	FVector EndLocation = CameraLocation + (CameraForward * MaxRange);

	CurrentAmmo--;

	OnAmmoChanged.Broadcast(CurrentAmmo, MaxMagazineAmmo);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	//ignora o proprio dono
	if (GetOwner())
	{
		QueryParams.AddIgnoredActor(GetOwner());
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, QueryParams);

	if (bHit)
	{
		if (ImpactParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}

		if (DecalMaterial)
		{
			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(8.0f, 8.0f, 8.0f), HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), 10.0f);
		}
		AActor* HitActor = HitResult.GetActor();

		if (HitActor)
		{
			UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage, CameraForward, HitResult, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	DrawDebugLine(GetWorld(), CameraLocation, bHit ? HitResult.ImpactPoint : EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);

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
			true
		);
	}
}

bool ARuptureWeaponBase::CanFire() const
{
	return CurrentAmmo > 0;
}

void ARuptureWeaponBase::Reload()
{
	if (CurrentAmmo < MaxMagazineAmmo && MaxReserveAmmo > 0)
	{
		int32 AmmoNeeded = MaxMagazineAmmo - CurrentAmmo;
		int32 AmmoToReload = FMath::Min(AmmoNeeded, MaxReserveAmmo);
		CurrentAmmo += AmmoToReload;
		MaxReserveAmmo -= AmmoToReload;

		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, MaxMagazineAmmo);
		}
	}

}
