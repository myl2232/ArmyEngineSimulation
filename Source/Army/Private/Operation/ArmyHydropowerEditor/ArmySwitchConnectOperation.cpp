#include "ArmySwitchConnectOperation.h"
#include "ArmyEditPoint.h"
#include "Style/XRStyle.h"
#include "Math/XRMath.h"
#include "SArmyInputBox.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "ArmyCommonTypes.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditorEngine.h"
#include "SceneManagement.h"
#include "Army3DManager.h"
#include "ArmyRoom.h"
#include "ArmyExtrusion/Public/XRExtrusionActor.h"
#include "ArmySceneData.h"
#include "ArmyActor.h"
#include "ArmyRectArea.h"
#include "ArmyFurniture.h"
#include "ArmySceneData.h"
#include "ArmyActor.h"
#include "ArmyFurnitureActor.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyGameInstance.h"
#include "ArmyObjectMinimal.h"
#include "ArmyActorConstant.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "ArmyLampSlotExtruder.h"

FArmySwitchConnectOperation::FArmySwitchConnectOperation(EModelType InBelongModel)
	:FArmyOperation (InBelongModel)
{
	IsUIOperating = false;
}

FArmySwitchConnectOperation::~FArmySwitchConnectOperation()
{

}

void FArmySwitchConnectOperation::Init()
{

}

void FArmySwitchConnectOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget /* = NULL */)
{

}

void FArmySwitchConnectOperation::Draw(FPrimitiveDrawInterface* PDI,const FSceneView* View)
{

}

void FArmySwitchConnectOperation::BeginOperation(XRArgument InArg /*= XRArgument ()*/)
{
	//取消箭头
	//ActorAxisRuler->SetAxisVisible(FArmyAxisRulerActor::AXIS_ALL, false);
	GVC->CancelDrop();
	//开启灯控设置时不显示坐标轴
	SetCoordAxisShow(false);
	//USelection::SelectNoneEvent.
	GXREditor->SelectNone(true,true,false);
	//初始化所有房间
	InitRoom();

	//初始化房间对应的开关
	InitFurniture();


	UpdateCanClick(SST::SST_ENTER);
	if (DetailView.IsValid())
	{
		//auto p = MakeShared<FArmySwitchConnectOperation>(this);
	//	TSharedPtr<FArmySwitchConnectOperation> pp(p);
		
		//DetailView->OwnerOperation= ;

		DetailView->ClearRootTree();
		/***
		* 1. 房间
		* 2. 某个房间的开关
		* 3. 某个开关已经关联的灯具,灯带 
		*/
		for (TWeakPtr<FArmyObject> RoomObject : RoomObjectes) 
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomObject.Pin());
			TSharedPtr<SArmySpaceTreeItem> LayerItem = MakeShareable(new SArmySpaceTreeItem);
			LayerItem->itemType = SArmySpaceTreeItem::IT_SPACE;
			LayerItem->SpaceName = FName(*Room->GetSpaceName()); 
			LayerItem->ItemID = Room->GetUniqueID();
			LayerItem->XRObject = RoomObject;
			TArray<TSharedPtr<TreeItemInfo>> TreeItemArray ;
			RoomFurnitureItemMap.MultiFind(Room->GetUniqueID(),TreeItemArray);
			for (TSharedPtr<TreeItemInfo>& Info : TreeItemArray)
			{
				TSharedPtr<SArmySpaceTreeItem> LayerItem1 = MakeShareable(new SArmySpaceTreeItem);
				LayerItem1->itemType = SArmySpaceTreeItem::IT_SWITCH;
				FString ActorName;
				Info->ActorItem->GetName(ActorName);
				LayerItem1->SpaceName = FName(* ActorName );
				LayerItem1->ItemID = Info->XRObjectItem.Pin()->GetUniqueID();
				LayerItem1->XRObject = Info->XRObjectItem;
				LayerItem1->Actor = Info->ActorItem;
				LayerItem->AddChild(LayerItem1);

				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(Info->XRObjectItem.Pin());
				for (int32 i = 0;i<Furniture->SwitchCoupletNum;i++)
				{
					int32 index = i+1;
					FString Desc = FString::Printf(TEXT("%d联"),index);
					TSharedPtr<SArmySpaceTreeItem> LayerItem2 = MakeShareable(new SArmySpaceTreeItem);
					LayerItem2->itemType = SArmySpaceTreeItem::IT_LIAN;
					LayerItem2->SpaceName = FName(*Desc);
					LayerItem2->ItemID = Info->XRObjectItem.Pin()->GetUniqueID();
					LayerItem2->XRObject = Info->XRObjectItem;
					LayerItem2->Actor = Info->XRObjectItem.Pin()->GetRelevanceActor();
					LayerItem2->SwtichIndex = index;
					LayerItem1->AddChild(LayerItem2);
					{
						TArray<FGuid> LightGuides;
						Furniture->RelatedFurnitureMap.MultiFind(index,LightGuides);
						for (auto& LightId:LightGuides)
						{
							TSharedPtr<SArmySpaceTreeItem> LayerItem3 = MakeShareable(new SArmySpaceTreeItem);
							TWeakPtr<FArmyObject> LightObjectWeak = FArmySceneData::Get()->GetObjectByGuid(E_ConstructionModel,LightId);
							if (LightObjectWeak.IsValid())
							{
								if (LightObjectWeak.Pin()->GetType()==OT_ComponentBase)
								{
									RefreshFurniture(LightObjectWeak,LayerItem3);
								}
								LayerItem2->AddChild(LayerItem3);
								continue;
							}
						}
					}
					{
						TArray<FGuid> LightGuides;
						Furniture->RelatedExtrusionLightMap.MultiFind(index, LightGuides);
						for (auto& LightId : LightGuides)
						{
							//灯带
							TSharedPtr<SArmySpaceTreeItem> LayerItem3 = MakeShareable(new SArmySpaceTreeItem);
							auto Lightactor = FArmyLampSlotExtruder::GetAllLightModel().Find(LightId);
							if (Lightactor)
							{
								RefreshExtrusion(LightId, LayerItem3);
								LayerItem2->AddChild(LayerItem3);
							}
						}
					}
				}

			}
			DetailView->AddItemToTree(LayerItem);
		}
		DetailView->Refresh();
	}

	OnBenginOrEndOperationDelegate.ExecuteIfBound(true);
}

