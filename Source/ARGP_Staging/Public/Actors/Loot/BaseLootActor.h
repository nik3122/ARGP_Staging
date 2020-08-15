// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/InteractableObject.h"
#include "BaseLootActor.generated.h"

class UInventoryComponent;
class USphereComponent;

UCLASS(Blueprintable, BlueprintType, Abstract)
class ARGP_STAGING_API ABaseLootActor : public AActor, public IInteractableObject
{
	GENERATED_BODY()
	
public:	
	ABaseLootActor();

	UFUNCTION(BlueprintCallable)
		void SetLootLevelAndType(float InLevel, bool bOnlyGold);

	UFUNCTION(BlueprintImplementableEvent)
		void PlayGoldReceivedFX(float InDeltaValue);

	UFUNCTION(BlueprintCallable)
		virtual void OnInteract(ARPGCharacterBase* PlayerCharacter) override;

	UFUNCTION(BlueprintCallable)
		virtual void OnEndMouseOver() override;

	UFUNCTION(BlueprintCallable)
		virtual void OnStartMouseOver() override;

	UFUNCTION(BlueprintCallable)
		virtual EProtagonistAffiliation GetObjectAffiliation() override;

	bool IsInteractable() override { return true; }

protected:
	virtual void BeginPlay() override;

private:

	void GiveLootToInventory(UInventoryComponent* InComponent);

protected:

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* LootMesh;

	UPROPERTY(EditInstanceOnly)
		float GoldToGive;

private:
	bool bOnlyGivesGold;

};
