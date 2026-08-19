#include "RuptureTargetDummy.h"

// Sets default values
ARuptureTargetDummy::ARuptureTargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;
	DummyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMesh"));
	DummyMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ARuptureTargetDummy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	
}

float ARuptureTargetDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Alvo atingido! Dano: %f | Vida restante: %f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
	return DamageAmount;
}