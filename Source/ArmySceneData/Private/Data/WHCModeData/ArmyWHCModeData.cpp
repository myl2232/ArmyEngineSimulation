#include "Data/WHCModeData/XRWHCModeData.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

int32 FWHCModeGlobalData::CurrentWHCClassify = 2;
FWHCGlobalParams FWHCModeGlobalData::GlobalParams;
TMap<int32, FWHCModeGlobalData::TCabinetInfoArr> FWHCModeGlobalData::CabinetInfoArrCollection;
FCabinetListInfo FWHCModeGlobalData::CabinetListInfo;
FWHCModeGlobalData::TCabinetTemplateMap FWHCModeGlobalData::CabinetTemplateMap;
FWHCModeGlobalData::TCabinetComponentMap FWHCModeGlobalData::CabinetComponentMap;
FWHCModeGlobalData::TStandaloneAccessoryMap FWHCModeGlobalData::StandaloneAccessoryMap;

FWHCModeGlobalData::TCoverDoorTypeMap FWHCModeGlobalData::CoverDoorTypeMap;
FWHCModeGlobalData::TCoverDoorTypeMap FWHCModeGlobalData::SlidingDoorTypeMap;
FWHCModeGlobalData::TCoverDoorTypeMap FWHCModeGlobalData::DrawerTypeMap;


FWHCModeGlobalData::TMtlMap FWHCModeGlobalData::CabinetMtlMap;

FWHCModeGlobalData::TPlatformMap FWHCModeGlobalData::PlatformMap;

FWHCModeGlobalData::TTableMap FWHCModeGlobalData::ToeSections;
FWHCModeGlobalData::TMtlMap FWHCModeGlobalData::ToeMtlMap;

FWHCModeGlobalData::TTableMap FWHCModeGlobalData::TopSections;
FWHCModeGlobalData::TMtlMap FWHCModeGlobalData::TopMtlMap;

FWHCModeGlobalData::TCabinetAccMap FWHCModeGlobalData::CabinetAccMap;

int32 FWHCModeGlobalData::SelectionState = 0;

FString GetStringFieldBackCompatible(const TSharedPtr<FJsonObject> &InJsonObj, const FString &InNewField, const FString &InOldField)
{
    FString Value = InJsonObj->GetStringField(InNewField);
    if (Value.IsEmpty())
        Value = InJsonObj->GetStringField(InOldField);
    return MoveTemp(Value);
}

bool GetArrayFieldBackCompatible(const TSharedPtr<FJsonObject> &InJsonObj, const FString &InNewField, const FString &InOldField, const TArray<TSharedPtr<FJsonValue>> *&InOutArr)
{
    InOutArr = nullptr;
    bool bRet = InJsonObj->TryGetArrayField(InNewField, InOutArr);
    if (!bRet)
        bRet = InJsonObj->TryGetArrayField(InOldField, InOutArr);
    return bRet;
}

void FDimInfo::Serialize(const FString & InObjectName, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart(InObjectName);
    JsonWriter->WriteValue(TEXT("current"), Current);
    JsonWriter->WriteValue(TEXT("min"), Min);
    JsonWriter->WriteValue(TEXT("max"), Max);
    JsonWriter->WriteObjectEnd();
}

void FDimInfo::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    Current = JsonObject->GetNumberField(TEXT("current"));
    const TSharedPtr<FJsonObject> & MinMaxObj = JsonObject->GetObjectField(TEXT("value"));
    Min = MinMaxObj->GetNumberField(TEXT("min"));
    Max = MinMaxObj->GetNumberField(TEXT("max"));
}

