// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/Build.h"
#include "UObject/Object.h"

#if WITH_EDITOR
#include "EdGraph/EdGraphNode.h"
#endif

#include "DlgEdge.h"
#include "DlgCondition.h"
#include "DlgEvent.h"
#include "DlgNodeData.h"
#include "DlgNode.generated.h"


class UDlgSystemSettings;
class UDlgContext;
class UDlgNode;
class USoundBase;
class USoundWave;
class UDialogueWave;
struct FDlgTextArgument;
class UDlgDialogue;

/**
 *  Abstract base class for Dialogue nodes
 *  Depending on the implementation in the child class the dialogue node can contain one or more lines of one or more participants,
 *  or simply some logic to go on in the UDlgNode graph
 */
UCLASS(BlueprintType, Abstract, EditInlineNew)
class DLGSYSTEM_API UDlgNode : public UObject
{
	GENERATED_BODY()

public:
	// Begin UObject Interface.
	/** UObject serializer. */
	void Serialize(FArchive& Ar) override;

	/** @return a one line description of an object. */
	FString GetDesc() override { return TEXT("INVALID DESCRIPTION"); }

#if WITH_EDITOR
	/**
	 * Called when a property on this object has been modified externally
	 *
	 * @param PropertyChangedEvent the property that was modified
	 */
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/**
	 * This alternate version of PostEditChange is called when properties inside structs are modified.  The property that was actually modified
	 * is located at the tail of the list.  The head of the list of the FNYStructProperty member variable that contains the property that was modified.
	 */
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	/**
	 * Callback used to allow object register its direct object references that are not already covered by
	 * the token stream.
	 *
	 * @param InThis Object to collect references from.
	 * @param Collector	FReferenceCollector objects to be used to collect references.
	*/
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	// End UObject Interface.

	// Begin own function
	// Used internally by the Dialogue editor:
	virtual FString GetNodeTypeString() const { return TEXT("INVALID"); }
#endif //WITH_EDITOR

#if WITH_EDITOR
	void SetGraphNode(UEdGraphNode* InNode) { GraphNode = InNode; }
	void ClearGraphNode() { GraphNode = nullptr; }
	UEdGraphNode* GetGraphNode() const { return GraphNode; }
#endif

	/** Broadcasts whenever a property of this dialogue changes. */
	DECLARE_EVENT_TwoParams(UDlgNode, FDialogueNodePropertyChanged, const FPropertyChangedEvent& /* PropertyChangedEvent */, int32 /* EdgeIndexChanged */);
	FDialogueNodePropertyChanged OnDialogueNodePropertyChanged;

	virtual bool HandleNodeEnter(UDlgContext* Context, TSet<const UDlgNode*> NodesEnteredWithThisStep);

	virtual bool ReevaluateChildren(UDlgContext* Context, TSet<const UDlgNode*> AlreadyEvaluated);

	virtual bool CheckNodeEnterConditions(const UDlgContext* Context, TSet<const UDlgNode*> AlreadyVisitedNodes) const;
	virtual bool HasAnySatisfiedChild(const UDlgContext* Context, TSet<const UDlgNode*> AlreadyVisitedNodes) const;

	virtual bool OptionSelected(int32 OptionIndex, UDlgContext* Context);

	//
	// Getters/Setters:
	//

	//
	// For the ParticipantName
	//

	UFUNCTION(BlueprintPure, Category = DlgNode)
	virtual FName GetNodeParticipantName() const { return OwnerName; }

	virtual void SetNodeParticipantName(const FName& InName) { OwnerName = InName; }

	//
	// For the EnterConditions
	//

	virtual bool HasAnyEnterConditions() const { return GetNodeEnterConditions().Num() > 0; }
	virtual const TArray<FDlgCondition>& GetNodeEnterConditions() const { return EnterConditions; }
	virtual void SetNodeEnterConditions(const TArray<FDlgCondition>& InEnterConditions) { EnterConditions = InEnterConditions; }

	// Gets the mutable enter condition at location EnterConditionIndex.
	virtual FDlgCondition* GetMutableEnterConditionAt(int32 EnterConditionIndex)
	{
		check(EnterConditions.IsValidIndex(EnterConditionIndex));
		return &EnterConditions[EnterConditionIndex];
	}

	//
	// For the EnterEvents
	//

	virtual bool HasAnyEnterEvents() const { return GetNodeEnterEvents().Num() > 0; }
	virtual const TArray<FDlgEvent>& GetNodeEnterEvents() const { return EnterEvents; }
	virtual void SetNodeEnterEvents(const TArray<FDlgEvent>& InEnterEvents) { EnterEvents = InEnterEvents; }

	//
	// For the Children
	//

	/// Gets this nodes children (edges) as a const/mutable array
	virtual const TArray<FDlgEdge>& GetNodeChildren() const { return Children; }
	virtual void SetNodeChildren(const TArray<FDlgEdge>& InChildren) { Children = InChildren; }
	virtual int32 GetNumNodeChildren() const { return Children.Num(); }
	virtual const FDlgEdge& GetNodeChildAt(int32 EdgeIndex) const { return Children[EdgeIndex]; }


	// Adds an Edge to the end of the Children Array.
	virtual void AddNodeChild(const FDlgEdge& InChild) { Children.Add(InChild); }

	// Removes the Edge at the specified EdgeIndex location.
	virtual void RemoveChildAt(int32 EdgeIndex)
	{
		check(Children.IsValidIndex(EdgeIndex));
		Children.RemoveAt(EdgeIndex);
	}

	// Removes all edges/children
	virtual void RemoveAllChildren() { Children.Empty(); }

	// Gets the mutable edge/child at location EdgeIndex.
	virtual FDlgEdge* GetSafeMutableNodeChildAt(int32 EdgeIndex)
	{
		check(Children.IsValidIndex(EdgeIndex));
		return &Children[EdgeIndex];
	}

