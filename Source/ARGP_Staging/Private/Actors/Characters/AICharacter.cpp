// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/AICharacter.h"
#include "Components/PostProcessComponent.h"
#include "Player/RPGPlayerControllerBase.h"
#include "Dialogue/DialogueComponent.h"
#include "Types/RPGTypes.h"
#include "DlgManager.h"
#include "DlgContext.h"

AAICharacter::AAICharacter()
{
	CurrentAffiliation = EProtagonistAffiliation::NEUTRAL;

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
}

void AAICharacter::BeginPlay()
{
	ARPGPlayerControllerBase* CurrCon = Cast<ARPGPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
	OnNPCDeath.AddDynamic(CurrCon, &ARPGPlayerControllerBase::HandleNPCDeath);
	Super::BeginPlay();
}

void AAICharacter::Die()
{
	// THIS CHARACTER HAS DIED
	OnNPCDeath.Broadcast(this); 
	Super::Die();	
}

void AAICharacter::OnStartMouseOver()
{
	HandleOutlineChange();
	GetMesh()->SetRenderCustomDepth(true);
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
	HandleOutlineChange();
}

EProtagonistAffiliation AAICharacter::GetObjectAffiliation()
{
	return CurrentAffiliation;
}
