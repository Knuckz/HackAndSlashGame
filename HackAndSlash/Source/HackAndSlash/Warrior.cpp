// Fill out your copyright notice in the Description page of Project Settings.

#include "Warrior.h"
#include "Engine.h"
#include "WarriorAnimation.h"
#include "TimerManager.h"



AWarrior::AWarrior()
{
}
void AWarrior::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	WarriorMesh = ACharacter::GetMesh();
	Animation = Cast<UWarriorAnimation>(WarriorMesh->GetAnimInstance());
}

void AWarrior::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (bNeedsToLeap)
	{
		this->ChangeLocation(DesiredLocation, deltaTime);
	}
	//Check if wasd was doubble tapped, dodge if true
	DodgeInput(deltaTime);

	//Keeps track of how long Left click is held. If held down long enough it executes the charged ability
	CheckAttackInputHeld();

	//If certain flags are set - do some physics
	DoPhysics();
}

void AWarrior::DoPhysics()
{
	if (Animation)
	{
		if (Animation->bAirAttackStart)
		{
			LaunchCharacter(FVector(0, 0, 750), false, true);
			Animation->bAirAttackStart = false;
		}

		if (Animation->bGroundPoundStart)
		{
			LaunchCharacter(FVector(0, 0, 800), false, true);
			Animation->bGroundPoundStart = false;
		}

		if (Animation->bGroundPoundAttack)
		{
			LaunchCharacter(FVector(0, 0, -1600), false, true);
			Animation->bGroundPoundAttack = false;
		}

		if (Animation->bSpinningSlashStart)
		{
			LaunchCharacter(FVector(0, 0, 500), false, true);
			Animation->bSpinningSlashStart = false;
		}

		if (Animation->bCanSlam)
		{
			LaunchCharacter(FVector(0, 0, -1600), false, true);
			Animation->bCanSlam = false;
		}
	}
}
void AWarrior::CheckAttackInputHeld()
{
	if (PlayerController)
	{
		if (bLeftClickHeld)
		{
			LeftClickTimer = PlayerController->GetInputKeyTimeDown(EKeys::LeftMouseButton);

			if (LeftClickTimer >= TimeToChargeLightAttack)
			{
				LightAttack();
			}
		}

		//Keeps track of how long right click is held. If held down long enough it executes the charged ability
		if (bRightClickHeld)
		{
			RightClickTimer = PlayerController->GetInputKeyTimeDown(EKeys::RightMouseButton);
			if (RightClickTimer >= TimeToChargeHeavyAttack)
			{
				HeavyAttack();
			}
		}
	}
}

void AWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Leap", IE_Pressed, this, &AWarrior::Leap);
	PlayerInputComponent->BindAction("LightAttack", IE_Released, this, &AWarrior::LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &AWarrior::HeavyAttack);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AWarrior::LightChargedAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AWarrior::HeavyChargedAttack);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AWarrior::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AWarrior::StopBlock);
}

void AWarrior::Leap()
{
		//let tick know we need to leap
		bNeedsToLeap = true;
		//Restart counter
		TotalTime = 0.0f;
		StartingLocation = this->GetActorLocation();
		DesiredLocation = this->GetActorLocation() + (this->GetActorForwardVector() * LeapRange);

		//Checks if we are about to lerp into somthing.
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, this->GetActorLocation(), DesiredLocation, ECollisionChannel::ECC_Visibility))
		{
			DesiredLocation = Hit.Location - FVector(5, 5, 0);
		}
	
}

void AWarrior::LightAttack()
{
	bLeftClickHeld = false;
	if (Animation)
	{
		if (LeftClickTimer >= TimeToChargeLightAttack)
		{
			Animation->UseLightChargedAttack();		
			bLightChargeAttackUsed = true;
		}
		else if (bLightChargeAttackUsed)
		{
			//Since letting go of left click initiates another attack we cancel it
			bLightChargeAttackUsed = false;
			return;
		}
		else
		{
			//Go to our animation instance and start basic combo
			Animation->UseLightAttack();
		}
	}

	//resets timer for button held time
	LeftClickTimer = 0.0f;
}


