// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARGP_Staging.h"
#include "GameFramework/PlayerController.h"
#include "Actors/InteractableObject.h"
#include "RPGPlayerControllerBase.generated.h"

class UDlgContext;
class URPGCharacterBase;
class AAICharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDeathDelegate, AAICharacter*, DeadCharacter);

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

	UFUNCTION(BlueprintImplementableEvent)
		void HandleNPCDeath(AAICharacter* DeadChar);

	UFUNCTION(BlueprintCallable)
		void DetermineDialogueOptsAndAssignText();

	void SetDialogueWidget(UUserWidget* val) { DialogueWidget = val; }

	void InitDialogue(AAICharacter* NPCActor);

	// TODO Delete this fam	

	UPROPERTY(EditDefaultsOnly)
		class UDlgDialogue* DialogueObject;

protected:
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* DialogueWidget;

private:

	void MoveToMouseCursor();
	void SetNewMoveDestination(const FVector DestLocation);

	void CheckActorUnderCursorInteractable();
	bool InRangeOfInteraction();
	void InteractWithObject();

private:

	class ARPGCharacterBase* Protagonist;

	TScriptInterface<IInteractableObject> InteractionActor;
	AActor* InteractionActorObject;
	TScriptInterface<IInteractableObject> HighlightedActor;

	bool bInDialogue;
	bool bMoveToMouseCursor;

	bool bIsCursorOverValidActor;
	bool bInteractionClicked;

	const float TARGET_REACHED_DISTANCE = 120.f;
	const float MIN_DISTANCE = 80.f;

	UDlgContext* CurrentDialogueContext;
};
