// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARGP_Staging.h"
#include "GameFramework/PlayerController.h"
#include "Actors/InteractableObject.h"
#include "GameplayTagContainer.h"
#include "Actors/Loot/BaseLootActor.h"
#include "Inventory/PlayerInventoryInterface.h"
#include "Actors/Loot/LootGenerator.h"
#include "Inventory/InventoryComponent.h"
#include "RPGPlayerControllerBase.generated.h"

class UDlgContext;
class APlayerCharacter;
class AAICharacter;
class UUICharacterWidget;
class ULootEngine;
class UQuestManager;

UCLASS()
class ARGP_STAGING_API ARPGPlayerControllerBase : public APlayerController, public IPlayerInventoryInterface
{
	GENERATED_BODY()

public:
	ARPGPlayerControllerBase();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	///////////////////////////////
	// MOUSE INTERACTION FUNCTIONS
	///////////////////////////////

	UFUNCTION(BlueprintCallable)
		bool IsCursorOverValidTarget() { return bIsCursorOverValidActor; }
	UFUNCTION(BlueprintCallable)
		void OnInteractionClicked();
	UFUNCTION(BlueprintCallable)
		void OnSetDestinationPressed();
	UFUNCTION(BlueprintCallable)
		void OnSetDestinationReleased();

	APlayerCharacter* GetProtagonist() { return Protagonist; }

	UFUNCTION(BlueprintCallable)
		bool GetCanCharacterMove() const { return bCharacterCanMove; }
	UFUNCTION(BlueprintCallable)
		void SetCanCharacterMove(bool val) { bCharacterCanMove = val; }

	//////////////////////////////
	// DIALOGUE FUNCTIONS
	//////////////////////////////

	UFUNCTION(BlueprintCallable)
		void OnDialogueOption0Pressed();
	UFUNCTION(BlueprintCallable)
		void OnDialogueOption1Pressed();
	UFUNCTION(BlueprintCallable)
		void OnDialogueOption2Pressed();
	UFUNCTION(BlueprintCallable)
		void OnDialogueOption3Pressed();

	void HandleDialogueOptionPressed(int32 OptionNumber);

	UDlgContext* GetCurrentDialogueContext() const { return CurrentDialogueContext; }
	void SetCurrentDialogueContext(UDlgContext* val) { CurrentDialogueContext = val; }

	UFUNCTION(BlueprintImplementableEvent)
		UUserWidget* CreateDialogueWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void AssignDialogueBoxText_4Opts(const FText& MainText, const FText& Option1Text, const FText& Option2Text, const FText& Option3Text, const FText& Option4Text);
	UFUNCTION(BlueprintImplementableEvent)
		void AssignDialogueBoxText_3Opts(const FText& MainText, const FText& Option1Text, const FText& Option2Text, const FText& Option3Text);
	UFUNCTION(BlueprintImplementableEvent)
		void AssignDialogueBoxText_2Opts(const FText& MainText, const FText& Option1Text, const FText& Option2Text);
	UFUNCTION(BlueprintImplementableEvent)
		void AssignDialogueBoxText_1Opts(const FText& MainText, const FText& Option1Text);

	UFUNCTION(BlueprintCallable)
		void DetermineDialogueOptsAndAssignText();

	void SetDialogueWidget(UUserWidget* val) { DialogueWidget = val; }

	UFUNCTION(BlueprintCallable)
		void InitDialogue(AAICharacter* NPCActor);

	//////////////////////////
	// UI DELEGATES
	//////////////////////////

	UFUNCTION()
		virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	UFUNCTION()
		virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	UFUNCTION()
		virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	UFUNCTION()
		virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	UFUNCTION()
		virtual void HandleGoldChanged(float NewGold, float DeltaValue);
	UFUNCTION()
		virtual void HandleMaxHealthChanged(float NewMaxHP);
	UFUNCTION()
		virtual void HandleMaxManaChanged(float NewMaxMana);

	////////////////////////
	// QUEST SYSTEM
	////////////////////////

	UFUNCTION(BlueprintCallable)
		UQuestManager* GetQuestManager() const { return QuestManager; }
	void SetQuestManager(class UQuestManager* val) { QuestManager = val; }

	///////////////////////
	// INVENTORY
	///////////////////////

	virtual const TMap<URPGItem*, FRPGItemData>& GetInventoryDataMap() const override
	{
		return InventoryComponent->InventoryData;
	}
	virtual const TMap<FRPGItemSlot, URPGItem*>& GetSlottedItemMap() const override
	{
		return InventoryComponent->SlottedItems;
	}
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return InventoryComponent->OnInventoryItemChangedNative;
	}
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() override
	{
		return InventoryComponent->OnSlottedItemChangedNative;
	}
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return InventoryComponent->OnInventoryLoadedNative;
	}

	UFUNCTION(BlueprintCallable)
		UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

private:

	void MoveToMouseCursor();
	void SetNewMoveDestination(const FVector DestLocation);

	void CheckActorUnderCursorInteractable();
	bool InRangeOfInteraction();
	void InteractWithObject();

	bool CanProtagonistMove();

protected:

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUICharacterWidget> MainWidgetClass;
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* DialogueWidget;

	// TODO Delete this fam	

	UPROPERTY(EditDefaultsOnly)
		class UDlgDialogue* DialogueObject;

private:

	class APlayerCharacter* Protagonist;

	TScriptInterface<IInteractableObject> InteractionActor;
	AActor* InteractionActorObject;
	TScriptInterface<IInteractableObject> HighlightedActor;

	bool bInDialogue;
	bool bMoveToMouseCursor;

	bool bIsCursorOverValidActor;
	bool bInteractionClicked;
	bool bCharacterCanMove = true;

	const float TARGET_REACHED_DISTANCE = 120.f;
	const float MIN_DISTANCE = 80.f;

	float DeltaTimeX;

	UDlgContext* CurrentDialogueContext;

	UUICharacterWidget* CurrentWidget;

	UQuestManager* QuestManager;
	UInventoryComponent* InventoryComponent;
};
