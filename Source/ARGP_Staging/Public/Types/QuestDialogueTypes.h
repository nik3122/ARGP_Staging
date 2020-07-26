// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestDialogueTypes.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FCharacterDialogueData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DlgData, meta = (AllowPrivateAccess = "true"))
		FName DlgParticipantName = FName("MyCharacterName");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DlgData, meta = (AllowPrivateAccess = "true"))
		FText DlgParticipantDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DlgData, meta = (AllowPrivateAccess = "true"))
		class UTexture2D* DlgParticipantIcon;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TMap<FName, int32> Integers;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TMap<FName, int32> Floats;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TMap<FName, FName> Names;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TSet<FName> TrueBools;
};
