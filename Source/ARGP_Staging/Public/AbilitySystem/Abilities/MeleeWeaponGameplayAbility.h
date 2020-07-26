// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "MeleeWeaponGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARGP_STAGING_API UMeleeWeaponGameplayAbility : public URPGGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
		void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
		void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
		void OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
		void OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()
		virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
		UAnimMontage* MontageToPlay;
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
		FGameplayTagContainer MontageEventTags;
	
};
