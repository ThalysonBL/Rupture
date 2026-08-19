#include "RuptureBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


ARuptureBaseCharacter::ARuptureBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->GroundFriction = 8.0;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

}

void ARuptureBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;	
}

void ARuptureBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARuptureBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ARuptureBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Max(DamageToApply, 0.f);
	CurrentHealth -= DamageToApply;
	if (CurrentHealth <= 0.f)
	{
		Die();
	}
	return DamageToApply;
}

void ARuptureBaseCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("O personagem morreu!"));
}