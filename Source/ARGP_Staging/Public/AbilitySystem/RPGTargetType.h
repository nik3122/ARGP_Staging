// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Types/RPGAbilityTypes.h"
#include "../ARGP_Staging.h"
#include "RPGTargetType.generated.h"

class ARPGCharacterBase;
class AActor;
struct FGameplayEventData;

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class ARGP_STAGING_API URPGTargetType : public UObject
{
	GENERATED_BODY()

public:
	URPGTargetType() {}

	UFUNCTION(BlueprintNativeEvent)
		void GetTargets(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintable)
class ARGP_STAGING_API URPGTargetType_UseOwner : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_UseOwner() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(NotBlueprintable)
class ARGP_STAGING_API URPGTargetType_UseEventData : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_UseEventData() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(Blueprintable, Abstract)
class ARGP_STAGING_API URPGTargetType_SphereTrace : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_SphereTrace() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

protected:

	UPROPERTY(EditDefaultsOnly)
		FVector Offset;
	UPROPERTY(EditDefaultsOnly)
		float SphereRadius;
	UPROPERTY(EditDefaultsOnly)
		float TraceLength;
	UPROPERTY(EditDefaultsOnly)
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
};

UCLASS(Blueprintable, Abstract)
class ARGP_STAGING_API URPGTargetType_ArcHitbox : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_ArcHitbox() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

protected:

	UPROPERTY(EditDefaultsOnly)
		FVector Offset;
	UPROPERTY(EditDefaultsOnly)
		float NumCones;
	UPROPERTY(EditDefaultsOnly)
		float ConeTraceArcWidth;
	UPROPERTY(EditDefaultsOnly)
		float SweepRadius;
	UPROPERTY(EditDefaultsOnly)
		float ArcDistance;
	UPROPERTY(EditDefaultsOnly)
		bool bShowDebug;
};