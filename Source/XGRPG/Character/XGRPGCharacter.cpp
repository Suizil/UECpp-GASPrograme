// Copyright Epic Games, Inc. All Rights Reserved.

#include "XGRPGCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "XGRPGGameplayTag.h"
#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGComboComponent.h"

#include "XGRPGPlayerController.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AXGRPGCharacter

AXGRPGCharacter::AXGRPGCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AXGRPGCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AXGRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AXGRPGCharacter::ActiveJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AXGRPGCharacter::UnActiveJump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AXGRPGCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AXGRPGCharacter::Look);

		// Meleeing
		EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Started, this, &AXGRPGCharacter::ActiveMelee);

		// AirAttack
		EnhancedInputComponent->BindAction(AirAttackAction, ETriggerEvent::Started, this, &AXGRPGCharacter::AirAttack);

		// 1
		EnhancedInputComponent->BindAction(KeyAction_1, ETriggerEvent::Started, this, &AXGRPGCharacter::Key_1);

		// 2
		EnhancedInputComponent->BindAction(KeyAction_2, ETriggerEvent::Started, this, &AXGRPGCharacter::Key_2);

		// 3
		EnhancedInputComponent->BindAction(KeyAction_3, ETriggerEvent::Started, this, &AXGRPGCharacter::Key_3);

		// 4
		EnhancedInputComponent->BindAction(KeyAction_4, ETriggerEvent::Started, this, &AXGRPGCharacter::Key_4);

		// Tab
		EnhancedInputComponent->BindAction(KeyAction_Tab, ETriggerEvent::Started, this, &AXGRPGCharacter::Key_Tab);


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AXGRPGCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AXGRPGCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AXGRPGCharacter::ActiveJump()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Jump"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);

}


void AXGRPGCharacter::UnActiveJump()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Jump"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagReleased(InputTag);

}

void AXGRPGCharacter::ActiveMelee()
{
	GetXGRPGComboComponent()->SetPressed();

	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Melee"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::AirAttack()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.AirAttack"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::Key_1()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Key.1"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::Key_2()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Key.2"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::Key_3()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Key.3"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::Key_4()
{
	FGameplayTag InputTag = XGRPGGameplayTags::FindTagByString(TEXT("InputTag.Key.4"), true);

	GetXGRPGAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AXGRPGCharacter::Key_Tab()
{
	  bool bShowMouse= GetXGRPGPlayerController()->bShowMouseCursor;

	  GetXGRPGPlayerController()->SetShowMouseCursor(!bShowMouse);

}

void AXGRPGCharacter::ComboMelee()
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		GetAbilitySystemComponent()->StopMontageIfCurrent(*GetXGRPGComboComponent()->GetLastPlayAnimMontage());

		ActiveMelee();

	}
}
