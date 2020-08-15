// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "PlayerCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProtagonistDeathDelegate, APlayerCharacter*, DeadCharacter);

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API APlayerCharacter : public ARPGCharacterBase
{
	GENERATED_BODY()

public:

	void Die() override;

	FProtagonistDeathDelegate OnProtagonistDeath;
	
};
