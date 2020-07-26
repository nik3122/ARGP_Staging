// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/AICharacter.h"
#include "DlgManager.h"
#include "Player/RPGPlayerControllerBase.h"
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
	if (CurrentAffiliation != EProtagonistAffiliation::ENEMY) {
		ARPGPlayerControllerBase* CurrCon = Cast<ARPGPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
		CurrCon->InitDialogue(this);
	}
	else {
		PlayerCharacter->DoMeleeAttack();
	}
}

void AAICharacter::SetAffiliationStatus(EProtagonistAffiliation val)
{
	CurrentAffiliation = val;
	HandleOutlineChange();
}