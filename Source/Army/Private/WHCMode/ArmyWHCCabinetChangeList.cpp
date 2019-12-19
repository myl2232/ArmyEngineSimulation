#include "ArmyWHCCabinetChangeList.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "UtilityTools/SCTShapeUtilityTools.h"
#include "SCTShape.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "SCTAccessShapeForBim.h"

FArmyWHCCabinetMtlChange::FArmyWHCCabinetMtlChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCCabinetMtlChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_CABINET_MTL);
    JsonWriter->WriteValue(TEXT("material_id"), Id);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCCabinetMtlChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("material_id"));
}

void FArmyWHCCabinetMtlChange::ProcessChange()
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
            FSCTShapeUtilityTool::ReplaceCabinetMaterial(MtlInfo->Mtl, Shape);
    }
}

FArmyWHCCabinetInsertionBoardMtlChange::FArmyWHCCabinetInsertionBoardMtlChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCCabinetInsertionBoardMtlChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL);
    JsonWriter->WriteValue(TEXT("material_id"), Id);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCCabinetInsertionBoardMtlChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    Id = JsonObject->GetIntegerField(TEXT("material_id"));
}

void FArmyWHCCabinetInsertionBoardMtlChange::ProcessChange()
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
            TSharedPtr<FCommonPakData> MtlData = MakeShareable(new FCommonPakData);
            MtlData->ID = MtlInfo->Id;
            MtlData->Name = MtlInfo->MtlName;
            MtlData->Url = MtlInfo->MtlUrl;
            MtlData->MD5 = MtlInfo->MtlMd5;
            MtlData->ThumbnailUrl = MtlInfo->MtlThumbnailUrl;
            MtlData->OptimizeParam = MtlInfo->MtlParam;
            FSCTShapeUtilityTool::ReplacInsertionBoardMaterial(Shape, MtlData.Get(), MtlInfo->Mtl);
        }
    }
}

FArmyWHCFrontBoardCabinetChange::FArmyWHCFrontBoardCabinetChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCFrontBoardCabinetChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_FRONT_BOARD_CAB);
    JsonWriter->WriteValue(TEXT("num_doors"), NumDoors);
    JsonWriter->WriteValue(TEXT("start_pt"), StartPoint);
    JsonWriter->WriteValue(TEXT("end_pt"), EndPoint);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCFrontBoardCabinetChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    NumDoors = JsonObject->GetIntegerField(TEXT("num_doors"));
    StartPoint = JsonObject->GetNumberField(TEXT("start_pt"));
    EndPoint = JsonObject->GetNumberField(TEXT("end_pt"));
}

void FArmyWHCFrontBoardCabinetChange::ProcessChange()
{
    FSCTShapeUtilityTool::ModifyCabinetFrontBoardSize(Shape, StartPoint, EndPoint);
    FSCTShapeUtilityTool::ModifyForntBoardCabinetDoorSheetNum(Shape, NumDoors);
}

FArmyWHCEmbbedElecDevChange::FArmyWHCEmbbedElecDevChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCEmbbedElecDevChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_EMBBED_ELEC_DEV);
    JsonWriter->WriteValue(TEXT("guid"), ElecDevGroupGuid.ToString());
    JsonWriter->WriteValue(TEXT("id"), ElecDevId);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCEmbbedElecDevChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), ElecDevGroupGuid);
    ElecDevId = JsonObject->GetIntegerField(TEXT("id"));
}

void FArmyWHCEmbbedElecDevChange::ProcessChange()
{
    const TSharedPtr<FStandaloneCabAccInfo> *AccInfoPtr = FWHCModeGlobalData::StandaloneAccessoryMap.Find(ElecDevId);
    if (AccInfoPtr != nullptr)
        FSCTShapeUtilityTool::ReplaceEmbedElectricalGroupWithGuid(Shape, ElecDevGroupGuid, (*AccInfoPtr)->AccShape.Get());
}

FArmyWHCDelEmbbedElecDevChange::FArmyWHCDelEmbbedElecDevChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCDelEmbbedElecDevChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{}

void FArmyWHCDelEmbbedElecDevChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{}

void FArmyWHCDelEmbbedElecDevChange::ProcessChange()
{}

FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange::FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_SWARDROBE_BOTTOM_BOARD_VIS);
    JsonWriter->WriteValue(TEXT("visibility"), Visibility ? TEXT("true") : TEXT("false"));

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    Visibility = JsonObject->GetStringField(TEXT("visibility")) == TEXT("true");
}

void FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange::ProcessChange()
{
    FSCTShapeUtilityTool::SetCabinetSlidingDoorDownPlateBoardValid(Shape, Visibility);
}

FArmyWHCSlidingwayChange::FArmyWHCSlidingwayChange(FSCTShape *InShape)
: IArmyWHCModeShapeChange(InShape)
{}

void FArmyWHCSlidingwayChange::Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue(TEXT("type"), SHAPE_CHANGE_SLIDINGWAY);
    JsonWriter->WriteValue(TEXT("id"), SlidingwayId);

    JsonWriter->WriteObjectEnd();
}

void FArmyWHCSlidingwayChange::Deserialize(const TSharedPtr<FJsonObject>& JsonObject)
{
    SlidingwayId = JsonObject->GetIntegerField(TEXT("id"));
}

void FArmyWHCSlidingwayChange::ProcessChange()
{
    FSCTShapeUtilityTool::ChangeCabinetSlidingDoorSlidways(Shape, SlidingwayId);
}