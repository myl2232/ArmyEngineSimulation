#include "ArmyWHCShapeDoorChangeList.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "UtilityTools/SCTShapeUtilityTools.h"
#include "SCTShape.h"
#include "SCTResManager.h"
#include "MaterialManager.h"

FArmyWHCSingleDoorGroupChange::FArmyWHCSingleDoorGroupChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCSingleDoorGroupChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DOOR_GROUP));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("template_id"), Id);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSingleDoorGroupChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), DoorGroupGuid);
    Id = JsonObject->GetIntegerField(TEXT("template_id"));
}

void FArmyWHCSingleDoorGroupChange::ProcessChange()
{
    TSharedPtr<FCoverDoorTypeInfo> * DoorTypeTemplate = nullptr;
    if (Shape->GetShapeCategory() == (int32)ECabinetType::EType_SlidingDoorWardrobe)
        DoorTypeTemplate = FWHCModeGlobalData::SlidingDoorTypeMap.Find(Id);
    else
        DoorTypeTemplate = FWHCModeGlobalData::CoverDoorTypeMap.Find(Id);
    check(DoorTypeTemplate != nullptr);
    FSCTShapeUtilityTool::ReplaceCabinetDoorWithGuid(Shape, DoorGroupGuid, (*DoorTypeTemplate)->DoorGroupShape.Get());
}

void FArmyWHCSingleDrawerGroupChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DRAWER_GROUP));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("template_id"), Id);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSingleDrawerGroupChange::ProcessChange()
{
    TSharedPtr<FCoverDoorTypeInfo> * DrawerTypeTemplate = FWHCModeGlobalData::DrawerTypeMap.Find(Id);
    check(DrawerTypeTemplate != nullptr);
    FSCTShapeUtilityTool::ReplaceCabinetDoorWithGuid(Shape, DoorGroupGuid, (*DrawerTypeTemplate)->DoorGroupShape.Get());
}

FArmyWHCSingleDoorGroupMtlChange::FArmyWHCSingleDoorGroupMtlChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCSingleDoorGroupMtlChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("mtl_id"), Id);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSingleDoorGroupMtlChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), DoorGroupGuid);
    Id = JsonObject->GetIntegerField(TEXT("mtl_id"));
}

void FArmyWHCSingleDoorGroupMtlChange::ProcessChange()
{
    TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
    if (MtlInfoPtr != nullptr)
    {
        TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
        if (MtlInfo->Mtl == nullptr)
        {
            MtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
                FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, MtlInfo->Id), 
                MtlInfo->MtlParam);
            if (MtlInfo->Mtl != nullptr)
                MtlInfo->Mtl->AddToRoot();
        }

        if (MtlInfo->Mtl != nullptr)
        {
            TSharedPtr<FCommonPakData> PakData = MakeShareable(new FCommonPakData);
            PakData->ID = MtlInfo->Id;
            PakData->Name = MtlInfo->MtlName;
            PakData->Url = MtlInfo->MtlUrl;
            PakData->MD5 = MtlInfo->MtlMd5;
            PakData->ThumbnailUrl = MtlInfo->MtlThumbnailUrl;
            PakData->OptimizeParam = MtlInfo->MtlParam;

            FSCTShapeUtilityTool::ReplaceCabinetDoorGroupMaterialWithGuid(Shape, DoorGroupGuid, PakData.Get(), MtlInfo->Mtl);
        }
    }
}

void FArmyWHCSingleDrawerGroupMtlChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("mtl_id"), Id);

    JsonWriter->WriteObjectEnd();
}

FArmyWHCSingleDoorGroupHandleChange::FArmyWHCSingleDoorGroupHandleChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCSingleDoorGroupHandleChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("handle_id"), HandleId);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSingleDoorGroupHandleChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), DoorGroupGuid);
    HandleId = JsonObject->GetIntegerField(TEXT("handle_id"));
}

void FArmyWHCSingleDoorGroupHandleChange::ProcessChange()
{
    FSCTShapeUtilityTool::ReplaceCabinetDoorGroupMetalWithGuid(Shape, DoorGroupGuid, HandleId);
}

void FArmyWHCSingleDrawerGroupHandleChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE));
    JsonWriter->WriteValue(TEXT("guid"), DoorGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("handle_id"), HandleId);

    JsonWriter->WriteObjectEnd();
}

FArmyWHCDoorOpenDirChange::FArmyWHCDoorOpenDirChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCDoorOpenDirChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), FString(SHAPE_CHANGE_DOOR_OPEN_DIR));
    JsonWriter->WriteValue(TEXT("guid"), DoorSheetGuid.ToString());
    JsonWriter->WriteValue(TEXT("open_dir"), OpenDir);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCDoorOpenDirChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), DoorSheetGuid);
    OpenDir = JsonObject->GetIntegerField(TEXT("open_dir"));
}

void FArmyWHCDoorOpenDirChange::ProcessChange()
{
    FSCTShapeUtilityTool::ReplaceCabinetDoorSheetOpenDoorDirectionWithGuid(Shape, DoorSheetGuid, OpenDir);
}