#include "Actors/Weapons/ProjectileActorBase.h"

AProjectileActorBase::AProjectileActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectileActorBase::BeginPlay()
{
	Super::BeginPlay();
}
