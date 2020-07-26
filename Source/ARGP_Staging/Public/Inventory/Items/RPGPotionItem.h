// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/RPGItem.h"
#include "RPGPotionItem.generated.h"

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API URPGPotionItem : public URPGItem
{
	GENERATED_BODY()

public:

	URPGPotionItem()
	{
		ItemType = URPGAssetManager::PotionItemType;
	}
	
};
