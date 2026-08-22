#include "RuptureBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"


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
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ARuptureBaseCharacter::Die);
	}
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
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	
	// 3. Separa os destinos: IA x Jogador
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Se for o Player, arranca o teclado/mouse dele, mas mantém a câmera
		DisableInput(PC);
	}
	else if (AController* AIController = GetController())
	{
		// Se for a Inteligência Artificial, ejeta a "mente" do corpo
		// Isso faz a Behavior Tree parar de rodar imediatamente
		AIController->UnPossess();
	}
}