#include "ArmyHardModeData.h"
#include "ArmyResourceModule.h"
#include "ArmyRectArea.h"
#include "ArmyFreePolygonArea.h"
#include "ArmyCircleArea.h"
#include "ArmyModelEntity.h"
#include "ArmyRoomEntity.h"
#include "ArmyWindow.h"
#include "ArmyExtrusionActor.h"
#include "ArmyDataTools.h"
#include "ArmyWallLine.h"
#include "ArmyWallActor.h"
#include "ArmyWindowActor.h"
#include "ArmyActorItem.h"
#include "ArmyPass.h"
#include "ArmyNewPass.h"
#include "ArmyConstructionManager.h"

#include "ArmyBaseBoardExtruder.h"
#include "ArmyCrownMouldingExtruder.h"
#include "ArmyLampSlotExtruder.h"


FArmyHardModeData::FArmyHardModeData()
{

}

FArmyHardModeData::~FArmyHardModeData()
{

}
bool FArmyHardModeData::ModelEntityIsValid(TSharedPtr<class FArmyModelEntity> ModelEntity)
{
	if (ModelEntity.IsValid() && ModelEntity->FurContentItem.IsValid())
	{
		return true;
	}
	return false;
}
FArmyHardModeData::FArmyHardModeData(TSharedPtr<class FArmyRoomEntity> RoomEntity)
{
	TSharedPtr<FArmyRoomSpaceArea> FloorSpaceArea;


	ModellingFloor = MakeShareable(new FModellingFloor);
	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = RoomEntity->GetFloorSpaceArray();
	for (auto It : RoomSpaceAreas)
	{
		if (It.IsValid())
		{
			FloorSpaceArea = It;

			TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem = It->GetStyle()->GetCurrentItem();
			ModellingFloor->BaseFloorMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
			ModellingFloor->BaseFloorMaterial->ItemType = IT_HardMode;
			ModellingFloor->BaseFloorMaterial->HardModeType = HMT_Floor_Material;
			// 获得勾选的施工项信息
			ModellingFloor->BaseFloorMaterial->SetPasteConstructionItem(FloorSpaceArea);

			//if (It->GetContentItemList().Num() > 0)
			if (It->GetBaseboardExtruder()->GetPaths().Num() > 0)
			{
				ModellingFloor->BaseFloorSkirtingLine = MakeShareable(new FArmyModelEntity(It->GetBaseboardExtruder()->GetPaths()[0].ContentItem));
				FString UniqueCodeString = TEXT("FloorExtrusion") + It->GetUniqueID().ToString();
				// 获得勾选的施工项信息
				ModellingFloor->BaseFloorSkirtingLine->SetExtrusionConstructionItem(UniqueCodeString);
				ModellingFloor->BaseFloorSkirtingLine->ItemType = IT_HardMode;
				ModellingFloor->BaseFloorSkirtingLine->HardModeType = HMT_Skirting;
			}


			ModellingFloor->DesignStyle.EdityType = It->GetStyle()->GetEditType();
			ModellingFloor->DesignStyle.M_AlignmentType = It->GetStyle()->GetAlignType();
			ModellingFloor->DesignStyle.InternalDist = It->GetStyle()->GetInternalDist();
			ModellingFloor->DesignStyle.XDirOffset = It->GetStyle()->GetXDirOffset();
			ModellingFloor->DesignStyle.YDirOffset = It->GetStyle()->GetYDirOffset();
			ModellingFloor->DesignStyle.RotationAngle = It->GetStyle()->GetRotationAngle();
			ModellingFloor->DesignStyle.HeightToFloor = It->GetStyle()->GetHeightToFloor();
			ModellingFloor->DesignStyle.GapColor = It->GetGapColor();
			break;
		}
	}

	RoomSpaceAreas.Empty();
	RoomSpaceAreas = RoomEntity->GetCeilingSpaceArray();
	ModellingRoof = MakeShareable(new FModellingRoof);
	for (auto It : RoomSpaceAreas)
	{
		if (It.IsValid())
		{
			ModellingRoof->WallType = 0;//TODO
			if (It->GetStyle()->GetEditType() == S_SeamlessStyle)
				ModellingRoof->MaterialType = 0;
			else
				ModellingRoof->MaterialType = 1;
			TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem = It->GetStyle()->GetCurrentItem();
			ModellingRoof->BaseRoofMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
			ModellingRoof->BaseRoofMaterial->ItemType = IT_HardMode;
			ModellingRoof->BaseRoofMaterial->HardModeType = HMT_Ceiling_Material;
			// 获得勾选的施工项信息
			ModellingRoof->BaseRoofMaterial->SetPasteConstructionItem(It);
			//if (It->GetContentItemList().Num() > 0)
			if (It->GetCrownMouldingExtruder()->GetPaths().Num() > 0)
			{
				ModellingRoof->BaseRoofSkirtingLine = MakeShareable(new FArmyModelEntity(It->GetCrownMouldingExtruder()->GetPaths()[0].ContentItem));
				FString UniqueCodeString = TEXT("RoofExtrusion") + It->GetUniqueID().ToString();
				// 获得勾选的施工项信息
				ModellingRoof->BaseRoofSkirtingLine->SetExtrusionConstructionItem(UniqueCodeString);
				ModellingRoof->BaseRoofSkirtingLine->ItemType = IT_HardMode;
				ModellingRoof->BaseRoofSkirtingLine->HardModeType = HMT_Ceiling_Line;
			}

			ModellingRoof->ExtrusionHeight = FArmySceneData::WallHeight - It->GetExtrusionHeight();


			FVector Center;
			FBox ParentAreaBox;
			TArray<FVector> Points = RoomEntity->GetRoom()->GetWorldPoints(true);
			FArmyHardModeData::ExtractMaximumRectangleInPolygonSimply(Points, Center, ParentAreaBox);

			ModellingRoof->BaseAreaWidth = ParentAreaBox.GetSize().X;// It->GetBounds().GetSize().X;
			ModellingRoof->BaseAreaHeight = ParentAreaBox.GetSize().Y; //It->GetBounds().GetSize().Y;

			TArray<TSharedPtr<FArmyBaseArea>> EditAreaList = It->GetEditAreas();
			if (EditAreaList.Num() > 0)
			{
				ModellingRoof->WallType = 1;
				for (auto ItArea : EditAreaList)
				{
					FVector min = ItArea->GetBounds().Min.X * ItArea->GetXDir() + ItArea->GetBounds().Min.Y * ItArea->GetYDir() + ItArea->GetPlaneCenter();
					FVector max = ItArea->GetBounds().Max.X * ItArea->GetXDir() + ItArea->GetBounds().Max.Y * ItArea->GetYDir() + ItArea->GetPlaneCenter();
					
					FBox Bounds = FBox(min, max);
					if (!ParentAreaBox.IsInsideXY(Bounds))
					{
						continue;
					}

					FTransform CurrentTransform;
					CurrentTransform.SetTranslation(
						FVector(Center.X - It->GetPlaneCenter().X, It->GetPlaneCenter().Y - Center.Y, 0.f)
					);

					FModellingAreaInfo CurrentAreaInfo;
					if (ItArea->GetType() == OT_RectArea)
						CurrentAreaInfo.AreaType = 0;
					else if (ItArea->GetType() == OT_CircleArea)
						CurrentAreaInfo.AreaType = 1;
					else if (ItArea->GetType() == OT_PolygonArea)
						CurrentAreaInfo.AreaType = 2;
					else if (ItArea->GetType() == OT_FreePolygonArea)
						CurrentAreaInfo.AreaType = 3;
					if (ItArea->GetStyle()->GetEditType() == S_SeamlessStyle)
						CurrentAreaInfo.MaterialType = 0;
					else
						CurrentAreaInfo.MaterialType = 1;
					if (CurrentAreaInfo.MaterialType == 1)
					{
						CurrentAreaInfo.DesignStyle.EdityType = ItArea->GetStyle()->GetEditType();
						CurrentAreaInfo.DesignStyle.M_AlignmentType = ItArea->GetStyle()->GetAlignType();
						CurrentAreaInfo.DesignStyle.InternalDist = ItArea->GetStyle()->GetInternalDist();
						CurrentAreaInfo.DesignStyle.XDirOffset = ItArea->GetStyle()->GetXDirOffset();
						CurrentAreaInfo.DesignStyle.YDirOffset = ItArea->GetStyle()->GetYDirOffset();
						CurrentAreaInfo.DesignStyle.RotationAngle = ItArea->GetStyle()->GetRotationAngle();
						CurrentAreaInfo.DesignStyle.HeightToFloor = ItArea->GetStyle()->GetHeightToFloor();
						CurrentAreaInfo.DesignStyle.GapColor = ItArea->GetGapColor();
					}
					else
					{
						CurrentAreaInfo.DesignStyle.WallPaintColor = ItArea->GetStyle()->GetWallPaintColor();
						if (ItArea->GetStyle()->GetEditType() == S_SeamlessStyle)
						{
							TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(ItArea->GetStyle());
							if (tempSeamless->SeamlesStyleType == 1)
							{
								CurrentAreaInfo.DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
							}
						}
					}
					TSharedPtr<class FContentItemSpace::FContentItem>  ContentItemArea = ItArea->GetStyle()->GetCurrentItem();
					CurrentAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity(ContentItemArea));
					// 获得勾选的施工项信息
					CurrentAreaInfo.ModellingMaterial->SetPasteConstructionItem(ItArea);

					TArray<TSharedPtr<FArmyHelpRectArea>> HelpAreas;

					if (ItArea->GetType() == OT_RectArea)
					{
						TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(ItArea);
						CurrentAreaInfo.LeftUpCornerPoint = CurrentTransform.TransformPosition(RectArea->GetLeftUpCornerPos());
						CurrentAreaInfo.AreaHeight = RectArea->GetRectHeight();
						CurrentAreaInfo.AreaWidth = RectArea->GetRectWidth();
						
						HelpAreas = RectArea->GetInnearHoleSurface();
					}
					else if (ItArea->GetType() == OT_CircleArea)
					{
						TSharedPtr<FArmyCircleArea> CircleArea = StaticCastSharedPtr<FArmyCircleArea>(ItArea);
						CurrentAreaInfo.Center = CurrentTransform.TransformPosition(CircleArea->GetPosition());
						CurrentAreaInfo.Radius = CircleArea->GetRadius();

						if(CircleArea->GetInnearHoleSurface().IsValid())
						{
							TSharedPtr<FArmyClinderArea> helpArea = CircleArea->GetInnearHoleSurface();
							FModellingHelpAreaInfo HelpAreaInfo;
							TSharedPtr<class FContentItemSpace::FContentItem>  ContentItemHelpArea = helpArea->GetStyle()->GetCurrentItem();
							HelpAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity(ContentItemHelpArea));
							// 获得勾选的施工项信息
							HelpAreaInfo.ModellingMaterial->SetPasteConstructionItem(helpArea);

							HelpAreaInfo.DesignStyle.EdityType = helpArea->GetStyle()->GetEditType();
							HelpAreaInfo.DesignStyle.M_AlignmentType = helpArea->GetStyle()->GetAlignType();
							HelpAreaInfo.DesignStyle.InternalDist = helpArea->GetStyle()->GetInternalDist();
							HelpAreaInfo.DesignStyle.XDirOffset = helpArea->GetStyle()->GetXDirOffset();
							HelpAreaInfo.DesignStyle.YDirOffset = helpArea->GetStyle()->GetYDirOffset();
							HelpAreaInfo.DesignStyle.RotationAngle = helpArea->GetStyle()->GetRotationAngle();
							HelpAreaInfo.DesignStyle.HeightToFloor = helpArea->GetStyle()->GetHeightToFloor();
							HelpAreaInfo.DesignStyle.GapColor = helpArea->GetGapColor();
							HelpAreaInfo.DesignStyle.WallPaintColor = helpArea->GetStyle()->GetWallPaintColor();
							if (helpArea->GetStyle()->GetEditType() == S_SeamlessStyle)
							{
								TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(helpArea->GetStyle());
								if (tempSeamless->SeamlesStyleType == 1)
								{
									HelpAreaInfo.DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
								}
							}
							CurrentAreaInfo.ModellingHelpAreas.Add(HelpAreaInfo);
						}
					}
					else if (ItArea->GetType() == OT_PolygonArea)
					{
						TSharedPtr<FArmyRegularPolygonArea> PolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ItArea);
						CurrentAreaInfo.NumSide = PolyArea->GetNumSide();
						CurrentAreaInfo.Position = CurrentTransform.TransformPosition(PolyArea->GetPosition());
						CurrentAreaInfo.StartPoint = CurrentTransform.TransformPosition(PolyArea->GetStartPoint());
						CurrentAreaInfo.Radius = PolyArea->GetRadius();

						HelpAreas = PolyArea->GetInnearHoleSurface();
					}
					else if (ItArea->GetType() == OT_FreePolygonArea)
					{
						TSharedPtr<FArmyFreePolygonArea> FreeArea = StaticCastSharedPtr<FArmyFreePolygonArea>(ItArea);
						FreeArea->GetVertexes(CurrentAreaInfo.ModellingVertices);
						for (int i = 0; i < CurrentAreaInfo.ModellingVertices.Num(); i++)
						{
							CurrentAreaInfo.ModellingVertices[i] = CurrentTransform.TransformPosition(CurrentAreaInfo.ModellingVertices[i]);
						}
						HelpAreas = FreeArea->GetInnearHoleSurface();
					}
					
					for (auto helpArea : HelpAreas)
					{
						FModellingHelpAreaInfo HelpAreaInfo;
						TSharedPtr<class FContentItemSpace::FContentItem>  ContentItemHelpArea = helpArea->GetStyle()->GetCurrentItem();
						HelpAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity(ContentItemHelpArea));
						// 获得勾选的施工项信息
						HelpAreaInfo.ModellingMaterial->SetPasteConstructionItem(helpArea);

						HelpAreaInfo.DesignStyle.EdityType = helpArea->GetStyle()->GetEditType();
						HelpAreaInfo.DesignStyle.M_AlignmentType = helpArea->GetStyle()->GetAlignType();
						HelpAreaInfo.DesignStyle.InternalDist = helpArea->GetStyle()->GetInternalDist();
						HelpAreaInfo.DesignStyle.XDirOffset = helpArea->GetStyle()->GetXDirOffset();
						HelpAreaInfo.DesignStyle.YDirOffset = helpArea->GetStyle()->GetYDirOffset();
						HelpAreaInfo.DesignStyle.RotationAngle = helpArea->GetStyle()->GetRotationAngle();
						HelpAreaInfo.DesignStyle.HeightToFloor = helpArea->GetStyle()->GetHeightToFloor();
						HelpAreaInfo.DesignStyle.GapColor = helpArea->GetGapColor();
						HelpAreaInfo.DesignStyle.WallPaintColor = helpArea->GetStyle()->GetWallPaintColor();
						if (helpArea->GetStyle()->GetEditType() == S_SeamlessStyle)
						{
							TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(helpArea->GetStyle());
							if (tempSeamless->SeamlesStyleType == 1)
							{
								HelpAreaInfo.DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
							}
						}
						CurrentAreaInfo.ModellingHelpAreas.Add(HelpAreaInfo);
					}

					CurrentAreaInfo.ExtrusionHeight = FArmySceneData::WallHeight - ItArea->GetExtrusionHeight();
					//if (ItArea->LampContentItem.IsValid())
					if (ItArea->GetLampSlotExtruder()->GetPaths().Num() > 0)
					{
						CurrentAreaInfo.LampSlotActor = MakeShareable(new FArmyModelEntity(ItArea->GetLampSlotExtruder()->GetPaths()[0].ContentItem));
						// 获得勾选的施工项信息
						//CurrentAreaInfo.LampSlotActor->SetPasteConstructionItem(ItArea);
						CurrentAreaInfo.LampSlotActor->ItemType = IT_HardMode;
						CurrentAreaInfo.LampSlotActor->HardModeType = HMT_Lamp_Trough;
					}
					//if (ItArea->GetContentItemList().Num() > 0)
					if (ItArea->GetCrownMouldingExtruder()->GetPaths().Num() > 0)
					{
						CurrentAreaInfo.ModelingLine = MakeShareable(new FArmyModelEntity(ItArea->GetCrownMouldingExtruder()->GetPaths()[0].ContentItem));
						FString UniqueCodeString = TEXT("RoofExtrusion") + ItArea->GetUniqueID().ToString();
						// 获得勾选的施工项信息
						CurrentAreaInfo.ModelingLine->SetExtrusionConstructionItem(UniqueCodeString);
						CurrentAreaInfo.ModelingLine->ItemType = IT_HardMode;
						CurrentAreaInfo.ModelingLine->HardModeType = HMT_Ceiling_Line;
					}
					ModellingRoof->SpaceAreaList.Push(CurrentAreaInfo);


				}
			}
			else
			{
				ModellingRoof->WallType = 0;
			}

			if (ModellingRoof->MaterialType == 1)
			{
				ModellingRoof->DesignStyle.EdityType = It->GetStyle()->GetEditType();
				ModellingRoof->DesignStyle.M_AlignmentType = It->GetStyle()->GetAlignType();
				ModellingRoof->DesignStyle.InternalDist = It->GetStyle()->GetInternalDist();
				ModellingRoof->DesignStyle.XDirOffset = It->GetStyle()->GetXDirOffset();
				ModellingRoof->DesignStyle.YDirOffset = It->GetStyle()->GetYDirOffset();
				ModellingRoof->DesignStyle.RotationAngle = It->GetStyle()->GetRotationAngle();
				ModellingRoof->DesignStyle.HeightToFloor = It->GetStyle()->GetHeightToFloor();
				ModellingRoof->DesignStyle.GapColor = It->GetGapColor();
			}
			else
			{
				ModellingRoof->DesignStyle.WallPaintColor = It->GetStyle()->GetWallPaintColor();
				if (It->GetStyle()->GetEditType() == S_SeamlessStyle)
				{
					TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(It->GetStyle());
					if (tempSeamless->SeamlesStyleType == 1)
					{
						ModellingRoof->DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
					}
				}
			}

			break;
		}
	}

	TMap<int, TSharedPtr<FArmyRoomSpaceArea>> BedRoomModellingSpaceList;
	TSharedPtr<FArmyRoomSpaceArea> SofaSpace;
	TSharedPtr<FArmyRoomSpaceArea> TVBenchSpace;
	if (RoomEntity->GetAiRoomType() == RT_Bedroom_Master)
	{
		GetRoomSpaceListFromBed(BedRoomModellingSpaceList, RoomEntity);
	}
	else if (RoomEntity->GetAiRoomType() == RT_LivingRoom)
	{
		SofaSpace = GetRoomSpaceFromSofaOrTVBench(RoomEntity, AI_Sofa_Master);
		TVBenchSpace = GetRoomSpaceFromSofaOrTVBench(RoomEntity, AI_TVBench);
	}
	RoomSpaceAreas.Empty();
	RoomSpaceAreas = RoomEntity->GetWallSpaceArray();

	TArray<TSharedPtr<FArmyModelEntity>> LampWallList;
	RoomEntity->GetModelEntityArrayByComponentId(AI_LampWall, LampWallList);
	for (auto It : RoomSpaceAreas)
	{
		if (It.IsValid())
		{
			TSharedPtr<FModellingWall> CurrentModellingWall = MakeShareable(new FModellingWall);
			TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem = It->GetStyle()->GetCurrentItem();
			CurrentModellingWall->BaseWallMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
			CurrentModellingWall->BaseWallMaterial->HardModeType = HMT_Wall_Material;
			// 获得勾选的施工项信息
			CurrentModellingWall->BaseWallMaterial->SetPasteConstructionItem(It);

			if (BedRoomModellingSpaceList.Contains(1) &&
				It == BedRoomModellingSpaceList.FindRef(1))
			{
				CurrentModellingWall->ModellingWallType = 1;
			}
			else if (BedRoomModellingSpaceList.Contains(2) &&
				It == BedRoomModellingSpaceList.FindRef(2))
			{
				CurrentModellingWall->ModellingWallType = 2;
				CurrentModellingWall->BaseWallMaterial->HardModeType = HMT_Main_Wall;
			}
			else if (TVBenchSpace.IsValid() && It == TVBenchSpace)
			{
				CurrentModellingWall->ModellingWallType = 3;
				CurrentModellingWall->BaseWallMaterial->HardModeType = HMT_Main_Wall;
			}
			else if (SofaSpace.IsValid() && It == SofaSpace)
			{
				CurrentModellingWall->ModellingWallType = 4;
			}
			else
			{
				CurrentModellingWall->ModellingWallType = 0;
			}
			TArray<TSharedPtr<FArmyBaseArea>> EditAreaList = It->GetEditAreas();
			if (EditAreaList.Num() > 0 && CurrentModellingWall->ModellingWallType > 0)
			{
				CurrentModellingWall->WallType = 1;

				for (auto ItArea : EditAreaList)
				{
					FModellingAreaInfo CurrentAreaInfo;
					
					for (auto ItLamp : LampWallList)
					{
						FVector LocalPos = It->GetPlaneLocalFromWorld(ItLamp->Location);
						if (ItArea->GetBounds().IsInsideXY(LocalPos))
						{
							ItLamp->IsInEditArea = true;
							CurrentAreaInfo.bHasLampWall = true;
							CurrentAreaInfo.LampWallPos = ItArea->GetPlaneLocalFromWorld(ItLamp->Location);
							break;
						}
					}

					if (ItArea->GetType() == OT_RectArea)
						CurrentAreaInfo.AreaType = 0;
					else if (ItArea->GetType() == OT_CircleArea)
						CurrentAreaInfo.AreaType = 1;
					else if (ItArea->GetType() == OT_PolygonArea)
						CurrentAreaInfo.AreaType = 2;
					else if (ItArea->GetType() == OT_FreePolygonArea)
						CurrentAreaInfo.AreaType = 3;
					
					if (ItArea->GetStyle().IsValid())
					{
						TSharedPtr<class FContentItemSpace::FContentItem>  ContentItemArea = ItArea->GetStyle()->GetCurrentItem();
						CurrentAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity(ContentItemArea));
						// 获得勾选的施工项信息
						CurrentAreaInfo.ModellingMaterial->SetPasteConstructionItem(ItArea);

						if (ItArea->GetStyle()->GetEditType() == S_SeamlessStyle)
						{
							CurrentAreaInfo.MaterialType = 0;
						}
						else
						{
							CurrentAreaInfo.MaterialType = 1;
						}
						if (CurrentAreaInfo.MaterialType == 1)
						{
							CurrentAreaInfo.DesignStyle.EdityType = ItArea->GetStyle()->GetEditType();
							CurrentAreaInfo.DesignStyle.M_AlignmentType = ItArea->GetStyle()->GetAlignType();
							CurrentAreaInfo.DesignStyle.InternalDist = ItArea->GetStyle()->GetInternalDist();
							CurrentAreaInfo.DesignStyle.XDirOffset = ItArea->GetStyle()->GetXDirOffset();
							CurrentAreaInfo.DesignStyle.YDirOffset = ItArea->GetStyle()->GetYDirOffset();
							CurrentAreaInfo.DesignStyle.RotationAngle = ItArea->GetStyle()->GetRotationAngle();
							CurrentAreaInfo.DesignStyle.HeightToFloor = ItArea->GetStyle()->GetHeightToFloor();
							CurrentAreaInfo.DesignStyle.GapColor = ItArea->GetGapColor();
						}
						else
						{
							CurrentAreaInfo.DesignStyle.WallPaintColor = ItArea->GetStyle()->GetWallPaintColor();
							if (ItArea->GetStyle()->GetEditType() == S_SeamlessStyle)
							{
								TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(ItArea->GetStyle());
								if (tempSeamless->SeamlesStyleType == 1)
								{
									CurrentAreaInfo.DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
								}
							}
						}
					}

					TArray<TSharedPtr<FContentItemSpace::FContentItem>> ContentItemList = ItArea->GetContentItemList();
					if (ContentItemList.Num() > 0)
					{
						CurrentAreaInfo.ModelingLine = MakeShareable(new FArmyModelEntity(ContentItemList[0]));
						// 获得勾选的施工项信息
						FString UniqueCodeString = TEXT("Extrusion") + ItArea->GetUniqueID().ToString();
						CurrentAreaInfo.ModelingLine->SetExtrusionConstructionItem(UniqueCodeString);
					}

					if (ItArea->GetType() == OT_RectArea)
					{
						TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(ItArea);
						CurrentAreaInfo.LeftUpCornerPoint = RectArea->GetLeftUpCornerPos();
						CurrentAreaInfo.AreaHeight = RectArea->GetRectHeight();
						CurrentAreaInfo.AreaWidth = RectArea->GetRectWidth();
					}
					else if (ItArea->GetType() == OT_CircleArea)
					{
						TSharedPtr<FArmyCircleArea> CircleArea = StaticCastSharedPtr<FArmyCircleArea>(ItArea);
						CurrentAreaInfo.Center = CircleArea->GetPosition();
						CurrentAreaInfo.Radius = CircleArea->GetRadius();
					}
					else if (ItArea->GetType() == OT_PolygonArea)
					{
						TSharedPtr<FArmyRegularPolygonArea> PolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ItArea);
						CurrentAreaInfo.NumSide = PolyArea->GetNumSide();
						CurrentAreaInfo.Position = PolyArea->GetPosition();
						CurrentAreaInfo.StartPoint = PolyArea->GetStartPoint();
						CurrentAreaInfo.Radius = PolyArea->GetRadius();
					}
					else if (ItArea->GetType() == OT_FreePolygonArea)
					{
						TSharedPtr<FArmyFreePolygonArea> FreeArea = StaticCastSharedPtr<FArmyFreePolygonArea>(ItArea);
						FreeArea->GetVertexes(CurrentAreaInfo.ModellingVertices);
					}

					CurrentModellingWall->SpaceAreaList.Push(CurrentAreaInfo);
				}
			}
			else
			{
				CurrentModellingWall->WallType = 0;
			}
			if (It->GetStyle()->GetEditType() == S_SeamlessStyle)
			{
				CurrentModellingWall->MaterialType = 0;
			}
			else
			{
				CurrentModellingWall->MaterialType = 1;
			}
			if (CurrentModellingWall->MaterialType == 1)
			{
				CurrentModellingWall->DesignStyle.EdityType = It->GetStyle()->GetEditType();
				CurrentModellingWall->DesignStyle.M_AlignmentType = It->GetStyle()->GetAlignType();
				CurrentModellingWall->DesignStyle.InternalDist = It->GetStyle()->GetInternalDist();
				CurrentModellingWall->DesignStyle.XDirOffset = It->GetStyle()->GetXDirOffset();
				CurrentModellingWall->DesignStyle.YDirOffset = It->GetStyle()->GetYDirOffset();
				CurrentModellingWall->DesignStyle.RotationAngle = It->GetStyle()->GetRotationAngle();
				CurrentModellingWall->DesignStyle.HeightToFloor = It->GetStyle()->GetHeightToFloor();
				CurrentModellingWall->DesignStyle.GapColor = It->GetGapColor();
			}
			else
			{
				CurrentModellingWall->DesignStyle.WallPaintColor = It->GetStyle()->GetWallPaintColor();
				if (It->GetStyle()->GetEditType() == S_SeamlessStyle)
				{
					TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(It->GetStyle());
					if (tempSeamless->SeamlesStyleType == 1)
					{
						CurrentModellingWall->DesignStyle.ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
					}
				}
			}
			CurrentModellingWall->BaseWallMaterial->ItemType = IT_HardMode;
			CurrentModellingWall->BaseAreaWidth = It->GetBounds().GetSize().X;
			CurrentModellingWall->BaseAreaHeight = It->GetBounds().GetSize().Y;

			ModellingWallList.Push(CurrentModellingWall);
		}
	}

	TArray<TSharedPtr<FArmyHardware>> ResultHardware;
	RoomEntity->GetRoom()->GetHardwareListRelevance(ResultHardware);

	ModellingBuckle = MakeShareable(new FModellingBuckle);

	TMap<int, int> ContentIndex;
	//TArray<TSharedPtr<FContentItemSpace::FContentItem>> ContentArray;
	TArray<BuckleCode> ContentArray;

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
					int VertorCode = -1;
					TSharedPtr<FContentItemSpace::FContentItem> ContentItem = Space->GetEdgeVectorCodeCloseToPos(FloorSpaceArea, VertorCode);
					if (ContentItem.IsValid())
					{
						FString UniqueCodeString = TEXT("Buckle") + FString::Printf(TEXT("%d"), VertorCode) + Space->GetUniqueID().ToString();

						ContentArray.Add(BuckleCode(ContentItem, UniqueCodeString));
						if (ContentIndex.Contains(ContentItem->ID))
						{
							ContentIndex[ContentItem->ID] = ContentIndex[ContentItem->ID] + 1;
						}
						else
						{
							ContentIndex.Add(ContentItem->ID, 1);
						}
					}
				}
			}
		}
	}
	ContentIndex.ValueSort(TGreater<int>());
	if (ContentIndex.Num() > 0)
	{
		int key = (*ContentIndex.CreateIterator()).Key;
		for (auto item : ContentArray)
		{
			if (item.ContentItem->ID == key)
			{
				ModellingBuckle->BuckleMaterial = MakeShareable(new FArmyModelEntity(item.ContentItem));
				// 获得勾选的施工项信息
				ModellingBuckle->BuckleMaterial->SetExtrusionConstructionItem(item.UniqueCodeString);
				ModellingBuckle->BuckleMaterial->ItemType = IT_HardMode;
				ModellingBuckle->BuckleMaterial->HardModeType = HMT_Door_Buckle;
				break;
			}
		}
	}
}