FString FCabinetComponent::GetCachePath(int32 InType) const
{
	switch (InType)
	{
	case 2:
	{
		int32 Index = FrontResourceUrl.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString FileName = Index != INDEX_NONE ? FrontResourceUrl.Right(FrontResourceUrl.Len() - Index - 1) : FrontResourceUrl;
		FString Path(FPaths::ProjectContentDir() / FString::Printf(TEXT("DBJCache/Component/%s_%s"), *FrontResourceMd5, *FileName));
		return MoveTemp(Path);
	}
	break;
	case 3:
	{
		int32 Index = SideResourceUrl.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString FileName = Index != INDEX_NONE ? SideResourceUrl.Right(SideResourceUrl.Len() - Index - 1) : SideResourceUrl;
		FString Path(FPaths::ProjectContentDir() / FString::Printf(TEXT("DBJCache/Component/%s_%s"), *SideResourceMd5, *FileName));
		return MoveTemp(Path);
	}
	break;
	case 1:
	default:
	{
		int32 Index = TopResourceUrl.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString FileName = Index != INDEX_NONE ? TopResourceUrl.Right(TopResourceUrl.Len() - Index - 1) : TopResourceUrl;
		FString Path(FPaths::ProjectContentDir() / FString::Printf(TEXT("DBJCache/Component/%s_%s"), *TopResourceMd5, *FileName));
		return MoveTemp(Path);
	}
	break;
	}
}

void FCabinetAccInfo::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteValue(TEXT("id"), Id);
    JsonWriter->WriteValue(TEXT("heightToGround"), AboveGround);
    JsonWriter->WriteValue(TEXT("type"), (int32)Type);
    JsonWriter->WriteValue(TEXT("boundToSocket"), bBoundToSocket ? 1 : 0);
    JsonWriter->WriteArrayStart(TEXT("relativeLocation"));
    JsonWriter->WriteValue(RelativeLocation.X);
    JsonWriter->WriteValue(RelativeLocation.Y);
    JsonWriter->WriteValue(RelativeLocation.Z);
    JsonWriter->WriteArrayEnd();
    JsonWriter->WriteValue(TEXT("name"), Name);
    JsonWriter->WriteValue(TEXT("code"), Code);
    JsonWriter->WriteValue(TEXT("pakUrl"), PakUrl);
    JsonWriter->WriteValue(TEXT("pakMd5"), PakMd5);
    JsonWriter->WriteValue(TEXT("optimizeParam"), OptimizedParam);
    JsonWriter->WriteValue(TEXT("thumbnailUrl"), ThumbnailUrl);
    JsonWriter->WriteValue(TEXT("brandName"), BrandName);
    JsonWriter->WriteValue(TEXT("version"), Version);
    WidthInfo.Serialize(TEXT("width"), JsonWriter);
    DepthInfo.Serialize(TEXT("depth"), JsonWriter);
    HeightInfo.Serialize(TEXT("height"), JsonWriter);

	JsonWriter->WriteObjectStart("viewLegend");
	for (auto Component : ComponentArray)
	{
		JsonWriter->WriteValue(TEXT("id"), Component->Id);
		JsonWriter->WriteValue(TEXT("type"), Component->Type);
		JsonWriter->WriteValue(TEXT("topViewLegend"), Component->TopResourceUrl);
		JsonWriter->WriteValue(TEXT("topViewLegendMd5"), Component->TopResourceMd5);
		JsonWriter->WriteValue(TEXT("frontViewLegend"), Component->FrontResourceUrl);
		JsonWriter->WriteValue(TEXT("frontViewLegendMd5"), Component->FrontResourceMd5);
		JsonWriter->WriteValue(TEXT("sideViewLegend"), Component->SideResourceUrl);
		JsonWriter->WriteValue(TEXT("sideViewLegendMd5"), Component->SideResourceMd5);
		break;//Ŀǰֻ��һ��
	}
	JsonWriter->WriteObjectEnd();
}

