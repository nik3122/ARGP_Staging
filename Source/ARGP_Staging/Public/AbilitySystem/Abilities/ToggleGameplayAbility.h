// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "ToggleGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API UToggleGameplayAbility : public URPGGameplayAbility
{
	GENERATED_BODY()
		
public:

	UToggleGameplayAbility();
	
protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintImplementableEvent)
		void HandleToggleActivation();
	UFUNCTION(BlueprintImplementableEvent)
		void HandleToggleDeactivation();
	


private:

	bool bIsAbilityActive;
};