// 获得硬装模型列表
TArray<TSharedPtr<class FArmyModelEntity>> FArmyHardModeData::GetHardModelList()
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;
	for (auto &It : ModellingWallList)
	{
		if (It->BaseWallMaterial.IsValid())
		{
			ModelArray.Add(It->BaseWallMaterial);
		}
		for (auto It2 : It->SpaceAreaList)
		{
			if (It2.ModellingMaterial.IsValid())
			{
				ModelArray.Add(It2.ModellingMaterial);
			}
			
			if (It2.ModelingLine.IsValid())
			{
				ModelArray.Add(It2.ModelingLine);
			}
		}
	}

	if (ModellingFloor.IsValid())
	{
		if (ModellingFloor->BaseFloorMaterial.IsValid())
		{
			ModelArray.Add(ModellingFloor->BaseFloorMaterial);
		}
		if (ModellingFloor->BaseFloorSkirtingLine.IsValid())
		{
			ModelArray.Add(ModellingFloor->BaseFloorSkirtingLine);
		}
	}
	if (ModellingBuckle.IsValid())
	{
		if (ModellingBuckle->BuckleMaterial.IsValid())
		{
			ModelArray.Add(ModellingBuckle->BuckleMaterial);
		}
	}
	if (ModellingRoof.IsValid())
	{
		if (ModellingRoof->BaseRoofMaterial.IsValid())
		{
			ModelArray.Add(ModellingRoof->BaseRoofMaterial);
		}
		if (ModellingRoof->BaseRoofSkirtingLine.IsValid())
		{
			ModelArray.Add(ModellingRoof->BaseRoofSkirtingLine);
		}
		for (auto It2 : ModellingRoof->SpaceAreaList)
		{
			if (It2.ModellingMaterial.IsValid())
			{
				ModelArray.Add(It2.ModellingMaterial);
			}
			if (It2.LampSlotActor.IsValid())
			{
				ModelArray.Add(It2.LampSlotActor);
			}
			if (It2.ModelingLine.IsValid())
			{
				ModelArray.Add(It2.ModelingLine);
			}
			for (auto HelpArea : It2.ModellingHelpAreas)
			{
				ModelArray.Add(HelpArea.ModellingMaterial);
			}
		}
	}

	TMap<EHardModeType, int> indexes;
	indexes.Add(HMT_None, -1);
	indexes.Add(HMT_Floor_Material, 1);
	indexes.Add(HMT_Skirting, 1);
	indexes.Add(HMT_Ceiling_Material, 1);
	indexes.Add(HMT_Ceiling_Line, 1);
	indexes.Add(HMT_Wall_Material, 1);
	indexes.Add(HMT_Main_Wall, 1);
	indexes.Add(HMT_Lamp_Trough, 1);
	indexes.Add(HMT_Window_Stone, 1);
	indexes.Add(HMT_Door_Stone, 1);
	indexes.Add(HMT_Door_Buckle, 1);
	indexes.Add(HMT_Pass_Model, 1);

	for (auto model : ModelArray)
	{
		if (indexes.Contains(model->HardModeType) && *indexes.Find(model->HardModeType) == 1)
		{
			model->IsDefault = true;
			indexes[model->HardModeType] = 0;
		}
		else
		{
			model->IsDefault = false;
		}
	}
	return ModelArray;
}
void FArmyHardModeData::AddUniqueModelEntity(TArray<TSharedPtr<FArmyModelEntity>> & Array,TSharedPtr<FArmyModelEntity> Entity)
{
	bool canAdd = true;
	for (auto model : Array)
	{
		if (model->ItemID == Entity->ItemID && model->HardModeType == Entity->HardModeType)
		{
			canAdd = false;
			break;
		}
	}
	if (canAdd)
	{
		Array.Add(Entity);
	}
}
TArray<TSharedPtr<class FArmyModelEntity>> FArmyHardModeData::GetHardSimpleModelList()
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;
	TArray<TSharedPtr<FArmyModelEntity>> ResultModelArray;

	for (auto &It : ModellingWallList)
	{
		if (ModelEntityIsValid(It->BaseWallMaterial))
		{
			AddUniqueModelEntity(ModelArray, It->BaseWallMaterial);
		}
	}
	if (ModellingFloor.IsValid())
	{
		if (ModelEntityIsValid(ModellingFloor->BaseFloorMaterial))
		{
			AddUniqueModelEntity(ModelArray, ModellingFloor->BaseFloorMaterial);
		}
		if (ModelEntityIsValid(ModellingFloor->BaseFloorSkirtingLine))
		{
			AddUniqueModelEntity(ModelArray, ModellingFloor->BaseFloorSkirtingLine);
		}
	}
	if (ModellingBuckle.IsValid())
	{
		if (ModelEntityIsValid(ModellingBuckle->BuckleMaterial))
		{
			AddUniqueModelEntity(ModelArray, ModellingBuckle->BuckleMaterial);
		}
	}
	if (ModellingRoof.IsValid())
	{
		if (ModelEntityIsValid(ModellingRoof->BaseRoofMaterial))
		{
			AddUniqueModelEntity(ModelArray, ModellingRoof->BaseRoofMaterial);
		}
		if (ModelEntityIsValid(ModellingRoof->BaseRoofSkirtingLine))
		{
			AddUniqueModelEntity(ModelArray, ModellingRoof->BaseRoofSkirtingLine);
		}
		for (auto It2 : ModellingRoof->SpaceAreaList)
		{
			if (ModelEntityIsValid(It2.LampSlotActor))
			{
				AddUniqueModelEntity(ModelArray, It2.LampSlotActor);
			}
		}
	}

	TMap<EHardModeType, int> indexes;
	indexes.Add(HMT_None, -1);
	indexes.Add(HMT_Floor_Material, 1);
	indexes.Add(HMT_Skirting, 1);
	indexes.Add(HMT_Ceiling_Material, 1);
	indexes.Add(HMT_Ceiling_Line, 1);
	indexes.Add(HMT_Wall_Material, 1);
	indexes.Add(HMT_Main_Wall, 1);
	indexes.Add(HMT_Lamp_Trough, 1);
	indexes.Add(HMT_Window_Stone, 1);
	indexes.Add(HMT_Door_Stone, 1);
	indexes.Add(HMT_Door_Buckle, 1);
	indexes.Add(HMT_Pass_Model, 1);

	for (auto model : ModelArray)
	{
		if (indexes.Contains(model->HardModeType) && *indexes.Find(model->HardModeType) == 1)
		{
			model->IsDefault = true;
			indexes[model->HardModeType] = 0;
		}
		else
		{
			model->IsDefault = false;
		}
		//ResultModelArray.Add(model);
	}
	return ModelArray;
	//return ResultModelArray;
}

