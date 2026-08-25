#include "EnemyHealthBarComponent.h"
#include "HealthComponent.h"
#include "RuptureEnemyHealthWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UEnemyHealthBarComponent::UEnemyHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(DrawSize);
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarWidgetComponent->SetHiddenInGame(true);
	HealthBarWidgetComponent->SetVisibility(false);
}

void UEnemyHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	
	if (!OwnerCharacter || !HealthBarWidgetComponent)
	{
		return;
	}
	
	HealthBarWidgetComponent->SetDrawSize(DrawSize);
	HealthBarWidgetComponent->SetWidgetClass(HealthBarWidgetClass);
	
	if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
	{
		const bool bHasSocket = !AttackSocketName.IsNone() && Mesh->DoesSocketExist(AttackSocketName);
		const FName SocketToUse = bHasSocket ? AttackSocketName : NAME_None;
		
		HealthBarWidgetComponent->AttachToComponent(
			Mesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketToUse
		);
	}
	
	HealthBarWidgetComponent->SetRelativeLocation(WidgetOffset);
	SetBarVisible(false);
	if (UHealthComponent* HealthComp = OwnerCharacter->FindComponentByClass<UHealthComponent>())
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &UEnemyHealthBarComponent::OnOwnerHealthChanged);
		HealthComp->OnDeath.AddDynamic(this, &UEnemyHealthBarComponent::OnOwnerDeath);
	}
	
}

void UEnemyHealthBarComponent::OnOwnerHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (!bHasBeenRevealed)
	{
		bHasBeenRevealed = true;
		SetBarVisible(true);
		if (URuptureEnemyhealthWidget* HealthBar = GetHealthBarWidget())
		{
			HealthBar->ShowBar();
		}
	}
	if (URuptureEnemyhealthWidget* HealthBar = GetHealthBarWidget())
	{
		HealthBar->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void UEnemyHealthBarComponent::OnOwnerDeath()
{
	SetBarVisible(false);
	if (URuptureEnemyhealthWidget* HealthBar = GetHealthBarWidget())
	{
		HealthBar->HideBar();
	}
}

URuptureEnemyhealthWidget* UEnemyHealthBarComponent::GetHealthBarWidget() const
{
	if (!HealthBarWidgetComponent)
	{
		return nullptr;
	}
	return Cast<URuptureEnemyhealthWidget>(HealthBarWidgetComponent->GetWidget());
}
void UEnemyHealthBarComponent::SetBarVisible(bool bVisible)
{
	if (!HealthBarWidgetComponent)
	{
		return;
	}
	HealthBarWidgetComponent->SetHiddenInGame(!bVisible);
	HealthBarWidgetComponent->SetVisibility(bVisible);
}