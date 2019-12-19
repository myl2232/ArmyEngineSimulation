#include "SCTStaticMeshManager.h"
#include "Paths.h"
#include "DownLoadMgr.h"
#include "UObjectBaseUtility.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "SCTResManager.h"
#include "SCTXRTypes.h"

const FMeshDataEntry * FMeshManager::FindMesh(const FString &FilePath) const
{
	const auto *CurrentSM = LoaclMeshMap.Find(FilePath);
	return CurrentSM;
}

void FMeshManager::UpLoadStaticMesh(const FString &FileUrl, const FString &FilePath, const FString & InParameter /*= TEXT("")*/)
{
	const FMeshDataEntry * Entry = FindMesh(FilePath);
	if (Entry)
	{
		return;
	}
	if (FPaths::FileExists(FilePath))
	{
		FMeshDataEntry NewEntry = LoadPakFile(FilePath, InParameter);
		check(NewEntry.Type != -1);
		LoaclMeshMap.Add(FilePath, MoveTemp(NewEntry));
	}
}

void FMeshManager::DownLoadPakFile(const FString& InFileUrl, const FString& InFileMD5, const FString& InFilePath)
{
	if (FPaths::FileExists(InFilePath))
	{
		return;
	}
	FDownloadMgr::Get()->DownloadFile(InFileUrl, InFilePath, InFileMD5);
}

FMeshDataEntry FMeshManager::LoadPakFile(const FString &PakPath, const FString & InParameter /*= TEXT("")*/)
{
	FMeshDataEntry Entry;
	FSCTVRSObject * VRSObj = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().LoadCustomObj(PakPath, InParameter);
	do
	{
		if (!VRSObj) break;		
		if (VRSObj->GetLampMeshObject())
		{
			Entry.Type = 0;
			Entry.MeshData.Mesh = VRSObj->GetLampMeshObject();
			Entry.Center = VRSObj->BoundOrigin;
			Entry.Size = 2.0f * VRSObj->BoundExtent;
		}
		else
		{
			Entry.Type = 1;
			Entry.MeshData.SkeletalMesh = VRSObj->GetSkeletalMeshObject();
			Entry.AnimationAsset = VRSObj->GetAnimSequenceObject();
			Entry.Center = VRSObj->BoundOrigin;
			Entry.Size = 2.0f * VRSObj->BoundExtent;
		}
		for (const auto & IterRef: VRSObj->MaterialList)
		{
			Entry.MaterialList.Emplace(TPair<FString, UMaterialInstanceDynamic*>(IterRef.SlotName, IterRef.DIM));
		}
		
	} while (false);	
	return MoveTemp(Entry);
}