// 获得地面对象
TArray<TSharedPtr<FModellingWall>> FArmyHardModeData::GetModellingWallList()
{
	return ModellingWallList;
}
TSharedPtr<FModellingWall> FArmyHardModeData::GetModellingWallByType(int type)
{
	for (auto ModellingWall : ModellingWallList)
	{
		if (ModellingWall->ModellingWallType == type)
		{
			return ModellingWall;
		}
	}
	return nullptr;
}

// 获得墙面对象
TSharedPtr<FModellingFloor> FArmyHardModeData::GetModellingFloor()
{
	return ModellingFloor;
}
TSharedPtr<FModellingRoof> FArmyHardModeData::GetModellingRoof()
{
	return ModellingRoof;
}
TSharedPtr<FModellingBuckle> FArmyHardModeData::GetModellingBuckle()
{
	return ModellingBuckle;
}

void FArmyHardModeData::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteObjectStart(TEXT("params"));

	//----------------------墙面造型-----------------------------------------------
	JsonWriter->WriteObjectStart(TEXT("ModellingWall"));
	JsonWriter->WriteArrayStart(TEXT("WallList"));
	for (auto It : ModellingWallList)
	{
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue(TEXT("WallType"), It->WallType);
		JsonWriter->WriteObjectStart(TEXT("BaseWallMaterial"));
		It->BaseWallMaterial->SerializeToJson(JsonWriter, 1);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteValue(TEXT("BaseAreaWidth"), It->BaseAreaWidth);
		JsonWriter->WriteValue(TEXT("BaseAreaHeight"), It->BaseAreaHeight);
		JsonWriter->WriteValue(TEXT("ModellingWallType"), (int32)It->ModellingWallType);

		
		JsonWriter->WriteValue(TEXT("MaterialType"), It->MaterialType);
		//if (It->MaterialType == 1)
			It->DesignStyle.SerializeToJson(JsonWriter, It->DesignStyle);

		if (It->WallType == 0)
		{
			JsonWriter->WriteObjectEnd();
			continue;
		}
		JsonWriter->WriteArrayStart(TEXT("SpaceAreaList"));
		for (auto& ItInfo : It->SpaceAreaList)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue(TEXT("bHasLampWall"), ItInfo.bHasLampWall);
			if (ItInfo.bHasLampWall)
				JsonWriter->WriteValue(TEXT("LampWallPos"), ItInfo.LampWallPos.ToString());

			JsonWriter->WriteValue(TEXT("AreaType"), ItInfo.AreaType);
			JsonWriter->WriteValue(TEXT("MaterialType"), ItInfo.MaterialType);
			//if (ItInfo.MaterialType == 1)
				ItInfo.DesignStyle.SerializeToJson(JsonWriter, ItInfo.DesignStyle);
			if (ItInfo.AreaType == 0)
			{
				JsonWriter->WriteValue(TEXT("LeftUpCornerPoint"), ItInfo.LeftUpCornerPoint.ToString());
				JsonWriter->WriteValue(TEXT("AreaWidth"), ItInfo.AreaWidth);
				JsonWriter->WriteValue(TEXT("AreaHeight"), ItInfo.AreaHeight);
			}
			else if (ItInfo.AreaType == 1)
			{
				JsonWriter->WriteValue(TEXT("Center"), ItInfo.Center.ToString());
				JsonWriter->WriteValue(TEXT("Radius"), ItInfo.Radius);
			}
			else if (ItInfo.AreaType == 2)
			{
				JsonWriter->WriteValue(TEXT("NumSide"), ItInfo.NumSide);
				JsonWriter->WriteValue(TEXT("Position"), ItInfo.Position.ToString());
				JsonWriter->WriteValue(TEXT("StartPoint"), ItInfo.StartPoint.ToString());
				JsonWriter->WriteValue(TEXT("Radius"), ItInfo.Radius);
			}
			else if (ItInfo.AreaType == 3)
			{
				JsonWriter->WriteArrayStart(TEXT("ModellingVertices"));
				for (auto& Point : ItInfo.ModellingVertices)
				{
					JsonWriter->WriteValue(Point.ToString());
				}
				JsonWriter->WriteArrayEnd();
			}

			JsonWriter->WriteObjectStart(TEXT("ModellingMaterial"));
			ItInfo.ModellingMaterial->SerializeToJson(JsonWriter, 1);
			JsonWriter->WriteObjectEnd();

			if (ItInfo.ModelingLine.IsValid())
			{
				JsonWriter->WriteObjectStart(TEXT("ModellingLine"));
				ItInfo.ModelingLine->SerializeToJson(JsonWriter, 1);
				JsonWriter->WriteObjectEnd();
			}

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteObjectEnd();

	//------------------------------地面造型------------------------------------------------
	JsonWriter->WriteObjectStart(TEXT("ModellingFloor"));

	ModellingFloor->DesignStyle.SerializeToJson(JsonWriter, ModellingFloor->DesignStyle);
	JsonWriter->WriteObjectStart(TEXT("BaseFloorMaterial"));
	ModellingFloor->BaseFloorMaterial->SerializeToJson(JsonWriter, 1);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("BaseFloorSkirtingLine"));
	if (ModellingFloor->BaseFloorSkirtingLine.IsValid())
		ModellingFloor->BaseFloorSkirtingLine->SerializeToJson(JsonWriter, 1);

	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectEnd();
	// ----  buckle ------------
	JsonWriter->WriteObjectStart(TEXT("ModellingBuckle"));
	if (ModellingBuckle.IsValid())
	{
		if (ModellingBuckle->BuckleMaterial.IsValid())
		{
			JsonWriter->WriteObjectStart(TEXT("BuckleMaterial"));
			ModellingBuckle->BuckleMaterial->SerializeToJson(JsonWriter, 1);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteObjectEnd();// End ModellingBuckle

	//------------------------------顶面造型------------------------------------------------
	JsonWriter->WriteObjectStart(TEXT("ModellingRoof"));

	JsonWriter->WriteValue(TEXT("WallType"), ModellingRoof->WallType);
	JsonWriter->WriteObjectStart(TEXT("BaseRoofMaterial"));
	ModellingRoof->BaseRoofMaterial->SerializeToJson(JsonWriter, 1);
	JsonWriter->WriteObjectEnd();
	if (ModellingRoof->BaseRoofSkirtingLine.IsValid())
	{
		JsonWriter->WriteObjectStart(TEXT("BaseRoofSkirtingLine"));
		ModellingRoof->BaseRoofSkirtingLine->SerializeToJson(JsonWriter, 1);
		JsonWriter->WriteObjectEnd();
	}
	//if (ModellingRoof->MaterialType == 1)
	{
		ModellingRoof->DesignStyle.SerializeToJson(JsonWriter, ModellingRoof->DesignStyle);
	}
	JsonWriter->WriteValue("ExtrusionHeight", ModellingRoof->ExtrusionHeight);
	JsonWriter->WriteValue("BaseAreaWidth", ModellingRoof->BaseAreaWidth);
	JsonWriter->WriteValue("BaseAreaHeight", ModellingRoof->BaseAreaHeight);

	if (ModellingRoof->WallType != 0)
	{
		//TODO:有顶面造型
		JsonWriter->WriteArrayStart(TEXT("SpaceAreaList"));
		for (auto& ItInfo : ModellingRoof->SpaceAreaList)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue(TEXT("AreaType"), ItInfo.AreaType);
			JsonWriter->WriteValue(TEXT("MaterialType"), ItInfo.MaterialType);
			//if (ItInfo.MaterialType == 1)
			ItInfo.DesignStyle.SerializeToJson(JsonWriter, ItInfo.DesignStyle);
			if (ItInfo.AreaType == 0)
			{
				JsonWriter->WriteValue(TEXT("LeftUpCornerPoint"), ItInfo.LeftUpCornerPoint.ToString());
				JsonWriter->WriteValue(TEXT("AreaWidth"), ItInfo.AreaWidth);
				JsonWriter->WriteValue(TEXT("AreaHeight"), ItInfo.AreaHeight);
			}
			else if (ItInfo.AreaType == 1)
			{
				JsonWriter->WriteValue(TEXT("Center"), ItInfo.Center.ToString());
				JsonWriter->WriteValue(TEXT("Radius"), ItInfo.Radius);
			}
			else if (ItInfo.AreaType == 2)
			{
				JsonWriter->WriteValue(TEXT("NumSide"), ItInfo.NumSide);
				JsonWriter->WriteValue(TEXT("Position"), ItInfo.Position.ToString());
				JsonWriter->WriteValue(TEXT("StartPoint"), ItInfo.StartPoint.ToString());
				JsonWriter->WriteValue(TEXT("Radius"), ItInfo.Radius);
			}
			else if (ItInfo.AreaType == 3)
			{
				JsonWriter->WriteArrayStart(TEXT("ModellingVertices"));
				for (auto& Point : ItInfo.ModellingVertices)
				{
					JsonWriter->WriteValue(Point.ToString());
				}
				JsonWriter->WriteArrayEnd();
			}
			JsonWriter->WriteValue(TEXT("ExtrusionHeight"), ItInfo.ExtrusionHeight);
			JsonWriter->WriteObjectStart(TEXT("ModellingMaterial"));
			ItInfo.ModellingMaterial->SerializeToJson(JsonWriter, 1);
			JsonWriter->WriteObjectEnd();

			if (ItInfo.LampSlotActor.IsValid())
			{
				JsonWriter->WriteObjectStart(TEXT("LampSlotActor"));
				ItInfo.LampSlotActor->SerializeToJson(JsonWriter, 1);
				JsonWriter->WriteObjectEnd();
			}

			if (ItInfo.ModelingLine.IsValid())
			{
				JsonWriter->WriteObjectStart(TEXT("ModelingLine"));
				ItInfo.ModelingLine->SerializeToJson(JsonWriter, 1);
				JsonWriter->WriteObjectEnd();
			}

			JsonWriter->WriteArrayStart(TEXT("ModellingHelpAreas"));
			for (auto HelpArea : ItInfo.ModellingHelpAreas)
			{
				JsonWriter->WriteObjectStart();

				HelpArea.DesignStyle.SerializeToJson(JsonWriter, HelpArea.DesignStyle);
				JsonWriter->WriteObjectStart(TEXT("ModellingMaterial"));
				HelpArea.ModellingMaterial->SerializeToJson(JsonWriter, 1);
				JsonWriter->WriteObjectEnd();

				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}
	JsonWriter->WriteObjectEnd();
	//End ModellingRoof

	JsonWriter->WriteObjectEnd();//End HardModeData
}

void FArmyHardModeData::Deserialization(const TSharedPtr<FJsonObject>& InJsonData, const TArray<TSharedPtr<FArmyModelEntity>> & HardArray)
{
	HardModeArray = HardArray;

	//TSharedPtr<FJsonObject> InJsonData = InJsonData->GetObjectField("params");

	// --------Wall-------------------------------------------------
	TSharedPtr<FJsonObject> WallJsonDta = InJsonData->GetObjectField("ModellingWall");
	const TArray<TSharedPtr<FJsonValue>> WallList = WallJsonDta->GetArrayField(TEXT("WallList"));

	for (auto ItJson : WallList)
	{
		TSharedPtr<FModellingWall> CurrentModellingWall = MakeShareable(new FModellingWall);
		ModellingWallList.Push(CurrentModellingWall);

		CurrentModellingWall->WallType = ItJson->AsObject()->GetNumberField("WallType");
		/*CurrentModellingWall->BaseWallMaterial = MakeShareable(new FArmyModelEntity());
		CurrentModellingWall->BaseWallMaterial->Deserialization(ItJson->AsObject()->GetObjectField("BaseWallMaterial"));*/
		CurrentModellingWall->BaseAreaWidth = ItJson->AsObject()->GetNumberField("BaseAreaWidth");
		CurrentModellingWall->BaseAreaHeight = ItJson->AsObject()->GetNumberField("BaseAreaHeight");

		CurrentModellingWall->ModellingWallType = (int32)ItJson->AsObject()->GetNumberField("ModellingWallType");
		if (CurrentModellingWall->ModellingWallType == 2 || CurrentModellingWall->ModellingWallType == 3)
		{
			CurrentModellingWall->BaseWallMaterial = GetModelEntityByType(HMT_Main_Wall);
		}
		else
		{
			CurrentModellingWall->BaseWallMaterial = GetModelEntityByType(HMT_Wall_Material);
		}

		
		CurrentModellingWall->MaterialType = ItJson->AsObject()->GetNumberField("MaterialType");
		//if (CurrentModellingWall->MaterialType == 1)
		if (ItJson->AsObject()->HasField("DesignStyle"))
		{
			CurrentModellingWall->DesignStyle.Deserialization(ItJson->AsObject()->GetObjectField("DesignStyle"));
		}

		if (CurrentModellingWall->WallType == 0)
			continue;

		const TArray<TSharedPtr<FJsonValue>> SpaceAreaList = ItJson->AsObject()->GetArrayField(TEXT("SpaceAreaList"));
		for (auto ItSpaceJson : SpaceAreaList)
		{
			FModellingAreaInfo CurrentAreaInfo;

			CurrentAreaInfo.bHasLampWall = ItSpaceJson->AsObject()->GetBoolField("bHasLampWall");
			if (CurrentAreaInfo.bHasLampWall)
				CurrentAreaInfo.LampWallPos.InitFromString(ItSpaceJson->AsObject()->GetStringField("LampWallPos"));

			CurrentAreaInfo.AreaType = ItSpaceJson->AsObject()->GetNumberField("AreaType");
			CurrentAreaInfo.MaterialType = ItSpaceJson->AsObject()->GetNumberField("MaterialType");
			//if (CurrentAreaInfo.MaterialType == 1)
			{
				CurrentAreaInfo.DesignStyle.Deserialization
				(ItSpaceJson->AsObject()->GetObjectField("DesignStyle"));
			}
			if (CurrentAreaInfo.AreaType == 0)
			{
				CurrentAreaInfo.LeftUpCornerPoint.InitFromString(ItSpaceJson->AsObject()->GetStringField("LeftUpCornerPoint"));
				CurrentAreaInfo.AreaWidth = ItSpaceJson->AsObject()->GetNumberField("AreaWidth");
				CurrentAreaInfo.AreaHeight = ItSpaceJson->AsObject()->GetNumberField("AreaHeight");
			}
			else if (CurrentAreaInfo.AreaType == 1)
			{
				CurrentAreaInfo.Center.InitFromString(ItSpaceJson->AsObject()->GetStringField("Center"));
				CurrentAreaInfo.Radius = ItSpaceJson->AsObject()->GetNumberField("Radius");
			}
			else if (CurrentAreaInfo.AreaType == 2)
			{
				CurrentAreaInfo.NumSide = ItSpaceJson->AsObject()->GetNumberField("NumSide");
				CurrentAreaInfo.Position.InitFromString(ItSpaceJson->AsObject()->GetStringField("Position"));
				CurrentAreaInfo.StartPoint.InitFromString(ItSpaceJson->AsObject()->GetStringField("StartPoint"));
				CurrentAreaInfo.Radius = ItSpaceJson->AsObject()->GetNumberField("Radius");
			}
			else if (CurrentAreaInfo.AreaType == 3)
			{
				TArray<FString> VertexesStrArray;
				ItSpaceJson->AsObject()->TryGetStringArrayField("ModellingVertices", VertexesStrArray);
				for (auto& ArrayIt : VertexesStrArray)
				{
					FVector Vertex;
					Vertex.InitFromString(ArrayIt);
					CurrentAreaInfo.ModellingVertices.Add(Vertex);
				}
			}
			CurrentAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity());
			CurrentAreaInfo.ModellingMaterial->Deserialization(ItSpaceJson->AsObject()->GetObjectField("ModellingMaterial"),1);
			CurrentAreaInfo.ModelingLine = MakeShareable(new FArmyModelEntity());
			CurrentAreaInfo.ModelingLine->Deserialization(ItSpaceJson->AsObject()->GetObjectField("ModellingLine"),1);

			CurrentModellingWall->SpaceAreaList.Push(CurrentAreaInfo);
		}
	}

	// -------- floor ------------
	TSharedPtr<FJsonObject> FloorJsonDta = InJsonData->GetObjectField("ModellingFloor");
	ModellingFloor = MakeShareable(new FModellingFloor);
	ModellingFloor->DesignStyle.Deserialization(FloorJsonDta->GetObjectField("DesignStyle"));
	/*ModellingFloor->BaseFloorMaterial = MakeShareable(new FArmyModelEntity());
	ModellingFloor->BaseFloorMaterial->Deserialization(FloorJsonDta->GetObjectField("BaseFloorMaterial"));
	ModellingFloor->BaseFloorSkirtingLine = MakeShareable(new FArmyModelEntity());
	ModellingFloor->BaseFloorSkirtingLine->Deserialization(FloorJsonDta->GetObjectField("BaseFloorSkirtingLine"));*/
	ModellingFloor->BaseFloorMaterial = GetModelEntityByType(HMT_Floor_Material);
	ModellingFloor->BaseFloorSkirtingLine = GetModelEntityByType(HMT_Skirting);
	// -------- Buckle --------------
	TSharedPtr<FJsonObject> BuckleJsonDta = InJsonData->GetObjectField("ModellingBuckle");
	ModellingBuckle = MakeShareable(new FModellingBuckle);
	ModellingBuckle->BuckleMaterial = GetModelEntityByType(HMT_Door_Buckle);
	// -------- roof ------------
	TSharedPtr<FJsonObject> RoofJsonDta = InJsonData->GetObjectField("ModellingRoof");
	ModellingRoof = MakeShareable(new FModellingRoof);
	ModellingRoof->WallType = RoofJsonDta->GetNumberField("WallType");
	ModellingRoof->MaterialType = RoofJsonDta->GetNumberField("MaterialType");
	//if (ModellingRoof->MaterialType == 1)
	ModellingRoof->DesignStyle.Deserialization(RoofJsonDta->GetObjectField("DesignStyle"));
	/*ModellingRoof->BaseRoofMaterial = MakeShareable(new FArmyModelEntity());
	ModellingRoof->BaseRoofMaterial->Deserialization(RoofJsonDta->GetObjectField("BaseRoofMaterial"));
	ModellingRoof->BaseRoofSkirtingLine = MakeShareable(new FArmyModelEntity());
	ModellingRoof->BaseRoofSkirtingLine->Deserialization(RoofJsonDta->GetObjectField("BaseRoofSkirtingLine"));*/
	ModellingRoof->BaseRoofMaterial = GetModelEntityByType(HMT_Ceiling_Material);
	ModellingRoof->BaseRoofSkirtingLine = GetModelEntityByType(HMT_Ceiling_Line);

	ModellingRoof->ExtrusionHeight = RoofJsonDta->GetNumberField("ExtrusionHeight");
	ModellingRoof->BaseAreaWidth = RoofJsonDta->GetNumberField("BaseAreaWidth");
	ModellingRoof->BaseAreaHeight = RoofJsonDta->GetNumberField("BaseAreaHeight");
	const TArray<TSharedPtr<FJsonValue>> SpaceAreaList = RoofJsonDta->GetArrayField(TEXT("SpaceAreaList"));
	for (auto ItSpaceJson : SpaceAreaList)
	{
		FModellingAreaInfo CurrentAreaInfo;

		const TArray<TSharedPtr<FJsonValue>> ModellingHelpAreas = ItSpaceJson->AsObject()->GetArrayField(TEXT("ModellingHelpAreas"));
		for (auto HelpArea : ModellingHelpAreas)
		{
			FModellingHelpAreaInfo HelpAreaInfo;
			HelpAreaInfo.DesignStyle.Deserialization(HelpArea->AsObject()->GetObjectField("DesignStyle"));
			HelpAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity());
			HelpAreaInfo.ModellingMaterial->Deserialization(HelpArea->AsObject()->GetObjectField("ModellingMaterial"),1);
			CurrentAreaInfo.ModellingHelpAreas.Add(HelpAreaInfo);
		}
		CurrentAreaInfo.AreaType = ItSpaceJson->AsObject()->GetNumberField("AreaType");
		CurrentAreaInfo.MaterialType = ItSpaceJson->AsObject()->GetNumberField("MaterialType");
		//if (CurrentAreaInfo.MaterialType == 1)
		{
			CurrentAreaInfo.DesignStyle.Deserialization
			(ItSpaceJson->AsObject()->GetObjectField("DesignStyle"));
		}
		if (CurrentAreaInfo.AreaType == 0)
		{
			CurrentAreaInfo.LeftUpCornerPoint.InitFromString(ItSpaceJson->AsObject()->GetStringField("LeftUpCornerPoint"));
			CurrentAreaInfo.AreaWidth = ItSpaceJson->AsObject()->GetNumberField("AreaWidth");
			CurrentAreaInfo.AreaHeight = ItSpaceJson->AsObject()->GetNumberField("AreaHeight");
		}
		else if (CurrentAreaInfo.AreaType == 1)
		{
			CurrentAreaInfo.Center.InitFromString(ItSpaceJson->AsObject()->GetStringField("Center"));
			CurrentAreaInfo.Radius = ItSpaceJson->AsObject()->GetNumberField("Radius");
		}
		else if (CurrentAreaInfo.AreaType == 2)
		{
			CurrentAreaInfo.NumSide = ItSpaceJson->AsObject()->GetNumberField("NumSide");
			CurrentAreaInfo.Position.InitFromString(ItSpaceJson->AsObject()->GetStringField("Position"));
			CurrentAreaInfo.StartPoint.InitFromString(ItSpaceJson->AsObject()->GetStringField("StartPoint"));
			CurrentAreaInfo.Radius = ItSpaceJson->AsObject()->GetNumberField("Radius");
		}
		else if (CurrentAreaInfo.AreaType == 3)
		{
			TArray<FString> VertexesStrArray;
			ItSpaceJson->AsObject()->TryGetStringArrayField("ModellingVertices", VertexesStrArray);
			for (auto& ArrayIt : VertexesStrArray)
			{
				FVector Vertex;
				Vertex.InitFromString(ArrayIt);
				CurrentAreaInfo.ModellingVertices.Add(Vertex);
			}
		}
		CurrentAreaInfo.ExtrusionHeight = ItSpaceJson->AsObject()->GetNumberField("ExtrusionHeight");

		CurrentAreaInfo.ModellingMaterial = MakeShareable(new FArmyModelEntity());
		CurrentAreaInfo.ModellingMaterial->Deserialization(ItSpaceJson->AsObject()->GetObjectField("ModellingMaterial"),1);

		/*CurrentAreaInfo.LampSlotActor = MakeShareable(new FArmyModelEntity());
		CurrentAreaInfo.LampSlotActor->Deserialization(ItSpaceJson->AsObject()->GetObjectField("LampSlotActor"));*/
		const TSharedPtr<FJsonObject> * outObject;
		if (ItSpaceJson->AsObject()->TryGetObjectField("LampSlotActor", outObject))
		{
			CurrentAreaInfo.LampSlotActor = GetModelEntityByType(HMT_Lamp_Trough);
		}
		if (ItSpaceJson->AsObject()->TryGetObjectField("ModelingLine",outObject))
		{
			CurrentAreaInfo.ModelingLine = MakeShareable(new FArmyModelEntity());
			CurrentAreaInfo.ModelingLine->Deserialization(ItSpaceJson->AsObject()->GetObjectField("ModelingLine"));
		}

		ModellingRoof->SpaceAreaList.Push(CurrentAreaInfo);
	}

}
TSharedPtr<FArmyModelEntity> FArmyHardModeData::GetModelEntityByType(EHardModeType hardModeType)
{
	for (auto hardmode : HardModeArray)
	{
		if (hardmode->HardModeType == hardModeType)
		{
			return hardmode;
		}
	}
	return nullptr;
}

