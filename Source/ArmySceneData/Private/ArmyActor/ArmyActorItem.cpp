#include "ArmyActorItem.h"
#include "ArmyCommonTypes.h"
#include "ArmyResourceModule.h"
#include "ArmyGameInstance.h"
#include "ArmyHydropowerModeController.h"
#include "ArmyEditorEngine.h"
#include "ArmyActor.h"

void FArmyActorItem::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    FArmyObject::SerializeToJson(JsonWriter);

    if (ContentItem.IsValid())
    {
        JsonWriter->WriteObjectStart("contentItem");
        ContentItem->SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }

    check(Actor && Actor->IsValidLowLevel());
    Location = Actor->GetActorLocation();
    Rotation = Actor->GetActorRotation();
    Scale = Actor->GetActorScale3D();
	AXRActor* Owner = Cast<AXRActor>(Actor->GetOwner());
	if (Owner&&Owner->IsValidLowLevel())
	{
		TWeakPtr<FArmyObject> Object = Owner->GetRelevanceObject();
		JsonWriter->WriteObjectStart(TEXT("object"));
		Object.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteValue("ObjID",Actor->GetObjID());
	JsonWriter->WriteValue("SynIDPrivate",Actor->GetSynID());
	JsonWriter->WriteValue("location", Location.ToString());
    JsonWriter->WriteValue("rotation", Rotation.ToString());
    JsonWriter->WriteValue("scale", Scale.ToString());



    SERIALIZEREGISTERCLASS(JsonWriter, FArmyActorItem)
}

void FArmyActorItem::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    FArmyObject::Deserialization(InJsonData);

	UXRResourceManager* ResMgr = FArmyResourceModule::Get ().GetResourceManager ();
	int32 ObjID = InJsonData->GetNumberField("ObjID");
	int32 SynIDPrivate = InJsonData->GetNumberField("SynIDPrivate");
	FVRSObject* ResultObj = ResMgr->GetObjFromObjID (ObjID);
	ContentItem = ResMgr->GetContentItemFromID(SynIDPrivate);
	if (ContentItem.IsValid())
	{
		ContentItem->Deserialization(InJsonData->GetObjectField("contentItem"));
	}
    //ContentItem = MakeShareable(new FContentItemSpace::FContentItem());
    //ContentItem->Deserialization(InJsonData->GetObjectField("contentItem"));

    Location.InitFromString(InJsonData->GetStringField("location"));
    Rotation.InitFromString(InJsonData->GetStringField("rotation"));
    Scale.InitFromString(InJsonData->GetStringField("scale"));

	TSharedPtr<FArmyFurniture> NewFurniture = nullptr;

	const TSharedPtr<FJsonObject> ObjectJson =  InJsonData->GetObjectField(TEXT("object"));
	

    if (!Actor || !Actor->IsValidLowLevel())
    {
        Actor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GVC->GetWorld(), ContentItem, Location, Rotation, Scale);
        //@欧石楠修改actor label
        Actor->SetActorLabel(ContentItem->Name);
        //添加是否是商品的标记
        if (ContentItem->bIsSale)
        {
            Actor->Tags.Add(FName("IsSale"));
        }

		if(ContentItem->GetComponent().IsValid())
		{
			if (ObjectJson.IsValid())
			{
				NewFurniture = MakeShareable(new FArmyFurniture);
				NewFurniture->Deserialization(ObjectJson);
			}
			//OnDelegate_3DTo2DGanged.ExecuteIfBound(ContentItem->GetComponent(),Actor);
			 TSharedPtr<FArmyDesignModeController> DesignController = GGI->DesignEditor->CurrentController;
			if (DesignController.IsValid())
			{
				DesignController->ThreeTo2DGanged(ContentItem->GetComponent(),Actor,NewFurniture);
				Actor->GetRootComponent()->TransformUpdated.Broadcast(Actor->GetRootComponent(),EUpdateTransformFlags::None, ETeleportType::None);
			}
			
		}
	
    }
    else
    {
        Actor->SetActorLocation(Location); 
        Actor->SetActorRotation(Rotation);
        Actor->SetActorScale3D(Scale);
		AXRActor* Owner = Cast<AXRActor>(Actor->GetOwner());
		if (Owner&&Owner->IsValidLowLevel())
			Owner->GetRelevanceObject().Pin()->Deserialization(ObjectJson);
		//EUpdateTransformFlags::None, CurrentScopedUpdate->bHasTeleported ? ETeleportType::TeleportPhysics : ETeleportType::None);
		//Actor->GetRootComponent()->TransformUpdated.Broadcast(Actor->GetRootComponent(),EUpdateTransformFlags::None,ETeleportType::None);
        GXREditor->UpdatePivotLocationForSelection();
        GVC->Invalidate(true, true);
    }
}

void FArmyActorItem::PostDelete()
{
    if (Actor && Actor->IsValidLowLevel())
    {
        Actor->Destroy();
    }
}