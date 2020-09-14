// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/AICharacter.h"
#include "Components/PostProcessComponent.h"
#include "Player/RPGPlayerControllerBase.h"
#include "Dialogue/DialogueComponent.h"
#include "Types/RPGTypes.h"
#include "Inventory/InventoryComponent.h"
#include "Actors/Loot/BaseLootActor.h"
#include "Game/LootEngine.h"
#include "Utils/RPGBlueprintLibrary.h"
#include "DlgManager.h"
#include "DlgContext.h"
#include "Game/RPGGameInstanceBase.h"

AAICharacter::AAICharacter()
{
	CurrentAffiliation = EProtagonistAffiliation::NEUTRAL;
	DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
}

void AAICharacter::BeginPlay()
{
	HandleAffiliationStatusChange();
	Super::BeginPlay();
	URPGGameInstanceBase* GameInst = URPGBlueprintLibrary::GetGameInstance(this);
	if (GameInst && GameInst->LootEngine) {
		OnNPCDeath.AddDynamic(GameInst->LootEngine, &ALootGenerator::HandleAIDeath);
	}
}

void AAICharacter::Die()
{
	OnNPCDeath.Broadcast(this, LootLevel);
	Super::Die();	
}

void AAICharacter::OnStartMouseOver()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(URPGBlueprintLibrary::GetStencilValue(CurrentAffiliation));
}

void AAICharacter::OnEndMouseOver()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AAICharacter::OnInteract(ARPGCharacterBase* PlayerCharacter)
{
	switch (CurrentAffiliation)
	{
	case EProtagonistAffiliation::ALLY:
	case EProtagonistAffiliation::NEUTRAL:
		StartDialogueWithPlayer();
		break;
	case EProtagonistAffiliation::ENEMY:
		PlayerCharacter->DoMeleeAttack();
		break;
	default:
		break;
	}
}

bool AAICharacter::IsInteractable()
{
	return IsAlive();
}

void AAICharacter::StartDialogueWithPlayer()
{
	ARPGPlayerControllerBase* CurrCon = Cast<ARPGPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
	if (CurrCon) {
		CurrCon->InitDialogue(this);
	}
}

void AAICharacter::SetAffiliationStatus(EProtagonistAffiliation val)
{
	CurrentAffiliation = val;
	HandleAffiliationStatusChange();
}

EProtagonistAffiliation AAICharacter::GetObjectAffiliation()
{
	return CurrentAffiliation;
}

void AAICharacter::HandleHitReactDuringAnimation()
{
	Super::HandleHitReactDuringAnimation();
	GetMesh()->bPauseAnims = true;
	FTimerHandle MeshPauseHandle;
	GetWorld()->GetTimerManager().SetTimer(MeshPauseHandle, [this]() {
		GetMesh()->bPauseAnims = false;
	}, .05, false);
}
