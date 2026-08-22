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

protected:
	virtual void BeginPlay() override;
	void Die();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
