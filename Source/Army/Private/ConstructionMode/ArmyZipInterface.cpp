#include "ArmyZipInterface.h"


FArmyCADZipInterface::~FArmyCADZipInterface()
{

}

void FArmyCADZipInterface::OnProgress(const FString& archive, float percentage, int32 bytes)
{

}

void FArmyCADZipInterface::OnDone(const FString& archive, EZipUtilityCompletionState CompletionState)
{
	OnCadDoneDelegate.ExecuteIfBound(DirPath,From,To);
}

void FArmyCADZipInterface::OnStartProcess(const FString& archive, int32 bytes)
{

}

void FArmyCADZipInterface::OnFileDone(const FString& archive, const FString& file)
{

}

void FArmyCADZipInterface::OnFileFound(const FString& archive, const FString& file, int32 size)
{

}

void FArmyCADZipInterface::SetFromTo(FString TempFrom, FString TempTo)
{
	From = TempFrom;
	To = TempTo;
}

void FArmyCADZipInterface::SetDirPath(FString InDirpath)
{
	DirPath = InDirpath;
}