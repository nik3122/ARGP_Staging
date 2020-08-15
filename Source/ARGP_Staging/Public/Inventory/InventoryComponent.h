// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/RPGTypes.h"
#include "InventoryComponent.generated.h"

class URPGGameplayAbility;
class AWeaponActorBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGoldChangedDelegate, float, NewGold, float, DeltaValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARGP_STAGING_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	void AddItemToInventory(EItemTypes Slot, URPGItem* InItem);
	void AddAbilityToCombatMap(ECombatHotkeys InHotkey, TSubclassOf<URPGGameplayAbility> InAbility);


	UFUNCTION(BlueprintCallable)
		void HandleGoldChange(float DeltaValue); 
	UPROPERTY()
		FGoldChangedDelegate OnGoldChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
		TMap<uint8, URPGItem*> InventoryMap;
	UPROPERTY(BlueprintReadOnly)
		TMap<uint8, TSubclassOf<URPGGameplayAbility>> CombatMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AWeaponActorBase* CurrentWeapon;
	UPROPERTY(BlueprintReadOnly)
		float OwnedGold;

private:

	FORCEINLINE uint8 GetItemTypeHash(const EItemTypes ItemType) { return (uint8)ItemType; }
	FORCEINLINE uint8 GetCombatHotkeyHash(const ECombatHotkeys HotkeyType) { return (uint8)HotkeyType; }
public:
	UFUNCTION(BlueprintCallable)
		TSubclassOf<URPGGameplayAbility> GetAbilityFromHotkey(ECombatHotkeys InHotkey);
	UFUNCTION(BlueprintCallable)
		float GetCurrentGold() { return OwnedGold; }
	UFUNCTION(BlueprintCallable)
		AWeaponActorBase* GetCurrentWeapon() const { return CurrentWeapon; }

	void SetCurrentWeapon(AWeaponActorBase* val) { CurrentWeapon = val; }
};
