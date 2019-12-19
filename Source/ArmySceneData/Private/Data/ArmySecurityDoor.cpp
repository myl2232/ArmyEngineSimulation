#include "ArmySecurityDoor.h"
#include "ArmyActorConstant.h"
#include "ArmyRect.h"

FArmySecurityDoor::FArmySecurityDoor()
	: FArmySingleDoor()
{
	ObjectType = OT_SecurityDoor;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("防盗门"));
	DoorType = 2;//防盗门
}

FArmySecurityDoor::FArmySecurityDoor(FArmySecurityDoor * Copy)
	: FArmySingleDoor(Copy)
{
}

FArmySecurityDoor::~FArmySecurityDoor()
{
}

void FArmySecurityDoor::Generate(UWorld* InWorld)
{
    FArmySingleDoor::Generate(InWorld);

    // 防盗门需要单独标识
    if (DoorActor && DoorActor->IsValidLowLevel())
    {
        DoorActor->Tags.Add(XRActorTag::SecurityDoor);
        DoorActor->SetActorLabel(TEXT("NOLIST-防盗门_") + GetUniqueID().ToString());
		AttachModelName = DoorActor->GetActorLabel();
    }
}

void FArmySecurityDoor::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	JsonWriter->WriteValue("SaleID", GetDoorSaleID());
	JsonWriter->WriteValue("DoorType", GetDoorType());
    JsonWriter->WriteValue("openDirectionID", OpenDirectionID);

	if (DoorActor)
	{
		UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
		//TSharedPtr<FContentItemSpace::FContentItem> item = ResMg->GetContentItemFromID(HoleWindow->GetUniqueID());
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMg->GetContentItemFromID(SaleID);
		if (ResultSynData.IsValid())
		{
			//ResultSynData->SerializeToJson(JsonWriter);
			GGI->DesignEditor->SaveFileList.AddUnique(SaleID);
		}
	}

	SERIALIZEREGISTERCLASS(JsonWriter, FArmySecurityDoor)
}

void FArmySecurityDoor::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);

	FVector Pos, Direction;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	Direction.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(Pos);
	SetDirection(Direction);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	SetDoorSaleID(InJsonData->GetNumberField("SaleID"));
	SetDoorType(InJsonData->GetNumberField("DoorType") == 0 ? 1 : InJsonData->GetNumberField("DoorType"));
    InJsonData->TryGetNumberField("openDirectionID", OpenDirectionID);

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);
	UpdateDoorHole();
}

void FArmySecurityDoor::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			TopViewFillingRect->Draw(PDI, View);
			TopViewTrackPolygon->Draw(PDI, View);
			TopViewRectImagePanel->Draw(PDI, View);
			TopViewDoorPlank->Draw(PDI, View);
			TopViewDoorTrack->Draw(PDI, View);
		}		
		else
		{
            RectImagePanel->bIsFilled = false;
            RectImagePanel->Draw(PDI, View);
            FArmyHardware::Draw(PDI, View);
            if (!FArmyObject::GetDrawModel(MODE_DOORPASS))
            {
                DoorPlank->Draw(PDI, View);
                DoorTrack->Draw(PDI, View);
            }
		}
	}
}