	// Unsafe version, can be null
	virtual FDlgEdge* GetMutableNodeChildAt(int32 EdgeIndex)
	{
		return Children.IsValidIndex(EdgeIndex) ? &Children[EdgeIndex] : nullptr;
	}

	// Gets the mutable Edge that corresponds to the provided TargetIndex or nullptr if nothing was found.
	virtual FDlgEdge* GetMutableNodeChildForTargetIndex(int32 TargetIndex);

	// Gets all the edges (children) indices that DO NOT have a valid TargetIndex (is negative).
	const TArray<int32> GetNodeOpenChildren_DEPRECATED() const;

	// Gathers associated participants, they are only added to the array if they are not yet there
	virtual void GetAssociatedParticipants(TArray<FName>& OutArray) const;

	// Updates the value of the texts from the default values or the remappings (if any)
	virtual void UpdateTextsValuesFromDefaultsAndRemappings(
		const UDlgSystemSettings* Settings, bool bEdges, bool bUpdateGraphNode = true
	);

	// Updates the namespace and key of all the texts depending on the settings
	virtual void UpdateTextsNamespacesAndKeys(const UDlgSystemSettings* Settings, bool bEdges, bool bUpdateGraphNode = true);

	// Rebuilds ConstructedText
	virtual void RebuildTextArguments(bool bEdges, bool bUpdateGraphNode = true);
	virtual void RebuildTextArgumentsFromPreview(const FText& Preview) {}

	// Constructs the ConstructedText.
	virtual void RebuildConstructedText(const UDlgContext* Context) {}

	// Gets the text arguments for this Node (if any). Used for FText::Format
	virtual const TArray<FDlgTextArgument>& GetTextArguments() const
	{
		static TArray<FDlgTextArgument> EmptyArray;
		return EmptyArray;
	};

	// Gets the Text of this Node. This can be the final formatted string.
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual const FText& GetNodeText() const { return FText::GetEmpty(); }

	/**
	 * Gets the Raw unformatted Text of this Node. Usually the same as GetNodeText but in case the node supports formatted string this
	 * is the raw form with all the arguments intact. To get the text arguments call GetTextArguments.
	 */
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual const FText& GetNodeUnformattedText() const { return GetNodeText(); }

	// Gets the voice of this Node as a SoundWave.
	UFUNCTION(BlueprintPure, Category = NodeData)
	USoundWave* GetNodeVoiceSoundWave() const;

	// Gets the voice of this Node as a SoundWave.
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual USoundBase* GetNodeVoiceSoundBase() const { return nullptr; }

	// Gets the voice of this Node as a DialogueWave. Only the first Dialogue context in the wave should be used.
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual UDialogueWave* GetNodeVoiceDialogueWave() const { return nullptr; }

	// Gets the speaker state ordered to this node (can be used e.g. for icon selection)
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual FName GetSpeakerState() const { return NAME_None; }
	virtual void AddAllSpeakerStatesIntoSet(TSet<FName>& OutStates) const {};

	// Gets the generic data asset of this Node.
	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual UObject* GetGenericData() const { return nullptr; }

	UFUNCTION(BlueprintPure, Category = NodeData)
	virtual UDlgNodeData* GetNodeData() const { return nullptr; }

	// Helper method to get directly the Dialogue
	UDlgDialogue* GetDialogue() const;

	// Helper functions to get the names of some properties. Used by the DlgSystemEditor module.
	static FName GetMemberNameOwnerName() { return GET_MEMBER_NAME_CHECKED(UDlgNode, OwnerName); }
	static FName GetMemberNameCheckChildrenOnEvaluation() { return GET_MEMBER_NAME_CHECKED(UDlgNode, bCheckChildrenOnEvaluation); }
	static FName GetMemberNameEnterConditions() { return GET_MEMBER_NAME_CHECKED(UDlgNode, EnterConditions); }
	static FName GetMemberNameEnterEvents() { return GET_MEMBER_NAME_CHECKED(UDlgNode, EnterEvents); }
	static FName GetMemberNameChildren() { return GET_MEMBER_NAME_CHECKED(UDlgNode, Children); }

	// Syncs the GraphNode Edges with our edges
	void UpdateGraphNode();

protected:
	void FireNodeEnterEvents(UDlgContext* Context);

protected:
#if WITH_EDITORONLY_DATA
	// Node's Graph representation, used to get position.
	UPROPERTY(Meta = (DlgNoExport))
	UEdGraphNode* GraphNode = nullptr;

	// Used to build the change event and broadcast it
	int32 BroadcastPropertyEdgeIndexChanged = INDEX_NONE;
#endif // WITH_EDITORONLY_DATA

	// Name of a participant (speaker) associated with this node.
	UPROPERTY(EditAnywhere, Category = DialogueNodeData, Meta = (DisplayName = "Participant Name"))
	FName OwnerName;

	/**
	 *  If it is set the node is only satisfied if at least one of its children is
	 *  Should not be used if entering this node can modify the condition results of its children.
	 */
	UPROPERTY(EditAnywhere, Category = DialogueNodeData)
	bool bCheckChildrenOnEvaluation = false;

	// Conditions necessary to enter this node
	UPROPERTY(EditAnywhere, Category = DialogueNodeData)
	TArray<FDlgCondition> EnterConditions;

	// Events fired when the node is reached in the dialogue
	UPROPERTY(EditAnywhere, Category = DialogueNodeData)
	TArray<FDlgEvent> EnterEvents;

	// Edges that point to Children of this Node
	UPROPERTY(EditAnywhere, EditFixedSize, AdvancedDisplay, Category = DialogueNodeData)
	TArray<FDlgEdge> Children;
};