void AWarrior::HeavyAttack()
{
	bRightClickHeld = false;
	if (Animation)
	{
		if (RightClickTimer >= TimeToChargeHeavyAttack)
		{
			Animation->UseHeavyChargedAttack();
			bHeavyChargeAttackUsed = true;
		}
		else if (bHeavyChargeAttackUsed)
		{
			//Since letting go of left click initiates another attack we cancel it
			bHeavyChargeAttackUsed = false;
			return;
		}
		else
		{
			Animation->UseHeavyAttack();
		}
	}

	//resets timer for button held time
	RightClickTimer = 0.0f;
}

void AWarrior::LightChargedAttack()
{
	bLeftClickHeld = true;
}

void AWarrior::HeavyChargedAttack()
{
	bRightClickHeld = true;
}

void AWarrior::Block()
{
	Animation->UseBlock();
}
void AWarrior::StopBlock()
{
	Animation->StopBlock();
}

void AWarrior::ChangeLocation(const FVector &dLocation, const float &deltaTime)
{
	TotalTime += deltaTime;
	//Get the percentage to completion of the cast time
	//When TotalTime >= 1.0 we are at dLocation and stop
	float LeapAlpha = TotalTime / LeapCastTime;
	//Lerp - Move at constant speed torwards Desired Location
	FVector NewLocation = FMath::Lerp(StartingLocation, dLocation, LeapAlpha);
	this->SetActorRelativeLocation(FVector(NewLocation.X, NewLocation.Y, StartingLocation.Z));

	if (LeapAlpha >= 1.0f)
	{
		bNeedsToLeap = false;
	}
}

void AWarrior::DodgeInput(const float &deltaTime)
{
	if (PlayerController)
	{
		if (bStartDodgeTimer)
		{
			DodgeTimer += deltaTime;
		}
		if (DodgeTimer > .50)
		{
			wKeyPresses = 0;
			aKeyPresses = 0;
			sKeyPresses = 0;
			dKeyPresses = 0;
			
			DodgeTimer = 0.0f;
		}
		
		if (PlayerController->WasInputKeyJustPressed(FKey("w")))
		{
			bStartDodgeTimer = true;
			wKeyPresses++;
			if (wKeyPresses > 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("dash forward"))
				float degreeDiff = CapsuleRotationToCameraRotation();
				if (degreeDiff > -45 && degreeDiff < 45)
				{
					SetCharacterYaw(0.0f);
					Animation->UseDodgeRollForward();
				}
				else if (degreeDiff > -135 && degreeDiff <= -45)
				{
					SetCharacterYaw(-90.0f);
					Animation->UseDodgeRollRight();
				}
				else if (degreeDiff < 135 && degreeDiff >= 45)
				{
					SetCharacterYaw(90.0f);
					Animation->UseDodgeRollLeft();
				}
				else if ((degreeDiff >= 135 && degreeDiff <= 180) || (degreeDiff <= -135 && degreeDiff >= -180))
				{
					SetCharacterYaw(180.0f);
					Animation->UseDodgeRollBack();
				}
				else if (degreeDiff > 180 || degreeDiff < -180)
				{
					UE_LOG(LogTemp, Warning, TEXT("degreeDiff is out of range"))
				}
				ResetKeyPresses();
				DodgeTimer = 0.0f;
			}
		}
		
		if (PlayerController->WasInputKeyJustPressed(FKey("a")))
		{
			bStartDodgeTimer = true;
			aKeyPresses++;
			if (aKeyPresses > 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("dash left"))
				float degreeDiff = CapsuleRotationToCameraRotation();
				if (degreeDiff > -45 && degreeDiff < 45)
				{
					SetCharacterYaw(0.0f);
					Animation->UseDodgeRollLeft();
				}
				else if (degreeDiff > -135 && degreeDiff <= -45)
				{
					SetCharacterYaw(-90.0f);
					Animation->UseDodgeRollForward();
				}
				else if (degreeDiff < 135 && degreeDiff >= 45)
				{
					SetCharacterYaw(90.0f);
					Animation->UseDodgeRollBack();
				}
				else if ((degreeDiff >= 135 && degreeDiff <= 180) || (degreeDiff <= -135 && degreeDiff >= -180))
				{
					SetCharacterYaw(180.0f);
					Animation->UseDodgeRollRight();
				}
				else if (degreeDiff > 180 || degreeDiff < -180)
				{
					UE_LOG(LogTemp, Warning, TEXT("degreeDiff is out of range"))
				}
				ResetKeyPresses();
				DodgeTimer = 0.0f;
			}
		}

		if (PlayerController->WasInputKeyJustPressed(FKey("s")))
		{
			bStartDodgeTimer = true;
			sKeyPresses++;
			if (sKeyPresses > 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("dash back"))
					float degreeDiff = CapsuleRotationToCameraRotation();
				if (degreeDiff > -45 && degreeDiff < 45)
				{
					SetCharacterYaw(0.0f);
					Animation->UseDodgeRollBack();
				}
				else if (degreeDiff > -135 && degreeDiff <= -45)
				{
					SetCharacterYaw(-90.0f);
					Animation->UseDodgeRollLeft();
				}
				else if (degreeDiff < 135 && degreeDiff >= 45)
				{
					SetCharacterYaw(90.0f);
					Animation->UseDodgeRollRight();
				}
				else if ((degreeDiff >= 135 && degreeDiff <= 180) || (degreeDiff <= -135 && degreeDiff >= -180))
				{
					SetCharacterYaw(180.0f);
					Animation->UseDodgeRollForward();
				}
				else if (degreeDiff > 180 || degreeDiff < -180)
				{
					UE_LOG(LogTemp, Warning, TEXT("degreeDiff is out of range"))
				}
				ResetKeyPresses();
				DodgeTimer = 0.0f;
			}
		}

		if (PlayerController->WasInputKeyJustPressed(FKey("d")))
		{
			bStartDodgeTimer = true;
			dKeyPresses++;
			if (dKeyPresses > 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("dash right"))
					float degreeDiff = CapsuleRotationToCameraRotation();
				if (degreeDiff > -45 && degreeDiff < 45)
				{
					SetCharacterYaw(0.0f);
					Animation->UseDodgeRollRight();
				}
				else if (degreeDiff > -135 && degreeDiff <= -45)
				{
					SetCharacterYaw(-90.0f);
					Animation->UseDodgeRollBack();
				}
				else if (degreeDiff < 135 && degreeDiff >= 45)
				{
					SetCharacterYaw(90.0f);
					Animation->UseDodgeRollForward();
				}
				else if ((degreeDiff >= 135 && degreeDiff <= 180) || (degreeDiff <= -135 && degreeDiff >= -180))
				{
					SetCharacterYaw(180.0f);
					Animation->UseDodgeRollLeft();
				}
				else if (degreeDiff > 180 || degreeDiff < -180)
				{
					UE_LOG(LogTemp, Warning, TEXT("degreeDiff is out of range"))
				}
				ResetKeyPresses();
				DodgeTimer = 0.0f;
			}
		}

	}
}


