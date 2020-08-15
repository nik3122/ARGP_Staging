// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ARGP_Staging.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstanceBase.generated.h"

class URPGItem;
class URPGSaveGame;
class ALootGenerator;

/**
 * 
 */
UCLASS(Abstract)
class ARGP_STAGING_API URPGGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ALootGenerator> LootEngineClass;

	ALootGenerator* LootEngine;

	virtual void Init() override;



	URPGGameInstanceBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetId, FRPGItemData> DefaultInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetType, int32> ItemSlotsPerType;
	UPROPERTY(BlueprintReadWrite, Category = Save)
		FString SaveSlot;
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 SaveUserIndex;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		void AddDefaultInventory(URPGSaveGame* SaveGame, bool bRemoveExtra = false);
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool IsValidItemSlot(FRPGItemSlot ItemSlot) const;
	UFUNCTION(BlueprintCallable, Category = Save)
		URPGSaveGame* GetCurrentSaveGame();
	UFUNCTION(BlueprintCallable, Category = Save)
		void SetSavingEnabled(bool bEnabled);
	UFUNCTION(BlueprintCallable, Category = Save)
		bool LoadOrCreateSaveGame();
	UFUNCTION(BlueprintCallable, Category = Save)
		bool WriteSaveGame();
	UFUNCTION(BlueprintCallable, Category = Save)
		void ResetSaveGame();

protected:
	UPROPERTY()
		URPGSaveGame* CurrentSaveGame;
	UPROPERTY()
		bool bSavingEnabled;
	
};
