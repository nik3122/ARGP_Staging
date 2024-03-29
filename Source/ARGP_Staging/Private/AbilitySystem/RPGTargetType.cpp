// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGTargetType.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "Actors/Characters/RPGCharacterBase.h"


void URPGTargetType::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void URPGTargetType_UseOwner::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void URPGTargetType_UseEventData::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}
}

void URPGTargetType_ArcHitbox::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	float DegreeIncrements = ConeTraceArcWidth / NumCones;
	float YawValueFirst = ConeTraceArcWidth * -.5;
	TArray<AActor*> AlreadyHitActors;
	for (int i = 0; i < NumCones; i++) {
		float YawValueSecond = DegreeIncrements * i;
		float YawFinal = YawValueFirst + YawValueSecond;
		FVector RotatedVector = TargetingCharacter->GetActorForwardVector().RotateAngleAxis(YawFinal, FVector(0, 0, 1)) * ArcDistance;
		FVector CharOwnerLocation = TargetingCharacter->GetActorLocation();
		CharOwnerLocation.Z += 50;
		FVector FinalVectorInput = RotatedVector + CharOwnerLocation;
		TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(TargetingCharacter);
		TArray<FHitResult> HitResults;
		if (bShowDebug) {
			UKismetSystemLibrary::SphereTraceMultiForObjects(TargetingCharacter, CharOwnerLocation, FinalVectorInput, SweepRadius, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResults, true);
		}
		else {
			UKismetSystemLibrary::SphereTraceMultiForObjects(TargetingCharacter, CharOwnerLocation, FinalVectorInput, SweepRadius, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResults, true);
		}

		for (FHitResult HitResult : HitResults) {
			AActor* HitActor = HitResult.Actor.Get();
			if (HitActor && !AlreadyHitActors.Contains(HitActor)) {
				OutHitResults.Add(HitResult);
				AlreadyHitActors.Add(HitActor);
			}
		}
	}
}

void URPGTargetType_SphereTrace::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	FVector EndVector = (TargetingCharacter->GetActorForwardVector() * TraceLength) + TargetingCharacter->GetActorLocation();
	TArray<AActor*> ActorsIgnore;
	TArray<FHitResult> TempHitResults;
	TArray<AActor*> AlreadyHitActors;
	UKismetSystemLibrary::SphereTraceMultiForObjects(TargetingCharacter, TargetingCharacter->GetActorTransform().TransformVector(Offset), EndVector, SphereRadius, ObjectTypes, false, ActorsIgnore, EDrawDebugTrace::ForDuration, TempHitResults, true);

	for (FHitResult HitResult : TempHitResults) {
		AActor* HitActor = HitResult.Actor.Get();
		if (HitActor && !AlreadyHitActors.Contains(HitActor)) {
			OutHitResults.Add(HitResult);
			AlreadyHitActors.Add(HitActor);
		}
	}
}
