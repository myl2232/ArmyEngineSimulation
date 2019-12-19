#include "MaterialManager.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"
#include "SCTResManager.h"
#include "SCTXRResourceModule.h"
#include "HAL/PlatformFilemanager.h"
#include "DownloadMgr.h"
#include "SCTCityHash.h"
#include "SCTGlobalDataForShape.h"


FMaterialManager::FMaterialManager()	
{
		
}
FMaterialManager::~FMaterialManager()
{	
}

void FMaterialManager::GetOrCreateMaterialInterface(const EMaterialType MaterialType, const FString & InUrl,
	const FString & InMaterialID , const FString & InMD5, const FMaterialLoadCallback & InCallback)
{	
	GetOrCreateMaterialInterfaceWithParameter(MaterialType,InUrl, InMaterialID, InMD5,TEXT(""), InCallback);
}

void FMaterialManager::GetOrCreateMaterialInterfaceWithParameter(const EMaterialType MaterialType, const FString & InUrl, 
	const FString & InMaterialID, const FString & InMD5, const FString & InParameter, const FMaterialLoadCallback & InCallback)
{
	FString TrueFileName = GetFullPathFromMaterialTypeAndID(MaterialType, InMaterialID);
	bool Exist = FPlatformFileManager::Get().GetPlatformFile().FileExists(*TrueFileName);
	if (Exist)
	{
		FString RetErrorMsg = TEXT("Create Material Wihtout download");
		UMaterialInterface* RetMaterial = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(TrueFileName, InParameter);
		InCallback.ExecuteIfBound(RetMaterial, RetErrorMsg);
	}
	else
	{

		if (InUrl.IsEmpty())
		{
			FString RetErrorMsg = TEXT("Invalid pak url");
			InCallback.ExecuteIfBound(nullptr, RetErrorMsg);
			return;
		}
		FDelegateHandle Handle = BindCallbackToMateralRequest(MaterialType, InMaterialID, InCallback);
		auto  MaterialDownLoadSuccessLam = [=](IJob::EJobState& InJobState) {

			UMaterialInterface* RetMaterial = nullptr;
			FString RetErrorMsg;
			if (InJobState == IJob::JS_Succeed)
			{
				RetMaterial = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(TrueFileName, InParameter);
				RetErrorMsg = TEXT("Download: ") + InUrl + TEXT(" succeeded!");
			}
			else
			{
				RetErrorMsg = TEXT("Download: ") + InUrl + TEXT(" failed!");
			}
			ExcCallbackFromMaterialTypeAndMaterialID(MaterialType, InMaterialID, RetMaterial, RetErrorMsg);
			RemoveCallbackFromMaterialTypeAndMaterialID(MaterialType, InMaterialID, Handle);
		};
		auto EdgeMaterialDownLoadFailerLam = [=](const FString&, const FString&) {

		};
		FDownloadTask::FOnJobFinished EdgeMaterialDownLoadFinished;
		EdgeMaterialDownLoadFinished.BindLambda(MaterialDownLoadSuccessLam);
#if 0
		FDownloadMgr::Get()->DownloadFile(InUrl, TrueFileName, InMD5, true,
			EdgeMaterialDownLoadSuccess, EdgeMaterialDownLoadFailed);
#else		
		FDownloadMgr::Get()->DownloadFile(InUrl, TrueFileName, TEXT(""), true,
			EdgeMaterialDownLoadFinished);
#endif
	}
}

UMaterialInterface * FMaterialManager::GetMaterialInterface(const EMaterialType InMaterialType, const FString & InMaterialID) const
{
	FString TrueFileName = GetFullPathFromMaterialTypeAndID(InMaterialType, InMaterialID);
	UMaterialInterface * ReturnMaterial = nullptr;
	bool Exist = FPlatformFileManager::Get().GetPlatformFile().FileExists(*TrueFileName);
	if (Exist)
	{	
		ReturnMaterial = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(TrueFileName);		
	}
	return ReturnMaterial;
}

