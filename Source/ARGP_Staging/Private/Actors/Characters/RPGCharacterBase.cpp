// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/RPGCharacterBase.h"
#include "Inventory/Items/RPGItem.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Components/PostProcessComponent.h"
#include "DlgDialogueParticipant.h"
#include "AbilitySystemGlobals.h"
#include "Player/RPGPlayerControllerBase.h"
#include "TimerManager.h"
#include "Inventory/Items/RPGWeaponItem.h"
#include "Actors/Weapons/WeaponActorBase.h"
#include "Inventory/InventoryComponent.h"
#include "Utils/RPGBlueprintLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/RPGGameplayAbility.h"

ARPGCharacterBase::ARPGCharacterBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	SetAttributeSet(CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet")));

	IndicatorDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("IndicatorDecal"));
	IndicatorDecal->SetupAttachment(RootComponent);

	CharacterLevel = 1;
	bAbilitiesInitialized = false;
	bCharacterCanMove = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false; 

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ARPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	ARPGPlayerControllerBase* CurrCon = Cast<ARPGPlayerControllerBase>(Controller);
	if (GetAttributeSet()) {
		GetAttributeSet()->OnDamaged.AddDynamic(this, &ARPGCharacterBase::HandleDamage);
		GetAttributeSet()->OnManaChanged.AddDynamic(this, &ARPGCharacterBase::HandleManaChanged);
		GetAttributeSet()->OnHealthChanged.AddDynamic(this, &ARPGCharacterBase::HandleHealthChanged);
		GetAttributeSet()->OnMoveSpeedChanged.AddDynamic(this, &ARPGCharacterBase::HandleMoveSpeedChanged);
	}
}

void ARPGCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InventorySource = NewController;

	if (InventorySource)
	{
		InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &ARPGCharacterBase::OnItemSlotChanged);
		InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &ARPGCharacterBase::RefreshSlottedGameplayAbilities);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();

		if (DefaultAttackStarter) {
			FTransform TempTrans = FTransform();
			AWeaponActorBase* TempWeapon = URPGBlueprintLibrary::SpawnWeaponActor(DefaultAttackStarter->WeaponActor, TempTrans, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, this, DefaultAttackStarter, DefaultWeaponSlot, this);
			if (TempWeapon) {
				TempWeapon->SetInstigator(this);
				TempWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponHandMount_rSocket");
				SetCurrentWeaponActor(TempWeapon);
			}
		}
	}
	bAbilitiesInitialized = SetupDefaultAttributes();
}

void ARPGCharacterBase::UnPossessed()
{
	if (InventorySource && InventoryUpdateHandle.IsValid())
	{
		InventorySource->GetSlottedItemChangedDelegate().Remove(InventoryUpdateHandle);
		InventoryUpdateHandle.Reset();

		InventorySource->GetInventoryLoadedDelegate().Remove(InventoryLoadedHandle);
		InventoryLoadedHandle.Reset();
	}
	InventorySource = nullptr;
}

void ARPGCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

void ARPGCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGCharacterBase, CharacterLevel);
}

bool ARPGCharacterBase::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		CharacterLevel = NewLevel;
		return true;
	}
	return false;
}

void ARPGCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ARPGCharacterBase::CanUseAnyAbility()
{
	return IsAlive() && !UGameplayStatics::IsGamePaused(this) && !IsUsingSkill();
}

bool ARPGCharacterBase::CanUseAnyWeapon()
{
	return IsAlive() && !UGameplayStatics::IsGamePaused(this) && !IsUsingMelee();
}

bool ARPGCharacterBase::WasHitFromFront(const FVector& ImpactPoint)
{
	const FVector& ActorLocation = GetActorLocation();
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());

	if (DistanceToRightLeftPlane >= 0)
	{
		return true;
	}
	else
	{
		return false;
	
	}  
	return true;
}

bool ARPGCharacterBase::IsPlayingHighPriorityMontage()
{
	return GetMesh()->GetAnimInstance()->Montage_IsPlaying(HighPriorityMontage);
}

void ARPGCharacterBase::PlayHighPriorityMontage(UAnimMontage* InMontage, FName StartSectionName)
{
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(InMontage)) {
		HighPriorityMontage = InMontage;
		PlayAnimMontage(HighPriorityMontage, 1.f, StartSectionName);
	}
}

