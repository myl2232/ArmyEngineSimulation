#include "ArmyHardModeExecute.h"
#include "ArmyResourceModule.h"
#include "ArmyRectArea.h"
#include "ArmyFreePolygonArea.h"
#include "ArmyCircleArea.h"
#include "ArmyRoom.h"
#include "ArmyHardware.h"
#include "ArmyWindow.h"
#include "ArmyWindowActor.h"
#include "ArmyRoomEntity.h"
#include "ArmyGameInstance.h"
#include "ArmyExtrusionActor.h"
#include "ArmyDataTools.h"
#include "ArmyWallLine.h"
#include "ArmyPass.h"
#include "ArmyNewPass.h"
#include "ArmyAutoDesignModel.h"


const TSharedRef<FArmyHardModeExecute>& FArmyHardModeExecute::Get()
{
	static const TSharedRef<FArmyHardModeExecute> Instance = MakeShareable(new FArmyHardModeExecute);
	return Instance;
}

void FArmyHardModeExecute::ExecHardMatching(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyHardModeData> InHardModeData)
{
	HardModeData = InHardModeData;
	RoomEntity = Room;

	ClearHardModeData(Room);
	ModellingFloorMatching(Room);
	ModellingWallMatching(Room);
	ModellingRoofMatching(Room);
	ModellingBuckleMatching(Room);
}

void FArmyHardModeExecute::ExecHardwareMatching(TSharedPtr<FArmyHardwareData> InHardwareData)
{
	if (!InHardwareData.IsValid())
	{
		return;
	}

	ClearHardwareData();

	HardwareData = InHardwareData;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	if (HardwareData->BridgeStoneList.Num() > 0 &&
		HardwareData->BridgeStoneList[0]->StoneMaterial.IsValid())
	{

		TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
		FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
		for (auto It : ResultSurfaces)
		{
			TSharedPtr<FArmyRoomSpaceArea> Space = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
			if ((FMath::IsNearlyEqual(Space->GetPlaneNormal().Z, 1.f, KINDA_SMALL_NUMBER)) &&
				(Space->GenerateFromObjectType == OT_Pass
					|| Space->GenerateFromObjectType == OT_NewPass
					|| Space->GenerateFromObjectType == OT_DoorHole
					|| Space->GenerateFromObjectType == OT_SlidingDoor
					|| Space->GenerateFromObjectType == OT_SecurityDoor
					|| Space->GenerateFromObjectType == OT_Door))
			{
				if (FArmyHardModeData::ModelEntityIsValid(HardwareData->BridgeStoneList[0]->StoneMaterial))
				{
					TilePasteStyle CurrentStyle;
					//TSharedPtr<class FContentItemSpace::FContentItem>  BridgeStoneContentItem
						//= HardwareData->BridgeStoneList[0]->StoneMaterial->FurContentItem;
					ShowDesignPackage(HardwareData->BridgeStoneList[0]->StoneMaterial, Space, CurrentStyle);
				}
			}
		}
	}
	
	
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto ItRoom : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(ItRoom.Pin());
		TArray< TSharedPtr<class FArmyHardware> > HardwareList;
		Room->GetHardwareListRelevance(HardwareList);
		for (auto Hardware : HardwareList)
		{
			if (Hardware->GetType() == OT_Window || 
				Hardware->GetType() == OT_FloorWindow
				)
			{
				TSharedPtr<FArmyWindow> Window = StaticCastSharedPtr<FArmyWindow>(Hardware);
				if (Window->GetIfGenerateWindowStone() == false || Window->HoleWindow == NULL)
					continue;
				
				if (HardwareData->WindowStoneList.Num() > 0 && FArmyHardModeData::ModelEntityIsValid(
					HardwareData->WindowStoneList[0]->StoneMaterial))
				{
					TSharedPtr<class FContentItemSpace::FContentItem>  StoneContentItem
						= HardwareData->WindowStoneList[0]->StoneMaterial->FurContentItem;
					UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(StoneContentItem);
					Window->SetContentItem(StoneContentItem);
				}
			}
			else if (Hardware->GetType() == OT_RectBayWindow ||
					Hardware->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> Window = StaticCastSharedPtr<FArmyRectBayWindow>(Hardware);
				if (Window->GetIfGenerateWindowStone() == false || Window->HoleWindow == NULL)
					continue;

				if (HardwareData->WindowStoneList.Num() > 0 && FArmyHardModeData::ModelEntityIsValid(
					HardwareData->WindowStoneList[0]->StoneMaterial))
				{
					TSharedPtr<class FContentItemSpace::FContentItem>  StoneContentItem
						= HardwareData->WindowStoneList[0]->StoneMaterial->FurContentItem;
					Window->SetContentItem(StoneContentItem);
				}
			}
		}
	}

	if (HardwareData->PassModelList.Num() > 0 &&
		HardwareData->PassModelList[0]->PassMaterial.IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> PassList;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, PassList);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, PassList);
		for (auto ItPass : PassList)
		{
			if (ItPass.Pin()->GetType() == OT_Pass)
			{
				TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(ItPass.Pin());
				if (Pass->GetIfGeneratePassModel())
					Pass->ReplaceGoods(HardwareData->PassModelList[0]->PassMaterial->FurContentItem, GGI->GetWorld());
			}
			else if (ItPass.Pin()->GetType() == OT_NewPass)
			{
				TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(ItPass.Pin());
				if (Pass->GetIfGeneratePassModel())
					Pass->ReplaceGoods(HardwareData->PassModelList[0]->PassMaterial->FurContentItem, GGI->GetWorld());
			}
		}
	}
	
}

void FArmyHardModeExecute::ClearHardModeData(TSharedPtr<class FArmyRoomEntity> Room)
{
	TArray< TSharedPtr<class FArmyHardware> > RoomHarewareList;
	Room->GetRoom()->GetHardwareListRelevance(RoomHarewareList);
	TArray<TWeakPtr<FArmyObject>> SimpleComponents;
	Room->GetRoom()->GetObjectsRelevance(SimpleComponents);

	TArray<TWeakPtr<FArmyObject>> RoomSpaceAreas;
	TArray<TWeakPtr<FArmyObject>> TotalEditAreas;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, RoomSpaceAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_CircleArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, TotalEditAreas);

	for (auto ItSpace : RoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> Space = StaticCastSharedPtr<FArmyRoomSpaceArea>(ItSpace.Pin());
		if (Space.IsValid())
		{
			if (Space->AttachRoomID == Room->GetRoom()->GetUniqueID().ToString())
				Space->ResetSurfaceStyle(true);
			else
			{
				if (Space->GenerateFromObjectType == OT_Window
					|| Space->GenerateFromObjectType == OT_FloorWindow ||
					Space->GenerateFromObjectType == OT_RectBayWindow ||
					Space->GenerateFromObjectType == OT_TrapeBayWindow)
				{
					for (auto ItWindow : RoomHarewareList)
					{
						if (Space->AttachRoomID == ItWindow->GetUniqueID().ToString())
							Space->ResetSurfaceStyle(true);
					}
				}
				else if(Space->GenerateFromObjectType == OT_Beam
					|| Space->GenerateFromObjectType == OT_Pillar
					|| Space->GenerateFromObjectType == OT_AirFlue
					|| Space->GenerateFromObjectType == OT_PackPipe
					|| Space->GenerateFromObjectType == OT_IndependentWall)
				{
					for (auto ItComponent : SimpleComponents)
					{
						if (Space->AttachRoomID == ItComponent.Pin()->GetUniqueID().ToString())
							Space->ResetSurfaceStyle(true);
					}
				}
			}
			
		}
	}
}