void FCabinetAccInfo::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("id"));
    AboveGround = JsonObject->GetNumberField(TEXT("heightToGround"));
    if (JsonObject->HasField(TEXT("type")))
        Type = (uint8)JsonObject->GetIntegerField(TEXT("type"));
    if (JsonObject->HasField(TEXT("boundToSocket")))
        bBoundToSocket = JsonObject->GetIntegerField(TEXT("boundToSocket")) == 1;
    const TArray<TSharedPtr<FJsonValue>> * RelLocationArrPtr = nullptr;
    if (JsonObject->TryGetArrayField(TEXT("relativeLocation"), RelLocationArrPtr))
    {
        RelativeLocation.X = RelLocationArrPtr->operator[](0)->AsNumber();
        RelativeLocation.Y = RelLocationArrPtr->operator[](1)->AsNumber();
        RelativeLocation.Z = RelLocationArrPtr->operator[](2)->AsNumber();
    }
    Name = JsonObject->GetStringField(TEXT("name"));
    Code = JsonObject->GetStringField(TEXT("code"));
    PakUrl = JsonObject->GetStringField(TEXT("pakUrl"));
    PakMd5 = JsonObject->GetStringField(TEXT("pakMd5"));
    OptimizedParam = JsonObject->GetStringField(TEXT("optimizeParam"));
    ThumbnailUrl = JsonObject->GetStringField(TEXT("thumbnailUrl"));
    BrandName = JsonObject->GetStringField(TEXT("brandName"));
    Version = JsonObject->GetStringField(TEXT("version"));

    const TSharedPtr<FJsonObject> * WidthObj = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("width"), WidthObj))
        WidthInfo.Deserialize(*WidthObj);

    const TSharedPtr<FJsonObject> * DepthObj = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("depth"), DepthObj))
        DepthInfo.Deserialize(*DepthObj);

    const TSharedPtr<FJsonObject> * HeightObj = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("height"), HeightObj))
        HeightInfo.Deserialize(*HeightObj);
	
	const TSharedPtr<FJsonObject> ComponentJsonObj = JsonObject->GetObjectField(TEXT("viewLegend"));
	if (ComponentJsonObj.IsValid())
	{
		int32 ComponentId = ComponentJsonObj->GetIntegerField(TEXT("id"));
		int32 ComponentType = ComponentJsonObj->GetIntegerField(TEXT("type"));

		switch (ComponentType)
		{
		case 105://���/������
		{
			TSharedPtr<FCabinetComponent> AccessoryComponent = MakeShareable(new FCabinetComponent);
			AccessoryComponent->Id = ComponentJsonObj->GetIntegerField(TEXT("id"));
			AccessoryComponent->Type = ComponentJsonObj->GetIntegerField(TEXT("type"));
			AccessoryComponent->TopResourceUrl = ComponentJsonObj->GetStringField(TEXT("topViewLegend"));
			AccessoryComponent->TopResourceMd5 = ComponentJsonObj->GetStringField(TEXT("topViewLegendMd5"));
			AccessoryComponent->FrontResourceUrl = ComponentJsonObj->GetStringField(TEXT("frontViewLegend"));
			AccessoryComponent->FrontResourceMd5 = ComponentJsonObj->GetStringField(TEXT("frontViewLegendMd5"));
			AccessoryComponent->SideResourceUrl = ComponentJsonObj->GetStringField(TEXT("sideViewLegend"));
			AccessoryComponent->SideResourceMd5 = ComponentJsonObj->GetStringField(TEXT("sideViewLegendMd5"));
			ComponentArray.Add(AccessoryComponent);
		}
		break;
		default:
			break;
		}
	}
}

FString FCabinetAccInfo::GetCachePath() const
{
    return FPaths::ProjectContentDir() / FString::Printf(TEXT("WHC/Accessories/Acc_%d_%s.pak"), Id, *PakMd5);
}

