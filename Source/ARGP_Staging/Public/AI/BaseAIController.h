// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

class ARPGCharacterBase;
class UBehaviorTreeComponent;
class UAIPerceptionComponent;
class UBehaviorTree;
class UBlackboardComponent;
class UAISenseConfig_Sight;
class ATextRenderActor;


/**
 * 
 */
UCLASS()
class ARGP_STAGING_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
public:

	ABaseAIController();

	virtual void OnPossess(APawn* InPawn);

	UFUNCTION(BlueprintNativeEvent, Category = "BaseAIController")
		void HandlePawnDeath();
	virtual void HandlePawnDeath_Implementation();

	virtual ARPGCharacterBase* GetPossessedPawn();

	UFUNCTION()
		virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	virtual void SetupAIController();
	virtual AActor* GetSeeingPawn();

protected:

	UPROPERTY(EditDefaultsOnly)
		UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly)
		bool bShouldRun;

private:

	ARPGCharacterBase* CurrentPawn;
	AActor* CurrentTarget;

	bool bInCombat;
	bool bPossessed;

	TArray<AActor*> VisibleEnemies;

	FName BlackboardEnemyKey;
	FName BlackboardCanSeeEnemyKey;

	UAISenseConfig_Sight* Sight;
	UAIPerceptionComponent* AIPerceptionComponent;
	UBehaviorTreeComponent* BehaviorTreeComponent;
	UBlackboardComponent* CurrentBlackboard;
};
