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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
	virtual void Die();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

};
