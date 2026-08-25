#include "RuptureEnemyBase.h"

#include "EnemyHealthBarComponent.h"
#include "HealthComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ARuptureEnemyBase::ARuptureEnemyBase()
{
	bUseControllerRotationYaw = false;
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 150.f, 0.f);
	}
	
	HealthBarComponent = CreateDefaultSubobject<UEnemyHealthBarComponent>(TEXT("HealthBarComponent"));	
}

void ARuptureEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Garante arma antes do Behavior Tree (AIC) consultar HasAmmo / FireWeapon.
	EnsureWeaponSpawned();
}

void ARuptureEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ARuptureEnemyBase::HandleDeath);
	}

	// Fallback caso PostInitializeComponents não tenha conseguido spawnar ainda.
	EnsureWeaponSpawned();

	UE_LOG(LogTemp, Warning, TEXT("Enemy[%s]: Controller=%s | Weapon=%s | BT=%s"),
		*GetName(),
		GetController() ? *GetController()->GetName() : TEXT("NONE"),
		CurrentWeapon ? *CurrentWeapon->GetName() : TEXT("NONE"),
		EnemyBehaviorTree ? *EnemyBehaviorTree->GetName() : TEXT("NONE"));
}

void ARuptureEnemyBase::EnsureWeaponSpawned()
{
	if (CurrentWeapon || !StartingWeaponClass || !GetWorld())
	{
		return;
	}

	// Evita spawn no CDO
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ARuptureWeaponBase>(
		StartingWeaponClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams
	);

	if (CurrentWeapon)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("WeaponSocket"));
		UE_LOG(LogTemp, Warning, TEXT("Enemy[%s]: arma spawnada -> %s"), *GetName(), *CurrentWeapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy[%s]: falha ao spawnar arma."), *GetName());
	}
}

void ARuptureEnemyBase::HandleDeath()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);

	DetachFromControllerPendingDestroy();

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
}

void ARuptureEnemyBase::FireWeapon()
{
	EnsureWeaponSpawned();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy[%s]: FireWeapon sem arma."), *GetName());
		return;
	}

	if (!GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy[%s]: FireWeapon sem Controller (IA não possessou)."), *GetName());
		return;
	}

	CurrentWeapon->Fire();
}
