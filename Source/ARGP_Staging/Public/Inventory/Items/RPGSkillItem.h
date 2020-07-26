// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/RPGItem.h"
#include "RPGSkillItem.generated.h"

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API URPGSkillItem : public URPGItem
{
	GENERATED_BODY()

public:
	URPGSkillItem()
	{
		ItemType = URPGAssetManager::SkillItemType;
	}

};
