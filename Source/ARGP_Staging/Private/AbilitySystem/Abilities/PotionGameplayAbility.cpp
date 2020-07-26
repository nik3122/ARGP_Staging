// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/PotionGameplayAbility.h"
#include "AbilitySystem/Tasks/Task_PlayMontageAndWaitForEvent.h"
#include "Player/RPGPlayerControllerBase.h"
#include "Utils/RPGAssetManager.h"

void UPotionGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {
			UTask_PlayMontageAndWaitForEvent* CurrTask = UTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, FName("None"), MontageToPlay, MontageEventTags);
			CurrTask->OnBlendOut.AddDynamic(this, &UPotionGameplayAbility::OnBlendOut);
			CurrTask->OnInterrupted.AddDynamic(this, &UPotionGameplayAbility::OnInterrupted);
			CurrTask->OnCancelled.AddDynamic(this, &UPotionGameplayAbility::OnCancelled);
			CurrTask->OnCompleted.AddDynamic(this, &UPotionGameplayAbility::OnCompleted);
			CurrTask->EventReceived.AddDynamic(this, &UPotionGameplayAbility::EventReceived);
			CurrTask->Activate();
		}
		else {
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

void UPotionGameplayAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void UPotionGameplayAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void UPotionGameplayAbility::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPotionGameplayAbility::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPotionGameplayAbility::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ApplyEffectContainer(EventTag, EventData);
	
}