UMaterialInterface * FMaterialManager::GetMaterialInterfaceWithParam(const EMaterialType InMaterialType, const FString & InMaterialID, const FString & InParameter)const
{
	FString TrueFileName = GetFullPathFromMaterialTypeAndID(InMaterialType, InMaterialID);
	UMaterialInterface * ReturnMaterial = nullptr;
	bool Exist = FPlatformFileManager::Get().GetPlatformFile().FileExists(*TrueFileName);
	if (Exist)
	{
		ReturnMaterial = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(TrueFileName,InParameter);
	}
	return ReturnMaterial;
}


const FString & FMaterialManager::GetFullPathFromMaterialTypeAndID(const EMaterialType MaterialType, const FString & InMaterialID) const
{
	static bool bIsFirst = true;
	if (bIsFirst)
	{
		const_cast<FMaterialManager*>(this)->BoardMaterialDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Material/Board/"));
		const_cast<FMaterialManager*>(this)->EdgeMaterialDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Material/Edge/"));
		const_cast<FMaterialManager*>(this)->CoverdMaterialDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Material/CoveredEdge/"));
		const_cast<FMaterialManager*>(this)->AluminumFoilMaterialDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Material/AluminumFoil/"));
		const_cast<FMaterialManager*>(this)->DoorMaterialDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Material/Door/"));
		bIsFirst = false;
	}
	static FString MaterialPath;
	switch (MaterialType)
	{
		case EMaterialType::Board_Material:
		{
			MaterialPath = BoardMaterialDir + TEXT("/") + TEXT("BoardMaterial_") + InMaterialID + TEXT(".pak");
		}
		break;
		case EMaterialType::Edge_Material:
		{
			MaterialPath = EdgeMaterialDir + TEXT("/") + TEXT("EdgeMaterial_") + InMaterialID + TEXT(".pak");
		}
		break;
		case EMaterialType::Covered_Material:
		{
			MaterialPath = CoverdMaterialDir + TEXT("/") + +TEXT("CoveredMaterial_") + InMaterialID + TEXT(".pak");
		}
		break;
		case EMaterialType::AluminumFoil_Material:
		{
			MaterialPath = AluminumFoilMaterialDir + TEXT("/") + +TEXT("AluminumFoilMaterial") + InMaterialID + TEXT(".pak");
		}
		break;
		case EMaterialType::Door_Material:
		{
			MaterialPath = DoorMaterialDir + TEXT("/") + +TEXT("DoorMaterial") + InMaterialID + TEXT(".pak");
		}
		break;
		default:
			MaterialPath.Empty();
		break;
	}
	return MaterialPath;
}

FDelegateHandle FMaterialManager::BindCallbackToMateralRequest(const EMaterialType MaterialType, const FString & InMaterialID,
	const FMaterialLoadCallback & InCallback)
{
	UINT64 UUID = FMaterialRequest::GetUUIDFromMaterialTypeAndFileID(MaterialType, InMaterialID);
	bool Find = false;
	FDelegateHandle Handle;
	auto Iter = CallbackRequestMap.Find(UUID);
	if (Iter)
	{		
		Handle = Iter->Get()->BindCallback(InCallback);
		Find = true;
	}	
	else
	{
		TSharedPtr<FMaterialRequest> MR = MakeShareable(new FMaterialRequest(MaterialType, InMaterialID));
		Handle = MR->BindCallback(InCallback);
		CallbackRequestMap.Add(UUID, MR);
	}
	return Handle;
}

bool FMaterialManager::ExcCallbackFromMaterialTypeAndMaterialID(const EMaterialType MaterialType, const FString & InMaterialID, UMaterialInterface * InMaterial, const FString & InErrorMsg)
{
	UINT64 UUID = FMaterialRequest::GetUUIDFromMaterialTypeAndFileID(MaterialType, InMaterialID);
	bool Result = false;
	FDelegateHandle Handle;
	auto Iter = CallbackRequestMap.Find(UUID);
	if (Iter)
	{
		Iter->Get()->CallbackMc.Broadcast(InMaterial, InErrorMsg);
		Result = true;
	}	
	return Result;
}