// 获得顶角线高度，自身高度
float FArmyHardModeData::GetCrownMouldingHeight()
{
	if (ModellingRoof.IsValid() && ModellingRoof->BaseRoofSkirtingLine.IsValid() && ModellingRoof->BaseRoofSkirtingLine->FurContentItem.IsValid())
	{
		FVector Size = FArmyDataTools::GetContextItemSize(ModellingRoof->BaseRoofSkirtingLine->FurContentItem);
		return Size.Z;
	}
	return 0;
}

// 获得吊顶下吊，自身高度
float FArmyHardModeData::GetCeilingDropOff()
{
	if (ModellingRoof.IsValid())
	{
		return (FArmySceneData::WallHeight - ModellingRoof->ExtrusionHeight);
	}
	return 0;
}

void FArmyHardModeData::GetRoomSpaceListFromBed(TMap<int, TSharedPtr<FArmyRoomSpaceArea>>& OutRoomSpaceList, TSharedPtr<FArmyRoomEntity>& InRoom)
{
	TArray<TSharedPtr<class FArmyModelEntity>> DoubleBedArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DoubleBed, DoubleBedArray);
	if (DoubleBedArray.Num() == 0)
		InRoom->GetModelEntityArrayByComponentId(AI_SingleBed, DoubleBedArray);

	if (DoubleBedArray.Num() > 0)
	{
		TSharedPtr<class FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir
		(*(DoubleBedArray[0])->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
		TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = InRoom->GetWallSpaceArray();
		for (auto It : RoomSpaceAreas)
		{
			if (It->AttachBspID == WallLine->GetUniqueID().ToString())
			{
				OutRoomSpaceList.Add(1, It);
			}
		}
		WallLine = FArmyDataTools::GetWallLineByModelInDir
		(*(DoubleBedArray[0])->Actor, FArmyDataTools::AI_DT_MODEL_FORWARD);
		for (auto It : RoomSpaceAreas)
		{
			if (It->AttachBspID == WallLine->GetUniqueID().ToString())
			{
				//OutRoomSpaceList.Push(It);
				OutRoomSpaceList.Add(2, It);
			}
		}
	}

}

