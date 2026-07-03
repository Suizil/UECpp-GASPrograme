// Copyright Epic Games, Inc. All Rights Reserved.

#include "LSRPGGameMode.h"



#include "LSRPGGameState.h"

#include "LSRPGPlayerController.h"
#include "LSRPGPlayerState.h"
#include "LSRPGCharacter.h"

#include "LSRPGHUD.h"

#include "UObject/ConstructorHelpers.h"

ALSRPGGameMode::ALSRPGGameMode()
{

	GameStateClass = ALSRPGGameState::StaticClass();

	PlayerControllerClass = ALSRPGPlayerController::StaticClass();
	PlayerStateClass = ALSRPGPlayerState::StaticClass();
	DefaultPawnClass = ALSRPGCharacter::StaticClass();

	HUDClass = ALSRPGHUD::StaticClass();

}
