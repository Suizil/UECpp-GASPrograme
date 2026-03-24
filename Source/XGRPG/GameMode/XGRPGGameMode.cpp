// Copyright Epic Games, Inc. All Rights Reserved.

#include "XGRPGGameMode.h"



#include "XGRPGGameState.h"

#include "XGRPGPlayerController.h"
#include "XGRPGPlayerState.h"
#include "XGRPGCharacter.h"

#include "XGRPGHUD.h"

#include "UObject/ConstructorHelpers.h"

AXGRPGGameMode::AXGRPGGameMode()
{

	GameStateClass = AXGRPGGameState::StaticClass();

	PlayerControllerClass = AXGRPGPlayerController::StaticClass();
	PlayerStateClass = AXGRPGPlayerState::StaticClass();
	DefaultPawnClass = AXGRPGCharacter::StaticClass();

	HUDClass = AXGRPGHUD::StaticClass();

}
