#include "Player/RPGPlayerControllerBase.h"
#include "Utils/RGPSaveGame.h"
#include "DlgContext.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "ARGP_Staging.h"
#include "Actors/InteractableObject.h"
#include "UMG/UI/UICharacterWidget.h"
#include "DlgManager.h"
#include "Blueprint/UserWidget.h"
#include "Game/RPGGameInstanceBase.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Actors/Characters/PlayerCharacter.h"
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
	DefaultClickTraceChannel = ECC_WorldDynamic;
}

void ARPGPlayerControllerBase::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	DeltaTimeX = DeltaTime;
	if (!bInDialogue) {
		if (bInteractionClicked && InRangeOfInteraction()) {
			InteractWithObject();
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
	CurrentWidget = Cast<UUICharacterWidget>(CreateWidget(this, MainWidgetClass));
	CurrentWidget->AddToViewport();
}

void ARPGPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ARPGPlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Protagonist = Cast<APlayerCharacter>(InPawn);
	if (Protagonist) {
		Protagonist->GetInventoryComponent()->OnGoldChanged.AddDynamic(this, &ARPGPlayerControllerBase::HandleGoldChanged);
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
	TArray<UObject*> Participants;
	Participants.Add(Protagonist);
	Participants.Add(NPCActor);
	SetCurrentDialogueContext(UDlgManager::StartDialogue(DialogueObject, Participants));
	SetDialogueWidget(CreateDialogueWidget());
	DetermineDialogueOptsAndAssignText();
}

void ARPGPlayerControllerBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	
}

void ARPGPlayerControllerBase::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{

}

void ARPGPlayerControllerBase::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{

}

void ARPGPlayerControllerBase::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{

}


void ARPGPlayerControllerBase::HandleGoldChanged(float NewGold, float DeltaValue)
{
	if (CurrentWidget) {
		CurrentWidget->SetGold(NewGold);
	}
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

	if (TempResult.bBlockingHit && TempResult.GetActor()) {
		if (GetPawn() != TempResult.GetActor() && TempResult.GetActor()->GetClass()->ImplementsInterface(UInteractableObject::StaticClass())) {
			HighlightedActor = TempResult.GetActor();
			if (HighlightedActor->IsInteractable()) {
				HighlightedActor->OnStartMouseOver();
				bIsCursorOverValidActor = true;
			}
			else {
				HighlightedActor->OnEndMouseOver();
				HighlightedActor = nullptr;
				bIsCursorOverValidActor = false;
			}
		}
	}
	else {
		if (HighlightedActor) {
			HighlightedActor->OnEndMouseOver();
		}
		HighlightedActor = nullptr;
		bIsCursorOverValidActor = false;
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
	case EProtagonistAffiliation::LOOT:
		if (DistanceToProtagonist < 150.f) {
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
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Protagonist->GetActorLocation(), TempActor->GetActorLocation());
	Protagonist->SetActorRotation(FRotator(Protagonist->GetActorRotation().Pitch, TargetRotation.Yaw, Protagonist->GetActorRotation().Roll));
	InteractionActor->OnInteract(Protagonist);
	bInteractionClicked = false;
	bMoveToMouseCursor = false;
	Protagonist->StopMovement();
}
