#include "EnemySpawnPortal.h"
#include "RuptureEnemyBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"

AEnemySpawnPortal::AEnemySpawnPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootScene);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PortalVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalVfx"));
	PortalVfx->SetupAttachment(RootScene);
	PortalVfx->bAutoActivate = false;
}

ARuptureEnemyBase* AEnemySpawnPortal::SpawnEnemy(TSubclassOf<ARuptureEnemyBase> EnemyClass)
{
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Portal[%s]: SpawnEnemy falhou — EnemyClass é NONE."), *GetName());
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Portal[%s]: SpawnEnemy falhou — GetWorld() é null."), *GetName());
		return nullptr;
	}

	// Character spawna pelo centro da cápsula. Se o portal está no chão (Z do pé),
	// precisamos subir pela half-height para não enterrar o mesh.
	float CapsuleHalfHeight = 88.0f;
	if (const ARuptureEnemyBase* CDO = EnemyClass->GetDefaultObject<ARuptureEnemyBase>())
	{
		if (const UCapsuleComponent* Capsule = CDO->GetCapsuleComponent())
		{
			CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		}
	}

	const FVector PortalLocation = GetActorLocation();
	FVector SpawnLocation = PortalLocation;
	SpawnLocation.Z += CapsuleHalfHeight;

	// Raycast para achar o chão real abaixo/acima do portal
	const FVector TraceStart = PortalLocation + FVector(0.f, 0.f, 200.f);
	const FVector TraceEnd = PortalLocation - FVector(0.f, 0.f, 500.f);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PortalSpawnFloor), false, this);
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		SpawnLocation = Hit.ImpactPoint;
		SpawnLocation.Z += CapsuleHalfHeight;
		UE_LOG(LogTemp, Warning, TEXT("Portal[%s]: chão encontrado em %s | halfHeight=%.1f"),
			*GetName(), *Hit.ImpactPoint.ToString(), CapsuleHalfHeight);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal[%s]: chão NÃO encontrado. Usando portal + halfHeight. Loc=%s"),
			*GetName(), *SpawnLocation.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("Portal[%s]: spawnando %s em %s"),
		*GetName(),
		*EnemyClass->GetName(),
		*SpawnLocation.ToString());

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ARuptureEnemyBase* SpawnedEnemy = World->SpawnActor<ARuptureEnemyBase>(
		EnemyClass,
		SpawnLocation,
		GetActorRotation(),
		Params
	);

	if (SpawnedEnemy)
	{
		// Garante AIController em inimigos spawnados em runtime
		if (!SpawnedEnemy->GetController())
		{
			SpawnedEnemy->SpawnDefaultController();
			UE_LOG(LogTemp, Warning, TEXT("Portal[%s]: SpawnDefaultController em %s -> %s"),
				*GetName(),
				*SpawnedEnemy->GetName(),
				SpawnedEnemy->GetController() ? *SpawnedEnemy->GetController()->GetName() : TEXT("AINDA NONE"));
		}

		UE_LOG(LogTemp, Warning, TEXT("Portal[%s]: spawn OK -> %s | pos=%s | Controller=%s"),
			*GetName(),
			*SpawnedEnemy->GetName(),
			*SpawnedEnemy->GetActorLocation().ToString(),
			SpawnedEnemy->GetController() ? *SpawnedEnemy->GetController()->GetName() : TEXT("NONE"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Portal[%s]: SpawnActor retornou nullptr para %s"),
			*GetName(), *EnemyClass->GetName());
	}

	return SpawnedEnemy;
}
