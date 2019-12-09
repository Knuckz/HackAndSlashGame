// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorAnimation.generated.h"

/*
*
*/
//enum for combo state
UENUM()
enum class EComboState : uint8
{
	NOT_ATTACKING,
	LIGHT_ONE_TWO,
	LIGHT_STAB,
	LIGHT_SPINNING_SWIPE,
	LIGHT_CHARGE,
	LIGHT_AIR_COMBO_BEGIN,
	HEAVY_ONE_TWO,
	HEAVY_UPPERCUT,
	HEAVY_JUMPING_GROUND_POUND,
	HEAVY_SPINNING_SLASH,
	HEAVY_WHIRLWIND,
	COMBO_END,
	DODGE_ROLL_FORWARD,
	DODGE_ROLL_LEFT,
	DODGE_ROLL_BACK,
	DODGE_ROLL_RIGHT,
	BLOCKING
};

UENUM()
enum class EDodgeState : uint8
{
	NOT_DODGING,
	DODGE_ROLL_FORWARD,
	DODGE_ROLL_LEFT,
	DODGE_ROLL_BACK,
	DODGE_ROLL_RIGHT
};

UENUM()
enum class EBlockState : uint8
{
	NOT_BLOCKING,
	BLOCKING
};

UCLASS()
class HACKANDSLASH_API UWarriorAnimation : public UAnimInstance
{
	GENERATED_BODY()
public:
	//Attacks
	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseLightAttack();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseLightChargedAttack();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseHeavyChargedAttack();

	//Dodge Rolls
	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseDodgeRollForward();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseDodgeRollLeft();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseDodgeRollBack();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseDodgeRollRight();

	//Block
	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UseBlock();

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void StopBlock();

private:
	UWarriorAnimation(const FObjectInitializer& ObjectInitializer);

	//Tick function
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	//updates some simple properties
	void UpdateAnimationProperties(float DeltaTime);

	//Called in the Tick function - This function keeps track of several cooldowns
	void AbilityTimers(float DeltaSeconds);

	//Keeps track of montage times and plays the animation
	void ManageComboMontages(UAnimMontage * Montage);

	//Keeps track of montage times and plays the animation
	void ManageDodgeMontages(UAnimMontage * Montage);

public:
	/*True means that we're currently in air - or falling*/
	UPROPERTY(BlueprintReadWrite, Category = "Anim Prop")
	bool bIsFalling;

	/*Holds the current speed of our character*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim Prop")
	float MovementSpeed;

	UPROPERTY(BlueprintReadWrite)
	bool bAirAttackStart = false;

	UPROPERTY(BlueprintReadWrite)
	bool bCanSlam = false;

	UPROPERTY(BlueprintReadWrite)
	bool bGroundPoundStart = false;

	UPROPERTY(BlueprintReadWrite)
	bool bGroundPoundAttack = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSpinningSlashStart = false;

protected:
	//Animation Montages
	////////////////////////////////////////////////////////////////////////////
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Light Attack Anims")
	UAnimMontage * LightOneTwoMontage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Light Attack Anims")
	UAnimMontage * LightStabMontage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Light Attack Anims")
	UAnimMontage * LightSpinningSwipe;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Light Attack Anims")
	UAnimMontage * LightCharge;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Light Attack Anims")
	UAnimMontage * LightBeginAirCombo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavyOneTwo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavyUppercut;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavyJumpingGroundPound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavySpinningSlash;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavyWhirlwind;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heavy Attack Anims")
	UAnimMontage * HeavySlam;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dodge Roll Anims")
	UAnimMontage * DodgeRollForward;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dodge Roll Anims")
	UAnimMontage * DodgeRollLeft;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dodge Roll Anims")
	UAnimMontage * DodgeRollBack;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dodge Roll Anims")
	UAnimMontage * DodgeRollRight;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Blocking Anims")
	UAnimMontage * Blocking;

	////////////////////////////////////////////////////////////////////////////


	//Some variables that dictate combos
	////////////////////////////////////////////////////////////////////////////
	UPROPERTY(BlueprintReadWrite, Category = "Anims")
	bool bCanComboLevel2;

	UPROPERTY(BlueprintReadWrite, Category = "Anims")
	bool bCanComboLevel3;

	UPROPERTY(BlueprintReadWrite, Category = "Anims")
	bool bCanComboAir;

	UPROPERTY()
	float ComboTime;

	UPROPERTY()
	float ComboTimeCounter = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim Cooldowns")
	float TimeToCombo = 0.25f;

	//Some Variabbles that dictate dodge rolls
	/////////////////////////////////////////////////////////////////////////
	UPROPERTY()
	float DodgeTime;

	UPROPERTY()
	float DodgeTimeCounter = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Cooldowns")
	float DodgeCoolDown = 1.5f;

	//Some Variabbles that dictate blocking
	/////////////////////////////////////////////////////////////////////////
	UPROPERTY()
	bool bBlockKeyHeld = false;

	UPROPERTY()
	float BlockTimeCounter = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Cooldowns")
	float MaxBlockTime = 2.0f;

private:
	UPROPERTY()
	EComboState ComboState = EComboState::NOT_ATTACKING;

	UPROPERTY()
	EDodgeState DodgeState = EDodgeState::NOT_DODGING;

	UPROPERTY()
	EBlockState BlockState = EBlockState::NOT_BLOCKING;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	AController * PlayerController;

	UPROPERTY()
	APawn * Pawn;
};
