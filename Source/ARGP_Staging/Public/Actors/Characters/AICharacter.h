// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Player/RPGPlayerControllerBase.h"
#include "AICharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNPCDeathDelegate, AAICharacter*, DeadCharacter, float, LootLevel);
/**
 * 
 */
UCLASS(Abstract)
class ARGP_STAGING_API AAICharacter : public ARPGCharacterBase, public IInteractableObject
{
	GENERATED_BODY()

public:
	AAICharacter();

	virtual void BeginPlay() override;

	void Die() override;

	void OnStartMouseOver() override;
	void OnEndMouseOver() override;
	void OnInteract(ARPGCharacterBase* PlayerCharacter) override;
	bool IsInteractable() override;

	void StartDialogueWithPlayer();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetAffiliationStatus(EProtagonistAffiliation val);
	UFUNCTION(BlueprintImplementableEvent)
		void HandleAffiliationStatusChange();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		virtual EProtagonistAffiliation GetObjectAffiliation() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		EProtagonistAffiliation CurrentAffiliation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UDialogueComponent* DialogueComponent;

	UPROPERTY(EditDefaultsOnly)
		float LootLevel;

	FNPCDeathDelegate OnNPCDeath; 
};
