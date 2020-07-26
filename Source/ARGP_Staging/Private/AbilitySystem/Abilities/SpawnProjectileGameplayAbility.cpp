// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SpawnProjectileGameplayAbility.h"
#include "AbilitySystem/Tasks/Task_PlayMontageAndWaitForEvent.h"
#include "Utils/RPGBlueprintLibrary.h"

void USpawnProjectileGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {
			UTask_PlayMontageAndWaitForEvent* CurrTask = UTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, FName("None"), MontageToPlay, MontageEventTags);
			CurrTask->OnBlendOut.AddDynamic(this, &USpawnProjectileGameplayAbility::OnCompleted);
			CurrTask->OnInterrupted.AddDynamic(this, &USpawnProjectileGameplayAbility::OnCancelled);
			CurrTask->OnCancelled.AddDynamic(this, &USpawnProjectileGameplayAbility::OnCancelled);
			CurrTask->OnCompleted.AddDynamic(this, &USpawnProjectileGameplayAbility::OnCompleted);
			CurrTask->EventReceived.AddDynamic(this, &USpawnProjectileGameplayAbility::EventReceived);
			CurrTask->Activate();
		}
		else {
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

void USpawnProjectileGameplayAbility::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	FRPGGameplayEffectContainerSpec CurrSpec = MakeEffectContainerSpec(EventTag, EventData, -1);
	if (URPGBlueprintLibrary::DoesEffectContainerSpecHaveEffects(CurrSpec)) {
		AActor* TempActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRPGGameplayEffectContainerSpec TempSpec = MakeEffectContainerSpec(EventTag, EventData);
		URPGBlueprintLibrary::SpawnProjectileActor(ProjectileClass, TempActor->GetActorLocation(), TempActor->GetActorRotation(), FVector(1.f, 1.f, 1.f), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, TempActor, CurrSpec, Cast<APawn>(TempActor));
	}
}

void USpawnProjectileGameplayAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USpawnProjectileGameplayAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void USpawnProjectileGameplayAbility::OnBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USpawnProjectileGameplayAbility::OnInterrupted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
