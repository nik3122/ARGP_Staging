// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARGP_Staging.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "Types/RPGTypes.h"
#include "AbilitySystemInterface.h"
#include "DlgDialogueParticipant.h"
#include "Types/QuestDialogueTypes.h"
#include "Inventory/PlayerInventoryInterface.h"
#include "Actors/InteractableObject.h"
#include "Inventory/Items/RPGWeaponItem.h"
#include "RPGCharacterBase.generated.h"


class URPGGameplayAbility;
class UGameplayEffect;
class AWeaponActorBase;
class URPGWeaponItem;
class UInventoryComponent;

UCLASS(Abstract)
class ARGP_STAGING_API ARPGCharacterBase : public ACharacter, public IAbilitySystemInterface, public IDlgDialogueParticipant
{
	GENERATED_BODY()

public:
	ARPGCharacterBase();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StopMovement();

	UFUNCTION(BlueprintCallable)
		void DelayedDestroy();
	UFUNCTION(BlueprintCallable)
		virtual void Die();

	URPGAttributeSet* GetAttributeSet() const { return AttributeSet; }
	void SetAttributeSet(URPGAttributeSet* val) { AttributeSet = val; }

	AWeaponActorBase* GetCurrentWeaponActor() const { return CurrentWeaponActor; }
	void SetCurrentWeaponActor(AWeaponActorBase* val) { CurrentWeaponActor = val; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void UseItemPotion();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities")
		bool DoMeleeAttack();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities")
		bool IsUsingMelee();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		bool DoSkillAttack();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities")
		bool IsUsingSkill();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsAlive() { return GetHealth() > 0.f; }

protected:
	//////////////////////////
	// ABILITIES
	//////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool CanUseAnyAbility();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool CanUseAnyWeapon();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, bool bAllowRemoteActivation = true);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetActiveAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

	bool SetupDefaultAttributes();
	void OnItemSlotChanged(FRPGItemSlot ItemSlot, URPGItem* Item);
	void RefreshSlottedGameplayAbilities();
	void AddStartupGameplayAbilities();
	void RemoveStartupGameplayAbilities();
	void AddSlottedGameplayAbilities();
	void FillSlottedAbilitySpecs(TMap<FRPGItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);
	void RemoveSlottedGameplayAbilities(bool bRemoveAll);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		URPGWeaponItem* DefaultAttackStarter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		FRPGItemSlot DefaultWeaponSlot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<URPGGameplayAbility>> GameplayAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TMap<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>> DefaultStarterSlottedAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FRPGItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;
	UPROPERTY()
		TScriptInterface<IPlayerInventoryInterface> InventorySource;
	UPROPERTY()
		URPGAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		URPGAttributeSet* AttributeSet;
	UPROPERTY()
		bool bAbilitiesInitialized;

	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;
	friend URPGAttributeSet;

protected:
	////////////////////////
	// EVENT HANDLERS
	////////////////////////

	virtual void HandleHitReactDuringAnimation();
	UFUNCTION(BlueprintImplementableEvent)
		void HandleMaterialHitFlicker();
	UFUNCTION()
		virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	UFUNCTION()
		virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	UFUNCTION()
		virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	UFUNCTION()
		virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

private:

	bool WasHitFromFront(const FVector& ImpactPoint);

	bool IsPlayingHighPriorityMontage();
	void PlayHighPriorityMontage(UAnimMontage* InMontage, FName StartSectionName);

protected:

	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
		int32 CharacterLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
		FCharacterAnimationStruct Animations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDecalComponent* IndicatorDecal;
	UPROPERTY(BlueprintReadOnly)
		TArray<UMaterialInstanceDynamic*> DynamicMeshMaterials;
	UPROPERTY(BlueprintReadOnly)
		AWeaponActorBase* CurrentWeaponActor;

private:

	UAnimMontage* HighPriorityMontage;	
	bool bCharacterCanMove;

public:
	FORCEINLINE virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override { return Cast<UAbilitySystemComponent>(AbilitySystemComponent); }
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool CanCharacterCanMove() const { return bCharacterCanMove; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetCharacterCanMove(bool val) { bCharacterCanMove = val; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		TArray<UMaterialInstanceDynamic*> GetDynamicMeshMaterials() const { return DynamicMeshMaterials; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetDynamicMeshMaterials(TArray<UMaterialInstanceDynamic*> val) { DynamicMeshMaterials = val; }
	UFUNCTION(BlueprintCallable)
		virtual float GetHealth() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMana() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxMana() const;
	UFUNCTION(BlueprintCallable)
		virtual float GetMoveSpeed() const;
	UFUNCTION(BlueprintCallable)
		virtual int32 GetCharacterLevel() const;
	UFUNCTION(BlueprintCallable)
		virtual bool SetCharacterLevel(int32 NewLevel);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DlgData, meta = (AllowPrivateAccess = "true"))
		FCharacterDialogueData CharDialogueData;

	FName GetParticipantName_Implementation() const override { return CharDialogueData.DlgParticipantName; }
	FText GetParticipantDisplayName_Implementation(FName ActiveSpeaker) const override { return CharDialogueData.DlgParticipantDisplayName; }
	UTexture2D* GetParticipantIcon_Implementation(FName ActiveSpeaker, FName ActiveSpeakerState) const override { return CharDialogueData.DlgParticipantIcon; }

	bool ModifyIntValue_Implementation(const FName ValueName, bool bDelta, int32 Value) override;
	bool ModifyFloatValue_Implementation(const FName ValueName, bool bDelta, float Value) override;
	bool ModifyBoolValue_Implementation(const FName ValueName, bool bValue) override;
	bool ModifyNameValue_Implementation(const FName ValueName, const FName NameValue) override;

	float GetFloatValue_Implementation(const FName ValueName) const override;
	int32 GetIntValue_Implementation(const FName ValueName) const override;
	bool GetBoolValue_Implementation(const FName ValueName) const override;
	FName GetNameValue_Implementation(const FName ValueName) const override;

	bool OnDialogueEvent_Implementation(UDlgContext* Context, FName EventName) override { return false; }
	bool CheckCondition_Implementation(const UDlgContext* Context, FName ConditionName) const override { return false; }
};
