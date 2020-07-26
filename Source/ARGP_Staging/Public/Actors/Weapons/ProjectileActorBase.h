// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/RPGAbilityTypes.h"
#include "ProjectileActorBase.generated.h"

UCLASS(Abstract, Blueprintable)
class ARGP_STAGING_API AProjectileActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileActorBase();

protected:
	virtual void BeginPlay() override;

	FRPGGameplayEffectContainerSpec ContainerSpec;

public:
	FRPGGameplayEffectContainerSpec GetContainerSpec() const { return ContainerSpec; }
	void SetContainerSpec(FRPGGameplayEffectContainerSpec val) { ContainerSpec = val; }
};
