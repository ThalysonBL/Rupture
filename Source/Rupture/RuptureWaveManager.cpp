#include "RuptureWaveManager.h"
#include "EnemySpawnPortal.h"
#include "RuptureEnemyBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ARuptureWaveManager::ARuptureWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
	Rounds.SetNum(TotalRounds);
	// Defaults — altera no editor
	Rounds[0] = { 2, 0, 3.f };
	Rounds[1] = { 3, 0, 3.f };
	Rounds[2] = { 3, 1, 3.f };
	Rounds[3] = { 4, 1, 3.f };
	Rounds[4] = { 4, 2, 3.f };
	Rounds[5] = { 5, 2, 3.f };
	Rounds[6] = { 5, 3, 3.f };
	Rounds[7] = { 6, 3, 3.f };
	Rounds[8] = { 6, 4, 3.f };
	Rounds[9] = { 7, 4, 3.f };
}

void ARuptureWaveManager::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("WaveManager: BeginPlay iniciado."));

	CollectPortals();

	UE_LOG(LogTemp, Warning, TEXT("WaveManager: portais encontrados = %d"), Portals.Num());
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: Rounds.Num() = %d | StartDelay = %.2f"), Rounds.Num(), StartDelay);
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: RiflemanClass = %s"),
		RiflemanClass ? *RiflemanClass->GetName() : TEXT("NONE"));
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: BrawlerClass = %s"),
		BrawlerClass ? *BrawlerClass->GetName() : TEXT("NONE"));

	if (Rounds.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Round[0] Rifleman=%d Brawler=%d Delay=%.2f"),
			Rounds[0].RiflemanCount,
			Rounds[0].BrawlerCount,
			Rounds[0].DelayAfterClear);
	}

	if (Portals.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: nenhum portal no nível. Abortando."));
		return;
	}

	if (!RiflemanClass && !BrawlerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: sete RiflemanClass e/ou BrawlerClass no Details. Abortando."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("WaveManager: agendando primeiro round em %.2f segundos."), StartDelay);

	GetWorldTimerManager().SetTimer(
		StartRoundTimerHandle,
		this,
		&ARuptureWaveManager::StartFirstRound,
		StartDelay,
		false
	);
}

void ARuptureWaveManager::CollectPortals()
{
	Portals.Reset();
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPortal::StaticClass(), Found);

	for (AActor* Actor : Found)
	{
		if (AEnemySpawnPortal* Portal = Cast<AEnemySpawnPortal>(Actor))
		{
			Portals.Add(Portal);
			UE_LOG(LogTemp, Log, TEXT("WaveManager: portal coletado -> %s em %s"),
				*Portal->GetName(),
				*Portal->GetActorLocation().ToString());
		}
	}
}

void ARuptureWaveManager::StartFirstRound()
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: StartFirstRound chamado."));
	StartRound(0);
}

void ARuptureWaveManager::StartRound(int32 RoundIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: StartRound(%d)"), RoundIndex);

	if (RoundIndex < 0 || RoundIndex >= TotalRounds || RoundIndex >= Rounds.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: índice inválido ou fim dos rounds. RoundIndex=%d Rounds.Num()=%d"),
			RoundIndex, Rounds.Num());
		OnAllRoundsCompleted.Broadcast();
		return;
	}

	CurrentRoundIndex = RoundIndex;
	AliveEnemies = 0;
	NextPortalIndex = 0;

	const FRoundDefinition& RoundDef = Rounds[CurrentRoundIndex];
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: iniciando Round %d | Rifleman=%d Brawler=%d"),
		CurrentRoundIndex + 1,
		RoundDef.RiflemanCount,
		RoundDef.BrawlerCount);

	OnRoundStarted.Broadcast(CurrentRoundIndex + 1);

	SpawnRoundEnemies(RoundDef);

	UE_LOG(LogTemp, Warning, TEXT("WaveManager: spawn concluído. AliveEnemies = %d"), AliveEnemies);

	if (AliveEnemies <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Round %d sem inimigos vivos após spawn."), CurrentRoundIndex + 1);
		HandleRoundCleared();
	}
}

AEnemySpawnPortal* ARuptureWaveManager::GetNextPortal()
{
	if (Portals.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: GetNextPortal sem portais."));
		return nullptr;
	}

	AEnemySpawnPortal* Portal = Portals[NextPortalIndex % Portals.Num()];
	NextPortalIndex++;
	return Portal;
}

