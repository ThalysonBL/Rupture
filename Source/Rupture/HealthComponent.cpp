#include "HealthComponent.h"
#include "Math/UnrealMathUtility.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}
void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Guard Clause: Evita subtrair dano negativo ou continuar batendo em mortos.
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Subtrai o dano garantindo que o HP nunca passe de MaxHealth e nunca fique abaixo de 0.
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	// Dispara o evento para atualizar a interface imediatamente.
	OnHealthChanged.Broadcast(CurrentHealth);

	// Valida a condição de morte.
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}