TSharedPtr<FArmyRoomSpaceArea> FArmyHardModeData::GetRoomSpaceFromSofaOrTVBench(TSharedPtr<FArmyRoomEntity>& InRoom, EAIComponentCode InCode)
{
	TArray<TSharedPtr<class FArmyModelEntity>> DoubleSofaArray;
	InRoom->GetModelEntityArrayByComponentId(InCode, DoubleSofaArray);

	if (DoubleSofaArray.Num() > 0)
	{
		TSharedPtr<class FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir
		(*(DoubleSofaArray[0])->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
		TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSpaceAreas = InRoom->GetWallSpaceArray();
		for (auto It : RoomSpaceAreas)
		{
			if (It->AttachBspID == WallLine->GetUniqueID().ToString())
				return It;
		}
	}

	return nullptr;
}

void TilePasteStyle::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, const TilePasteStyle& InStyle)
{
	JsonWriter->WriteObjectStart(TEXT("DesignStyle"));
	JsonWriter->WriteValue(TEXT("EdityType"), (int32)InStyle.EdityType);
	JsonWriter->WriteValue(TEXT("M_AlignmentType"), (int32)InStyle.M_AlignmentType);
	JsonWriter->WriteValue(TEXT("InternalDist"), InStyle.InternalDist);
	JsonWriter->WriteValue(TEXT("XDirOffset"), InStyle.XDirOffset);
	JsonWriter->WriteValue(TEXT("YDirOffset"), InStyle.YDirOffset);
	JsonWriter->WriteValue(TEXT("RotationAngle"), InStyle.RotationAngle);
	JsonWriter->WriteValue(TEXT("HeightToFloor"), InStyle.HeightToFloor);
	JsonWriter->WriteValue(TEXT("GapColor"), InStyle.GapColor.ToString());
	JsonWriter->WriteValue(TEXT("WallPaintColor"), InStyle.WallPaintColor.ToString());
	JsonWriter->WriteValue(TEXT("ColorPaintValueKey"), InStyle.ColorPaintValueKey);
	JsonWriter->WriteObjectEnd();
}