void ARuptureWaveManager::SpawnRoundEnemies(const FRoundDefinition& RoundDef)
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: SpawnRoundEnemies RiflemanCount=%d BrawlerCount=%d"),
		RoundDef.RiflemanCount, RoundDef.BrawlerCount);

	for (int32 i = 0; i < RoundDef.RiflemanCount; ++i)
	{
		if (!RiflemanClass)
		{
			UE_LOG(LogTemp, Error, TEXT("WaveManager: tentou spawnar Rifleman mas RiflemanClass está NONE."));
			break;
		}

		if (AEnemySpawnPortal* Portal = GetNextPortal())
		{
			UE_LOG(LogTemp, Warning, TEXT("WaveManager: spawnando Rifleman %d/%d no portal %s"),
				i + 1, RoundDef.RiflemanCount, *Portal->GetName());

			if (ARuptureEnemyBase* Enemy = Portal->SpawnEnemy(RiflemanClass))
			{
				RegisterEnemy(Enemy);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("WaveManager: falha ao spawnar Rifleman no portal %s"), *Portal->GetName());
			}
		}
	}

	for (int32 i = 0; i < RoundDef.BrawlerCount; ++i)
	{
		if (!BrawlerClass)
		{
			UE_LOG(LogTemp, Error, TEXT("WaveManager: tentou spawnar Brawler mas BrawlerClass está NONE."));
			break;
		}

		if (AEnemySpawnPortal* Portal = GetNextPortal())
		{
			UE_LOG(LogTemp, Warning, TEXT("WaveManager: spawnando Brawler %d/%d no portal %s"),
				i + 1, RoundDef.BrawlerCount, *Portal->GetName());

			if (ARuptureEnemyBase* Enemy = Portal->SpawnEnemy(BrawlerClass))
			{
				RegisterEnemy(Enemy);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("WaveManager: falha ao spawnar Brawler no portal %s"), *Portal->GetName());
			}
		}
	}
}

void ARuptureWaveManager::RegisterEnemy(ARuptureEnemyBase* Enemy)
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: RegisterEnemy recebeu nullptr."));
		return;
	}

	UHealthComponent* HealthComp = Enemy->FindComponentByClass<UHealthComponent>();
	if (HealthComp)
	{
		HealthComp->OnDeath.AddDynamic(this, &ARuptureWaveManager::OnEnemyDied);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: inimigo %s sem HealthComponent."), *Enemy->GetName());
	}

	AliveEnemies++;

	ActiveEnemies.Add(Enemy);
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: inimigo registrado %s | AliveEnemies=%d"),
		*Enemy->GetName(), AliveEnemies);
}

void ARuptureWaveManager::OnEnemyDied()
{
	AliveEnemies = FMath::Max(0, AliveEnemies - 1);
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: inimigo morreu | AliveEnemies=%d"), AliveEnemies);

	if (AliveEnemies <= 0)
	{
		HandleRoundCleared();
	}
}

void ARuptureWaveManager::HandleRoundCleared()
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: Round %d limpo."), CurrentRoundIndex + 1);
	OnRoundCleared.Broadcast(CurrentRoundIndex + 1);

	if (CurrentRoundIndex >= TotalRounds - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: todos os rounds concluídos."));
		OnAllRoundsCompleted.Broadcast();
		return;
	}

	const float Delay = Rounds[CurrentRoundIndex].DelayAfterClear;
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: próximo round em %.2f segundos."), Delay);

	GetWorldTimerManager().SetTimer(
		NextRoundTimerHandle,
		this,
		&ARuptureWaveManager::StartNextRound,
		Delay,
		false
	);
}

void ARuptureWaveManager::StartNextRound()
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: StartNextRound -> %d"), CurrentRoundIndex + 1);
	StartRound(CurrentRoundIndex + 1);
}



void ARuptureWaveManager::CancelRoundTimers()
{
	GetWorldTimerManager().ClearTimer(StartRoundTimerHandle);
	GetWorldTimerManager().ClearTimer(NextRoundTimerHandle);
}

void ARuptureWaveManager::ClearActiveEnemies()
{
	for (ARuptureEnemyBase* Enemy : ActiveEnemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
		}
	}
	ActiveEnemies.Reset();
	AliveEnemies = 0;

	UE_LOG(LogTemp, Warning, TEXT("WaveManager: inimigos limpos."));

}

void ARuptureWaveManager::RestartCurrentRound()
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: RestartCurrentRound (%d)"), CurrentRoundIndex + 1);
	ClearActiveEnemies();
	StartRound(CurrentRoundIndex);
}
void ARuptureWaveManager::RestartAllRounds()
{
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: RestartAllRounds"));
	ClearActiveEnemies();
	StartRound(0);
}