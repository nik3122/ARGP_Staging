#include "Utils/RPGBlueprintLibrary.h"
#include "Inventory/Items/RPGItem.h"
#include "Actors/Weapons/WeaponActorBase.h"
#include "Actors/Weapons/ProjectileActorBase.h"
#include "Player/RPGPlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Characters/PlayerCharacter.h"
#include "Components/PostProcessComponent.h"
#include "Actors/Loot/BaseLootActor.h"
#include "Game/RPGGameInstanceBase.h"
#include "Game/LootEngine.h"
#include "Inventory/Items/RPGWeaponItem.h"

URPGBlueprintLibrary::URPGBlueprintLibrary()
{

}

AWeaponActorBase* URPGBlueprintLibrary::SpawnWeaponActor(TSubclassOf<AWeaponActorBase> Class, FTransform Transform, ESpawnActorCollisionHandlingMethod CollisionHandling, AActor* Owner, URPGWeaponItem* Item, FRPGItemSlot Slot, APawn* Instigator)
{
	UWorld* CurrWorld = Owner->GetWorld();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (CurrWorld) {
		AWeaponActorBase* SpawnedActor = CurrWorld->SpawnActor<AWeaponActorBase>(Class, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		SpawnedActor->SetSlot(Slot);
		SpawnedActor->SetWeaponItem(Item);
		return SpawnedActor;
	}
	return nullptr;
}

AProjectileActorBase* URPGBlueprintLibrary::SpawnProjectileActor(TSubclassOf<AProjectileActorBase> Class, FVector SpawnLocation, FRotator SpawnRotation, FVector SpawnScale, ESpawnActorCollisionHandlingMethod CollisionHandling, AActor* Owner, FRPGGameplayEffectContainerSpec& ContainerSpec, APawn* Instigator)
{
	UWorld* CurrWorld = Owner->GetWorld();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (CurrWorld) {
		AProjectileActorBase* SpawnedActor = CurrWorld->SpawnActor<AProjectileActorBase>(Class, SpawnLocation, SpawnRotation, Params);
		SpawnedActor->SetContainerSpec(ContainerSpec);
		return SpawnedActor;
	}
	return nullptr;
}

bool URPGBlueprintLibrary::IsInEditor()
{
	return GIsEditor;
}

bool URPGBlueprintLibrary::EqualEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
	return A == B;
}

bool URPGBlueprintLibrary::NotEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
	return A != B;
}

bool URPGBlueprintLibrary::IsValidItemSlot(const FRPGItemSlot& ItemSlot)
{
	return ItemSlot.IsValid();
}

bool URPGBlueprintLibrary::DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool URPGBlueprintLibrary::DoesEffectContainerSpecHaveTargets(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FRPGGameplayEffectContainerSpec URPGBlueprintLibrary::AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FRPGGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> URPGBlueprintLibrary::ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

UInventoryComponent* URPGBlueprintLibrary::GetPlayerInventory()
{
	ARPGPlayerControllerBase* CurrCon = GetFirstPlayerController();
	if (CurrCon) {
		return CurrCon->GetInventoryComponent();
	}	
	return nullptr;
}

UQuestManager* URPGBlueprintLibrary::GetQuestManager()
{
	ARPGPlayerControllerBase* CurrCon = GetFirstPlayerController();
	if (CurrCon) {
		return CurrCon->GetQuestManager();
	}
	return nullptr;
}

ARPGPlayerControllerBase* URPGBlueprintLibrary::GetFirstPlayerController()
{
	TArray<APlayerController*> Cons;
	if (GEngine) {
		GEngine->GetAllLocalPlayerControllers(Cons);
		if (Cons.Num() > 0 && Cons[0]) {
			ARPGPlayerControllerBase* CurrCon = Cast<ARPGPlayerControllerBase>(Cons[0]);
			return CurrCon;
		}
	}
	return nullptr;
}

int32 URPGBlueprintLibrary::GetStencilValue(EProtagonistAffiliation InAffiliation)
{
	switch (InAffiliation)
	{
	default:
		break;
	case EProtagonistAffiliation::ALLY:
		return 252;
		break;
	case EProtagonistAffiliation::ENEMY:
		return 254;
		break;
	case EProtagonistAffiliation::NEUTRAL:
		return 255;
		break;
	case EProtagonistAffiliation::LOOT:
		return 253;
		break;
	}
	return 255;
}

URPGGameInstanceBase* URPGBlueprintLibrary::GetGameInstance(AActor* InContext)
{
	URPGGameInstanceBase* ReturnInst = Cast<URPGGameInstanceBase>(UGameplayStatics::GetGameInstance(InContext));
	return ReturnInst;
}