void ARPGCharacterBase::DelayedDestroy()
{
	GetWorld()->DestroyActor(CurrentWeaponActor);
	GetWorld()->DestroyActor(this);
}

void ARPGCharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	if (Animations.DeathMontage)
	{
		float AnimDuration = PlayAnimMontage(Animations.DeathMontage);
		if (GetLocalRole() < ROLE_Authority)
			return;
		FTimerHandle DeathTimer;
		GetWorldTimerManager().SetTimer(DeathTimer, this, &ARPGCharacterBase::DelayedDestroy, AnimDuration - .25, false);
	}
	else
	{
		if (GetLocalRole() < ROLE_Authority)
			return;
		DelayedDestroy();
	}
}

bool ARPGCharacterBase::SetupDefaultAttributes()
{
	if (!AbilitySystemComponent)
	{
		return false;
	}
	return false;
}

void ARPGCharacterBase::OnItemSlotChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
	RefreshSlottedGameplayAbilities();
}

void ARPGCharacterBase::RefreshSlottedGameplayAbilities()
{
	if (bAbilitiesInitialized)
	{
		RemoveSlottedGameplayAbilities(false);
		AddSlottedGameplayAbilities();
	}
}

void ARPGCharacterBase::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		for (TSubclassOf<URPGGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}

		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		AddSlottedGameplayAbilities();

		bAbilitiesInitialized = true;
	}
}

bool ARPGCharacterBase::ActivateAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}

	return false;
}

void ARPGCharacterBase::GetActiveAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);

		if (FoundSpec)
		{
			TArray<UGameplayAbility*> AbilityInstances = FoundSpec->GetAbilityInstances();

			for (UGameplayAbility* ActiveAbility : AbilityInstances)
			{
				ActiveAbilities.Add(Cast<URPGGameplayAbility>(ActiveAbility));
			}
		}
	}
}

bool ARPGCharacterBase::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

bool ARPGCharacterBase::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

void ARPGCharacterBase::RemoveStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && bAbilitiesInitialized)
	{
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && GameplayAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
		{
			AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		}

		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);

		RemoveSlottedGameplayAbilities(true);

		bAbilitiesInitialized = false;
	}
}

void ARPGCharacterBase::AddSlottedGameplayAbilities()
{
	TMap<FRPGItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	FillSlottedAbilitySpecs(SlottedAbilitySpecs);

	for (const TPair<FRPGItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpecs)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid())
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
		}
	}
}

void ARPGCharacterBase::FillSlottedAbilitySpecs(TMap<FRPGItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{	
	for (const TPair<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>>& DefaultPair : DefaultStarterSlottedAbilities)
	{
		if (DefaultPair.Value.Get())
		{
			SlottedAbilitySpecs.Add(DefaultPair.Key, FGameplayAbilitySpec(DefaultPair.Value, GetCharacterLevel(), INDEX_NONE, this));
		}
	}

	if (InventorySource)
	{
		const TMap<FRPGItemSlot, URPGItem*>& SlottedItemMap = InventorySource->GetSlottedItemMap();

		for (const TPair<FRPGItemSlot, URPGItem*>& ItemPair : SlottedItemMap)
		{
			URPGItem* SlottedItem = ItemPair.Value;
			int32 AbilityLevel = GetCharacterLevel();

			if (SlottedItem && SlottedItem->ItemType.GetName() == FName(TEXT("Weapon")))
			{
				AbilityLevel = SlottedItem->AbilityLevel;
			}

			if (SlottedItem && SlottedItem->GrantedAbility)
			{
				SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, AbilityLevel, INDEX_NONE, SlottedItem));
			}
		}
	}

}

void ARPGCharacterBase::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	TMap<FRPGItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;

	if (!bRemoveAll)
	{
		FillSlottedAbilitySpecs(SlottedAbilitySpecs);
	}

	for (TPair<FRPGItemSlot, FGameplayAbilitySpecHandle>& ExistingPair : SlottedAbilities)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(ExistingPair.Value);
		bool bShouldRemove = bRemoveAll || !FoundSpec;

		if (!bShouldRemove)
		{
			FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(ExistingPair.Key);

			if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject)
			{
				bShouldRemove = true;
			}
		}

		if (bShouldRemove)
		{
			if (FoundSpec)
			{
				AbilitySystemComponent->ClearAbility(ExistingPair.Value);
			}
			ExistingPair.Value = FGameplayAbilitySpecHandle();
		}
	}
}

