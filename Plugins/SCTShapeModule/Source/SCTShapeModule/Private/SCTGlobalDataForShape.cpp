#include "SCTGlobalDataForShape.h"
#include "SCTBaseShape.h"
#include "SCTOutline.h"


TSharedPtr<FGlobalDataForShape> FGlobalDataForShape::GlobalDataForShape;

FGlobalDataForShape * FGlobalDataForShape::GetInsPtr()
{
	if (!GlobalDataForShape.IsValid())
	{
		GlobalDataForShape = MakeShareable<FGlobalDataForShape>(new FGlobalDataForShape());
	}
	return GlobalDataForShape.Get();
}

void FGlobalDataForShape::ReleaseInsPtr()
{
	if (GlobalDataForShape.IsValid())
	{
		GlobalDataForShape = nullptr;
	}
}

bool FGlobalDataForShape::GetSingleVennerMat(int32 & OutId,FString & OutPakUrl, FString & OutPakMd5, FString & OutParameter)
{
	check(VeneerMtMapList.Num() > 0);
	bool IsDefault = false;	
	bool FindResult = false;
	for (auto & Ref : VeneerMtMapList)
	{
		IsDefault = *AnyCast<bool>(&(Ref[TEXT("defaulted")]));		
		if (IsDefault)
		{
			OutId = *AnyCast<int32>(&(Ref[TEXT("id")]));
			TArray<FString> *Urls = AnyCast<TArray<FString>>(&(Ref[TEXT("pakUrls")]));
			check(Urls && Urls->Num() > 0);
			OutPakUrl = (*Urls)[0];
			TArray<FString> *Md5s = AnyCast<TArray<FString>>(&(Ref[TEXT("pakMd5s")]));
			check(Md5s && Md5s->Num() > 0);
			OutPakMd5 = (*Md5s)[0];
			OutParameter = TEXT("");
			FindResult = true;
			break;
		}
	}
	return FindResult;
}

bool FGlobalDataForShape::GetEdgeBandingNameLIst(TArray<FString> & OutNameList)
{	
	OutNameList.Empty();
	for (auto & Ref : EdgeBandingList)
	{
		OutNameList.Emplace(*AnyCast<FString>(&(Ref[TEXT("name")])));		
	}
	return true;
}

bool FGlobalDataForShape::GetEdgeBandingIndexById(const int32 InId, int32 & OutIndex)
{
	bool FindResult = false;
	int32  Id = 0;
	int32 RetIndex = -1;
	for (auto & Ref : EdgeBandingList)
	{
		int32 Id = *AnyCast<int32>(&(Ref[TEXT("id")]));
		++RetIndex;
		if (Id == InId)
		{			
			FindResult = true;
			break;
		}
	}
	OutIndex = RetIndex;
	return FindResult;
}

bool FGlobalDataForShape::GetEdgeBandingIdByName(const FString & InName, int32 & OutId)
{
	bool FindResult = false;
	FString Name;
	int32 RetIndex = -1;
	for (auto & Ref : EdgeBandingList)
	{		
		Name = *AnyCast<FString>(&(Ref[TEXT("name")]));
		if (Name == InName)
		{
			RetIndex = *AnyCast<int32>(&(Ref[TEXT("id")]));
			FindResult = true;
			break;
		}		
	}
	OutId = RetIndex;
	return FindResult;
}

bool FGlobalDataForShape::GetSpecialEdgeMaterialInfo(const int32 InId, FString & OutName)
{
	bool FindResult = false;
	int32  Id = 0;
	for (auto & Ref : EdgeBandingList)
	{
		int32 Id = *AnyCast<int32>(&(Ref[TEXT("id")]));
		if (Id != InId) continue;
		OutName = *AnyCast<FString>(&(Ref[TEXT("name")]));
		FindResult = true;
	}
	return FindResult;
}



TSharedPtr<FSCTOutline> FGlobalDataForShape::GetSpaceOutlineByType(EOutlineType InType)
{
	for (int32 i=0; i<SpaceOutlineList.Num(); ++i)
	{
		if (SpaceOutlineList[i]->GetOutlineType() == InType)
		{
			return SpaceOutlineList[i];
		}
	}
	return nullptr;
}

TSharedPtr<FSCTOutline> FGlobalDataForShape::GetSpaceOutlineByName(const FString& InName)
{
	for (int32 i = 0; i < SpaceOutlineList.Num(); ++i)
	{
		if (SpaceOutlineList[i]->GetOutlineName() == InName)
		{
			return SpaceOutlineList[i];
		}
	}
	return nullptr;
}

bool FGlobalDataForShape::GetSpecialBaseMaterialInfo(const int32 InId, const int32 InCategory, FString & OutName)
{
	bool FindResult = false;
	int32 CategoryID = 0, Id = 0;
	for (auto & Ref : BaseMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("category")]));
		if (CategoryID != InCategory) continue;
		Id = *AnyCast<int32>(&(Ref[TEXT("id")]));		
		if (Id != InId) continue;
		OutName = *AnyCast<FString>(&(Ref[TEXT("name")]));
		FindResult = true;
	}
	return FindResult;
}

bool FGlobalDataForShape::GetSpecialBaseMaterialInfo(const int32 InId, const int32 InCategory, TArray<double>& OutList)
{
	bool FindResult = false;
	int32 CategoryID = 0, Id = 0;
	for (auto & Ref : BaseMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("category")]));
		if (CategoryID != InCategory) continue;
		Id = *AnyCast<int32>(&(Ref[TEXT("id")]));
		if (Id != InId) continue;
		OutList = *AnyCast<TArray<double>>(&(Ref[TEXT("heightList")]));
		FindResult = true;
	}
	return FindResult;
}

