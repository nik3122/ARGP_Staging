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

	UFUNCTION(BlueprintCallable)
		float GetHealth() { return Health; }
	UFUNCTION(BlueprintCallable)
		float GetMaxHealth() { return MaxHealth; }
	void SetHealth(float InHealth);
	void SetMaxHealth(float InMaxHealth);
	void AddHealth(float InDelta);

private:

	UPROPERTY(transient)
		FText Gold;
	
	UPROPERTY(transient)
		float Health;

	UPROPERTY(transient)
		float MaxHealth;
};
