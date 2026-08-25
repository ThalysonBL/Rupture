#include "EnemySpawnPortal.h"
#include "RuptureEnemyBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

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
	if (!EnemyClass || !GetWorld())
	{
		return nullptr;
	}
	
	FActorSpawnParameters  Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	return GetWorld()->SpawnActor<ARuptureEnemyBase>(
		EnemyClass,
		GetActorLocation(),
		GetActorRotation(),
		Params
	);
}