void TilePasteStyle::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	EdityType = (EStyleType)((int32)InJsonData->GetNumberField("EdityType"));
	M_AlignmentType = (AlignmentType)((int32)InJsonData->GetNumberField("M_AlignmentType"));
	InternalDist = InJsonData->GetNumberField("internalDist");
	XDirOffset = InJsonData->GetNumberField("xDirOffset");
	YDirOffset = InJsonData->GetNumberField("yDirOffset");
	RotationAngle = InJsonData->GetNumberField("rotationAngle");
	HeightToFloor = InJsonData->GetNumberField("heightToFloor");
	GapColor.InitFromString(InJsonData->GetStringField("GapColor"));
	if (InJsonData->HasField("WallPaintColor"))
	{
		WallPaintColor.InitFromString(InJsonData->GetStringField("WallPaintColor"));
		ColorPaintValueKey = InJsonData->GetStringField("ColorPaintValueKey");
	}
	else
	{
		IsHaveWallPaintColor = false;
	}
	

}

FArmyHardwareData::FArmyHardwareData()
{
	TArray<TWeakPtr<FArmyObject>> ResultSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, ResultSurfaces);
	TMap<int32, uint32> CurrentMap;
	TSharedPtr<class FContentItemSpace::FContentItem>  ContentItemBridgeStone;
	uint32 MaxNum = 1;
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
			TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem = Space->GetStyle()->GetCurrentItem();
			if (ContentItem.IsValid())
			{
				if (CurrentMap.Find(ContentItem->ID) == nullptr)
				{
					CurrentMap.Add(ContentItem->ID, 1);
					TSharedPtr<FBridgeStone> CurrentBridgeStone = MakeShareable(new FBridgeStone);
					CurrentBridgeStone->StoneMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
					CurrentBridgeStone->StoneMaterial->ItemType = IT_HardMode;
					CurrentBridgeStone->StoneMaterial->HardModeType = HMT_Door_Stone;
					// 获得勾选的施工项信息
					CurrentBridgeStone->StoneMaterial->SetPasteConstructionItem(Space);
					BridgeStoneList.Add(CurrentBridgeStone);
				}

				//else
				//{
				//	uint32* CurrentValue = CurrentMap.Find(ContentItem->ID);
				//	++(*CurrentValue);
				//	if (*CurrentValue > MaxNum)
				//	{
				//		MaxNum = (*CurrentValue);
				//		ContentItemBridgeStone = ContentItem;//将数量最多的过门石材质获取
				//	}	
				//}
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
				if (Window->GetIfGenerateWindowStone() == false)
					continue;
				if (Window->HoleWindow && Window->HoleWindow->GetSynID() > 0)
				{
					TSharedPtr<FWindowStone> WindowStone = MakeShareable(new FWindowStone);
					UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
					TSharedPtr<FContentItemSpace::FContentItem> ContentItem = ResMg->GetContentItemFromID(Window->HoleWindow->GetSynID());
					WindowStone->StoneMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
					WindowStone->StoneMaterial->ItemType = IT_HardMode;
					WindowStone->StoneMaterial->HardModeType = HMT_Window_Stone;
					WindowStone->RoomType = FArmyDataTools::GetRoomType(Room->GetSpaceId());
					WindowStoneList.Add(WindowStone);

					break;
				}
			}
			else if (Hardware->GetType() == OT_RectBayWindow ||
				Hardware->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> Window = StaticCastSharedPtr<FArmyRectBayWindow>(Hardware);
				if (Window->GetIfGenerateWindowStone() == false)
					continue;
				if (Window->HoleWindow && Window->HoleWindow->GetSynID() > 0)
				{
					TSharedPtr<FWindowStone> WindowStone = MakeShareable(new FWindowStone);
					UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
					TSharedPtr<FContentItemSpace::FContentItem> ContentItem = ResMg->GetContentItemFromID(Window->HoleWindow->GetSynID());
					WindowStone->StoneMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
					WindowStone->StoneMaterial->ItemType = IT_HardMode;
					WindowStone->StoneMaterial->HardModeType = HMT_Window_Stone;
					WindowStone->RoomType = FArmyDataTools::GetRoomType(Room->GetSpaceId());
					WindowStoneList.Add(WindowStone);

					break;
				}
			}
		}
	}

	CurrentMap.Empty();
	MaxNum = 1;
	TArray<TWeakPtr<FArmyObject>> PassList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, PassList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, PassList);
	for (auto ItPass : PassList)
	{
		TSharedPtr<class FContentItemSpace::FContentItem>  ContentItem;
		if (ItPass.Pin()->GetType() == OT_Pass)
		{
			TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(ItPass.Pin());
			if (!Pass->GetIfGeneratePassModel())
				continue;
			ContentItem = Pass->PassContentItem;
		}
		else if (ItPass.Pin()->GetType() == OT_NewPass)
		{
			TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(ItPass.Pin());
			if (!Pass->GetIfGeneratePassModel())
				continue;
			ContentItem = Pass->PassContentItem;
		}
		if (ContentItem.IsValid())
		{
			if (CurrentMap.Find(ContentItem->ID) == nullptr)
			{
				CurrentMap.Add(ContentItem->ID, 1);
				TSharedPtr<FPassModel> CurrentPassModel = MakeShareable(new FPassModel);
				CurrentPassModel->PassMaterial = MakeShareable(new FArmyModelEntity(ContentItem));
				CurrentPassModel->PassMaterial->ItemType = IT_HardMode;
				CurrentPassModel->PassMaterial->HardModeType = HMT_Pass_Model;
				PassModelList.Add(CurrentPassModel);
			}
			else
			{
				uint32* CurrentValue = CurrentMap.Find(ContentItem->ID);
				++(*CurrentValue);
				if (*CurrentValue > MaxNum)
				{
					MaxNum = (*CurrentValue);
					ContentItemBridgeStone = ContentItem;//将数量最多的过门石材质获取
				}	
			}
		}
	}
	if (PassModelList.Num() > 1)
		for (int i = 0; i < PassModelList.Num(); i++)
		{
			if (PassModelList[i]->PassMaterial->FurContentItem->ID == ContentItemBridgeStone->ID)
			{
				PassModelList.Swap(0, i);
				break;
			}
		}
}

