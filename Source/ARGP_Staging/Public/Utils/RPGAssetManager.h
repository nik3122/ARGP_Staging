// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARGP_Staging.h"
#include "Engine/AssetManager.h"
#include "RPGAssetManager.generated.h"

class URPGItem;

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API URPGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	URPGAssetManager() {}
	virtual void StartInitialLoading() override;

	static const FPrimaryAssetType	PotionItemType;
	static const FPrimaryAssetType	SkillItemType;
	static const FPrimaryAssetType	TokenItemType;
	static const FPrimaryAssetType	WeaponItemType;

	static URPGAssetManager& Get();

	URPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
	
};
