// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARGP_Staging.h"
#include "GameFramework/PlayerController.h"
#include "Actors/InteractableObject.h"
#include "GameplayTagContainer.h"
#include "Actors/Loot/BaseLootActor.h"
#include "Actors/Loot/LootGenerator.h"
#include "RPGPlayerControllerBase.generated.h"

class UDlgContext;
class APlayerCharacter;
class AAICharacter;
class UUICharacterWidget;
class ULootEngine;

UCLASS()
class ARGP_STAGING_API ARPGPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	ARPGPlayerControllerBase();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;


	UFUNCTION(BlueprintCallable)
		bool IsCursorOverValidTarget() { return bIsCursorOverValidActor; }
	UFUNCTION(BlueprintCallable)
		void OnInteractionClicked();
	UFUNCTION(BlueprintCallable)
		void OnSetDestinationPressed();
	UFUNCTION(BlueprintCallable)
		void OnSetDestinationReleased();

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
	void InitDialogue(AAICharacter* NPCActor);

	APlayerCharacter* GetProtagonist() { return Protagonist; }

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

private:

	void MoveToMouseCursor();
	void SetNewMoveDestination(const FVector DestLocation);

	void CheckActorUnderCursorInteractable();
	bool InRangeOfInteraction();
	void InteractWithObject();

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

	const float TARGET_REACHED_DISTANCE = 120.f;
	const float MIN_DISTANCE = 80.f;

	float DeltaTimeX;

	UDlgContext* CurrentDialogueContext;

	UUICharacterWidget* CurrentWidget;
};
