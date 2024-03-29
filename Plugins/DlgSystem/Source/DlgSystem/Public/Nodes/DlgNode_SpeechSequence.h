// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "Nodes/DlgNode.h"
#include "DlgSystemSettings.h"
#include "DlgNode_SpeechSequence.generated.h"

class USoundWave;
class USoundBase;
class UDialogueWave;

USTRUCT(BlueprintType, Blueprintable)
struct DLGSYSTEM_API FDlgSpeechSequenceEntry
{
	GENERATED_USTRUCT_BODY()
	// NOTE: don't create a default constructor here, because otherwise if will fail because some CDO BS after you convert nodes to speech sequence

public:
	/** The Participant Name (speaker) associated with this speech entry. */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (DisplayName = "Participant Name"))
	FName Speaker;

	/** Text that will appear when this node participant name speaks to someone else. */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (MultiLine = true))
	FText Text;

	/** Text that will appear when you want to continue down this edge to the next conversation. Usually "Next". */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (MultiLine = true))
	FText EdgeText;

	/** State of the speaker attached to the entry. Passed to the GetParticipantIcon function. */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData)
	FName SpeakerState;

	/** Node data that you can customize yourself with your own data types */
	UPROPERTY(EditAnywhere, Instanced, Category = DialogueNodeData)
	UDlgNodeData* NodeData;

	// Voice attached to this node. The Sound Wave variant.
	// NOTE: You should probably use the NodeData
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (DlgSaveOnlyReference))
	USoundBase* VoiceSoundWave = nullptr;

	// Voice attached to this node. The Dialogue Wave variant. Only the first wave from the dialogue context array should be used.
	// NOTE: You should probably use the NodeData
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (DlgSaveOnlyReference))
	UDialogueWave* VoiceDialogueWave = nullptr;

	// Any generic object you would like
	// NOTE: You should probably use the NodeData
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (DlgSaveOnlyReference))
	UObject* GenericData = nullptr;
};


/**
 * Sequence of speeches - each can have a different speaker independently from the node owner.
 * The node stays active and proceeds one step in the SpeechSequence (internal) array until everyone said everything.
 */
UCLASS(BlueprintType)
class DLGSYSTEM_API UDlgNode_SpeechSequence : public UDlgNode
{
	GENERATED_BODY()

public:
	// Begin UObject Interface.
	/** @return a one line description of an object. */
	FString GetDesc() override
	{
		return TEXT("Sequence of speeches - each can have a different speaker independently from the node owner.\nThe node stays active and proceeds one step in the SpeechSequence (internal) array until everyone said everything.");
	}
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Begin UDlgNode interface
	void UpdateTextsValuesFromDefaultsAndRemappings(const UDlgSystemSettings* Settings, bool bEdges, bool bUpdateGraphNode = true) override;
	void UpdateTextsNamespacesAndKeys(const UDlgSystemSettings* Settings, bool bEdges, bool bUpdateGraphNode = true) override;
	bool HandleNodeEnter(UDlgContext* Context, TSet<const UDlgNode*> NodesEnteredWithThisStep) override;
	bool ReevaluateChildren(UDlgContext* Context, TSet<const UDlgNode*> AlreadyEvaluated) override;
	bool OptionSelected(int32 OptionIndex, UDlgContext* Context) override;

	// Getters
	const FText& GetNodeText() const override;
	UDlgNodeData* GetNodeData() const override;
	USoundBase* GetNodeVoiceSoundBase() const override;
	UDialogueWave* GetNodeVoiceDialogueWave() const override;
	FName GetSpeakerState() const override;
	void AddAllSpeakerStatesIntoSet(TSet<FName>& OutStates) const override;
	UObject* GetGenericData() const override;
	FName GetNodeParticipantName() const override;
	void GetAssociatedParticipants(TArray<FName>& OutArray) const override;

#if WITH_EDITOR
	FString GetNodeTypeString() const override { return TEXT("Speech Sequence"); }
#endif

	// Begin own functions
	/** Fills the inner edges from the corresponding  input data (SpeechSequence)  */
	void AutoGenerateInnerEdges();

	/** Gets the SpeechSequence as a const array */
	const TArray<FDlgSpeechSequenceEntry>& GetNodeSpeechSequence() const { return SpeechSequence; }

	/** Gets the SpeechSequence as a mutable array */
	TArray<FDlgSpeechSequenceEntry>* GetMutableNodeSpeechSequence() { return &SpeechSequence; }

	/** Tells us if the speech sequence has any speeches (aka not empty) */
	bool HasSpeechSequences() const { return SpeechSequence.Num() > 0; }

	/** Helper functions to get the names of some properties. Used by the DlgSystemEditor module. */
	static FName GetMemberNameSpeechSequence() { return GET_MEMBER_NAME_CHECKED(UDlgNode_SpeechSequence, SpeechSequence); }

protected:
	/** Array of important stuff to say */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData)
	TArray<FDlgSpeechSequenceEntry> SpeechSequence;

	/** inner edge, filled automatically based on SpeechSequence */
	UPROPERTY()
	TArray<FDlgEdge> InnerEdges;

	/** The current active index in the SpeechSequence array */
	int32 ActualIndex = INDEX_NONE;
};
