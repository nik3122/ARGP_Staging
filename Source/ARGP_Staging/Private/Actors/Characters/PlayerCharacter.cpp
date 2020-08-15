
#include "Actors/Characters/PlayerCharacter.h"

void APlayerCharacter::Die()
{
	OnProtagonistDeath.Broadcast(this);
	Super::Die();
}
