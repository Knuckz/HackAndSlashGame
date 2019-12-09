// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HackAndSlashCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AHackAndSlashCharacter

AHackAndSlashCharacter::AHackAndSlashCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Sets up the camera
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = StartingCameraDistance; // The camera is following at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

												   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
												   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
void AHackAndSlashCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	this->PositionCamera();
}

void AHackAndSlashCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	//camera zoom
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &AHackAndSlashCharacter::ZoomOut);
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AHackAndSlashCharacter::ZoomIn);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHackAndSlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHackAndSlashCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHackAndSlashCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHackAndSlashCharacter::LookUpAtRate);
}

void AHackAndSlashCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHackAndSlashCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHackAndSlashCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHackAndSlashCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AHackAndSlashCharacter::ZoomOut()
{
	if (Controller != NULL)
	{
		DesiredCameraDistance += ZoomRate;
	}
}

void AHackAndSlashCharacter::ZoomIn()
{
	if (Controller != NULL)
	{
		DesiredCameraDistance -= ZoomRate;
	}
}

void AHackAndSlashCharacter::PositionCamera()
{
	//If Controller is okay, and desiredCameraDistance is met skip (condition fails before equal check if controller is missing
	if (Controller != NULL && (!FMath::IsNearlyEqual(this->CameraBoom->TargetArmLength, DesiredCameraDistance)))
	{
		float DeltaTime = this->GetWorld()->GetDeltaSeconds();

		//Smoothly repositions camera as long as desired position is in between min and max distance
		this->CameraBoom->TargetArmLength = FMath::FInterpTo(this->CameraBoom->TargetArmLength, FMath::Clamp(DesiredCameraDistance, MinCameraDistance, MaxCameraDistance), DeltaTime, InterpSpeed);
	}
}