void FCabinetWholeAccInfo::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    Self->Serialize(JsonWriter);
    JsonWriter->WriteArrayStart(TEXT("defaultParts"));
    for (auto &SubAccInfo : SubAccInfos)
    {
        JsonWriter->WriteObjectStart();
        SubAccInfo->Serialize(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    JsonWriter->WriteArrayEnd();
}

void FCabinetWholeAccInfo::Deserialize(const TSharedPtr<class FJsonObject>& JsonObject)
{
    Self = MakeShareable(new FCabinetAccInfo);
    Self->Deserialize(JsonObject);
    const TArray<TSharedPtr<FJsonValue>> &SubAccJsonArr = JsonObject->GetArrayField(TEXT("defaultParts"));
    for (const auto & SubAccJson : SubAccJsonArr)
    {
        FCabinetAccInfo * SubAccInfo = new FCabinetAccInfo;
        SubAccInfo->Deserialize(SubAccJson->AsObject());
        SubAccInfos.Emplace(MakeShareable(SubAccInfo));
    }
}

void FTableInfo::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteValue(TEXT("id"), Id);
    JsonWriter->WriteValue(TEXT("name"), Name);
    JsonWriter->WriteValue(TEXT("thumbnailUrl"), ThumbnailUrl);
    JsonWriter->WriteValue(TEXT("cadUrl"), cadUrl);
    JsonWriter->WriteObjectStart(TEXT("sectionHeight"));
    JsonWriter->WriteValue(TEXT("current"), CurrentHeight);
    if (AvailableHeights.Num() > 0)
    {
        JsonWriter->WriteArrayStart(TEXT("value"));
        for (const auto & Height : AvailableHeights)
            JsonWriter->WriteValue(Height);
        JsonWriter->WriteArrayEnd();
    }
    JsonWriter->WriteObjectEnd();
    if (mtlId > 0)
    {
        JsonWriter->WriteObjectStart(TEXT("defaultMaterial"));
        JsonWriter->WriteValue(TEXT("id"), mtlId);
        JsonWriter->WriteObjectEnd();
    }
}

void FTableInfo::Deserialize(const TSharedPtr<class FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("id"));
    Name = JsonObject->GetStringField(TEXT("name"));
    ThumbnailUrl = GetStringFieldBackCompatible(JsonObject, TEXT("thumbnailUrl"), TEXT("thumbnail_url"));
    cadUrl = GetStringFieldBackCompatible(JsonObject, TEXT("cadUrl"), TEXT("cad_url"));
    const TSharedPtr<FJsonObject> * SectionHeightPtr = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("sectionHeight"), SectionHeightPtr))
    {
        const TSharedPtr<FJsonObject> & SectionHeight = *SectionHeightPtr;
        CurrentHeight = SectionHeight->GetNumberField(TEXT("current"));
        const TArray<TSharedPtr<FJsonValue>> * HeightArrPtr = nullptr; 
        if (SectionHeight->TryGetArrayField(TEXT("value"), HeightArrPtr))
        {
            for (const auto & Height : (*HeightArrPtr))
                AvailableHeights.Emplace(Height->AsNumber());
        }
    }
    const TSharedPtr<FJsonObject> * DefaultMtlJsonObjPtr = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("defaultMaterial"), DefaultMtlJsonObjPtr))
    {
        mtlId = (*DefaultMtlJsonObjPtr)->GetIntegerField(TEXT("id"));
    }
}

FString FTableInfo::GetCachedPath() const
{
	FString PathPart; FString FilenamePart; FString ExtensionPart;
	FPaths::Split(cadUrl, PathPart, FilenamePart, ExtensionPart);
    return FPaths::ProjectContentDir() / FString::Printf(TEXT("WHC/Platform/%s.%s"),*FilenamePart,*ExtensionPart);
}

void FMtlInfo::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteValue(TEXT("id"), Id);
    JsonWriter->WriteValue(TEXT("name"), MtlName);
    JsonWriter->WriteValue(TEXT("thumbnailUrl"), MtlThumbnailUrl);
    JsonWriter->WriteValue(TEXT("pakUrl"), MtlUrl);
    JsonWriter->WriteValue(TEXT("pakMd5"), MtlMd5);
    JsonWriter->WriteValue(TEXT("param"), MtlParam);
}

