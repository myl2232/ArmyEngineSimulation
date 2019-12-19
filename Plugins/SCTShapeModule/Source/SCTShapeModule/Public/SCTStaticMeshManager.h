
#pragma once

#include "CoreMinimal.h"
#include "SCTShapeData.h"


class SCTSHAPEMODULE_API FMeshManager
{
public:
	FMeshManager() = default;
	~FMeshManager() = default;
public:	
	const FMeshDataEntry * FindMesh(const FString &FilePath) const;
	void UpLoadStaticMesh(const FString &FileUrl, const FString &FilePath,const FString & InParameter = TEXT(""));	
	void DownLoadPakFile(const FString& InFileUrl, const FString& InFileMD5, const FString& InFilePath);
private:	
	FMeshDataEntry LoadPakFile(const FString &PakPath, const FString & InParameter = TEXT(""));
private:	
	TMap<FString, FMeshDataEntry> LoaclMeshMap;
};