void FArmySwitchConnectOperation::EndOperation()
{
	UpdateCanClick(SST::SST_EXIT);
	GVC->SetAutoMultiSelectEnabled(false);
	SetCoordAxisShow(true);
	CloseSelectFilter();
	
	//取消所有选择
	GXREditor->SelectNone(true, true, false);

	OnBenginOrEndOperationDelegate.ExecuteIfBound(false);
}

void FArmySwitchConnectOperation::Tick()
{

}

bool FArmySwitchConnectOperation::CanExit()
{
	if (CurrentItem.IsValid() )
		return false;

	if (IsUIOperating)
		return false;

	return true;
}

void FArmySwitchConnectOperation::ProcessClick(FSceneView& View,HHitProxy* HitProxy,FKey Key,EInputEvent Event,uint32 HitX,uint32 HitY)
{

}

void FArmySwitchConnectOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y)
{

}

bool FArmySwitchConnectOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event)
{
	return false;
}

void FArmySwitchConnectOperation::RefreshFurniture(TWeakPtr<FArmyObject>& LightObjectWeak,TSharedPtr<SArmySpaceTreeItem>& LayerItem)
{
	
	if (LightObjectWeak.IsValid())
	{
		AActor* XRActor = Cast<AActor>(LightObjectWeak.Pin()->GetRelevanceActor());
		FString LightName;
		XRActor->GetName(LightName);
		
		LayerItem->itemType = SArmySpaceTreeItem::IT_LIGHT;
		LayerItem->SpaceName = FName(*LightName);
		LayerItem->ItemID = LightObjectWeak.Pin()->GetUniqueID();
		LayerItem->XRObject = LightObjectWeak;
		LayerItem->Actor = XRActor;

	}
	
}

//void FArmySwitchConnectOperation::RefreshExtrusion(TWeakPtr<FArmyObject>& InObject,TSharedPtr<SArmySpaceTreeItem>& LayerItem)
//{
//	if (InObject.IsValid())
//	{
//		TWeakPtr<FArmyBaseArea> RectArea = StaticCastSharedPtr<FArmyBaseArea>(InObject.Pin());
//		AActor* XRActor = RectArea.Pin()->GetLampSlotActor();
//		if (XRActor)
//		{
//			FString LightName;
//			XRActor->GetName(LightName);
//
//			LayerItem->itemType = SArmySpaceTreeItem::IT_LIGHT;
//			LayerItem->SpaceName = FName(*LightName);
//			LayerItem->ItemID = RectArea.Pin()->GetUniqueID();
//			LayerItem->XRObject = InObject;
//			LayerItem->Actor = XRActor;
//		}
//	}
//}

