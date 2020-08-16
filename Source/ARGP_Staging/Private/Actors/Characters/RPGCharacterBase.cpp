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
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/RPGGameplayAbility.h"

ARPGCharacterBase::ARPGCharacterBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	SetAttributeSet(CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet")));

	CharacterLevel = 1;
	bAbilitiesInitialized = false;

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

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		if (DefaultAttackStarter) {
			GiveAbilityAndAddToInventory(ECombatHotkeys::DEFAULT_ATTACK, DefaultAttackStarter->GrantedAbility);
			URPGWeaponItem* WeaponItem = Cast<URPGWeaponItem>(DefaultAttackStarter);
			if (WeaponItem) {
				FTransform TempTrans = FTransform();
				AWeaponActorBase* TempWeapon = URPGBlueprintLibrary::SpawnWeaponActor(WeaponItem->WeaponActor, TempTrans, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, this, WeaponItem, DefaultWeaponSlot, this);
				if (TempWeapon) {
					TempWeapon->SetInstigator(this);
					TempWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponHandMount_rSocket");
					InventoryComponent->SetCurrentWeapon(TempWeapon);
				}
			}
		}
	}
	bAbilitiesInitialized = SetupDefaultAttributes();
}

void ARPGCharacterBase::UnPossessed()
{

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

void ARPGCharacterBase::GiveAbilityAndAddToInventory(ECombatHotkeys InHotkey, TSubclassOf<URPGGameplayAbility> AbilityClass)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
		InventoryComponent->AddAbilityToCombatMap(ECombatHotkeys::DEFAULT_ATTACK, AbilityClass);
	}
}

void ARPGCharacterBase::ActivateAbilityByClass(TSubclassOf<URPGGameplayAbility> AbilityClass)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass, false);
	}
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
	if (InventoryComponent && InventoryComponent->GetCurrentWeapon()) {
		GetWorld()->DestroyActor(InventoryComponent->GetCurrentWeapon());
	}
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

	if (!DefaultAttributes)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1.0f, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		return true;
	}
	return false;
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
	if (IsAlive() && !IsUsingMelee()) {
		if (WasHitFromFront(HitInfo.ImpactPoint)) {
			PlayAnimMontage(Animations.HitFront);
		}
		else {
			PlayAnimMontage(Animations.HitBack);
		}
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