// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../../ARGP_Staging.h"
#include "Utils/RPGAssetManager.h"
#include "RPGItem.generated.h"

class URPGGameplayAbility;

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class ARGP_STAGING_API URPGItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URPGItem()
		: Price(0)
		, MaxCount(1)
		, MaxLevel(1)
		, AbilityLevel(1)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FSlateBrush ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
		int32 MaxCount;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
		bool IsConsumable() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
		int32 MaxLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TSubclassOf<URPGGameplayAbility> GrantedAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		int32 AbilityLevel;

	UFUNCTION(BlueprintCallable, Category = Item)
		FString GetIdentifierString() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
};
