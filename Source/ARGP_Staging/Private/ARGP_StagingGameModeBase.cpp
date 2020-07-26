// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ARGP_StagingGameModeBase.h"
#include "Game/RPGGameStateBase.h"
#include "Player/RPGPlayerControllerBase.h"

AARGP_StagingGameModeBase::AARGP_StagingGameModeBase()
{
	GameStateClass = ARPGGameStateBase::StaticClass();
	PlayerControllerClass = ARPGPlayerControllerBase::StaticClass();
}
