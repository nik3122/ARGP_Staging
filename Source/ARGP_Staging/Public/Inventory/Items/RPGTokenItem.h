// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/RPGItem.h"
#include "RPGTokenItem.generated.h"

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API URPGTokenItem : public URPGItem
{
	GENERATED_BODY()

public:

	URPGTokenItem()
	{
		ItemType = URPGAssetManager::TokenItemType;
		MaxCount = 0;
	}
	
};
