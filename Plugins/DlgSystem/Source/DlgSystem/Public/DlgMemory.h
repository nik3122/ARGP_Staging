// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "DlgMemory.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct DLGSYSTEM_API FDlgHistory
{
	GENERATED_USTRUCT_BODY()
public:
	// List of already visited node indices
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DialogueHistoryData)
	TSet<int32> VisitedNodeIndices;

public:
	bool operator==(const FDlgHistory& Other) const;
};

// Singleton to store Dialogue history
// TODO: investigate if this is multiplayer friendly, it does not seem so as there exists only a single global dialogue memory
class DLGSYSTEM_API FDlgMemory
{
public:
	static FDlgMemory* GetInstance()
	{
		static FDlgMemory Instance;
		return &Instance;
	}
	static FDlgMemory& Get()
	{
		auto* Instance = GetInstance();
		check(Instance != nullptr);
		return *Instance;
	}

	// Removes all entries
	void Empty() { HistoryMap.Empty(); }

	// Adds an entry to the map or overrides an existing one
	void SetEntry(const FGuid& DlgGuid, const FDlgHistory& History);

	// Returns the entry for the given name, or nullptr if it does not exist */
	FDlgHistory* GetEntry(const FGuid& DlgGuid) { return HistoryMap.Find(DlgGuid); }

	void SetNodeVisited(const FGuid& DlgGuid, int32 NodeIndex);
	bool IsNodeVisited(const FGuid& DlgGuid, int32 NodeIndex) const;

	const TMap<FGuid, FDlgHistory>& GetHistoryMaps() const { return HistoryMap; }
	void SetHistoryMap(const TMap<FGuid, FDlgHistory>& Map) { HistoryMap = Map; }

	void Serialize(FArchive& Ar);
private:

	 // Key: Dialogue unique identifier Guid
	 // Value: set of already visited nodes
	TMap<FGuid, FDlgHistory> HistoryMap;
};

// operator overloads for serialization
FORCEINLINE FArchive& operator<<(FArchive &Ar, FDlgHistory& History)
{
	Ar << History.VisitedNodeIndices;
	return Ar;
}
