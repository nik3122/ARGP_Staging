#include "Inventory/InventoryComponent.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "Types/RPGTypes.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::AddItemToInventory(EItemTypes Slot, URPGItem* InItem)
{
	TPair<uint8, URPGItem*> AddPair;
	AddPair.Key = GetItemTypeHash(Slot);
	AddPair.Value = InItem;
	InventoryMap.Add(AddPair);
}

void UInventoryComponent::AddAbilityToCombatMap(ECombatHotkeys InHotkey, TSubclassOf<URPGGameplayAbility> InAbility)
{
	TPair<uint8, TSubclassOf<URPGGameplayAbility>> AddPair;
	AddPair.Key = GetCombatHotkeyHash(InHotkey);
	AddPair.Value = InAbility;
	CombatMap.Add(AddPair);
}

TSubclassOf<URPGGameplayAbility> UInventoryComponent::GetAbilityFromHotkey(ECombatHotkeys InHotkey)
{
	return *CombatMap.Find(GetCombatHotkeyHash(InHotkey));
}

AWeaponActorBase* UInventoryComponent::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}
