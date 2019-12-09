// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorAnimation.h"
#include "EngineMinimal.h"

UWarriorAnimation::UWarriorAnimation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UWarriorAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateAnimationProperties(DeltaSeconds);
	AbilityTimers(DeltaSeconds);
}

void UWarriorAnimation::AbilityTimers(float DeltaSeconds)
{
	if (ComboState != EComboState::NOT_ATTACKING)
	{
		ComboTimeCounter += DeltaSeconds;
	}

	if (DodgeState != EDodgeState::NOT_DODGING)
	{
		DodgeTimeCounter += DeltaSeconds;
	}

	if (BlockState != EBlockState::NOT_BLOCKING)
	{
		BlockTimeCounter += DeltaSeconds;
	}

	if (ComboTimeCounter > ComboTime)
	{
		ComboState = EComboState::NOT_ATTACKING;
		bCanComboLevel2 = false;
		bCanComboLevel3 = false;
		ComboTimeCounter = 0.0f;
	}

	if (DodgeTimeCounter > DodgeCoolDown)
	{
		DodgeState = EDodgeState::NOT_DODGING;
		DodgeTimeCounter = 0.0f;
	}

	if (BlockTimeCounter > MaxBlockTime)
	{
		BlockState = EBlockState::NOT_BLOCKING;
		StopBlock();
		BlockTimeCounter = 0.0f;
	}
}
void UWarriorAnimation::UpdateAnimationProperties(float DeltaSeconds)
{
	//Get Pawn
	Pawn = TryGetPawnOwner();

	//Get the pawn which is affected by our anim instance
	if (Pawn)
	{
		//Update our falling property
		bIsFalling = Pawn->GetMovementComponent()->IsFalling();

		//Update our movement speed
		MovementSpeed = Pawn->GetVelocity().Size();	
	}
}

void UWarriorAnimation::ManageComboMontages(UAnimMontage * Montage)
{
	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Montage failed"))
		return;
	}
	float MontageTime = Montage_Play(Montage);
	ComboTime = MontageTime + TimeToCombo;
	ComboTimeCounter = 0.0f;
}

////////////////////////////////////////////////////////////
//			Light Attacks
void UWarriorAnimation::UseLightAttack()
{
	//if falling don't attack
	if (bIsFalling)
	{
		return;
	}

	if (ComboState == EComboState::NOT_ATTACKING)
	{
		ComboState = EComboState::LIGHT_ONE_TWO;
		ManageComboMontages(LightOneTwoMontage);
	}
	else if (ComboState == EComboState::LIGHT_ONE_TWO && bCanComboLevel2)
	{
		ComboState = EComboState::LIGHT_SPINNING_SWIPE;
		ManageComboMontages(LightSpinningSwipe);
	}
	else if (ComboState == EComboState::LIGHT_SPINNING_SWIPE && bCanComboLevel3)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(LightStabMontage);
	}
	//Heavy branch
	else if (ComboState == EComboState::HEAVY_ONE_TWO && bCanComboLevel2)
	{
		ComboState = EComboState::LIGHT_ONE_TWO;
		ManageComboMontages(LightOneTwoMontage);
		bCanComboLevel2 = false;
	}
	else if (ComboState == EComboState::HEAVY_UPPERCUT && bCanComboLevel3)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(LightCharge);
	}
}

////////////////////////////////////////////////////////////
//			Heavy Attacks
void UWarriorAnimation::UseHeavyAttack()
{
	//if falling don't attack
	if (bIsFalling && !bCanComboAir)
	{
		return;
	}

	if (ComboState == EComboState::NOT_ATTACKING)
	{
		ComboState = EComboState::HEAVY_ONE_TWO;
		ManageComboMontages(HeavyOneTwo);
	}
	else if (ComboState == EComboState::HEAVY_ONE_TWO && bCanComboLevel2)
	{
		ComboState = EComboState::HEAVY_UPPERCUT;
		ManageComboMontages(HeavyUppercut);
	}
	else if (ComboState == EComboState::HEAVY_UPPERCUT && bCanComboLevel3)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(HeavyJumpingGroundPound);
	}
	//light branch
	else if (ComboState == EComboState::LIGHT_ONE_TWO && bCanComboLevel2)
	{
		ComboState = EComboState::HEAVY_ONE_TWO;
		ManageComboMontages(HeavyOneTwo);
		bCanComboLevel2 = false;
	}
	else if (ComboState == EComboState::LIGHT_SPINNING_SWIPE && bCanComboLevel3)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(HeavySpinningSlash);
	}
	else if (ComboState == EComboState::LIGHT_AIR_COMBO_BEGIN && bCanComboAir)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(HeavySlam);
	}
}

////////////////////////////////////////////////////////////
//			Charged Attacks
void UWarriorAnimation::UseLightChargedAttack()
{
	//if falling don't attack
	if (bIsFalling)
	{
		return;
	}

	//Plays Light_Begin_Air
	if (ComboState == EComboState::NOT_ATTACKING)
	{
		ComboState = EComboState::LIGHT_AIR_COMBO_BEGIN;
		ManageComboMontages(LightBeginAirCombo);
	}
}

void UWarriorAnimation::UseHeavyChargedAttack()
{
	//if falling don't attack
	if (bIsFalling)
	{
		return;
	}

	//Plays Heavy_Whirlwind 
	if (ComboState == EComboState::NOT_ATTACKING)
	{
		ComboState = EComboState::COMBO_END;
		ManageComboMontages(HeavyWhirlwind);
	}
}

////////////////////////////////////////////////////////////
//			Dodge Rolls
void UWarriorAnimation::ManageDodgeMontages(UAnimMontage * Montage)
{
	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dodge Montage failed"))
		return;
	}

	float MontageTime = Montage_Play(Montage);
	ComboTime = MontageTime + DodgeCoolDown;
	DodgeTimeCounter = 0.0f;
}

void UWarriorAnimation::UseDodgeRollForward()
{
	if (DodgeState == EDodgeState::NOT_DODGING)
	{
		DodgeState = EDodgeState::DODGE_ROLL_FORWARD;
		ManageDodgeMontages(DodgeRollForward);
	}
}

void UWarriorAnimation::UseDodgeRollLeft()
{
	if (DodgeState == EDodgeState::NOT_DODGING)
	{
		DodgeState = EDodgeState::DODGE_ROLL_LEFT;
		ManageDodgeMontages(DodgeRollLeft);
	}
}

void UWarriorAnimation::UseDodgeRollBack()
{
	if (DodgeState == EDodgeState::NOT_DODGING)
	{
		DodgeState = EDodgeState::DODGE_ROLL_BACK;
		ManageDodgeMontages(DodgeRollBack);
	}
}

void UWarriorAnimation::UseDodgeRollRight()
{
	if (DodgeState == EDodgeState::NOT_DODGING)
	{
		DodgeState = EDodgeState::DODGE_ROLL_RIGHT;
		ManageDodgeMontages(DodgeRollRight);
	}
}

////////////////////////////////////////////////////////////
//			Blocking
void UWarriorAnimation::UseBlock()
{
	PlayerController = Pawn->GetController();

	if (!Blocking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocking Montage failed"))
		return;
	}

	BlockState = EBlockState::BLOCKING;
	Montage_Play(Blocking);
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
	}
}

void UWarriorAnimation::StopBlock()
{
	BlockState = EBlockState::NOT_BLOCKING;
	Montage_JumpToSection(FName("Block_End"), Blocking);
	BlockTimeCounter = 0.0f;

	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false);
	}
}