FArmyHardwareData::FArmyHardwareData(TArray<TSharedPtr<FArmyModelEntity>> InHardModeArray)
{
	HardModeArray = InHardModeArray;

	TSharedPtr<FBridgeStone> BridgeStone = MakeShareable(new FBridgeStone);
	BridgeStone->StoneMaterial = GetModelEntityByType(HMT_Door_Stone);
	BridgeStoneList.Add(BridgeStone);

	TSharedPtr<FWindowStone> WindowStone = MakeShareable(new FWindowStone);
	WindowStone->StoneMaterial = GetModelEntityByType(HMT_Window_Stone);
	WindowStoneList.Add(WindowStone);

	TSharedPtr<FPassModel> PassModel = MakeShareable(new FPassModel);
	PassModel->PassMaterial = GetModelEntityByType(HMT_Pass_Model);
	PassModelList.Add(PassModel);
}
void FArmyHardwareData::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteObjectStart(TEXT("params"));

	//过门石
	JsonWriter->WriteArrayStart(TEXT("BridgeStoneList"));
	for (auto& It : BridgeStoneList)
	{
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteObjectStart(TEXT("BridgeStoneMaterial"));
		It->StoneMaterial->SerializeToJson(JsonWriter, 1);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//窗台石
	JsonWriter->WriteArrayStart(TEXT("WindowStoneList"));
	for (auto& It : WindowStoneList)
	{
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue(TEXT("RoomType"), (int32)It->RoomType);
		JsonWriter->WriteObjectStart(TEXT("WindowStoneMaterial"));
		It->StoneMaterial->SerializeToJson(JsonWriter, 1);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//垭口包边
	JsonWriter->WriteArrayStart(TEXT("PassModelList"));
	for (auto& It : PassModelList)
	{
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteObjectStart(TEXT("PassModelMaterial"));
		It->PassMaterial->SerializeToJson(JsonWriter, 1);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();
}

void FArmyHardwareData::Deserialization(const TArray<TSharedPtr<FArmyModelEntity>> & HardArray)
{
	HardModeArray = HardArray;

	TSharedPtr<FBridgeStone> BridgeStone = MakeShareable(new FBridgeStone);
	BridgeStone->StoneMaterial = GetModelEntityByType(HMT_Door_Stone);
	BridgeStoneList.Add(BridgeStone);

	TSharedPtr<FWindowStone> WindowStone = MakeShareable(new FWindowStone);
	WindowStone->StoneMaterial = GetModelEntityByType(HMT_Window_Stone);
	WindowStoneList.Add(WindowStone);

	TSharedPtr<FPassModel> PassModel = MakeShareable(new FPassModel);
	PassModel->PassMaterial = GetModelEntityByType(HMT_Pass_Model);
	PassModelList.Add(PassModel);

	//TSharedPtr<FJsonObject> HardwareJson = InJsonData->GetObjectField("params");

	//const TArray<TSharedPtr<FJsonValue>> BridgeStoneListJson = HardwareJson->GetArrayField(TEXT("BridgeStoneList"));
	//for (auto ItJson : BridgeStoneListJson)
	//{
	//	TSharedPtr<FBridgeStone> BridgeStone = MakeShareable(new FBridgeStone);
	//	/*BridgeStone->StoneMaterial = MakeShareable(new FArmyModelEntity);
	//	BridgeStone->StoneMaterial->Deserialization(ItJson->AsObject()->GetObjectField("BridgeStoneMaterial"));*/
	//	BridgeStone->StoneMaterial = GetModelEntityByType(HMT_Window_Stone);
	//	BridgeStoneList.Add(BridgeStone);
	//}

	//const TArray<TSharedPtr<FJsonValue>> WindowListJson = HardwareJson->GetArrayField(TEXT("WindowStoneList"));
	//for (auto ItJson : WindowListJson)
	//{
	//	TSharedPtr<FWindowStone> WindowStone = MakeShareable(new FWindowStone);
	//	WindowStone->RoomType = (ERoomType)((int32)ItJson->AsObject()->GetNumberField("RoomType"));
	//	/*WindowStone->StoneMaterial = MakeShareable(new FArmyModelEntity);
	//	WindowStone->StoneMaterial->Deserialization(ItJson->AsObject()->GetObjectField("WindowStoneMaterial"));*/
	//	WindowStone->StoneMaterial = GetModelEntityByType(HMT_Window_Stone);
	//	WindowStoneList.Add(WindowStone);
	//}
}

// 获得硬装模型列表
TArray<TSharedPtr<class FArmyModelEntity>> FArmyHardwareData::GetHardModelList()
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;
	for (auto &It : WindowStoneList)
	{
		if (It->StoneMaterial.IsValid() && It->StoneMaterial->FurContentItem.IsValid())
		{
			ModelArray.Add(It->StoneMaterial);
		}
	}
	for (auto &It : BridgeStoneList)
	{
		if (It->StoneMaterial.IsValid() && It->StoneMaterial->FurContentItem.IsValid())
		{
			ModelArray.Add(It->StoneMaterial);
		}
	}
	for (auto &It : PassModelList)
	{
		if (It->PassMaterial.IsValid() && It->PassMaterial->FurContentItem.IsValid())
		{
			ModelArray.Add(It->PassMaterial);
		}
	}
	TMap<EHardModeType, int> indexes;
	indexes.Add(HMT_None, -1);
	indexes.Add(HMT_Floor_Material, 1);
	indexes.Add(HMT_Skirting, 1);
	indexes.Add(HMT_Ceiling_Material, 1);
	indexes.Add(HMT_Ceiling_Line, 1);
	indexes.Add(HMT_Wall_Material, 1);
	indexes.Add(HMT_Main_Wall, 1);
	indexes.Add(HMT_Lamp_Trough, 1);
	indexes.Add(HMT_Window_Stone, 1);
	indexes.Add(HMT_Door_Stone, 1);
	indexes.Add(HMT_Pass_Model, 1);

	for (auto model : ModelArray)
	{
		if (indexes.Contains(model->HardModeType) && *indexes.Find(model->HardModeType) == 1)
		{
			model->IsDefault = true;
			indexes[model->HardModeType] = 0;
		}
		else
		{
			model->IsDefault = false;
		}
	}
	return ModelArray;
}
TSharedPtr<FArmyModelEntity> FArmyHardwareData::GetModelEntityByType(EHardModeType hardModeType)
{
	for (auto hardmode : HardModeArray)
	{
		if (hardmode->HardModeType == hardModeType)
		{
			return hardmode;
		}
	}
	return nullptr;
}


void FArmyHardModeData::ExtractMaximumRectangleInPolygonSimply(const TArray<FVector>&Polygon, FVector & Center, FBox & Box)
{
	TArray<FVector> PointArr;
	FArmyDataTools::GetMaxQuadInPolygon(Polygon, Center, PointArr, Box);
}