// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HackAndSlashGameMode.h"
#include "HackAndSlashCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHackAndSlashGameMode::AHackAndSlashGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Warriors/Blueprints/BP_Warrior"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