void FArmyHardModeExecute::ClearHardwareData()
{
	TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
	for (auto It : ResultSurfaces)
	{
		TSharedPtr<FArmyRoomSpaceArea> Space = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
		if ((FMath::IsNearlyEqual(Space->GetPlaneNormal().Z, 1.f, KINDA_SMALL_NUMBER)) &&
			(Space->GenerateFromObjectType == OT_Pass
				|| Space->GenerateFromObjectType == OT_DoorHole
				|| Space->GenerateFromObjectType == OT_SlidingDoor
				|| Space->GenerateFromObjectType == OT_SecurityDoor
				|| Space->GenerateFromObjectType == OT_Door))
		{
			Space->ResetSurfaceStyle(true);
		}
	}

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto ItRoom : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(ItRoom.Pin());
		TArray< TSharedPtr<class FArmyHardware> > HardwareList;
		Room->GetHardwareListRelevance(HardwareList);
		for (auto Hardware : HardwareList)
		{
			if (Hardware->GetType() == OT_Window || 
				Hardware->GetType() == OT_FloorWindow)
			{
				TSharedPtr<FArmyWindow> Window = StaticCastSharedPtr<FArmyWindow>(Hardware);
				if (Window->GetIfGenerateWindowStone() == false || Window->HoleWindow == NULL)
					continue;
				Window->HoleWindow->WindowStoneName = TEXT("");
				Window->SetContentItem(nullptr);
			}
			else if (Hardware->GetType() == OT_RectBayWindow ||
					 Hardware->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> Window = StaticCastSharedPtr<FArmyRectBayWindow>(Hardware);
				if (Window->GetIfGenerateWindowStone() == false || Window->HoleWindow == NULL)
					continue;
				/*Window->HoleWindow->SetWindowBoardMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
				Window->HoleWindow->WindowStoneName = TEXT("");
				Window->HoleWindow->SetSynID(-1);*/
				Window->HoleWindow->WindowStoneName = TEXT("");
				Window->SetContentItem(nullptr);
			}
		}
	}
}

void FArmyHardModeExecute::ModellingWallMatching(TSharedPtr<class FArmyRoomEntity> Room)
{
	TMap<int,TSharedPtr<FArmyRoomSpaceArea>> BedRoomModellingSpaceList;
	TSharedPtr<FArmyRoomSpaceArea> SofaSpace;
	TSharedPtr<FArmyRoomSpaceArea> TVBenchSpace;
	if (Room->GetAiRoomType() == RT_Bedroom_Master)
	{
		HardModeData->GetRoomSpaceListFromBed(BedRoomModellingSpaceList, Room);

		if (BedRoomModellingSpaceList.FindRef(1).IsValid())
			BedRoomSpace = BedRoomModellingSpaceList.FindRef(1);
	}
	else if (Room->GetAiRoomType() == RT_LivingRoom)
	{
		SofaSpace = HardModeData->GetRoomSpaceFromSofaOrTVBench(Room, AI_Sofa_Master);
		TVBenchSpace = HardModeData->GetRoomSpaceFromSofaOrTVBench(Room, AI_TVBench);

		SofaBackSpace = SofaSpace;
	}
		//基础面的铺贴
	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = Room->GetWallSpaceArray();
	TSharedPtr<class FContentItemSpace::FContentItem>  WindowContentItem = nullptr;
	for (auto ItSpaceArea : RoomSpaceAreas)
	{
		if (ItSpaceArea.IsValid() && (ItSpaceArea->GetRoomStatus() & FArmyBaseArea::RoomStatus::FLAG_DEFAULTSTYLE))
			{
				TSharedPtr<FModellingWall>  ItModellingWall;
				
				if (BedRoomModellingSpaceList.Contains(1) &&
					BedRoomModellingSpaceList.FindRef(1).IsValid() &&
					ItSpaceArea.Get() == BedRoomModellingSpaceList.FindRef(1).Get())
				{
					ItModellingWall = HardModeData->GetModellingWallByType(1);
				}
				else if (BedRoomModellingSpaceList.Contains(2) &&
					BedRoomModellingSpaceList.FindRef(2).IsValid() &&
					ItSpaceArea.Get() == BedRoomModellingSpaceList.FindRef(2).Get())
				{
					ItModellingWall = HardModeData->GetModellingWallByType(2);
				}
				else if (TVBenchSpace.IsValid() && ItSpaceArea.Get() == TVBenchSpace.Get())
				{
					ItModellingWall = HardModeData->GetModellingWallByType(3);
				}
				else if (SofaSpace.IsValid() && ItSpaceArea.Get() == SofaSpace.Get())
				{
					ItModellingWall = HardModeData->GetModellingWallByType(4);
				}
				if (!ItModellingWall.IsValid())
				{
					ItModellingWall = HardModeData->GetModellingWallByType(0);
					if (!ItModellingWall.IsValid())
					{
						continue;
					}

					if (!ItModellingWall->BaseWallMaterial.IsValid())
					{
						continue;
					}
					else
					{
						if (!WindowContentItem.IsValid())
						{
							WindowContentItem = ItModellingWall->BaseWallMaterial->FurContentItem;
							ModellingWindows(Room, ItModellingWall);
							ModellingSimpleComponents(Room, ItModellingWall);
						}
					}
				}
				if (FArmyHardModeData::ModelEntityIsValid(ItModellingWall->BaseWallMaterial))
				{
					//TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem
						//= ItModellingWall->BaseWallMaterial->FurContentItem;
					ShowDesignPackage(ItModellingWall->BaseWallMaterial, ItSpaceArea, ItModellingWall->DesignStyle);
				}
				if (Room->GetAiRoomType() == RT_Bedroom_Master || Room->GetAiRoomType() == RT_LivingRoom)
				{
					if (ItModellingWall->WallType == 1 && ItModellingWall->SpaceAreaList.Num() > 0)
					{
						for (int i = 0; i < ItModellingWall->SpaceAreaList.Num(); i++)
						{
							TSharedPtr<FArmyBaseArea> EditArea = CreateInternalSpaceArea(ItSpaceArea, ItModellingWall->SpaceAreaList[i].DesignStyle,
								ItModellingWall->SpaceAreaList[i], ItModellingWall->BaseAreaWidth, ItModellingWall->BaseAreaHeight);
						
							if (!EditArea.IsValid())
							{
								while (ItSpaceArea->GetEditAreas().Num() > 0)
								{
									FArmySceneData::Get()->Delete(ItSpaceArea->GetEditAreas().Top());
								}
								break;
							}
						}
					}
				}
			}
	}
	
}