void FArmySwitchConnectOperation::RefreshExtrusion(const FGuid& LightID, TSharedPtr<SArmySpaceTreeItem>& LayerItem)
{
	//if (InObject.IsValid())
	//{
		//TWeakPtr<FArmyBaseArea> RectArea = StaticCastSharedPtr<FArmyBaseArea>(InObject.Pin());
	auto Lightactor= FArmyLampSlotExtruder::GetAllLightModel().Find(LightID);
	if (Lightactor)
	{	
		AActor* XRActor = Cast<AActor>(*Lightactor);
		if (XRActor)
		{
			FString LightName;
			XRActor->GetName(LightName);
			LayerItem->LightType = SwitchConnectLightType::Extrusion;
			LayerItem->itemType = SArmySpaceTreeItem::IT_LIGHT;
			LayerItem->SpaceName = FName(*LightName);
			LayerItem->ItemID = LightID;
			LayerItem->XRObject = nullptr;
			LayerItem->Actor = XRActor;
		}
	}
}

void FArmySwitchConnectOperation::SetDetailView(TSharedPtr<FArmyHydropowerSwitchConnect> InDetailView)
{
	DetailView = InDetailView;
	//void OnCheckStateChanged(ECheckBoxState InCheck,SArmySpaceTreeItemPtr CurrentItem);
	DetailView->CheckStateChanged.BindRaw(this,&FArmySwitchConnectOperation::CheckState);
	DetailView->OnDelete.BindRaw(this,&FArmySwitchConnectOperation::DeleteItem);
	DetailView->OnItemClicked.BindRaw(this, &FArmySwitchConnectOperation::OnUIClicked);
}

void FArmySwitchConnectOperation::CheckState(ECheckBoxState InCheck,SArmySpaceTreeItemPtr InCurrentItem)
{
	
	if (InCheck==ECheckBoxState::Checked&&!CurrentItem.IsValid())
	{
		CurrentItem = InCurrentItem;

		//郭子阳 1.15
		//添加时不显示坐标
		//SetCoordAxisShow(false);

		//郭子阳 1.14
		//再次选中时高亮已选择的灯
		auto lights = InCurrentItem->GetChildren();
		GXREditor->DeselectActor(true, true);
		for (auto light : lights)
		{
			if (light->LightType == SwitchConnectLightType::Extrusion)
			{
				auto Lightactor = FArmyLampSlotExtruder::GetAllLightModel().Find(light->ItemID);
				if(Lightactor)
				{ 
					GXREditor->SelectActor(*Lightactor, true, true);
				}
			}
			else
			{ 
				auto Obj = light->XRObject.Pin();
				if (Obj.IsValid())
				{
					GXREditor->SelectActor(Obj->GetRelevanceActor(), true, true);
				}
			}
		}

		//只允许选择灯具
		OpenSelectFilter(); 

		UpdateCanClick(SST::SST_ADD);
	}
	else if (InCheck!=ECheckBoxState::Checked&&CurrentItem.IsValid())
	{
		//郭子阳 1.15
		//结束添加时允许显示坐标
		//SetCoordAxisShow(true);

		//取消只允许选择灯具
		CloseSelectFilter();


		UpdateCanClick(SST::SST_EXIT_ADD);
		TArray<AActor*> OutActors;
		GXREditor->GetSelectedActors(OutActors);
		for (AActor* InActor : OutActors)
		{
			TSharedPtr<FArmyFurniture> CurrentFurniture = StaticCastSharedPtr<FArmyFurniture>(CurrentItem->XRObject.Pin());
			TWeakPtr<FArmyObject> Object = nullptr;
			AArmyExtrusionActor* ExtrusionActor = nullptr;
			TArray<FGuid> arr;
			CurrentFurniture->RelatedFurnitureMap.MultiFind(CurrentItem->SwtichIndex,arr);
			bool bNormal = true;
			if (InActor->IsA(AArmyExtrusionActor::StaticClass()))
			{
				ExtrusionActor = Cast<AArmyExtrusionActor>(InActor);
				Object =  StaticCastSharedPtr<FArmyObject>(ExtrusionActor->AttachSurfaceArea.Pin());
				bNormal = false;
			}
			else
			{
				AXRActor* Owner = Cast<AXRActor>(InActor->GetOwner());
				if (Owner->IsValidLowLevel())
					Object = Owner->GetRelevanceObject();
			}		
			if (Object.IsValid() && !arr.Contains(Object.Pin()->GetUniqueID()))
			{
				CurrentFurniture->RelatedFurnitureMap.Add(CurrentItem->SwtichIndex, Object.Pin()->GetUniqueID());
				TSharedPtr<SArmySpaceTreeItem> LayerItem3 = MakeShareable(new SArmySpaceTreeItem);
				if (Object.Pin()->GetType() == OT_ComponentBase)
					RefreshFurniture(Object, LayerItem3);
					InCurrentItem->AddChild(LayerItem3);
			}
			else if(ExtrusionActor)
			{
				auto LightIDPtr = FArmyLampSlotExtruder::GetAllLightModel().FindKey(ExtrusionActor);

				TArray<FGuid> IDs;
				CurrentFurniture->RelatedExtrusionLightMap.MultiFind(CurrentItem->SwtichIndex, IDs);
				if (LightIDPtr && !IDs.Contains(*LightIDPtr))
				{
					CurrentFurniture->RelatedExtrusionLightMap.Add(CurrentItem->SwtichIndex, *LightIDPtr);
					TSharedPtr<SArmySpaceTreeItem> LayerItem3 = MakeShareable(new SArmySpaceTreeItem);
					RefreshExtrusion(*LightIDPtr, LayerItem3);
					InCurrentItem->AddChild(LayerItem3);
				}
			}
		}
		DetailView->Refresh();
		GXREditor->SelectNone(false,true,false);
		CurrentItem = nullptr;
	}
}

