// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SilentBot.generated.h"

UCLASS()
class ARGP_STAGING_API ASilentBot : public APawn
{
	GENERATED_BODY()

public:
	ASilentBot();

protected:
	virtual void BeginPlay() override;

public:	
};
