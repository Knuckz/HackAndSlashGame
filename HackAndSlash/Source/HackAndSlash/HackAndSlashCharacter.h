// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HackAndSlashCharacter.generated.h"

UCLASS(config=Game)
class AHackAndSlashCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AHackAndSlashCharacter();

	void Tick(float deltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** The Rate at which the camera moves when zooming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float ZoomRate = 50.0f;

	/** The Starting distance at which the camera is positioned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float StartingCameraDistance = 300.0f;

	/** The maximum distance the camera can be from its character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float MaxCameraDistance = 900.0f;

	/** The min distance the camera can be from the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float MinCameraDistance = 0.0f;

	/** The speed at which the camera changes position */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float InterpSpeed = 2.0f;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void ZoomOut();
	void ZoomIn();
	void PositionCamera();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	// Default camera distance;
	float DesiredCameraDistance = StartingCameraDistance;
};