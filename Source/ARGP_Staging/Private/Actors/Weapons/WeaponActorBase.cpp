#include "Actors/Weapons/WeaponActorBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Actors/InteractableObject.h"
#include "TimerManager.h"

AWeaponActorBase::AWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(RootComponent);
	CapsuleCollision->SetCollisionProfileName("OverlapOnlyPawn");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();
	CapsuleCollision->IgnoreActorWhenMoving(this, true);
	CapsuleCollision->SetGenerateOverlapEvents(false);
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActorBase::ActorBeginOverlap);
	CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponActorBase::ActorEndOverlap);
}

void AWeaponActorBase::BeginWeaponAttack(FGameplayTag InTag, float InAttackDelayTime, int32 InAttackDelayCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Attack called"));
	CapsuleCollision->SetGenerateOverlapEvents(true);
	AttackEventTag = InTag;
	AttackDelayTime = InAttackDelayTime;
	AttackDelayCount = InAttackDelayCount;
	bIsAttacking = true;
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponActorBase::EndWeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("End Attack called"));
	CapsuleCollision->SetGenerateOverlapEvents(false);
	bIsAttacking = false;
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActorBase::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin called"));
	if (bIsAttacking && !bActorHasBeenHit && OtherActor && OtherActor != GetInstigator() && OtherActor->Implements<UAbilitySystemInterface>()) {
		bActorHasBeenHit = true;
 		FGameplayEventData EventData;
		EventData.Target = OtherActor;
		EventData.Instigator = GetInstigator();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), AttackEventTag, EventData);
		HitPause();
	}
}

void AWeaponActorBase::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End called"));
	FTimerHandle  DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]() {
		bActorHasBeenHit = false;
	}, .2f, false);
}