void ARPGCharacterBase::HandleHitReactDuringAnimation()
{
	return;
}

bool ARPGCharacterBase::ModifyIntValue_Implementation(const FName ValueName, bool bDelta, int32 Value)
{
	if (!CharDialogueData.Integers.Contains(ValueName))
		CharDialogueData.Integers.Add(ValueName, 0);

	if (bDelta)
		CharDialogueData.Integers[ValueName] += Value;
	else
		CharDialogueData.Integers[ValueName] = Value;

	return true;

}

bool ARPGCharacterBase::ModifyFloatValue_Implementation(const FName ValueName, bool bDelta, float Value)
{
	if (!CharDialogueData.Floats.Contains(ValueName))
		CharDialogueData.Floats.Add(ValueName, 0.0f);

	if (bDelta)
		CharDialogueData.Floats[ValueName] += Value;
	else
		CharDialogueData.Floats[ValueName] = Value;

	return true;

}

bool ARPGCharacterBase::ModifyBoolValue_Implementation(const FName ValueName, bool bValue)
{
	if (bValue)
		CharDialogueData.TrueBools.Add(ValueName);
	else
		CharDialogueData.TrueBools.Remove(ValueName);

	return true;

}

bool ARPGCharacterBase::ModifyNameValue_Implementation(const FName ValueName, const FName NameValue)
{
	if (CharDialogueData.Names.Contains(ValueName))
		CharDialogueData.Names[ValueName] = NameValue;
	else
		CharDialogueData.Names.Add(ValueName, NameValue);

	return true;

}

float ARPGCharacterBase::GetFloatValue_Implementation(const FName ValueName) const
{
	return CharDialogueData.Floats.Contains(ValueName) ? CharDialogueData.Floats[ValueName] : 0.0f;
}

int32 ARPGCharacterBase::GetIntValue_Implementation(const FName ValueName) const
{
	return CharDialogueData.Integers.Contains(ValueName) ? CharDialogueData.Integers[ValueName] : 0;
}

bool ARPGCharacterBase::GetBoolValue_Implementation(const FName ValueName) const
{
	return CharDialogueData.TrueBools.Contains(ValueName);
}

FName ARPGCharacterBase::GetNameValue_Implementation(const FName ValueName) const
{
	return CharDialogueData.Names.Contains(ValueName) ? CharDialogueData.Names[ValueName] : NAME_None;
}

void ARPGCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacterBase* InstigatorPawn, AActor* DamageCauser)
{
	if (!IsAlive()) {
		return;
	}
	HandleMaterialHitFlicker();
	if (!IsUsingMelee()) {
		if (WasHitFromFront(HitInfo.ImpactPoint)) {
			PlayAnimMontage(Animations.HitFront);
		}
		else {
			PlayAnimMontage(Animations.HitBack);
		}
	}
	else {
		HandleHitReactDuringAnimation();
	}
}

void ARPGCharacterBase::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		if (!IsAlive()) {
			Die();
		}
	}
}

void ARPGCharacterBase::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{

	}
}

void ARPGCharacterBase::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();

	if (bAbilitiesInitialized)
	{

	}
}

float ARPGCharacterBase::GetHealth() const
{
	return GetAttributeSet()->GetHealth();
}

float ARPGCharacterBase::GetMaxHealth() const
{
	return GetAttributeSet()->GetMaxHealth();
}

float ARPGCharacterBase::GetMana() const
{
	return GetAttributeSet()->GetMana();
}

float ARPGCharacterBase::GetMaxMana() const
{
	return GetAttributeSet()->GetMaxMana();
}

float ARPGCharacterBase::GetMoveSpeed() const
{
	return GetAttributeSet()->GetMoveSpeed();
}

int32 ARPGCharacterBase::GetCharacterLevel() const
{
	return CharacterLevel;
}

void ARPGCharacterBase::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
}