void AWarrior::ResetKeyPresses()
{
	wKeyPresses = 0;
	aKeyPresses = 0;
	sKeyPresses = 0;
	dKeyPresses = 0;
	
	return;
}

float AWarrior::CapsuleRotationToCameraRotation()
{
	FRotator cameraRotator = GetFollowCamera()->GetComponentRotation();
	const FRotator CameraYaw(0, cameraRotator.Yaw, 0);
	FRotator characterRotator = GetCapsuleComponent()->GetComponentRotation();
	const FRotator characterYaw(0, characterRotator.Yaw, 0);

	const FVector Direction = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::X);
	const FVector CharDirection = FRotationMatrix(characterYaw).GetUnitAxis(EAxis::X);
	float angleDiffRad = Direction.CosineAngle2D(CharDirection);
	float angleDiffDegree = FMath::RadiansToDegrees(FMath::Acos(angleDiffRad));
	
	//Use dot product to determine sign of degree difference between camera right vector and character forward vector
	FVector cameraRight = GetFollowCamera()->GetRightVector();
	float dotProd = FVector::DotProduct(cameraRight, CharDirection);
	if (dotProd < 0)
	{
		angleDiffDegree *= -1;
	}

	UE_LOG(LogTemp, Warning, TEXT("degree difference: %f"), angleDiffDegree)
	//GetCapsuleComponent()->SetWorldRotation(FRotator(characterRotator.Pitch, cameraRotator.Yaw, characterRotator.Roll));

	return angleDiffDegree;

}

void AWarrior::SetCharacterYaw(const float YawChange)
{
	FRotator cameraRotator = GetFollowCamera()->GetComponentRotation();
	FRotator characterRotator = GetCapsuleComponent()->GetComponentRotation();
	const FRotator characterYaw(0, characterRotator.Yaw, 0);
	GetCapsuleComponent()->SetWorldRotation(FRotator(characterRotator.Pitch, cameraRotator.Yaw + YawChange, characterRotator.Roll));
}