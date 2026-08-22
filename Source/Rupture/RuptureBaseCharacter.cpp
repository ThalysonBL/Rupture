#include "RuptureBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"


ARuptureBaseCharacter::ARuptureBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->GroundFriction = 8.0;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ARuptureBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARuptureBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARuptureBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARuptureBaseCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("O personagem morreu!"));
}