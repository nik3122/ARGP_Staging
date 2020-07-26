#include "Actors/Bots/SilentBot.h"

ASilentBot::ASilentBot()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASilentBot::BeginPlay()
{
	Super::BeginPlay();
	
}\

