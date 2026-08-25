// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoundTypes.h"
#include "RuptureWaveManager.generated.h"

class AEnemySpawnPortal;
class ARuptureEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStartedSignature, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundClearedSignature, int32, RoundNumber);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllRoundsCompletedSignature);

UCLASS()
class RUPTURE_API ARuptureWaveManager : public AActor
{
	GENERATED_BODY()

public:
	ARuptureWaveManager();

	UPROPERTY(BlueprintAssignable, Category = "Rounds|Events")
	FOnRoundStartedSignature OnRoundStarted;

	UPROPERTY(BlueprintAssignable, Category = "Rounds|Events")
	FOnRoundClearedSignature OnRoundCleared;

	UPROPERTY(BlueprintAssignable, Category = "Rounds|Events")
	FOnAllRoundsCompletedSignature OnAllRoundsCompleted;
	
	UFUNCTION(BlueprintCallable, Category = "Rounds")
	void RestartCurrentRound();

	UFUNCTION(BlueprintCallable, Category = "Rounds")
	void RestartAllRounds();

	UFUNCTION(BlueprintPure, Category = "Rounds")
	int32 GetCurrentRoundNumber() const { return CurrentRoundIndex + 1; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rounds")
	TArray<FRoundDefinition> Rounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rounds")
	TSubclassOf<ARuptureEnemyBase> RiflemanClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rounds")
	TSubclassOf<ARuptureEnemyBase> BrawlerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rounds")
	float StartDelay = 2.0f;
	
	static constexpr int32 TotalRounds = 10;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rounds")
	int32 CurrentRoundIndex = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rounds")
	int32 AliveEnemies = 0;
	
	UPROPERTY()
	TArray<AEnemySpawnPortal*> Portals;
	
	FTimerHandle StartRoundTimerHandle;
	
	FTimerHandle NextRoundTimerHandle;
	
	int32 NextPortalIndex = 0;
	
	void CollectPortals();
	
	void StartRound(int32 RoundIndex);
	
	void SpawnRoundEnemies(const FRoundDefinition& RoundDef);
	
	void RegisterEnemy(ARuptureEnemyBase* Enemy);
	
	void HandleRoundCleared();
	
	AEnemySpawnPortal* GetNextPortal();
	
	UFUNCTION()
	void OnEnemyDied();

	UFUNCTION()
	void OnPlayerDied();
	
	UFUNCTION()
	void StartFirstRound();
	
	UFUNCTION()
	void StartNextRound();
	
	UPROPERTY()
	TArray<TObjectPtr<ARuptureEnemyBase>> ActiveEnemies;
	
	void ClearActiveEnemies();
	void CancelRoundTimers();
	void StopAllEnemyFire();
	void CollectOrphanWeapons();
	void RefillPlayerAmmo();
	void BindPlayerDeath();
};
