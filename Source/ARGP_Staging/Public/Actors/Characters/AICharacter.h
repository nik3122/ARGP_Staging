// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Player/RPGPlayerControllerBase.h"
#include "AICharacter.generated.h"

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
	void StartDialogueWithPlayer();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetAffiliationStatus(EProtagonistAffiliation val);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		virtual EProtagonistAffiliation GetObjectAffiliation() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		EProtagonistAffiliation CurrentAffiliation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UDialogueComponent* DialogueComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UPostProcessComponent* PostProcessComponent;

	UPROPERTY(BlueprintAssignable)
		FCharacterDeathDelegate OnNPCDeath;

};