bool FGlobalDataForShape::GetSpecialBaseMaterialIndex(const int32 InId, const int32 InCategory,int32 & OutIndex)
{
	int32 RetIndex = -1;
	bool FindResult = false;
	int32 CategoryID = 0, Id = 0;
	for (auto & Ref : BaseMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("category")]));
		if (CategoryID != InCategory) continue;
		Id = *AnyCast<int32>(&(Ref[TEXT("id")]));
		++RetIndex;
		if (Id == InId)
		{
			FindResult = true;
			break;
		}				
	}
	OutIndex = RetIndex;
	return FindResult;
}

TArray<FString> FGlobalDataForShape::GetBaseMtlNameListInfo(const int32& InCategory)
{
	TArray<FString> ResNamList;
	ResNamList.Empty();

	int32 CategoryID = 0;
	for (auto & Ref : BaseMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("category")]));
		if (CategoryID != InCategory) continue;
		ResNamList.Emplace(*AnyCast<FString>(&(Ref[TEXT("name")])));
	}
	return ResNamList;
}

bool FGlobalDataForShape::GetSpecialBaseMaterialId(const int32 InCategory, const FString & InRefName, int32 & OutId)
{
	int32 RetIndex = -1;
	bool FindResult = false;
	int32 CategoryID = 0;
	FString Name;
	for (auto & Ref : BaseMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("category")]));
		if (CategoryID != InCategory) continue;
		Name = *AnyCast<FString>(&(Ref[TEXT("name")]));		
		if (Name == InRefName)
		{
			RetIndex = *AnyCast<int32>(&(Ref[TEXT("id")]));
			FindResult = true;
			break;
		}		
	}
	OutId = RetIndex;
	return FindResult;
}


bool FGlobalDataForShape::GetSingleDefaultMaterialInfo(const int32 InCategory, int32 & OutID,
	FString & OutUrl, FString & OutMd5, FString & OutName,FString & OutThumbnailUrl)
{
	
	bool FindResult = false;
	int32 CategoryID = 0;
	for (auto & Ref : DefatultMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("substrateCategory")]));
		if (CategoryID != InCategory) continue;
		OutID = *AnyCast<int32>(&(Ref[TEXT("id")]));
		OutUrl = *AnyCast<FString>(&(Ref[TEXT("pakUrl")]));
		OutMd5 = *AnyCast<FString>(&(Ref[TEXT("pakMd5")]));
		OutName = *AnyCast<FString>(&(Ref[TEXT("name")]));
		OutThumbnailUrl = *AnyCast<FString>(&(Ref[TEXT("thumbnailUrl")])); 
		FindResult = true;
		break;
	}
	return FindResult;
}

TArray<FString> FGlobalDataForShape::GetDefaultMtlNameListInfo(const int32& InSubstrateCategory)
{
	TArray<FString> ResNamList;
	ResNamList.Empty();

	int32 CategoryID = 0;
	for (auto & Ref : DefatultMaterialList)
	{
		CategoryID = *AnyCast<int32>(&(Ref[TEXT("substrateCategory")]));
		if (CategoryID != InSubstrateCategory) continue;
		
		ResNamList.Emplace(*AnyCast<FString>(&(Ref[TEXT("name")])));
	}
	return ResNamList;
}

const TSharedPtr<FDoorSubWidgetDefaultInfoBase> FGlobalDataForShape::GetDoorHanleMetalsDefaultLocation(const int32 InDoorType, const EMetalsType InMetalType, const int32 InDoorOpenDirection) const
{
	TSharedPtr<FDoorSubWidgetDefaultInfoBase> RetValue = nullptr;
	const TSharedPtr<FDoorDefaultInfo> Door = GetDoorMetalsDefaultLocation(InDoorType);
	if (!Door.IsValid())
	{
		return nullptr;
	}
	if (InDoorOpenDirection != -1)
	{	
		const int32 DesIndex = InDoorOpenDirection > 0 ?  InDoorOpenDirection - 1 : 0;
		check(Door->MetalDefaultLocationList->Num() >= (InDoorOpenDirection + 1));
		int32 IterInter = 0;
		for (const auto & Ref : *(Door->MetalDefaultLocationList.Get()))
		{
			if (Ref->MetalType != InMetalType) continue;			
			if (IterInter == DesIndex)
			{
				RetValue = Ref;
				break;
			}
			++IterInter;
		}
	}
	else
	{
		check(Door->MetalDefaultLocationList->Num() >= 0);
		RetValue = Door->MetalDefaultLocationList->operator[](0);
		for (const auto & Ref : *(Door->MetalDefaultLocationList.Get()))
		{
			if (Ref->MetalType != InMetalType) continue;
			RetValue = Ref;
			break;
		}
	}	
	return RetValue;	
}

const TArray<FServerTag>& FGlobalDataForShape::GetSpecialTypeTagsPool(EShapeType InType) const
{
	check(HasSpecialTypeTagsPool(InType));
	return 	ServerTags.FindChecked(InType);
}

const TSharedPtr<FDoorDefaultInfo> FGlobalDataForShape::GetDoorMetalsDefaultLocation(const int32 InDoorType) const
{
	// InDoorType 1 移门 2 掩门 3 抽面
	const TSharedPtr<FDoorDefaultInfo> * FindDoor = DoorDefalutSettingList.Find(InDoorType);
	if (!FindDoor)
	{
		return nullptr;
	}
	return (*FindDoor);
}