void FMtlInfo::Deserialize(const TSharedPtr<class FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("id"));
    MtlName = JsonObject->GetStringField(TEXT("name"));
    MtlThumbnailUrl = GetStringFieldBackCompatible(JsonObject, TEXT("thumbnailUrl"), TEXT("thumbnail_url"));
    MtlUrl = GetStringFieldBackCompatible(JsonObject, TEXT("pakUrl"), TEXT("url"));
    MtlMd5 = GetStringFieldBackCompatible(JsonObject, TEXT("pakMd5"), TEXT("md5"));
    MtlParam = GetStringFieldBackCompatible(JsonObject, TEXT("optimizeParam"), TEXT("param"));
}

FString FMtlInfo::GetCachedPath() const
{
    return FPaths::ProjectContentDir() / FString::Printf(TEXT("WHC/Platform/Mtl_%d_%s.pak"), Id, *MtlMd5);
}

void FPlatformInfo::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteValue(TEXT("id"), Id);
    if (PlatformMtl.IsValid())
    {
        JsonWriter->WriteObjectStart(TEXT("material"));
        PlatformMtl->Serialize(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    if (FrontSections.Num() > 0)
    {
        JsonWriter->WriteArrayStart(TEXT("fontSections"));
        for (auto &Section : FrontSections)
        {
            JsonWriter->WriteObjectStart();
            Section->Serialize(JsonWriter);
            JsonWriter->WriteObjectEnd();
        }
        JsonWriter->WriteArrayEnd();
    }
    if (BackSections.Num() > 0)
    {
        JsonWriter->WriteArrayStart(TEXT("backSections"));
        for (auto &Section : BackSections)
        {
            JsonWriter->WriteObjectStart();
            Section->Serialize(JsonWriter);
            JsonWriter->WriteObjectEnd();
        }
        JsonWriter->WriteArrayEnd();
    }
}

void FPlatformInfo::Deserialize(const TSharedPtr<class FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("id"));
    const TSharedPtr<FJsonObject> * MtlPtr = nullptr;
    if (JsonObject->TryGetObjectField(TEXT("material"), MtlPtr))
    {
        PlatformMtl = MakeShareable(new FMtlInfo);
        PlatformMtl->Deserialize(*MtlPtr);
    }
    const TArray<TSharedPtr<FJsonValue>> * FrontSectionArrPtr = nullptr; 
    if (GetArrayFieldBackCompatible(JsonObject, TEXT("fontSections"), TEXT("front_shapes"), FrontSectionArrPtr))
    // if (JsonObject->TryGetArrayField(TEXT("fontSections"), FrontSectionArrPtr))
    {
        for (const auto &FrontSection : (*FrontSectionArrPtr))
        {
            FTableInfo *TableInfo = new FTableInfo;
            TableInfo->Deserialize(FrontSection->AsObject());
            FrontSections.Emplace(MakeShareable(TableInfo));
        }
    }
    const TArray<TSharedPtr<FJsonValue>> * BackSectionArrPtr = nullptr; 
    if (JsonObject->TryGetArrayField(TEXT("backSections"), BackSectionArrPtr))
    {
        for (const auto &BackSection : (*BackSectionArrPtr))
        {
            FTableInfo *TableInfo = new FTableInfo;
            TableInfo->Deserialize(BackSection->AsObject());
            BackSections.Emplace(MakeShareable(TableInfo));
        }
    }
}

bool FCoverDoorTypeInfo::IsDoorTypeHasValidCache() const
{
    return RcState == ECabinetResourceState::CabRc_Complete;
}

FString FCoverDoorTypeInfo::GetJsonFileCachePath() const
{
    return FPaths::ProjectContentDir() / FString::Printf(TEXT("WHC/CoverDoor/CoverDoor_%d.json"));
}

bool FCabinetInfo::IsCabinetHasValidCache() const
{
    return CabRcState == ECabinetResourceState::CabRc_Complete;
}

FString FCabinetInfo::GetJsonFileCachePath() const
{
    FString Path(FPaths::ProjectContentDir() / FString::Printf(TEXT("WHC/Shapes/%d_%d_%s.json"), Type, Id, *JsonMd5));
    return MoveTemp(Path);
}

