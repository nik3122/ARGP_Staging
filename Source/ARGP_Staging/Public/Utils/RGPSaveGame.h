// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ARGP_Staging.h"
#include "GameFramework/SaveGame.h"
#include "Inventory/Items/RPGItem.h"
#include "RGPSaveGame.generated.h"

namespace ERPGSaveGameVersion
{
	enum type
	{
		Initial,
		AddedInventory,
		AddedItemData,

		// -----<new versions must be added before this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

UCLASS(BlueprintType)
class ARGP_STAGING_API URPGSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Constructor */
	URPGSaveGame()
	{
		SavedDataVersion = ERPGSaveGameVersion::LatestVersion;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FPrimaryAssetId, FRPGItemData> InventoryData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FRPGItemSlot, FPrimaryAssetId> SlottedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		FString UserId;

protected:
	UPROPERTY()
		TArray<FPrimaryAssetId> InventoryItems_DEPRECATED;

	UPROPERTY()
		int32 SavedDataVersion;

	virtual void Serialize(FArchive& Ar) override;
};