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

	UFUNCTION(BlueprintPure)
		FText GetGold() { return Gold; }
	void SetGold(float InGold);

	UFUNCTION(BlueprintPure)
		float GetHealth() { return Health; }
	UFUNCTION(BlueprintPure)
		float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintPure)
		float GetMana() { return Mana; }
	UFUNCTION(BlueprintPure)
		float GetMaxMana() { return MaxMana; }

	void SetHealth(float InHealth);
	void SetMaxHealth(float InMaxHealth);
	void AddHealth(float InDelta);

	void SetMana(float InMana);
	void SetMaxMana(float InMaxMana);
	void AddMana(float InDelta);

private:

	UPROPERTY(transient)
		FText Gold;
	
	UPROPERTY(transient)
		float Health;

	UPROPERTY(transient)
		float MaxHealth;

	UPROPERTY(transient)
		float Mana;

	UPROPERTY(transient)
		float MaxMana;
};
