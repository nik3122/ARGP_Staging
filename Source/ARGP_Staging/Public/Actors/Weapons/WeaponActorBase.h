// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Inventory/Items/RPGItem.h"
#include "WeaponActorBase.generated.h"

class UMeshComponent;
class USphereComponent;
class UCapsuleComponent;

UCLASS(Blueprintable, Abstract)
class ARGP_STAGING_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponActorBase();

	UFUNCTION(BlueprintCallable)
		void BeginWeaponAttack(FGameplayTag InTag, float InAttackDelayTime, int32 InAttackDelayCount);

	UFUNCTION(BlueprintCallable)
		void EndWeaponAttack();

	UFUNCTION(BlueprintImplementableEvent)
		void HitPause();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly)
		UMeshComponent* Mesh;
	USphereComponent* Root;
	UPROPERTY(EditDefaultsOnly)
		UCapsuleComponent* CapsuleCollision;		

	UPROPERTY(EditDefaultsOnly)
		FRPGItemSlot Slot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		URPGItem* RPGWeaponItem;
	UPROPERTY(EditDefaultsOnly)
		float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Defaults")
		FGameplayTag AttackEventTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Defaults")
		float AttackDelayTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Defaults")
		int32 AttackDelayCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Defaults")
		bool bEnableAttackDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Defaults")
		bool bIsAttacking;

private:

	bool bActorHasBeenHit;

public:
	FRPGItemSlot GetSlot() const { return Slot; }
	void SetSlot(FRPGItemSlot val) { Slot = val; }
	URPGItem* GetWeaponItem() const { return RPGWeaponItem; }
	void SetWeaponItem(URPGItem* val) { RPGWeaponItem = val; }
	float GetRange() { return Range; }
};
