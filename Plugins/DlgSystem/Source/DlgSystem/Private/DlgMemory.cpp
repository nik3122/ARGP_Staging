// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgMemory.h"
#include "DlgHelper.h"


bool FDlgHistory::operator==(const FDlgHistory& Other) const
{
	return FDlgHelper::IsSetEqual(VisitedNodeIndices, Other.VisitedNodeIndices);
}

void FDlgMemory::SetEntry(const FGuid& DlgGuid, const FDlgHistory& History)
{
	FDlgHistory* OldEntry = HistoryMap.Find(DlgGuid);

	if (OldEntry == nullptr)
	{
		HistoryMap.Add(DlgGuid, History);
	}
	else
	{
		*OldEntry = History;
	}
}

void FDlgMemory::SetNodeVisited(const FGuid& DlgGuid, int32 NodeIndex)
{
	FDlgHistory* History = HistoryMap.Find(DlgGuid);
	// Add it if it does not exist already
	if (History == nullptr)
	{
		History = &HistoryMap.Add(DlgGuid);
	}

	History->VisitedNodeIndices.Add(NodeIndex);
}

bool FDlgMemory::IsNodeVisited(const FGuid& DlgGuid, int32 NodeIndex) const
{
	const FDlgHistory* History = HistoryMap.Find(DlgGuid);
	if (History == nullptr)
	{
		return false;
	}

	const int32* FoundIndex = History->VisitedNodeIndices.Find(NodeIndex);
	return FoundIndex != nullptr;
}

void FDlgMemory::Serialize(FArchive& Ar)
{
	Ar << HistoryMap;
}

