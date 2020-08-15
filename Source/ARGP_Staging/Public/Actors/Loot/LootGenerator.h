// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootGenerator.generated.h"

class AAICharacter;
class ABaseLootActor;

UCLASS()
class ARGP_STAGING_API ALootGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ALootGenerator();

	ABaseLootActor* GenerateLootObject(FVector TargetLocation, float InLevel);

	UFUNCTION()
		void HandleAIDeath(AAICharacter* DeadChar, float InLevel = 1.0f);

protected:
	virtual void BeginPlay() override;

private:



protected:

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABaseLootActor> LootClass;

};
