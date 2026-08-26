#include "RupturePlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HealthComponent.h"
#include "InputActionValue.h"
#include "RuptureWeaponBase.h"


ARupturePlayerCharacter::ARupturePlayerCharacter() {
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationYaw = true;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = false;

  // Define a velocidade dessa rotação (Pitch, Yaw, Roll). 500 no Yaw dá um giro
  // bem fluido.
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
  SpringArm->SetupAttachment(RootComponent);
  SpringArm->TargetArmLength = 300.0f;

  SpringArm->SocketOffset = FVector(0.0f, 70.0f, 55.0f);

  // braços devem mexer junto com a camera
  SpringArm->bUsePawnControlRotation = true;

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;

  GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
  GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
  GetCharacterMovement()->SetCrouchedHalfHeight(48.f);
}

void ARupturePlayerCharacter::BeginPlay() {
  Super::BeginPlay();

  if (StartingWeaponClass) {
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    CurrentWeapon = GetWorld()->SpawnActor<ARuptureWeaponBase>(
        StartingWeaponClass, GetActorLocation(), GetActorRotation(),
        SpawnParams);
    if (CurrentWeapon) {
      // Regras de Ancoragem: Faça a arma "pular" para a posição e rotação
      // exatas do Socket
      FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget,
                                                true);

      // Gruda a arma no esqueleto (Mesh) do personagem, no socket
      // "WeaponSocket"
      CurrentWeapon->AttachToComponent(GetMesh(), AttachmentRules,
                                       FName("WeaponSocket"));
      OnWeaponEquipped.Broadcast(CurrentWeapon);
    }
  }

  if (APlayerController *PlayerController =
          Cast<APlayerController>(Controller)) {
    // Após vir do menu (UIOnly), força input de jogo de novo
    PlayerController->bShowMouseCursor = false;
    PlayerController->SetIgnoreLookInput(false);
    PlayerController->SetIgnoreMoveInput(false);
    PlayerController->EnableInput(PlayerController);
    PlayerController->SetInputMode(FInputModeGameOnly());

    if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer())) {
      if (InputMappingContext) {
        Subsystem->AddMappingContext(InputMappingContext, 0);
      } else {
        UE_LOG(LogTemp, Error,
               TEXT("Player: InputMappingContext está None no BP."));
      }
    }

    UE_LOG(LogTemp, Warning, TEXT("Player: input mode GameOnly aplicado."));
  }
}

void ARupturePlayerCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  // Atualiza o AimPitch para a animação
  AimPitch = GetBaseAimRotation().Pitch;
}

void ARupturePlayerCharacter::SetupPlayerInputComponent(
    class UInputComponent *PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);
  if (UEnhancedInputComponent *EnhancedInputComponent =
          CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
                                       this, &ACharacter::Jump);
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
                                       this, &ACharacter::StopJumping);

    // Mover e Olhar (Triggered = enquanto estiver segurando/mexendo)
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
                                       this, &ARupturePlayerCharacter::Move);
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
                                       this, &ARupturePlayerCharacter::Look);
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered,
                                       this,
                                       &ARupturePlayerCharacter::StartFire);
    EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed,
                                       this,
                                       &ARupturePlayerCharacter::StopFire);
    EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started,
                                       this, &ARupturePlayerCharacter::Reload);
    EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started,
                                       this, &ARupturePlayerCharacter::StartCrouch);
    EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed,
                                       this, &ARupturePlayerCharacter::StopCrouch);
  }
}

void ARupturePlayerCharacter::Move(const struct FInputActionValue &Value) {
  FVector2D MovementVector = Value.Get<FVector2D>();
  if (Controller != nullptr) {
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // calcula o vetor que aponta para frente
    const FVector ForwardDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // empurra o personagem para frente ou para trás
    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
  }
}

void ARupturePlayerCharacter::Look(const struct FInputActionValue &Value) {
  FVector2D LookAxisVector = Value.Get<FVector2D>();
  if (Controller != nullptr) {
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
  }
}

void ARupturePlayerCharacter::StartFire(const struct FInputActionValue &Value) {
  if (HealthComponent && HealthComponent->IsDead()) {
    return;
  }

  if (CurrentWeapon) {
    CurrentWeapon->StartFire();
  }
}

void ARupturePlayerCharacter::StopFire(const struct FInputActionValue &Value) {
  if (CurrentWeapon) {
    CurrentWeapon->StopFire();
  }
}

void ARupturePlayerCharacter::Reload(const struct FInputActionValue &Value) {
  if (CurrentWeapon) {
    CurrentWeapon->Reload();
  }
}

void ARupturePlayerCharacter::Die() {
  // Para o tiro automático mesmo com o gatilho ainda pressionado
  if (CurrentWeapon) {
    CurrentWeapon->StopFire();
  }

  Super::Die();
}

void ARupturePlayerCharacter::RefillWeaponAmmo() {
  if (CurrentWeapon) {
    CurrentWeapon->ResetAmmoToFull();
  }
}

void ARupturePlayerCharacter::Revive() {
  if (CurrentWeapon) {
    CurrentWeapon->StopFire();
  }

  if (HealthComponent) {
    HealthComponent->ResetHealth();
  }

  RefillWeaponAmmo();

  GetCapsuleComponent()->SetCollisionEnabled(
      ECollisionEnabled::QueryAndPhysics);

  GetMesh()->SetSimulatePhysics(false);
  GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
  GetMesh()->AttachToComponent(
      GetCapsuleComponent(),
      FAttachmentTransformRules::SnapToTargetNotIncludingScale);
  GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
  GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

  if (UCharacterMovementComponent *MoveComp = GetCharacterMovement()) {
    MoveComp->SetMovementMode(MOVE_Walking);
  }

  if (APlayerController *PC = Cast<APlayerController>(GetController())) {
    EnableInput(PC);
  }

  UE_LOG(LogTemp, Warning, TEXT("Player: Revive() concluído."));
}


void ARupturePlayerCharacter::StartCrouch(const FInputActionValue& Value)
{
	if (HealthComponent && HealthComponent->IsDead())
	{
		return;
	}
	Crouch();
}

void ARupturePlayerCharacter::StopCrouch(const FInputActionValue& Value)
{
	UnCrouch();
}