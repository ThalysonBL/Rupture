#include "RuptureWeaponBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "AIController.h"

// Sets default values
ARuptureWeaponBase::ARuptureWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	LastFireTime = 0.0f;
}

// Called when the game starts or when spawned
void ARuptureWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxMagazineAmmo;
	ReserveAmmo = MaxReserveAmmo;

	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
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

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon[%s]: Fire abortado — sem Owner/Controller."), *GetName());
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerPawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// IA: mira no foco (player). Sem isso o GetPlayerViewPoint da AI costuma apontar errado.
	if (!OwnerPawn->GetController()->IsPlayerController())
	{
		if (AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController()))
		{
			if (AActor* FocusActor = AIController->GetFocusActor())
			{
				CameraLocation = OwnerPawn->GetActorLocation() + FVector(0.f, 0.f, 60.f);
				CameraRotation = (FocusActor->GetActorLocation() + FVector(0.f, 0.f, 50.f) - CameraLocation).Rotation();
			}
		}
	}

	FVector CameraForward = CameraRotation.Vector();

	float SpreadInDegrees = 0.0f;
	if (OwnerPawn->GetController()->IsPlayerController())
	{
		SpreadInDegrees = 0.5f;
	}
	else
	{
		SpreadInDegrees = 4.5f;
	}

	// 2. Converte para Radianos (que é o formato que a matemática da Unreal exige)
	float SpreadInRadians = FMath::DegreesToRadians(SpreadInDegrees);

	// 3. Calcula a nova direção aplicando o cone de erro
	FVector FinalShootDirection = FMath::VRandCone(CameraForward, SpreadInRadians);

	// 4. Define o destino final do LineTrace usando a direção com erro
	FVector EndLocation = CameraLocation + (FinalShootDirection * MaxRange);

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
	//variação lateral
	float RandomYaw = FMath::RandRange(-HorizontalRecoil, HorizontalRecoil);
	OwnerPawn->AddControllerYawInput(RandomYaw);

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
