#include "RuptureEnemyBase.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"

void ARuptureEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// Conecta o evento de morte do componente � fun��o deste inimigo
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ARuptureEnemyBase::HandleDeath);
	}
	
	if (StartingWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		CurrentWeapon = GetWorld()->SpawnActor<ARuptureWeaponBase>(StartingWeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (CurrentWeapon)
		{
			// Regras de Ancoragem: Faça a arma "pular" para a posição e rotação exatas do Socket
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

			// Gruda a arma no esqueleto (Mesh) do personagem, no socket "WeaponSocket"
			CurrentWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("WeaponSocket"));
		}
	}
}

void ARuptureEnemyBase::HandleDeath() const
{
	// Desliga a colis�o do corpo invis�vel (c�psula)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Ativa a gravidade na malha para o boneco desabar no ch�o (Ragdoll)
	GetMesh()->SetSimulatePhysics(true);
}

void ARuptureEnemyBase::FireWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}