void FArmySwitchConnectOperation::DeleteItem(SArmySpaceTreeItemPtr CurrentItem)
{
	SArmySpaceTreeItemPtr ParentItem = CurrentItem->GetParent();
	if (ParentItem.IsValid())
	{
		FGuid ObjectID = ParentItem->XRObject.Pin()->GetUniqueID();
		TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(FArmySceneData::Get()->GetObjectByGuidAlone(ObjectID).Pin());
		
		FGuid LightID = CurrentItem->ItemID;
		//if (CurrentItem->XRObject.IsValid())
		//{
		//	LightID = CurrentItem->XRObject.Pin()->GetUniqueID();
		//}
		//else
		//{
		//	//灯带
		//	AArmyExtrusionActor* ExtrusionActor = Cast<AArmyExtrusionActor>(CurrentItem->Actor);
		//	LightID=
		//	if (!ExtrusionActor || !ExtrusionActor->IsValidLowLevel())
		//	{
		//		//发生错误
		//		return;
		//	}
		//	auto LightIDPtr = FArmyLampSlotExtruder::GetAllLightModel().FindKey(ExtrusionActor);
		//	if (LightIDPtr)
		//	{
		//		LightID = *LightIDPtr;
		//	}
		//	else
		//	{
		//		//发生错误
		//		return;
		//	}
		//}

		furniture->RelatedFurnitureMap.RemoveSingle(ParentItem->SwtichIndex, LightID);
		furniture->RelatedExtrusionLightMap.RemoveSingle(ParentItem->SwtichIndex, LightID);
		ParentItem->RemoveChild(CurrentItem);
		DetailView->Refresh();
	}
}

//选择灯具

void FArmySwitchConnectOperation::OnUIClicked(SArmySpaceTreeItemPtr Item) {

	if (IsAddingLight)
	{
		return;
	}

	IsUIOperating = true;
	switch (Item->itemType)
	{
	case SArmySpaceTreeItem::IT_SWITCH:
	case SArmySpaceTreeItem::IT_LIAN: 
	case SArmySpaceTreeItem::IT_LIGHT:
		GXREditor->DeselectActor(true, true);

		if (Item->LightType == SwitchConnectLightType::Extrusion)
		{
			auto Lightactor = FArmyLampSlotExtruder::GetAllLightModel().Find(Item->ItemID);
			if (Lightactor)
			{
				GXREditor->SelectActor(*Lightactor, true, true);
			}
		}
		else
		{
			auto Obj = Item->XRObject.Pin();
			if (Obj.IsValid())
			{
				GXREditor->SelectActor(Obj->GetRelevanceActor(), true, true);
			}
		}

		//GXREditor->SelectActor(Item->XRObject.Pin()->GetRelevanceActor(), true, true);
		break;
	default:
		break;
	}
	IsUIOperating = false;
}

