#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RuptureBaseCharacter.generated.h"


UCLASS()
class RUPTURE_API ARuptureBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARuptureBaseCharacter();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float CurrentHealth;

	void Die();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
