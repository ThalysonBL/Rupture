// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPortal.generated.h"

class ARuptureEnemyBase;
class USceneComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class RUPTURE_API AEnemySpawnPortal : public AActor
{
	GENERATED_BODY()
public:
	AEnemySpawnPortal();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	ARuptureEnemyBase* SpawnEnemy(TSubclassOf<ARuptureEnemyBase> EnemyClass);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* PortalVfx;
};
