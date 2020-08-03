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
#include "Actors/InteractableObject.h"
#include "RPGCharacterBase.generated.h"


class URPGGameplayAbility;
class UGameplayEffect;
class AWeaponActorBase;
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

	bool IsPlayingHighPriorityMontage();
	void PlayHighPriorityMontage(UAnimMontage* InMontage, FName StartSectionName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		bool DoMeleeAttack();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities")
		bool IsUsingMelee();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		bool DoSkillAttack();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Abilities")
		bool IsUsingSkill();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void UseItemPotion();

	UFUNCTION(BlueprintCallable)
		void DelayedDestroy();
	UFUNCTION(BlueprintCallable)
		virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GiveAbilityAndAddToInventory(ECombatHotkeys InHotkey, TSubclassOf<URPGGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void ActivateAbilityByClass(TSubclassOf<URPGGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsAlive() { return GetHealth() > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool CanUseAnyAbility();

	UFUNCTION(BlueprintPure)
		bool WasHitFromFront(const FVector& ImpactPoint);

protected:

	UFUNCTION(BlueprintImplementableEvent)
		void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent)
		void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent)
		void HandleOutlineChange();

	bool SetupDefaultAttributes();

	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack)
		int32 AttackDelayCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack)
		float AttackDelayTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack)
		FName MeleeStartSection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
		bool bIsProtectedByShield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
		bool bInvincible;

	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
		int32 CharacterLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		URPGItem* DefaultAttackStarter;

	UPROPERTY(BlueprintReadOnly)
		UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
		FCharacterAnimationStruct Animations;

private:

	UAnimMontage* HighPriorityMontage;	

	UPROPERTY()
		URPGAbilitySystemComponent* AbilitySystemComponent;
	
	UPROPERTY()
		URPGAttributeSet* AttributeSet;

	UPROPERTY()
		bool bAbilitiesInitialized;

	friend URPGAttributeSet;

public:
	FORCEINLINE virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override { return Cast<UAbilitySystemComponent>(AbilitySystemComponent); }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int32 GetAttackDelayCount() const { return AttackDelayCount; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetAttackDelayCount(int32 val) { AttackDelayCount = val; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		float GetAttackDelayTime() const { return AttackDelayTime; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetAttackDelayTime(float val) { AttackDelayTime = val; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		FName GetMeleeStartSection() const { return MeleeStartSection; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetMeleeStartSection(FName val) { MeleeStartSection = val; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsProtectedByShield() const { return bIsProtectedByShield; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetIsProtectedByShield(bool val) { bIsProtectedByShield = val; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsInvincible() const { return bInvincible; }
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetIsInvincible(bool val) { bInvincible = val; }
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

	UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }
};
