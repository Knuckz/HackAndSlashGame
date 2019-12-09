// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "HackAndSlashCharacter.h"
#include "Warrior.generated.h"


class UWarriorAnimation;

UCLASS()
class HACKANDSLASH_API AWarrior : public AHackAndSlashCharacter
{
	GENERATED_BODY()
	
public:
	AWarrior();

	//Variables that effect Leap 
	/////////////////////////////////////////////////////
	float TotalTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, category = "Leap")
	float LeapCastTime = .25f;

	UPROPERTY(EditDefaultsOnly, category = "Leap")
	float LeapRange = 1000.0f;

	//Variables that effect LightAttack
	/////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, category = "LightAttack")
	float TimeToChargeLightAttack = 0.25;

	//Variables that effect HeavyAttack
	/////////////////////////////////////////////////////
	UPROPERTY(EditDefaultsOnly, category = "HeavyAttack")
	float TimeToChargeHeavyAttack = 0.50;

private:
	void BeginPlay() override;

	void Tick(float deltaTime) override;

	void CheckAttackInputHeld();

	void DoPhysics();

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	USkeletalMeshComponent * WarriorMesh;

	UPROPERTY()
	UWarriorAnimation * Animation;

	UPROPERTY()
	APlayerController * PlayerController;

	//Leap
	void Leap();
	void ChangeLocation(const FVector &DesiredLocation, const float &deltaTime);
	bool bNeedsToLeap = false;
	FVector StartingLocation;
	FVector DesiredLocation;

	//light attacks
	void LightAttack();
	void LightChargedAttack();
	bool bLeftClickHeld = false;
	bool bLightChargeAttackUsed = false;
	float LeftClickTimer = 0.0f;

	//heavy attacks
	void HeavyAttack();
	void HeavyChargedAttack();
	bool bRightClickHeld = false;
	bool bHeavyChargeAttackUsed = false;
	float RightClickTimer = 0.0f;

	//blocks
	void Block();
	void StopBlock();

	//dodges
	void DodgeInput(const float &deltaTime);
	float CapsuleRotationToCameraRotation();
	void SetCharacterYaw(const float YawChange);
	/*Resets the amount of keypresses recorded in DodgeInput*/
	void ResetKeyPresses();

	uint16 wKeyPresses = 0;
	uint16 aKeyPresses = 0;
	uint16 sKeyPresses = 0;
	uint16 dKeyPresses = 0;
	bool bStartDodgeTimer = false;
	float DodgeTimer = 0.0f;
};

