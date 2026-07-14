// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_ProjectGameMode.h"
#include "GAM415_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGAM415_ProjectGameMode::AGAM415_ProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