bool FMaterialManager::RemoveCallbackFromMaterialTypeAndMaterialID(const EMaterialType MaterialType, 
	const FString & InMaterialID,const FDelegateHandle InHandle)
{
	UINT64 UUID = FMaterialRequest::GetUUIDFromMaterialTypeAndFileID(MaterialType, InMaterialID);
	bool Result = false;	
	auto Iter = CallbackRequestMap.Find(UUID);
	if (Iter)
	{
		Iter->Get()->CallbackMc.Remove(InHandle);
		if (!(Iter->Get()->CallbackMc.IsBound()))
		{
			CallbackRequestMap.Remove(UUID);
		}
		Result = true;
	}	
	return Result;
}


FMaterialManager::FMaterialRequest::FMaterialRequest(const EMaterialType InMaterialType, const FString & InFileId)
	: RequestType(InMaterialType)
	, RequestID(InFileId)
{
	UUID = GetUUIDFromMaterialTypeAndFileID(RequestType, RequestID);	
}

FMaterialManager::FMaterialRequest::~FMaterialRequest()
{
}

UINT64 FMaterialManager::FMaterialRequest::GetUUIDFromMaterialTypeAndFileID(const EMaterialType InMaterialType, const FString & InFileId)
{
	FString PreStr = TEXT("UnDefine");
	switch (InMaterialType)
	{
	case EMaterialType::Board_Material:
		PreStr = TEXT("BoardMaterial");
		break;
	case EMaterialType::Edge_Material:
		PreStr = TEXT("EdgeMaterial");
		break;
	case EMaterialType::Covered_Material:
		PreStr = TEXT("CoveredMaterial");
		break;
	case EMaterialType::AluminumFoil_Material:
		PreStr = TEXT("AluminumFoilMaterial");
		break;
	case EMaterialType::Door_Material:
		PreStr = TEXT("DoorMaterial");
		break;
	default:
		check(false);
		break;
	}
	PreStr += InFileId;
	return CityHash64(TCHAR_TO_ANSI(*PreStr), PreStr.Len());;
}

FDelegateHandle FMaterialManager::FMaterialRequest::BindCallback(const FMaterialLoadCallback InCallback)
{
	return CallbackMc.Add(InCallback);	
}



FMaterialManagerInstatnce::FMaterialManagerInstatnce()
	:MaterialMgr(nullptr)
{}

FMaterialManagerInstatnce::~FMaterialManagerInstatnce()
{

}
FMaterialManagerInstatnce & FMaterialManagerInstatnce::GetIns()
{
	if (Singleton == nullptr)
	{
		Singleton = new FMaterialManagerInstatnce();
	}
	return *Singleton;
}

void FMaterialManagerInstatnce::ReleaseIns()
{
	if (Singleton != nullptr)
	{
		delete Singleton;
		Singleton = nullptr;
	}
}

FMaterialManager & FMaterialManagerInstatnce::GetMaterialManagerRef()
{
	return *GetMaterialManagerPtr();
}

FMaterialManager * FMaterialManagerInstatnce::GetMaterialManagerPtr()
{
	check(MaterialMgr);
	return MaterialMgr;
}

bool FMaterialManagerInstatnce::StartUp()
{
	check(!MaterialMgr)	
	MaterialMgr = new FMaterialManager();
	return true;
}

bool FMaterialManagerInstatnce::ShutDown()
{
	check(MaterialMgr)
	delete MaterialMgr;
	MaterialMgr = nullptr;
	return true;
}


FMaterialManagerInstatnce*  FMaterialManagerInstatnce::Singleton = nullptr;

