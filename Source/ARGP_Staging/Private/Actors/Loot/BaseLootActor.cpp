#include "Actors/Loot/BaseLootActor.h"
#include "Components/SphereComponent.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Utils/RPGBlueprintLibrary.h"
#include "Inventory/InventoryComponent.h"

ABaseLootActor::ABaseLootActor()
{
	PrimaryActorTick.bCanEverTick = false;

	LootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LootMesh"));

	bOnlyGivesGold = true;
	GoldToGive = 1.f;
}

void ABaseLootActor::BeginPlay()
{
	Super::BeginPlay();
}


void ABaseLootActor::SetLootLevelAndType(float InGoldToGive, bool bOnlyGold)
{
	GoldToGive = InGoldToGive;
	bOnlyGivesGold = bOnlyGold;
}

void ABaseLootActor::OnInteract(ARPGCharacterBase* PlayerCharacter)
{
	if (PlayerCharacter) {
		ARPGPlayerControllerBase* CurrCon = URPGBlueprintLibrary::GetFirstPlayerController();
		if (!CurrCon) {
			return;
		}

		UInventoryComponent* FoundInvComp = CurrCon->GetInventoryComponent();
		if (FoundInvComp) {
			GiveLootToInventory(FoundInvComp);
		}
	}
}

void ABaseLootActor::OnEndMouseOver()
{
	LootMesh->SetRenderCustomDepth(false);
}

void ABaseLootActor::OnStartMouseOver()
{
	LootMesh->SetRenderCustomDepth(true);
	LootMesh->SetCustomDepthStencilValue(URPGBlueprintLibrary::GetStencilValue(EProtagonistAffiliation::LOOT));
}

EProtagonistAffiliation ABaseLootActor::GetObjectAffiliation()
{
	return EProtagonistAffiliation::LOOT;
}

void ABaseLootActor::GiveLootToInventory(UInventoryComponent* InComponent)
{
	// Change to have GameInstance (or GameMode/PlayerController) hold a LOOT_AGENT object. This object would reference a DataTable that runs calculations on how much gold should be given based on level.
	if (InComponent) {
		InComponent->HandleGoldChange(GoldToGive);
		PlayGoldReceivedFX(GoldToGive);
		if (!bOnlyGivesGold) {
			// TODO Handle equipment loot
			// InComponent->AddItemToInventory(EItemTypes::WEAPON, LOOT_AGENT.GetItemFromLevel(LootLevel))
		}
	}
	GetWorld()->DestroyActor(this);
}