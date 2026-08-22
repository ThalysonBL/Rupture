#pragma once

#include "CoreMinimal.h"
#include "RuptureBaseCharacter.h"
#include "RupturePlayerCharacter.generated.h"

class ARuptureWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquippedSignature, ARuptureWeaponBase*, EquippedWeapon);

UCLASS()
class RUPTURE_API ARupturePlayerCharacter : public ARuptureBaseCharacter
{
	GENERATED_BODY()

	public:
		ARupturePlayerCharacter();
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponEquippedSignature OnWeaponEquipped;

	protected:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UCameraComponent* FollowCamera;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		//Input
		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* InputMappingContext;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* MoveAction;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* LookAction;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* JumpAction;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* FireAction;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ReloadAction;

		void Move(const struct FInputActionValue& Value);
		void Look(const struct FInputActionValue& Value);

		void StartFire(const struct FInputActionValue& Value);
		void StopFire(const struct FInputActionValue& Value);
		void Reload(const struct FInputActionValue& Value);

		UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ARuptureWeaponBase> StartingWeaponClass;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		ARuptureWeaponBase* CurrentWeapon;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float AimPitch;

		UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UUserWidget> AmmoHUDClass;

		UPROPERTY()
		class UUserWidget* AmmoHUDInstance;
};
