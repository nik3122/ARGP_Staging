#include "Actors/Loot/LootGenerator.h"
#include "Actors/Loot/BaseLootActor.h"
#include "Actors/Characters/AICharacter.h"

ALootGenerator::ALootGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ALootGenerator::BeginPlay()
{
	Super::BeginPlay();
}

ABaseLootActor* ALootGenerator::GenerateLootObject(FVector TargetLocation, float InLevel)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	UWorld* CurrWorld = GetWorld();
	if (CurrWorld) {
		ABaseLootActor* SpawnedActor = CurrWorld->SpawnActor<ABaseLootActor>(LootClass, TargetLocation, FRotator::ZeroRotator, Params);
		SpawnedActor->SetLootLevelAndType(InLevel * 15, true);
		return SpawnedActor;
	}
	return nullptr;
}

void ALootGenerator::HandleAIDeath(AAICharacter* DeadChar, float InLevel)
{
	if (DeadChar) {
		FVector SpawnLoc = DeadChar->GetActorLocation();
		SpawnLoc.Z -= DeadChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		GenerateLootObject(SpawnLoc, InLevel);
	}
}