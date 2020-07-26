// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "BaseAnimInstance.generated.h"

class ARPGCharacterBase;

USTRUCT(BlueprintType)
struct FBaseAnimInstanceProxy : public FAnimInstanceProxy
{

	GENERATED_BODY()

		FBaseAnimInstanceProxy()

		: FAnimInstanceProxy()

	{}

	FBaseAnimInstanceProxy(UAnimInstance* Instance);

	virtual void Update(float DeltaSeconds) override;

	void SetAnimInstance(UAnimInstance* InInstance);

private:

	void UpdatePawnOwner();

	UAnimInstance* AnimInstance;
	ARPGCharacterBase* PlayerCharacter;

public:

	// CHAR VALUES

	UPROPERTY(transient, EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsFalling;

	UPROPERTY(transient, EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsMoving;

	UPROPERTY(transient, EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsJumping;

	UPROPERTY(transient, EditAnywhere, BlueprintReadWrite, Category = Animation)
		float Speed;

	UPROPERTY(transient, EditAnywhere, BlueprintReadWrite, Category = Animation)
		float Direction;
};


/**
 * 
 */
UCLASS()
class ARGP_STAGING_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

private:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "BaseAnimInstanceProxy", meta = (AllowPrivateAccess = "true"))
		FBaseAnimInstanceProxy Proxy;

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override {
		return &Proxy;
	}

	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {

	}

	friend struct FBaseAnimInstanceProxy;
	
};
