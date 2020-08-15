// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UICharacterWidget.generated.h"

class UUICharacterData;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = UI)
class ARGP_STAGING_API UUICharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void SetupWidget();
	UFUNCTION(BlueprintImplementableEvent)
		void RefreshValues();

	UFUNCTION(BlueprintCallable)
		FText GetGold() { return Gold; }

	void SetGold(float InGold);

private:

	UPROPERTY(transient)
		FText Gold;
	
};