void FArmyHardModeExecute::ModellingFloorMatching(TSharedPtr<class FArmyRoomEntity> Room)
{
	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = Room->GetFloorSpaceArray();
	for (auto It : RoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea =It;
		if (RoomSpaceArea.IsValid())
		{
			if (RoomSpaceArea->SurfaceType == 0) // 地 & 顶
			{
				TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem;
				if (FArmyHardModeData::ModelEntityIsValid(HardModeData->GetModellingFloor()->BaseFloorMaterial))
				{
					//ContentItem = HardModeData->GetModellingFloor()->BaseFloorMaterial->FurContentItem;
					ShowDesignPackage(HardModeData->GetModellingFloor()->BaseFloorMaterial, RoomSpaceArea, HardModeData->GetModellingFloor()->DesignStyle);
				}
				if (FArmyHardModeData::ModelEntityIsValid(HardModeData->GetModellingFloor()->BaseFloorSkirtingLine))
				{
					//ContentItem = HardModeData->GetModellingFloor()->BaseFloorSkirtingLine->FurContentItem;
					ShowDesignPackage(HardModeData->GetModellingFloor()->BaseFloorSkirtingLine, RoomSpaceArea, HardModeData->GetModellingFloor()->DesignStyle);
				}
			}
		}
	}
}
void FArmyHardModeExecute::ModellingRoofMatching(TSharedPtr<class FArmyRoomEntity> Room)
{
	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = Room->GetCeilingSpaceArray();
	for (auto It : RoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = It;
		if (RoomSpaceArea.IsValid())
		{
			if (HardModeData->GetModellingRoof()->ExtrusionHeight > 0.000001f)
			{
				RoomSpaceArea->SetExtrusionHeight(FArmySceneData::WallHeight - HardModeData->GetModellingRoof()->ExtrusionHeight);
			}
			TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem;
			if (FArmyHardModeData::ModelEntityIsValid(HardModeData->GetModellingRoof()->BaseRoofMaterial))
			{
				//ContentItem = HardModeData->GetModellingRoof()->BaseRoofMaterial->FurContentItem;
				ShowDesignPackage(HardModeData->GetModellingRoof()->BaseRoofMaterial, RoomSpaceArea, HardModeData->GetModellingRoof()->DesignStyle);
				TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
				FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
				for (auto It : ResultSurfaces)
				{
					TSharedPtr<FArmyRoomSpaceArea> BeamSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
					if (BeamSpace->GenerateFromObjectType == OT_Beam)
					{
						TilePasteStyle BeamStyle = HardModeData->GetModellingRoof()->DesignStyle;
						BeamStyle.HeightToFloor = 0.f;
						ShowDesignPackage(HardModeData->GetModellingRoof()->BaseRoofMaterial, BeamSpace, BeamStyle);
					}
				}
			}
			if (Room->GetAiRoomType() != RT_LivingRoom)
			{
				for (int i = 0; i < HardModeData->GetModellingRoof()->SpaceAreaList.Num(); i++)
				{
					FModellingAreaInfo InAreaInfo = HardModeData->GetModellingRoof()->SpaceAreaList[i];
					TSharedPtr<FArmyBaseArea> ResultArea = CreateInternalArea(
						RoomSpaceArea,
						HardModeData->GetModellingRoof()->SpaceAreaList[i].DesignStyle,
						InAreaInfo,
						HardModeData->GetModellingRoof()->BaseAreaWidth,
						HardModeData->GetModellingRoof()->BaseAreaHeight
					);
					if (ResultArea.IsValid())
					{
						if (FArmyHardModeData::ModelEntityIsValid(InAreaInfo.LampSlotActor))
						{
							ShowDesignPackage(InAreaInfo.LampSlotActor, ResultArea, InAreaInfo.DesignStyle);
						}
						if (FArmyHardModeData::ModelEntityIsValid(InAreaInfo.ModelingLine))
						{
							TilePasteStyle currentStyle;
							ShowDesignPackage(InAreaInfo.ModelingLine, ResultArea, currentStyle);
						}
						TArray<TSharedPtr<FArmyHelpRectArea>> HelpAreas;

						if (InAreaInfo.AreaType == 0)
						{
							TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(ResultArea);
							HelpAreas = RectArea->GetInnearHoleSurface();
						}
						else if (InAreaInfo.AreaType == 1)
						{
							TSharedPtr<FArmyCircleArea> CircleArea = StaticCastSharedPtr<FArmyCircleArea>(ResultArea);
							if (CircleArea->GetInnearHoleSurface().IsValid())
							{
								TSharedPtr<FArmyClinderArea> HelpArea = CircleArea->GetInnearHoleSurface();
								if (0 < InAreaInfo.ModellingHelpAreas.Num())
								{
									FModellingHelpAreaInfo HelpAreaInfo = InAreaInfo.ModellingHelpAreas[0];
									ShowDesignPackage(HelpAreaInfo.ModellingMaterial, HelpArea, HelpAreaInfo.DesignStyle);
								}
							}
						}
						else if (InAreaInfo.AreaType == 2)
						{
							TSharedPtr<FArmyRegularPolygonArea> PolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ResultArea);
							HelpAreas = PolyArea->GetInnearHoleSurface();
						}
						else if (InAreaInfo.AreaType == 3)
						{
							TSharedPtr<FArmyFreePolygonArea> PolyArea = StaticCastSharedPtr<FArmyFreePolygonArea>(ResultArea);
							HelpAreas = PolyArea->GetInnearHoleSurface();
						}
						for (int i = 0;i < HelpAreas.Num(); i++)
						{
							TSharedPtr<FArmyHelpRectArea> HelpArea = HelpAreas[i];
							if (i < InAreaInfo.ModellingHelpAreas.Num())
							{
								FModellingHelpAreaInfo HelpAreaInfo = InAreaInfo.ModellingHelpAreas[i];
								ShowDesignPackage(HelpAreaInfo.ModellingMaterial, HelpArea, HelpAreaInfo.DesignStyle);
							}
						}
					}
				}
			}
			if (FArmyHardModeData::ModelEntityIsValid(HardModeData->GetModellingRoof()->BaseRoofSkirtingLine))
			{
				//ContentItem = HardModeData->GetModellingRoof()->BaseRoofSkirtingLine->FurContentItem;
				ShowDesignPackage(HardModeData->GetModellingRoof()->BaseRoofSkirtingLine, RoomSpaceArea, HardModeData->GetModellingRoof()->DesignStyle);
			}
		}
	}
}

