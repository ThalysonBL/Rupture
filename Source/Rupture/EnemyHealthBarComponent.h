// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyHealthBarComponent.generated.h"

class UWidgetComponent;
class UHealthComponent;
class URuptureEnemyhealthWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUPTURE_API UEnemyHealthBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyHealthBarComponent();
	virtual void BeginPlay() override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthBarWidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<URuptureEnemyhealthWidget> HealthBarWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName AttackSocketName = TEXT("head");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector WidgetOffset = FVector(0.f, 0.f, 40.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D DrawSize = FVector2D(120.f, 16.f);
	
	UFUNCTION()
	void OnOwnerHealthChanged(float CurrentHealth, float MaxHealth);
	
	UFUNCTION()
	void OnOwnerDeath();

private:
	bool bHasBeenRevealed  = false;
	
	URuptureEnemyhealthWidget* GetHealthBarWidget() const;
	
	void SetBarVisible(bool bVisible);
};
