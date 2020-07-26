// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UInteractableObject : public UInterface
{
	GENERATED_BODY()
};

class ARGP_STAGING_API IInteractableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		virtual void OnInteract(ARPGCharacterBase* PlayerCharacter) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void OnEndMouseOver() = 0;

	UFUNCTION(BlueprintCallable)
		virtual void OnStartMouseOver() = 0;
};