void FArmyHardModeExecute::ModellingBuckleMatching(TSharedPtr<class FArmyRoomEntity> Room)
{
	TSharedPtr<FArmyRoomSpaceArea> FloorSpaceArea;

	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = Room->GetFloorSpaceArray();
	for (auto It : RoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = It;
		if (RoomSpaceArea.IsValid())
		{
			if (RoomSpaceArea->SurfaceType == 0) // 地 & 顶
			{
				FloorSpaceArea = RoomSpaceArea;
			}
		}
	}
	TArray<TSharedPtr<FArmyHardware>> ResultHardware;
	Room->GetRoom()->GetHardwareListRelevance(ResultHardware);

	TMap<int, int> ContentIndex;
	TArray<TSharedPtr<FContentItemSpace::FContentItem>> ContentArray;

	for (auto hardware : ResultHardware)
	{
		TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
		FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
		for (auto It : ResultSurfaces)
		{
			TSharedPtr<FArmyRoomSpaceArea> Space = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
			if ((FMath::IsNearlyEqual(Space->GetPlaneNormal().Z, 1.f, KINDA_SMALL_NUMBER)) &&
				(Space->GenerateFromObjectType == OT_Pass
					|| Space->GenerateFromObjectType == OT_DoorHole
					|| Space->GenerateFromObjectType == OT_SlidingDoor
					|| Space->GenerateFromObjectType == OT_SecurityDoor
					|| Space->GenerateFromObjectType == OT_Door))
			{
				FString  roomId = Space->AttachRoomID;
				FString  harwareid = hardware->GetUniqueID().ToString();
				if (roomId == harwareid)
				{
					if (HardModeData->GetModellingBuckle()->BuckleMaterial.IsValid())
					{
						if (FArmyHardModeData::ModelEntityIsValid(HardModeData->GetModellingBuckle()->BuckleMaterial))
						{
							int VectorCode = -1;
							Space->GetEdgeVectorCodeCloseToPos(FloorSpaceArea, VectorCode);
							Space->SetVectorCode(VectorCode);
							//TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem = HardModeData->GetModellingBuckle()->BuckleMaterial->FurContentItem;
							TilePasteStyle CurrentStyle;
							ShowDesignPackage(HardModeData->GetModellingBuckle()->BuckleMaterial, Space, CurrentStyle);
						}
					}
				}
			}
		}
	}
	

}

void FArmyHardModeExecute::ModellingWindows(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FModellingWall> InModellingWall)
{
	TArray< TSharedPtr<class FArmyHardware> > RoomHarewareList;
	Room->GetRoom()->GetHardwareListRelevance(RoomHarewareList);
	TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
	for (auto It : ResultSurfaces)
	{
		TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
		if (Surface.IsValid() && 
			(Surface->GenerateFromObjectType == OT_Window ||
			Surface->GenerateFromObjectType == OT_FloorWindow ||
			Surface->GenerateFromObjectType == OT_RectBayWindow ||
			Surface->GenerateFromObjectType == OT_TrapeBayWindow))
		{
			for (auto ItWindow : RoomHarewareList)
			{
				if (Surface->AttachRoomID == ItWindow->GetUniqueID().ToString()
					&& InModellingWall->BaseWallMaterial.IsValid())
				{
					ShowDesignPackage(InModellingWall->BaseWallMaterial, Surface, InModellingWall->DesignStyle);
				}
			}
		}
	}
}

void FArmyHardModeExecute::ExecDoors()
{
	TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
	TArray<TWeakPtr<FArmyObject>> PassList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, PassList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, PassList);
	for (auto ItPass : PassList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(ItPass.Pin());
		if (Hardware->GetType() == OT_Pass)
		{
			TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(Hardware);
			if (Pass->GetIfGeneratePassModel())
				continue;
		}
		else if (Hardware->GetType() == OT_NewPass)
		{
			TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(Hardware);
			if (Pass->GetIfGeneratePassModel())
				continue;
		}

		TSharedPtr<FArmyRoomSpaceArea>  FirstRoomSpace;
		TSharedPtr<FArmyRoomSpaceArea>  SecondRoomSpace;
		TSharedPtr<FArmyRoomSpaceArea>  UseRoomSpace;
		if (Hardware->FirstRelatedRoom.IsValid())
		{
			if (FArmyDataTools::GetRoomType(Hardware->FirstRelatedRoom->GetSpaceId()) == RT_LivingRoom)
			{
				FirstRoomSpace = SofaBackSpace;
			}
			else if (FArmyDataTools::GetRoomType(Hardware->FirstRelatedRoom->GetSpaceId()) == RT_Bedroom_Master)
			{
				FirstRoomSpace = BedRoomSpace;
			}
			
			if (!FirstRoomSpace.IsValid())
			{
				TArray<TSharedPtr<class FArmyWallLine>> WallLines;
				WallLines = Hardware->FirstRelatedRoom->GetWallLines();
				if (WallLines.Num() > 0)
					for (auto ItSpace : ResultSurfaces)
					{
						TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(ItSpace.Pin());
						if (Surface->AttachBspID == WallLines[0]->GetUniqueID().ToString())
							FirstRoomSpace = Surface;
					}
			}
				
		}
		if (Hardware->SecondRelatedRoom.IsValid())
		{
			if (FArmyDataTools::GetRoomType(Hardware->SecondRelatedRoom->GetSpaceId()) == RT_LivingRoom)
			{
				SecondRoomSpace = SofaBackSpace;
			}
			else if (FArmyDataTools::GetRoomType(Hardware->SecondRelatedRoom->GetSpaceId()) == RT_Bedroom_Master)
			{
				SecondRoomSpace = BedRoomSpace;
			}

			if (!SecondRoomSpace.IsValid())
			{
				TArray<TSharedPtr<class FArmyWallLine>> WallLines;
				WallLines = Hardware->SecondRelatedRoom->GetWallLines();
				if (WallLines.Num() > 0)
					for (auto ItSpace : ResultSurfaces)
					{
						TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(ItSpace.Pin());
						if (Surface->AttachBspID == WallLines[0]->GetUniqueID().ToString())
						{
							SecondRoomSpace = Surface;
						}
						/*if (WallLines[0]->GetUniqueID().ToString().Contains(Surface->AttachBspID))
							SecondRoomSpace = Surface;*/
					}
			}

		}

		int FirstStyle = -1;//0-墙漆，1-壁纸，2-瓷砖或者地板
		int SecondStyle = -1;
		if (!FirstRoomSpace.IsValid() || !SecondRoomSpace.IsValid())
		{
			continue;
		}
		if (FirstRoomSpace->GetMatStyle()->GetEditType() == S_SeamlessStyle)
		{
			TSharedPtr<FArmySeamlessStyle> Style = StaticCastSharedPtr<FArmySeamlessStyle>(FirstRoomSpace->GetMatStyle());
			if (Style.IsValid())
			{
				if (Style->SeamlesStyleType == 0)
					FirstStyle = 1;
				else
					FirstStyle = 0;
			}
		}
		else
			FirstStyle = 2;
		if (SecondRoomSpace->GetMatStyle()->GetEditType() == S_SeamlessStyle)
		{
			TSharedPtr<FArmySeamlessStyle> Style = StaticCastSharedPtr<FArmySeamlessStyle>(SecondRoomSpace->GetMatStyle());
			if (Style.IsValid())
			{
				if (Style->SeamlesStyleType == 0)
					SecondStyle = 1;
				else
					SecondStyle = 0;
			}
		}
		else
			SecondStyle = 2;

		if (FirstStyle < SecondStyle)
			UseRoomSpace = FirstRoomSpace;
		else if (FirstStyle > SecondStyle)
			UseRoomSpace = SecondRoomSpace;
		else//判断面积
		{
			float FirstArea, FirstPerimeter;
			float SecondArea, SecondPerimeter;
			Hardware->FirstRelatedRoom->GetRoomAreaAndPerimeter(FirstArea, FirstPerimeter);
			Hardware->SecondRelatedRoom->GetRoomAreaAndPerimeter(SecondArea, SecondPerimeter);
			if (FirstArea > SecondArea)
				UseRoomSpace = FirstRoomSpace;
			else
				UseRoomSpace = SecondRoomSpace;
		}

		for (auto It : ResultSurfaces)
		{
			TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
			if (Surface.IsValid()
				&& !FMath::IsNearlyEqual(Surface->GetPlaneNormal().Z, 1.f, KINDA_SMALL_NUMBER)
				&& (Surface->GenerateFromObjectType == OT_Pass ||
					Surface->GenerateFromObjectType == OT_NewPass)
				&& Surface->AttachRoomID == ItPass.Pin()->GetUniqueID().ToString())
			{
				TilePasteStyle Tps;
				Tps.M_AlignmentType = UseRoomSpace->GetStyle()->GetAlignType();
				Tps.InternalDist = UseRoomSpace->GetStyle()->GetInternalDist();
				Tps.XDirOffset = UseRoomSpace->GetStyle()->GetXDirOffset();
				Tps.YDirOffset = UseRoomSpace->GetStyle()->GetYDirOffset();
				Tps.RotationAngle = UseRoomSpace->GetStyle()->GetRotationAngle();
				Tps.HeightToFloor = UseRoomSpace->GetStyle()->GetHeightToFloor();
				Tps.GapColor = UseRoomSpace->GetGapColor();
				ShowDesignPackage(UseRoomSpace->GetMatStyle(), Surface, Tps);
			}
		}
	}
}