void FArmySwitchConnectOperation::InitRoom()
{
	RoomObjectes.Empty();
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if (InObjects.Num ()==0)
		return ;
	for (int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if (object->GetType()!=OT_InternalRoom)
			continue;
		RoomObjectes.AddUnique(object);
	}
}

void FArmySwitchConnectOperation::InitFurniture()
{
	RoomFurnitureItemMap.Empty();
	for (TActorIterator<AXRFurnitureActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		AXRFurnitureActor* FurnitureActor = *ActorItr;
		if (FurnitureActor->IsValidLowLevel())
		{
			TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(FurnitureActor->GetRelevanceObject().Pin());
			if (!Furniture.IsValid())
				continue;
			if (!Furniture->IsSwitch())
				continue;
			FVector ActorLocation = Furniture->GetRelevanceActor()->GetActorLocation();
			for (auto& RoomObj:RoomObjectes)
			{
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());
				if ( Room->IsPointInRoom(ActorLocation))
				{
					FGuid RoomID = Room->GetUniqueID();
					TSharedPtr<TreeItemInfo> ItemInfo = MakeShareable(new TreeItemInfo);
					ItemInfo->ActorItem = Furniture->GetRelevanceActor();
					ItemInfo->XRObjectItem = Furniture;
					RoomFurnitureItemMap.Add(RoomID,ItemInfo);
				}
			}
		}
		
	}
}

void FArmySwitchConnectOperation::UpdateCanClick(SST InShowType)
{
    FString FilePath = FArmyActorPath::GetLightPath().ToString();
	FString SwitchFilePath = FArmyActorPath::GetSwitchPath().ToString();
	switch(InShowType)
	{
	case SST_ENTER:
	{
		for (auto ObjGroup:SArmyModelContentBrowser::ObjectGroupCategoriesArray)
		{
			if (ObjGroup->ModelName==GGI->DesignEditor->GetCurrentDesignMode())
			{
				if (SwitchFilePath.Equals(ObjGroup->ItemPath))
				{
					GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(ObjGroup->ItemPath)),false,true,true);
				}
				else
				{
					GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(ObjGroup->ItemPath)),false,false,false);
				}
			}
		}
	}
	break;
	case SST_ADD:
		IsAddingLight = true;

		GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(SwitchFilePath)),false,true,false);
		GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(FilePath)),false,true,true);

	break;
	case SST_EXIT_ADD:
		IsAddingLight = false;

		GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(SwitchFilePath)),false,true,true);

		break;
	case SST_EXIT:
	{		
		GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(GGI->DesignEditor->GetCurrentDesignMode())),false,true,true);
	}
	break;
	}
}

void FArmySwitchConnectOperation::SetCoordAxisShow(bool show)
{
	auto World = GVC->GetWorld();
	auto EditorViewportClient = Cast<UArmyEditorViewportClient>(GXREditor->GameViewportForWorld(World));
	EditorViewportClient->bForceHiddenWidget = !show;
}

void FArmySwitchConnectOperation::OnSelectedSomething(UObject * obj)
{
	AActor * actor = Cast<AActor>(obj);
	if (actor)
	{
		DetailView->HighLightCorrespondingUI(actor);
	}
}

void FArmySwitchConnectOperation::OpenSelectFilter()
{
	GXREditor->CanSelectFilter.BindRaw(this, &FArmySwitchConnectOperation::CanSelect);
	GXREditor->OpenSelectFilter();
}

void FArmySwitchConnectOperation::CloseSelectFilter()
{
	GXREditor->CloseSelectFilter();
}
bool FArmySwitchConnectOperation::CanSelect(AActor * actor)
{
    if (actor->GetFolderPath() == FArmyActorPath::GetLightPath() ||
        actor->GetFolderPath() == FArmyActorPath::GetLightSourcePath())
    {
        return true;
    }

	static TArray<FString> IDs{ "GC0418","GC0400" };

	if (IsAddingLight)
	{
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromItemID(actor->GetSynID());
		
		if (ActorItemInfo.IsValid() && IDs.Contains(ActorItemInfo->codeStrId))
		{
			return true;
		}
	}
	return false;
}
