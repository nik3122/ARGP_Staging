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
