// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BaseAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Characters/RPGCharacterBase.h"

FBaseAnimInstanceProxy::FBaseAnimInstanceProxy(UAnimInstance* Instance)
{

}

void FBaseAnimInstanceProxy::Update(float DeltaSeconds)
{
	UpdatePawnOwner();
	if (PlayerCharacter && AnimInstance) {
		bIsFalling = PlayerCharacter->GetCharacterMovement()->IsFalling();
		Speed = PlayerCharacter->GetVelocity().Size();
		FRotator CharRot = PlayerCharacter->GetActorRotation();
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(PlayerCharacter->GetBaseAimRotation(), UKismetMathLibrary::Conv_VectorToRotator(PlayerCharacter->GetVelocity())).Yaw * -1.f;

		bIsJumping = PlayerCharacter->bWasJumping;
		bIsMoving = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	}
}

void FBaseAnimInstanceProxy::SetAnimInstance(UAnimInstance* InInstance)
{
	AnimInstance = InInstance;
}


void FBaseAnimInstanceProxy::UpdatePawnOwner()
{
	if (AnimInstance) {
		if (!PlayerCharacter) {
			PlayerCharacter = Cast<ARPGCharacterBase>(AnimInstance->TryGetPawnOwner());
		}
	}
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Proxy.SetAnimInstance(this);
}
