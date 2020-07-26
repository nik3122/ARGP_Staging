// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MeleeWeaponGameplayAbility.h"
#include "AbilitySystem/Tasks/Task_PlayMontageAndWaitForEvent.h"

void UMeleeWeaponGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {
			UTask_PlayMontageAndWaitForEvent* CurrTask = UTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, FName("None"), MontageToPlay, MontageEventTags);
			CurrTask->OnBlendOut.AddDynamic(this, &UMeleeWeaponGameplayAbility::OnBlendOut);
			CurrTask->OnInterrupted.AddDynamic(this, &UMeleeWeaponGameplayAbility::OnInterrupted);
			CurrTask->OnCancelled.AddDynamic(this, &UMeleeWeaponGameplayAbility::OnCancelled);
			CurrTask->OnCompleted.AddDynamic(this, &UMeleeWeaponGameplayAbility::OnCompleted);
			CurrTask->EventReceived.AddDynamic(this, &UMeleeWeaponGameplayAbility::EventReceived);
			CurrTask->Activate();
		}
		else {
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

void UMeleeWeaponGameplayAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMeleeWeaponGameplayAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void UMeleeWeaponGameplayAbility::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMeleeWeaponGameplayAbility::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMeleeWeaponGameplayAbility::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ApplyEffectContainer(EventTag, EventData);
}
