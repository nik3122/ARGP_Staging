// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Actors/Characters/RPGCharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"

ABaseAIController::ABaseAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(FName("BehaviorTreeComponent"));
	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(FName("BehaviorTree"));
	CurrentBlackboard = CreateDefaultSubobject<UBlackboardComponent>(FName("BlackboardComp"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("AIPerceptionComp"));

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
	Sight->SightRadius = 2000.f;
	Sight->LoseSightRadius = 2500.f;
	Sight->PeripheralVisionAngleDegrees = 60.f;
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	BlackboardEnemyKey = "TargetActor";
	BlackboardCanSeeEnemyKey = "CanSeeEnemy";

	AIPerceptionComponent->ConfigureSense(*Sight);
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	FRotator InRot;
	FVector InLoc;
	GetPlayerViewPoint(InLoc, InRot);
	InPawn->SetActorLocationAndRotation(InLoc, InRot);
	InPawn->AddActorLocalRotation(FRotator(0.f, -90.f, 0.f));
	CurrentPawn = Cast<ARPGCharacterBase>(InPawn);
	bPossessed = true;
	CurrentPawn->GetAbilitySystemComponent()->InitAbilityActorInfo(CurrentPawn, CurrentPawn);
	SetupAIController();

}

void ABaseAIController::HandlePawnDeath_Implementation()
{
	UnPossess();
	bPossessed = false;
	BehaviorTreeComponent->StopTree();
}

ARPGCharacterBase* ABaseAIController::GetPossessedPawn()
{
	return CurrentPawn;
}

void ABaseAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors) {
		ARPGCharacterBase* CurrPawn = Cast<ARPGCharacterBase>(Actor);
		if (CurrPawn && CurrentPawn && CurrPawn->IsAlive() && !GetSeeingPawn())
		{
			CurrentBlackboard->SetValueAsObject(BlackboardEnemyKey, Actor);
		}
	}
}

void ABaseAIController::SetupAIController()
{
	if (BehaviorTree && bShouldRun)
	{
		CurrentBlackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
	AIPerceptionComponent->OnPerceptionUpdated.RemoveAll(this);
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerceptionUpdated);
}

AActor* ABaseAIController::GetSeeingPawn()
{
	UObject* object = CurrentBlackboard->GetValueAsObject(BlackboardEnemyKey);
	return object ? Cast<AActor>(object) : nullptr;
}
