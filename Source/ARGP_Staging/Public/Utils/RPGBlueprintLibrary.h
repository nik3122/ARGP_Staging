// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../ARGP_Staging.h"
#include "Types/RPGTypes.h"
#include "Inventory/Items/RPGItem.h"
#include "Inventory/Items/RPGWeaponItem.h"
#include "Types/RPGAbilityTypes.h"
#include "RPGBlueprintLibrary.generated.h"

class AProjectileActorBase;

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API URPGBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	URPGBlueprintLibrary();

	UFUNCTION(BlueprintCallable)
		static AWeaponActorBase* SpawnWeaponActor(TSubclassOf<AWeaponActorBase> Class, FTransform Transform, ESpawnActorCollisionHandlingMethod CollisionHandling, AActor* Owner, URPGWeaponItem* Item, FRPGItemSlot Slot, APawn* Instigator);

	UFUNCTION(BlueprintCallable)
		static AProjectileActorBase* SpawnProjectileActor(TSubclassOf<AProjectileActorBase> Class, FVector SpawnLocation, FRotator SpawnRotation, FVector SpawnScale, ESpawnActorCollisionHandlingMethod CollisionHandling, AActor* Owner, FRPGGameplayEffectContainerSpec& ContainerSpec, APawn* Instigator);

	UFUNCTION(BlueprintPure, Category = Loading)
		static bool IsInEditor();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (RPGItemSlot)", CompactNodeTitle = "==", Keywords = "== equal"), Category = Inventory)
		static bool EqualEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqualEqual (RPGItemSlot)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = Inventory)
		static bool NotEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B);

	UFUNCTION(BlueprintPure, Category = Inventory)
		static bool IsValidItemSlot(const FRPGItemSlot& ItemSlot);

	UFUNCTION(BlueprintPure, Category = Ability)
		static bool DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintPure, Category = Ability)
		static bool DoesEffectContainerSpecHaveTargets(const FRPGGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
		static FRPGGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	UFUNCTION(BlueprintCallable, Category = Ability)
		static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec);	
};