void FArmyHardModeExecute::ModellingSimpleComponents(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FModellingWall> InModellingWall)
{
	TArray<TWeakPtr<FArmyObject>> SimpleComponents;
	Room->GetRoom()->GetObjectsRelevance(SimpleComponents);
	TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
	for (auto It : ResultSurfaces)
	{
		TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
		if (Surface.IsValid() && (Surface->GenerateFromObjectType == OT_Beam
			|| Surface->GenerateFromObjectType == OT_Pillar
			|| Surface->GenerateFromObjectType == OT_AirFlue
			|| Surface->GenerateFromObjectType == OT_PackPipe
			|| Surface->GenerateFromObjectType == OT_IndependentWall))
		{
			for (auto ItComponent : SimpleComponents)
			{
				if (Surface->AttachRoomID == ItComponent.Pin()->GetUniqueID().ToString()
					&& InModellingWall->BaseWallMaterial.IsValid())
				{
					ShowDesignPackage(InModellingWall->BaseWallMaterial, Surface, InModellingWall->DesignStyle);
				}
			}
		}
	}
}

// 铺贴颜值包通用方法
void FArmyHardModeExecute::ComShowDesignPackage(TSharedPtr<FContentItemSpace::FContentItem> ContentItem, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItem, TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle)
{
	if (!editArea.IsValid())
		return;
	if (!ContentItem.IsValid())
		return;

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FContentItemSpace::FResObj> ResObj = resArr[0];
	TSharedPtr<FContentItemSpace::FProductObj> PdtObj = ContentItem->ProObj;

	TSharedPtr<FContentItemSpace::FArmyHardModeRes> Res = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeRes>(ResObj);
	TSharedPtr<FContentItemSpace::FArmyHardModeProduct> Pro = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeProduct>(PdtObj);
	//checkSlow(Pro->Width != 0.0f);
	//checkSlow(Pro->Length != 0.0f);
	FString _FilePath = ResObj->FilePath;
	if (!FPaths::FileExists(*_FilePath))//文件不存在直接返回
		return;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	CategryApplicationType PavingMethod = Res->ResourceCategryType;

	TSharedPtr<FArmyBaseEditStyle> MatStyle = NULL;
	switch (PavingMethod)
	{
	case CAT_None:
		break;
	case CAT_WallPaper:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmySeamlessStyle());
			}
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
			if (ContentItem->codeStrId.Equals(TEXT("GC0294")))
			{
				if (InDesignStyle.IsHaveWallPaintColor)
				{
					MatStyle->SetWallPaintColor(InDesignStyle.WallPaintColor);
				}
				else
				{
					FLinearColor color;
					MID->GetVectorParameterValue("Diffuse_Color", color);
					MatStyle->SetWallPaintColor(color);
				}
				TSharedPtr<FArmySeamlessStyle> tempStyle = StaticCastSharedPtr<FArmySeamlessStyle>(MatStyle);
				tempStyle->SeamlesStyleType = 1;
				tempStyle->ColorPaintValueKey = InDesignStyle.ColorPaintValueKey;
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		}
		break;
	}
	case CAT_Continue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmyContinueStyle());
			}
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_SlopeContinue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmySlopeContinueStyle());
			}
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_People:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmyHerringBoneStyle());
			}
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Worker:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmyWorkerStyle());
			}
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Wind:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmyWhirlwindStyle());
			}
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Space:
	{
		break;
		MatStyle = GetEditStyle(InDesignStyle.EdityType);
		if (!MatStyle.IsValid())
		{
			MatStyle = MakeShareable(new FArmyIntervalStyle());
		}
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);

			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_Interlace:
	{
		break;
		MatStyle = GetEditStyle(InDesignStyle.EdityType);
		if (!MatStyle.IsValid())
		{
			MatStyle = MakeShareable(new FArmyCrossStyle());
		}
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_TrapeZoid:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = GetEditStyle(InDesignStyle.EdityType);
			if (!MatStyle.IsValid())
			{
				MatStyle = MakeShareable(new FArmyTrapezoidStyle());
			}
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_AntiCornerBrickMethod:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (!mat)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
			break;
		}
		if (editArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(editArea);
			SCOPE_TRANSACTION(TEXT("修改矩形区域无角砖波打线"));

			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			ResultArea->SetBodaBrickAndThickness(Pro->Length / 10.f, Pro->Width / 10.f, ContentItem);
			ResultArea->SetBodaMaterial(MID, ContentItem->ID);
			ResultArea->SetBodaTextureURL(Res->FilePath);

			ResultArea->SetCornerType(AntiCornerBrick);
			ResultArea->SetState(OS_Normal);
			editArea->Modify();
		}
		else if (editArea->GetType() == OT_PolygonArea)
		{
			TSharedPtr<FArmyRegularPolygonArea> ResultRegularPolygon = StaticCastSharedPtr<FArmyRegularPolygonArea>(editArea);
			SCOPE_TRANSACTION(TEXT("修改多边形区域无角砖波打线"));
			FString _FilePath = Res->FilePath;
			ResultRegularPolygon->SetBodaThickness(10);
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			ResultRegularPolygon->SetBodaMaterial(MID, ContentItem->ID);

			ResultRegularPolygon->SetState(OS_Normal);
			ResultRegularPolygon->SetBodaTextureURL(_FilePath);
			editArea->Modify();
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_CornerBrickMethod:
	{
		break;
		if (editArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(editArea);
			SCOPE_TRANSACTION(TEXT("矩形区域有角砖添加"));
			ResultArea->SetCornerType(CornerBrick);
			FString _FilePath1 = Res->FilePath;
			FString _FilePath2 = Res->FilePath;
			ResultArea->SetBodaTextureURL(_FilePath1);
			ResultArea->SetCornerTextureURL(_FilePath2);
			UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
			UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
			if (FloorTexture2 != NULL && FloorTexture1 != NULL)
			{
				ResultArea->SetState(OS_Normal);
			}
			editArea->Modify();
		}

		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_FloorKickingLine:
	{
		FString _FilePath = Res->FilePath;
		if (editArea->SurfaceType == 0)
		{
			if (editArea->GetType() == OT_RoomSpaceArea)
			{
				//SCOPE_TRANSACTION(TEXT("房间区域添加踢脚线"));
				TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				roomArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
				editArea->Modify();
				// 设置施工项数据
				if (InConstructionItem.IsValid())
				{
					FString UniqueCodeString = TEXT("FloorExtrusion") + editArea->GetUniqueID().ToString();
					FArmyAutoDesignModel::Get()->InitExtrusionConstructionItem(ContentItem->ID, UniqueCodeString, InConstructionItem);
				}
			}
			editArea->SkitLineGoodsID(ContentItem->ID);
			
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_CrownMoulding:
	{
		if (editArea->SurfaceType == 2)
		{
			editArea->SkitLineGoodsID(ContentItem->ID);

			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
			if (tempRoom->GenerateFromObjectType == OT_Beam
				|| editArea->GetType() == OT_HelpRectArea
				|| editArea->GetType() == OT_ClinderArea)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
				break;
			}

			//SCOPE_TRANSACTION(TEXT("房间区域添加顶角线"));
			FString FileMaterialPath = TEXT("");
			if (resArr.Num() > 1)
			{
				FileMaterialPath = resArr[1]->FilePath;
			}
			editArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
			editArea->Modify();
			// 设置施工项数据
			if (InConstructionItem.IsValid())
			{
				FString UniqueCodeString = TEXT("RoofExtrusion") + editArea->GetUniqueID().ToString();
				FArmyAutoDesignModel::Get()->InitExtrusionConstructionItem(ContentItem->ID, UniqueCodeString, InConstructionItem);
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_MoldingLine:
	{
		if (editArea->SurfaceType == 1 && editArea->GetType() != OT_RoomSpaceArea)
		{
			if (editArea->GetType() == OT_RectArea ||
				editArea->GetType() == OT_CircleArea ||
				editArea->GetType() == OT_FreePolygonArea ||
				editArea->GetType() == OT_PolygonArea)
			{
				SCOPE_TRANSACTION(TEXT("墙面区域添加造型线"));
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				editArea->GenerateWallMoldingLine(GVC->GetWorld(), ContentItem);
				editArea->SkitLineGoodsID(ContentItem->ID);
				editArea->Modify();
				// 设置施工项数据
				if (InConstructionItem.IsValid())
				{
					FString UniqueCodeString = TEXT("Extrusion") + editArea->GetUniqueID().ToString();
					FArmyAutoDesignModel::Get()->InitExtrusionConstructionItem(ContentItem->ID, UniqueCodeString, InConstructionItem);
				}
			}
		}
	}
	break;
	case CAT_LampSlot:
	{
		if (editArea->SurfaceType == 2)
		{
			if (editArea->GetDeltaHeight() != 0.0f)
			{
				SCOPE_TRANSACTION(TEXT("顶面添加灯槽"));
				editArea->GenerateLampSlot(GVC->GetWorld(), ContentItem, 13.f);
				editArea->SkitLineGoodsID(ContentItem->ID);
				editArea->Modify();
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请首先对吊顶下吊"));
			}
		}
		break;
	}
	case CAT_Buckle:
	{
		/* @梁晓菲 V1.2版本只支持在过门石区域单边添加扣条*/
		if (editArea->SurfaceType == 0 && editArea->GetType() == OT_RoomSpaceArea)
		{
			TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
			if (roomArea->GenerateFromObjectType == OT_Pass || roomArea->GenerateFromObjectType == OT_NewPass ||
				roomArea->GenerateFromObjectType == OT_Door || roomArea->GenerateFromObjectType == OT_SecurityDoor ||
				roomArea->GenerateFromObjectType == OT_SlidingDoor)
			{
				editArea->SetbIsSingleBuckle(true);
				editArea->GenerateBuckleActorLine(GVC->GetWorld(), ContentItem);
				// 设置施工项数据
				if (InConstructionItem.IsValid())
				{
					FString UniqueCodeString = TEXT("Buckle") + FString::Printf(TEXT("%d"), editArea->GetVectorCode()) + editArea->GetUniqueID().ToString();
					FArmyAutoDesignModel::Get()->InitExtrusionConstructionItem(ContentItem->ID, UniqueCodeString, InConstructionItem);
				}
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请在过门石区域添加扣条"));
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("请在过门石区域添加扣条"));
		}
		break;
	}
	default:
		break;
	}
	if (MatStyle.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		TSharedPtr<FArmyHardModeRes> tempRes = StaticCastSharedPtr<FArmyHardModeRes>(resArr[0]);

		MatStyle->SetGoodsID(ContentItem->ID);
		MatStyle->SetPlaneInfo(editArea->GetPlaneCenter(), editArea->GetXDir(), editArea->GetYDir());
		MatStyle->SetCurrentItem(ContentItem);
		// auto design style

		if (PavingMethod != CAT_WallPaper)//有铺法
		{
			MatStyle->SetStyleType(InDesignStyle.EdityType);
			MatStyle->SetAlignType(InDesignStyle.M_AlignmentType);
			MatStyle->SetInternalDist(InDesignStyle.InternalDist);
			MatStyle->SetXDirOffset(InDesignStyle.XDirOffset);
			MatStyle->SetYDirOffset(InDesignStyle.YDirOffset);
			MatStyle->SetRotationAngle(InDesignStyle.RotationAngle);
			MatStyle->SetHeightToFloor(InDesignStyle.HeightToFloor);

		    editArea->SetGapColor(InDesignStyle.GapColor);
		}

		editArea->SetStyle(MatStyle);
		editArea->Modify();
		// 设置施工项数据
		if (InConstructionItem.IsValid())
		{
			FArmyAutoDesignModel::Get()->InitPasteConstructionItem(editArea, InConstructionItem);
		}
	}

	// ---------------

}

// 铺贴颜值包
void FArmyHardModeExecute::ShowDesignPackage(TSharedPtr<class FArmyModelEntity> InModelEntity, TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle)
{
	if (!InModelEntity.IsValid())
		return;

	ComShowDesignPackage(InModelEntity->FurContentItem, InModelEntity->ConstructionItemData, editArea, InDesignStyle);
}

// 铺贴颜值包
void FArmyHardModeExecute::ShowDesignPackage(TSharedPtr<class FArmyBaseEditStyle> InStyle, TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle)
{
	if (!InStyle.IsValid())
		return;

	ComShowDesignPackage(InStyle->GetCurrentItem(), InStyle->ConstructionItemData, editArea, InDesignStyle);
}

TSharedPtr<FArmyBaseArea> FArmyHardModeExecute::CreateInternalSpaceArea(TSharedPtr<FArmyRoomSpaceArea> ParentArea, const TilePasteStyle& InDesignStyle,
	FModellingAreaInfo& InAreaInfo, float BaseAreaWidth, float BaseAreaHeight)
{

	TSharedPtr<FArmyBaseArea> ResultArea = nullptr;
	if (InAreaInfo.AreaType == 0)
		ResultArea = MakeShareable(new FArmyRectArea());
	else if (InAreaInfo.AreaType == 1)
		ResultArea = MakeShareable(new FArmyCircleArea());
	else if (InAreaInfo.AreaType == 2)
		ResultArea = MakeShareable(new FArmyRegularPolygonArea());
	else if (InAreaInfo.AreaType == 3)
		ResultArea = MakeShareable(new FArmyFreePolygonArea());

	ResultArea->SurportPlaceArea = ParentArea->SurportPlaceArea;
	TSharedPtr<FArmyBaseEditStyle> MatStyle;
	if (ParentArea->SurfaceType == 0)
	{
		MatStyle = MakeShareable(new FArmyContinueStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else if (ParentArea->SurfaceType == 1)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureWall(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/WallTexture.WallTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
		MatStyle->SetMainTextureLocalPath(Path);


	}
	ResultArea->SurfaceType = ParentArea->SurfaceType;
	ResultArea->SetPlaneInfo(ParentArea->GetXDir(), ParentArea->GetYDir(), ParentArea->GetPlaneCenter());
	FBox ParentAreaBox = ParentArea->GetBounds();

	FTransform CurrentTransform;
	float CurrentScale = 1.f;
	if (ParentAreaBox.GetSize().X < ParentAreaBox.GetSize().Y)
		CurrentScale = ParentAreaBox.GetSize().X / BaseAreaWidth;
	else
		CurrentScale = ParentAreaBox.GetSize().Y / BaseAreaHeight;
	//FVector CurrentPos(ParentAreaBox.GetExtent().X - BaseAreaWidth / 2.f, ParentAreaBox.GetExtent().Y - BaseAreaHeight / 2.f, 0.f);
	//CurrentTransform.SetScale3D(FVector(CurrentScale, CurrentScale, 1.f));
	CurrentTransform.SetScale3D(FVector(ParentAreaBox.GetSize().X / BaseAreaWidth, ParentAreaBox.GetSize().Y / BaseAreaHeight, 1.f));
	
	
	if (InAreaInfo.bHasLampWall)
	{
		FVector CurrentPos = CurrentTransform.TransformPosition(InAreaInfo.LampWallPos);
		InAreaInfo.LampWallWorldPos = ResultArea->PlaneToTranlate(CurrentPos);
		InAreaInfo.LampWallNormal = ResultArea->GetPlaneNormal();
	}

	if (InAreaInfo.AreaType == 0)
	{
		TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(ResultArea);
		RectArea->SetLeftUpCornerPos(CurrentTransform.TransformPosition(InAreaInfo.LeftUpCornerPoint));
		RectArea->SetRectWidth(InAreaInfo.AreaWidth * ParentAreaBox.GetSize().X / BaseAreaWidth);
		RectArea->SetRectHeight(InAreaInfo.AreaHeight * ParentAreaBox.GetSize().Y / BaseAreaHeight);
	}
	else if (InAreaInfo.AreaType == 1)
	{
		TSharedPtr<FArmyCircleArea> CircleArea = StaticCastSharedPtr<FArmyCircleArea>(ResultArea);
		CircleArea->SetCenterAndRadius(CurrentTransform.TransformPosition(InAreaInfo.Center), InAreaInfo.Radius * CurrentScale);
	}
	else if (InAreaInfo.AreaType == 2)
	{
		TSharedPtr<FArmyRegularPolygonArea> PolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ResultArea);
		PolyArea->SetNumSide(InAreaInfo.NumSide);
		PolyArea->SetPosition(CurrentTransform.TransformPosition(InAreaInfo.Position));
		PolyArea->SetStartPoint(CurrentTransform.TransformPosition(InAreaInfo.StartPoint));
		PolyArea->SetRadius(InAreaInfo.Radius * CurrentScale);
	}
	else if (InAreaInfo.AreaType == 3)
	{
		TSharedPtr<FArmyFreePolygonArea> PolyArea = StaticCastSharedPtr<FArmyFreePolygonArea>(ResultArea);
		TArray<FVector> NewPoints;
		for (auto& It : InAreaInfo.ModellingVertices)
			NewPoints.Push(CurrentTransform.TransformPosition(It));

		PolyArea->SetVertices(NewPoints);
	}
	TArray<FVector> Vertexes;
	ResultArea->GetVertexes(Vertexes);

	/*if (!ParentArea->TestAreaCanAddOrNot(Vertexes, ParentArea->GetUniqueID()))
	{
		return nullptr;
	}*/
	if (!ParentArea->TestAreaCanAddOrNot(ResultArea->GetOutArea()->Vertices, ResultArea->GetUniqueID()))
	{
		return nullptr;
	}

	MatStyle->SetPlaneInfo(ResultArea->GetPlaneCenter(), ResultArea->GetXDir(), ResultArea->GetYDir());
	ResultArea->SetStyle(MatStyle);
	FGuid GUID = ParentArea->GetUniqueID();
	ResultArea->SetParentGuid(GUID);

	XRArgument arg;
	arg._ArgUint32 = E_HardModel;
	ResultArea->SurfaceType = ParentArea->SurfaceType;
	if (ParentArea->SurfaceType != 0)
		MatStyle->SetInternalDist(0.0f);

	//TSharedPtr<class FContentItemSpace::FContentItem>  ChildContentItem
		//= InAreaInfo.ModellingMaterial->FurContentItem;
	ShowDesignPackage(InAreaInfo.ModellingMaterial, ResultArea, InDesignStyle);
	if (InAreaInfo.ModelingLine.IsValid())
	{
		ShowDesignPackage(InAreaInfo.ModelingLine, ResultArea, InDesignStyle);
	}

	if (ParentArea->SurfaceType == 2)
	{
		//如果是顶，设置下吊
		ResultArea->SetExtrusionHeight(FArmySceneData::WallHeight - InAreaInfo.ExtrusionHeight);
	}
	
	FArmySceneData::Get()->Add(ResultArea, arg, ParentArea);

	return ResultArea;
}

TSharedPtr<FArmyBaseArea> FArmyHardModeExecute::CreateInternalArea(TSharedPtr<FArmyRoomSpaceArea> ParentArea, const TilePasteStyle& InDesignStyle,
	FModellingAreaInfo InAreaInfo, float BaseAreaWidth, float BaseAreaHeight)
{
	TSharedPtr<FArmyBaseArea> ResultArea = nullptr;
	if (InAreaInfo.AreaType == 0)
		ResultArea = MakeShareable(new FArmyRectArea());
	else if (InAreaInfo.AreaType == 1)
		ResultArea = MakeShareable(new FArmyCircleArea());
	else if (InAreaInfo.AreaType == 2)
		ResultArea = MakeShareable(new FArmyRegularPolygonArea());
	else if (InAreaInfo.AreaType == 3)
		ResultArea = MakeShareable(new FArmyFreePolygonArea());

	ResultArea->SurportPlaceArea = ParentArea->SurportPlaceArea;
	TSharedPtr<FArmyBaseEditStyle> MatStyle;
	if (ParentArea->SurfaceType == 0)
	{
		MatStyle = MakeShareable(new FArmyContinueStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else if (ParentArea->SurfaceType == 1)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureWall(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/WallTexture.WallTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
		MatStyle->SetMainTextureLocalPath(Path);

	}
	FVector Center;
	FBox ParentAreaBox;// = ParentArea->GetBounds();
	TArray<FVector> Points = RoomEntity->GetRoom()->GetWorldPoints(true);
	
	FArmyHardModeData::ExtractMaximumRectangleInPolygonSimply(Points, Center, ParentAreaBox);
	Center.Z = ParentArea->GetPlaneCenter().Z;

	ResultArea->SurfaceType = ParentArea->SurfaceType;
	ResultArea->SetPlaneInfo(ParentArea->GetXDir(), ParentArea->GetYDir(), ParentArea->GetPlaneCenter());
	//ResultArea->SetPlaneInfo(ParentArea->GetXDir(), ParentArea->GetYDir(), Center);
	
	FTransform CurrentTransform;
	float CurrentScale = 1.f;
	CurrentScale = FMath::Min(ParentAreaBox.GetSize().X / BaseAreaWidth, ParentAreaBox.GetSize().Y / BaseAreaHeight);
	
	CurrentTransform.SetScale3D(FVector(CurrentScale, CurrentScale, 1.f));
	
	CurrentTransform.SetTranslation( 
			FVector(ParentArea->GetPlaneCenter().X - Center.X,Center.Y - ParentArea->GetPlaneCenter().Y,0.f)
			);
	

	if (InAreaInfo.AreaType == 0)
	{
		TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(ResultArea);
		RectArea->SetLeftUpCornerPos((CurrentTransform.TransformPosition(InAreaInfo.LeftUpCornerPoint)));
		RectArea->SetRectWidth(InAreaInfo.AreaWidth * CurrentScale);
		RectArea->SetRectHeight(InAreaInfo.AreaHeight * CurrentScale);

	}
	else if (InAreaInfo.AreaType == 1)
	{
		TSharedPtr<FArmyCircleArea> CircleArea = StaticCastSharedPtr<FArmyCircleArea>(ResultArea);
		CircleArea->SetCenterAndRadius(CurrentTransform.TransformPosition(InAreaInfo.Center), InAreaInfo.Radius * CurrentScale);
	}
	else if (InAreaInfo.AreaType == 2)
	{
		TSharedPtr<FArmyRegularPolygonArea> PolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ResultArea);
		PolyArea->SetNumSide(InAreaInfo.NumSide);
		PolyArea->SetPosition(CurrentTransform.TransformPosition(InAreaInfo.Position));
		PolyArea->SetStartPoint(CurrentTransform.TransformPosition(InAreaInfo.StartPoint));
		PolyArea->SetRadius(InAreaInfo.Radius * CurrentScale);
	}
	else if (InAreaInfo.AreaType == 3)
	{
		TSharedPtr<FArmyFreePolygonArea> PolyArea = StaticCastSharedPtr<FArmyFreePolygonArea>(ResultArea);
		TArray<FVector> NewPoints;
		for (auto& It : InAreaInfo.ModellingVertices)
			NewPoints.Push(CurrentTransform.TransformPosition(It));

		PolyArea->SetVertices(NewPoints);
	}
	TArray<FVector> Vertexes;
	ResultArea->GetVertexes(Vertexes);

	/*if (!ParentArea->TestAreaCanAddOrNot(Vertexes, ResultArea->GetUniqueID()))
	{
		return nullptr;
	}*/
	if (!ParentArea->TestAreaCanAddOrNot(ResultArea->GetOutArea()->Vertices, ResultArea->GetUniqueID()))
	{
		return nullptr;
	}

	MatStyle->SetPlaneInfo(ResultArea->GetPlaneCenter(), ResultArea->GetXDir(), ResultArea->GetYDir());
	ResultArea->SetStyle(MatStyle);
	FGuid GUID = ParentArea->GetUniqueID();
	ResultArea->SetParentGuid(GUID);

	XRArgument arg;
	arg._ArgUint32 = E_HardModel;
	ResultArea->SurfaceType = ParentArea->SurfaceType;
	if (ParentArea->SurfaceType != 0)
		MatStyle->SetInternalDist(0.0f);

	//TSharedPtr<class FContentItemSpace::FContentItem>  ChildContentItem
		//= InAreaInfo.ModellingMaterial->FurContentItem;
	ShowDesignPackage(InAreaInfo.ModellingMaterial, ResultArea, InDesignStyle);

	if (ParentArea->SurfaceType == 2)
	{
		//如果是顶，设置下吊
		if (InAreaInfo.ExtrusionHeight > 0.000001f)
		{
			ResultArea->SetExtrusionHeight(FArmySceneData::WallHeight - InAreaInfo.ExtrusionHeight);
		}
	}

	FArmySceneData::Get()->Add(ResultArea, arg, ParentArea);

	return ResultArea;
}

TSharedPtr<FArmyBaseEditStyle> FArmyHardModeExecute::GetEditStyle(EStyleType type)
{
	TSharedPtr<FArmyBaseEditStyle> Style = nullptr;
	switch (type)
	{
	case S_ContinueStyle:
		Style = MakeShareable(new FArmyContinueStyle());
		break;
	case S_SlopeContinueStyle:
		Style = MakeShareable(new FArmySlopeContinueStyle());
		break;
	case S_HerringBoneStyle:
		Style = MakeShareable(new FArmyHerringBoneStyle());
		break;
	case S_TrapeZoidStyle:
		Style = MakeShareable(new FArmyTrapezoidStyle());
		break;
	case S_WhirlwindStyle:
		Style = MakeShareable(new FArmyWhirlwindStyle());
		break;
	case S_WorkerStyle:
		Style = MakeShareable(new FArmyWorkerStyle());
		break;
	case S_CrossStyle:
		Style = MakeShareable(new FArmyCrossStyle());
		break;
	case S_IntervalStyle:
		Style = MakeShareable(new FArmyIntervalStyle());
		break;
	case S_SeamlessStyle:
		Style = MakeShareable(new FArmySeamlessStyle());
		break;
	case S_BodaAreaStyle:
		break;
	default:
		break;
	}

	return Style;
}