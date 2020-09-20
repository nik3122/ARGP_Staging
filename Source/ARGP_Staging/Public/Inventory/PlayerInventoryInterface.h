// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Items/RPGItem.h"
#include "PlayerInventoryInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlayerInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class ARGP_STAGING_API IPlayerInventoryInterface
{
	GENERATED_BODY()

public:
	virtual const TMap<URPGItem*, FRPGItemData>& GetInventoryDataMap() const = 0;
	virtual const TMap<FRPGItemSlot, URPGItem*>& GetSlottedItemMap() const = 0;
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
