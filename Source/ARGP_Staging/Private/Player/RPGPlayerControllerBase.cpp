#include "Player/RPGPlayerControllerBase.h"
#include "Utils/RGPSaveGame.h"
#include "DlgContext.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "ARGP_Staging.h"
#include "Actors/InteractableObject.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "DlgManager.h"
#include "Blueprint/UserWidget.h"
#include "Game/RPGGameInstanceBase.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Actors/Weapons/WeaponActorBase.h"
#include "Actors/Characters/AICharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Inventory/Items/RPGItem.h"

ARPGPlayerControllerBase::ARPGPlayerControllerBase()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableMouseOverEvents = true;
	DefaultClickTraceChannel = ECC_Pawn;
}

void ARPGPlayerControllerBase::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!bInDialogue) {
		if (bInteractionClicked && InRangeOfInteraction()) {
			Protagonist->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Protagonist->GetActorLocation(), InteractionActorObject->GetActorLocation()));
			InteractionActor->OnInteract(Protagonist);
			bInteractionClicked = false;
			bMoveToMouseCursor = false;
			Protagonist->StopMovement();
			return;
		}

		if (bMoveToMouseCursor) {
			MoveToMouseCursor();
		}
	}

	CheckActorUnderCursorInteractable();
}

void ARPGPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void ARPGPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ARPGPlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Protagonist = Cast<ARPGCharacterBase>(InPawn);
	if (Protagonist) {

	}
}

void ARPGPlayerControllerBase::OnInteractionClicked()
{
	if (HighlightedActor) {
		InteractionActor = HighlightedActor;
		InteractionActorObject = Cast<AActor>(HighlightedActor.GetObject());
		bInteractionClicked = true;
		SetNewMoveDestination(InteractionActorObject->GetActorLocation());
	}
}

void ARPGPlayerControllerBase::OnDialogueOption0Pressed()
{
	HandleDialogueOptionPressed(0);
}

void ARPGPlayerControllerBase::OnDialogueOption1Pressed()
{
	HandleDialogueOptionPressed(1);
}

void ARPGPlayerControllerBase::OnDialogueOption2Pressed()
{
	HandleDialogueOptionPressed(2);
}

void ARPGPlayerControllerBase::OnDialogueOption3Pressed()
{
	HandleDialogueOptionPressed(3);
}

void ARPGPlayerControllerBase::HandleDialogueOptionPressed(int32 OptionNumber)
{
	if (!bInDialogue || OptionNumber > 3 || OptionNumber < 0 || OptionNumber >= CurrentDialogueContext->GetOptionNum()) {
		return;
	}
	if (CurrentDialogueContext) {
		if (!CurrentDialogueContext->ChooseChild(OptionNumber)) {
			CurrentDialogueContext = nullptr;
			bInDialogue = false;
			DialogueWidget->RemoveFromViewport();
		}
		else {
			DetermineDialogueOptsAndAssignText();
		}
	}
}

void ARPGPlayerControllerBase::DetermineDialogueOptsAndAssignText()
{
	//TODO delete this fam
	FString temp = "";
	if (!CurrentDialogueContext) {
		return;
	}
	switch (CurrentDialogueContext->GetOptionNum())
	{
	default:
		break;
	case 4:
		AssignDialogueBoxText_4Opts(CurrentDialogueContext->GetActiveNodeText(), CurrentDialogueContext->GetOptionText(0), CurrentDialogueContext->GetOptionText(1), CurrentDialogueContext->GetOptionText(2), CurrentDialogueContext->GetOptionText(3));
		break;
	case 3:
		AssignDialogueBoxText_3Opts(CurrentDialogueContext->GetActiveNodeText(), CurrentDialogueContext->GetOptionText(0), CurrentDialogueContext->GetOptionText(1), CurrentDialogueContext->GetOptionText(2));
		break;
	case 2:
		AssignDialogueBoxText_2Opts(CurrentDialogueContext->GetActiveNodeText(), CurrentDialogueContext->GetOptionText(0), CurrentDialogueContext->GetOptionText(1));
		break;
	case 1:
		AssignDialogueBoxText_1Opts(CurrentDialogueContext->GetActiveNodeText(), CurrentDialogueContext->GetOptionText(0));
		break;
	}
}

void ARPGPlayerControllerBase::InitDialogue(AAICharacter* NPCActor)
{
	bInDialogue = true;
	NPCActor->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NPCActor->GetActorLocation(), Protagonist->GetActorLocation()));
	TArray<UObject*> Participants;
	Participants.Add(Protagonist);
	Participants.Add(NPCActor);
	SetCurrentDialogueContext(UDlgManager::StartDialogue(DialogueObject, Participants));
	SetDialogueWidget(CreateDialogueWidget());
	DetermineDialogueOptsAndAssignText();
	FHitResult TempResult;
}

void ARPGPlayerControllerBase::MoveToMouseCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (Hit.bBlockingHit) {
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ARPGPlayerControllerBase::SetNewMoveDestination(const FVector DestLocation)
{
	if (Protagonist)
	{
		float const Distance = FVector::Dist(DestLocation, Protagonist->GetActorLocation());
		if ((Distance > MIN_DISTANCE))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ARPGPlayerControllerBase::OnSetDestinationPressed()
{
	bInteractionClicked = false;
	bMoveToMouseCursor = true;
}

void ARPGPlayerControllerBase::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
}

void ARPGPlayerControllerBase::CheckActorUnderCursorInteractable()
{
	FHitResult TempResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	GetHitResultUnderCursorForObjects(ObjectType, true, TempResult);

	if (TempResult.bBlockingHit) {
		if (GetPawn() != TempResult.GetActor() && TempResult.GetActor()->GetClass()->ImplementsInterface(UInteractableObject::StaticClass())) {
			HighlightedActor = TempResult.GetActor();
			HighlightedActor->OnStartMouseOver();
			bIsCursorOverValidActor = true;
		}
	}
	else {
		if (HighlightedActor) {
			HighlightedActor->OnEndMouseOver();
			HighlightedActor = nullptr;
			bIsCursorOverValidActor = false;
		}
	}
}

bool ARPGPlayerControllerBase::InRangeOfInteraction()
{

	if (!InteractionActorObject || !Protagonist) {
		return false;
	}

	float DistanceToProtagonist = FVector::Dist(InteractionActorObject->GetActorLocation(), Protagonist->GetActorLocation());

	switch (InteractionActor->GetObjectAffiliation())
	{
	case EProtagonistAffiliation::ALLY:
	case EProtagonistAffiliation::NEUTRAL:
		if (DistanceToProtagonist < TARGET_REACHED_DISTANCE) {
			return true;
		}
		break;
	case EProtagonistAffiliation::ENEMY:
		if( DistanceToProtagonist < Protagonist->GetInventoryComponent()->GetCurrentWeapon()->GetRange()) {
			return true;
		}
		break;
	default:
		return false;
		break;
	}
	return false;
}

void ARPGPlayerControllerBase::InteractWithObject()
{
	AActor* TempActor = Cast<AActor>(InteractionActor.GetObject());
	Protagonist->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Protagonist->GetActorLocation(), TempActor->GetActorLocation()));
	InteractionActor->OnInteract(Protagonist);
	bInteractionClicked = false;
	bMoveToMouseCursor = false;
	Protagonist->StopMovement();
}
