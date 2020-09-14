// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/UI/UICharacterWidget.h"
#include "Player/RPGPlayerControllerBase.h"

void UUICharacterWidget::SetupWidget()
{
	SetGold(0);
	RefreshValues();
}

void UUICharacterWidget::SetGold(float InGold)
{
	Gold = FText::AsNumber(InGold);
	RefreshValues();
}

void UUICharacterWidget::SetHealth(float InHealth)
{
	MaxHealth = InHealth;
	Health = InHealth;
	RefreshValues();
}

void UUICharacterWidget::SetMaxHealth(float InMaxHealth)
{
	MaxHealth = InMaxHealth;
	RefreshValues();
}

void UUICharacterWidget::AddHealth(float InDelta)
{
	Health += InDelta;
	RefreshValues();
}

void UUICharacterWidget::SetMana(float InMana)
{
	MaxMana = InMana;
	Mana = InMana;
	RefreshValues();
}

void UUICharacterWidget::SetMaxMana(float InMaxMana)
{
	MaxMana = InMaxMana;
	RefreshValues();
}

void UUICharacterWidget::AddMana(float InDelta)
{
	Mana += InDelta;
	RefreshValues();
}
