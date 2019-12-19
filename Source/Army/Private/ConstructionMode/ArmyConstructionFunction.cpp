#include "ArmyConstructionFunction.h"
#include "Math/XRMath.h"
#include "ArmySceneData.h"
#include "ArmyDimensions.h"
#include "ArmyObjAutoRuler.h"
#include "ArmyFurniture.h"
#include "ArmyWindow.h"
#include "ArmyRectBayWindow.h"
#include "ArmyCornerBayWindow.h"
#include "ArmyBaseArea.h"
#include "ArmyRoom.h"
#include "ArmyGameInstance.h"
#include "ArmyEntrance.h"
#include "ArmyPass.h"
#include "ArmyBeam.h"
#include "ArmyLampControlLines.h"
#include "ArmyPolygon.h"
#include "ArmyWHCTableObject.h"
#include "ArmyWallLine.h"
#include "ArmySymbol.h"
#include "ArmyWHCDoorSheet.h"
#include "ArmyCustomDefine.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeTableActor.h"
#include "SCTBoardActor.h"
#include "SCTDoorGroup.h"
#include "SCTModelShape.h"
#include "WHCMode/common/XRSpline.h"
#include "SCTShapeUtilityTools.h"

#include "ArmyRectArea.h"
#include "ArmyCircleArea.h"
#include "ArmyRegularPolygonArea.h"

#define DISCARD_REPEAT

#define OffsetValue 15.0f


ObjectClassType FArmyConstructionFunction::GetAnnotationClassTypeByObject(ObjectClassType InObjClass)
{
	ObjectClassType ClassType = InObjClass;
	switch (InObjClass)
	{
	case Class_AddWall:
		ClassType = Class_AnnotationAddWall;
		break;
	case Class_ElectricBoxH:
		ClassType = Class_AnnotationElectricBoxH;
		break;
	case Class_ElectricBoxL:
		ClassType = Class_AnnotationElectricBoxL;
		break;
	case Class_AirConditionerPoint:
		ClassType = Class_AnnotationAirConditionerPoint;
		break;
	case Class_Furniture:
		ClassType = Class_AnnotationFurniture;
		break;
	case Class_Switch:
		ClassType = Class_AnnotationSwitch;
		break;
	case Class_Socket:
		ClassType = Class_AnnotationSocket;
		break;
	case Class_Socket_H:
		ClassType = Class_AnnotationSocket_H;
		break;
	case Class_Socket_L:
		ClassType = Class_AnnotationSocket_L;
		break;
	case Class_WaterRoutePoint:
		ClassType = Class_AnnotationWaterPoint;
		break;
	case Class_WaterPipe:
		ClassType = Class_AnnotationSewerPipe;
		break;
	case Class_Light:
		ClassType = Class_AnnotationLamp;
		break;
	case Class_Ceilinglayout:
		ClassType = Class_AnnotationCeilingObj;
		break;
	case Class_Beam:
		ClassType = Class_AnnotationBeam;
		break;
	case Class_Heater:
		ClassType = Class_AnnotationHeater;
		break;
	case Class_GasMeter:
	case Class_GasPipe:
		ClassType = Class_AnnotationGas;
		break;
	case Class_FloordRainPoint://地漏下水
	{
		ClassType = Class_AnnotationFloordRainPoint;//地漏下水
	}
	break;
	case Class_ClosestoolRainPoint://马桶下水
	{
		ClassType = Class_AnnotationClosestoolRainPoint;//马桶下水
	}
	break;
	case Class_WashBasinRainPoint://水盆下水
	{
		ClassType = Class_AnnotationWashBasinRainPoint;//水盆下水
	}
	break;
	case Class_HomeEntryWaterPoint: // 给水点位,
	{
		ClassType = Class_AnnotationHomeEntryWaterPoint;//进户水
	}
	break;
	case Class_DiversityWater://分集水器
	{
		ClassType = Class_AnnotationWaterPoint;//  分集水器
	}
	break;
	case Class_HotWaterPoint://热水点位
	{
		ClassType = Class_AnnotationHotWaterPoint;//热水点位
	}
	break;
	case Class_ColdWaterPoint://冷水点位
	{
		ClassType = Class_AnnotationColdWaterPoint;//冷水点位
	}
	break;
	case Class_HotAndColdWaterPoint://冷熱水
	{
		ClassType = Class_AnnotationHotAndColdWaterPoint;//冷热水点位
	}
	break;
	case Class_RecycledWaterPoint://中水
	{
		ClassType = Class_AnnotationRecycledWaterPoint;//中水点位
	}
	break;

	default:
		break;
	}
	return ClassType;
}
ObjectClassType FArmyConstructionFunction::GetClassTypeByObject(FObjectPtr InObj, int32 InType)
{
	ObjectClassType ClassType = Class_Other;

	switch (InObj->GetType())
	{
	case OT_Wall:
	case OT_ArcWall:
	case OT_InternalRoom:
	case OT_OutRoom:
	case OT_BaseWall:
	{
		ClassType = Class_BaseWall;
	}
	break;
	case OT_Beam:
	{
		ClassType = Class_Beam;
	}
	break;
	case OT_ModifyWall:
	{
		ClassType = Class_BreakWall;
	}
	break;
	case OT_AddWall:
	case OT_IndependentWall:
	case OT_PackPipe:
	{
		ClassType = Class_AddWall;
	}
	break;
	case OT_CurtainBox:
	{
		ClassType = Class_Ceilinglayout;
		break;
	}
	case OT_RoomSpaceArea:
	case OT_BaseBoard:
	case OT_RectArea:
	case OT_WaterKnifeArea:
	case OT_PolygonArea:
	case OT_CircleArea:
	case OT_FreePolygonArea:// 自由多边形绘制区域
	case OT_BridgeStoneArea://过门石区域
	{
		TSharedPtr<FArmyBaseArea> Area = StaticCastSharedPtr<FArmyBaseArea>(InObj);
		if (Area.IsValid())
		{
			if (Area->SurfaceType == 0)
			{
				ClassType = Class_Floorlayout;
			}
			else if (Area->SurfaceType == 1)
			{
				ClassType = Class_Walllayout;
			}
			else if (Area->SurfaceType == 2)
			{
				FArmyRoomSpaceArea* RoomArea = InObj->AsassignObj< FArmyRoomSpaceArea >();
				if (RoomArea &&
					(RoomArea->GenerateFromObjectType == OT_Pass ||
						RoomArea->GenerateFromObjectType == OT_Door ||
						RoomArea->GenerateFromObjectType == OT_Window ||
						RoomArea->GenerateFromObjectType == OT_DoorHole))
				{
					ClassType = Class_Beam;
					break;
				}
				else
				{
					ClassType = Class_Ceilinglayout;
				}
			}
		}
	}
	break;
	case OT_Door:
		//case OT_SecurityDoor:
	case OT_SlidingDoor:
	{
		if (InType == 0)//0 self 2 label
		{
			//ClassType = Class_Door;
			ClassType = Class_Pass;
		}
		else if (InType == 2)
		{
			ClassType = Class_AnnotationDoor;
		}
	}
	break;
	case OT_Window:
	case OT_FloorWindow:
	case OT_RectBayWindow:
	case OT_TrapeBayWindow:
	case OT_CornerBayWindow:
	{
		if (InType == 0)
		{
			ClassType = Class_Window;
		}
		else if (InType == 2)
		{
			ClassType = Class_AnnotationWindow;
		}
	}
	break;
	case OT_SecurityDoor:
	{
		if (InType == 2)
		{
			ClassType = Class_AnnotationDoor;
			break;
		}
	}
	case OT_Pass:
	case OT_DoorHole:
	{
		ClassType = Class_Pass;
	}
	break;
	case OT_Dimensions:
	{
		TSharedPtr<FArmyDimensions> Dimensions = StaticCastSharedPtr<FArmyDimensions>(InObj);
		if (Dimensions.IsValid() && Dimensions->ClassType != -1)
		{
			ClassType = GetAnnotationClassTypeByObject(ObjectClassType(Dimensions->ClassType));
		}
	}
	break;
	case OT_InSideWallDimensions:
	{
		ClassType = Class_AnnotationInSide;
	}
	break;
	case OT_OutSideWallDimensions:
	{
		ClassType = Class_AnnotationOutSide;
	}
	break;
	case OT_AddWallDimensions:
	{
		ClassType = Class_AnnotationAddWall;
	}
	break;
	case OT_DeleteWallDimensions:
	{
		ClassType = Class_AnnotationDeleteWall;
	}
	break;
	case OT_LampControlLine:
	{
		ClassType = Class_Switch;
	}
	break;
	case OT_Symbol:
	case OT_CupboardTable:
	{
		ClassType = Class_SCTCabinet;
	}
	break;
	case OT_CabinetFace://柜体立面图
	case OT_DoorSheetFace://门板立面图
	case OT_CupboardTableFace://台面立面图
	case OT_SunBoardFace://见光板
	case OT_ResizeFace://调整板立面
	case OT_TopLineFace://上线条立面图
	case OT_FootLineFace://踢脚板立面图
	case OT_UserDefine:
	{
		TSharedPtr<FArmyCustomDefine> UserDefine = StaticCastSharedPtr<FArmyCustomDefine>(InObj);
		ClassType = ObjectClassType(UserDefine->GetBelongClass());
	}
	break;
	case OT_ComponentBase:
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(InObj);
		if (Furniture.IsValid())
		{
			ClassType = ObjectClassType(Furniture->BelongClass);
			if (InType == 2)
			{
				ClassType = GetAnnotationClassTypeByObject(ClassType);
			}
		}
	}
	break;

	case OT_Dewatering_Point://排水点位
	{
		ClassType = Class_WaterPipe;//  下水主管道
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_FloorDrain_Point://地漏
	case OT_Basin://地漏下水
	{
		ClassType = Class_FloordRainPoint;//地漏下水
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Closestool://马桶下水
	{
		ClassType = Class_ClosestoolRainPoint;//马桶下水
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Basin://水盆下水
	{
		ClassType = Class_WashBasinRainPoint;//水盆下水
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Supply: // 给水点位,
	{
		ClassType = Class_HomeEntryWaterPoint;//进户水
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Separator_Point://分集水器
	{
		ClassType = Class_DiversityWater;//  分集水器
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Hot_Point://热水点位
	{
		ClassType = Class_HotWaterPoint;//热水点位
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Chilled_Point://冷水点位
	{
		ClassType = Class_ColdWaterPoint;//冷水点位
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Hot_Chilled_Point://冷熱水
	{
		ClassType = Class_HotAndColdWaterPoint;//冷热水点位
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Normal_Point://中水
	{
		ClassType = Class_RecycledWaterPoint;//中水点位
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_Water_Point:
	{
		ClassType = Class_WaterRoutePoint;
		if (InType == 2)
		{
			ClassType = GetAnnotationClassTypeByObject(ClassType);
		}
	}
		break;
	case OT_StrongElectricity_25:
	case OT_StrongElectricity_4:
	case OT_StrongElectricity_Single:
	case OT_StrongElectricity_Double:
	case OT_WeakElectricity_TV:
	case OT_WeakElectricity_Net:
	case OT_WeakElectricity_Phone:
	case OT_StrongElectricity_25_Linker:
	case OT_StrongElectricity_4_Linker:
	case OT_StrongElectricity_Single_Linker:
	case OT_StrongElectricity_Double_Linker:
	case OT_WeakElectricity_TV_Linker:
	case OT_WeakElectricity_Net_Linker:
	case OT_WeakElectricity_Phone_Linker:
	{
		ClassType = Class_ElectricWire;
	}
	break;
	case OT_DownLeadLabel:
	{
		FArmyDownLeadLabel* Label = InObj->AsassignObj<FArmyDownLeadLabel>();
		FObjectPtr BindObj;
		int32 ObjTag = -1;
		Label->GetObjTag(BindObj, ObjTag);
		if (BindObj.IsValid())
		{
			ClassType = GetAnnotationClassTypeByObject(GetClassTypeByObject(BindObj));
		}
		break;
	}
	break;
	case OT_TextLabel:
	{
		FArmyTextLabel* TextLabel = InObj->AsassignObj<FArmyTextLabel>();
		switch (TextLabel->GetLabelType())
		{
		case FArmyTextLabel::LT_None:
		{
			FObjectPtr BindObj;
			int32 ObjTag = -1;
			TextLabel->GetObjTag(BindObj, ObjTag);
			if (BindObj.IsValid())
			{
				ClassType = GetClassTypeByObject(BindObj, 2);
			}
			break;
		}
		case FArmyTextLabel::LT_SpaceHeight:
		{
			ClassType = Class_SpaceHeight;
			break;
		}
		case FArmyTextLabel::LT_BaseGroundHeight:
		{
			ClassType = Class_BaseGroundHeight;
			break;
		}
		case FArmyTextLabel::LT_PostGroundHeight:
		{
			ClassType = Class_PostGroundHeight;
			break;
		}
		case FArmyTextLabel::LT_SpaceName:
		{
			ClassType = Class_SpaceName;
			break;
		}
		case FArmyTextLabel::LT_SpaceArea:
		{
			ClassType = Class_SpaceArea;
			break;
		}
		case FArmyTextLabel::LT_SpacePerimeter:
		{
			ClassType = Class_SpacePerimeter;
			break;
		}
		case FArmyTextLabel::LT_Beam:
		{
			ClassType = Class_AnnotationBeam;
			break;
		}
		default:
			break;
		}
	}
	break;
	case OT_ConstructionFrame:
	case OT_Compass:
	{
		ClassType = Class_ConstructionFrame;
	}
	break;
	default:
		break;
	}
	return ClassType;
}

void FArmyConstructionFunction::GenerateAutoDimensions()
{
	//默认从原始户型中获取数据
	auto GetOtherInnerArray = [this](TArray<PointInfo>& TopArray, TArray<PointInfo>& BottomArray, TArray<PointInfo>& LeftArray, TArray<PointInfo>& RightArray, EModelType ModelType = E_HomeModel)->TArray<InnerLineInfo> {
		TArray<FObjectWeakPtr> RoomList;
		FArmySceneData::Get()->GetObjects(ModelType, OT_InternalRoom, RoomList);
		FArmySceneData::Get()->GetObjects(ModelType, OT_OutRoom, RoomList);

		TArray<TSharedPtr<FArmyLine>> AllRoomLines;
		//TArray<TSharedPtr<FArmyLine>> InnerRoomLines;
		TArray<PointInfo> AllRoomPoints;
		TArray<InnerLineInfo> OtherInerArray;
		BoundBox = FBox(ForceInitToZero);
		for (auto ObjIt : RoomList)
		{
			BoundBox += ObjIt.Pin()->GetBounds();

			if (ObjIt.Pin()->GetType() == OT_InternalRoom)//外墙不参与尺寸标注
			{
				TArray<TSharedPtr<FArmyLine>> TempLines;
				ObjIt.Pin()->GetLines(TempLines);
				AllRoomLines.Append(TempLines);

				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(ObjIt.Pin());
				//InnerRoomLines.Append(TempLines);
				int32 PointType = 2;
				for (auto L : TempLines)
				{
					AllRoomPoints.AddUnique(PointInfo(PointType, L->GetStart(), Room));
					AllRoomPoints.AddUnique(PointInfo(PointType, L->GetEnd(), Room));
					//#ifndef DISCARD_REPEAT
					InnerLineInfo LInfo;
					LInfo.MainLine = L;
					LInfo.RelationInnerRoom = Room;
					LInfo.DetailPoints.AddUnique(L->GetStart());
					LInfo.DetailPoints.AddUnique(L->GetEnd());
					OtherInerArray.AddUnique(LInfo);
					//#endif // ! DISCARD_REPEAT
				}
			}
		}

		//户型结构
		TArray<FObjectWeakPtr> PillarList;
		//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Beam, PillarList);//梁
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, PillarList);//柱子
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, PillarList);//风道

		for (auto ObjIt : PillarList)
		{
			TArray< TSharedPtr<FArmyLine> > PillarInnerLineArray;
			ObjIt.Pin()->GetLines(PillarInnerLineArray);

			//筛选出梁、柱子、风道中多余的线段并将同一直线上的直线合并成一条
			TArray< TSharedPtr<FArmyLine> > TargetPillarInnerLineArray;
			TArray< TSharedPtr<FArmyLine> > RemovePillarInnerLineArray;
			for (TSharedPtr<FArmyLine> LineIt : PillarInnerLineArray)
			{
				//如果该线条是在RemovePillarInnerLineArray中，则退出
				if (RemovePillarInnerLineArray.Contains(LineIt))
				{
					continue;
				}

				for (TSharedPtr<FArmyLine> TempLineIt : PillarInnerLineArray)
				{
					//是自身线条，Continue
					if ((LineIt->GetStart() - TempLineIt->GetStart()).Size() < 0.001 &&
						(LineIt->GetEnd() - TempLineIt->GetEnd()).Size() < 0.001)
					{
						continue;
					}

					//先找出两条线中相同的点
					//取另外的两个点进行相连，并合并成一条直线存储在TargetPillarInnerLineArray
					if ((LineIt->GetStart() - TempLineIt->GetStart()).Size() < 0.001)
					{
						if (FMath::Abs(FVector::Distance(LineIt->GetEnd(), TempLineIt->GetEnd()) - (FVector::Distance(LineIt->GetStart(), LineIt->GetEnd()) + FVector::Distance(TempLineIt->GetStart(), TempLineIt->GetEnd()))) < 0.001)
						{
							RemovePillarInnerLineArray.AddUnique(LineIt);
							RemovePillarInnerLineArray.AddUnique(TempLineIt);

							//合并成一条直线
							TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(LineIt->GetEnd(), TempLineIt->GetEnd()));
							TargetPillarInnerLineArray.Emplace(Line);
							break;
						}
					}
					if ((LineIt->GetStart() - TempLineIt->GetEnd()).Size() < 0.001)
					{
						if (FMath::Abs(FVector::Distance(LineIt->GetEnd(), TempLineIt->GetStart()) - (FVector::Distance(LineIt->GetStart(), LineIt->GetEnd()) + FVector::Distance(TempLineIt->GetStart(), TempLineIt->GetEnd()))) < 0.001)
						{
							RemovePillarInnerLineArray.AddUnique(LineIt);
							RemovePillarInnerLineArray.AddUnique(TempLineIt);

							TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(LineIt->GetEnd(), TempLineIt->GetStart()));
							TargetPillarInnerLineArray.Emplace(Line);
							break;
						}
					}
					if ((LineIt->GetEnd() - TempLineIt->GetStart()).Size() < 0.001)
					{
						if (FMath::Abs(FVector::Distance(LineIt->GetStart(), TempLineIt->GetEnd()) - (FVector::Distance(LineIt->GetStart(), LineIt->GetEnd()) + FVector::Distance(TempLineIt->GetStart(), TempLineIt->GetEnd()))) < 0.001)
						{
							RemovePillarInnerLineArray.AddUnique(LineIt);
							RemovePillarInnerLineArray.AddUnique(TempLineIt);

							TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(LineIt->GetStart(), TempLineIt->GetEnd()));
							TargetPillarInnerLineArray.Emplace(Line);
							break;
						}
					}
					if ((LineIt->GetEnd() - TempLineIt->GetEnd()).Size() < 0.001)
					{
						if (FMath::Abs(FVector::Distance(LineIt->GetStart(), TempLineIt->GetStart()) - (FVector::Distance(LineIt->GetStart(), LineIt->GetEnd()) + FVector::Distance(TempLineIt->GetStart(), TempLineIt->GetEnd()))) < 0.001)
						{
							RemovePillarInnerLineArray.AddUnique(LineIt);
							RemovePillarInnerLineArray.AddUnique(TempLineIt);

							TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(LineIt->GetStart(), TempLineIt->GetStart()));
							TargetPillarInnerLineArray.Emplace(Line);
							break;
						}
					}
				}
			}

			TArray<TSharedPtr<FArmyLine>> RemoveInerArray;
			for (auto & LInfo : OtherInerArray)//inner room line
			{
				FVector LStart = LInfo.MainLine->GetStart();
				FVector LEnd = LInfo.MainLine->GetEnd();
				LStart.Z = 0;
				LEnd.Z = 0;

				for (auto L : TargetPillarInnerLineArray)
				{
					FVector ClosestToStartPos = FMath::ClosestPointOnLine(LStart, LEnd, L->GetStart());
					FVector ClosestToEndPos = FMath::ClosestPointOnLine(LStart, LEnd, L->GetEnd());
					bool bStart = (ClosestToStartPos - L->GetStart()).Size() < 0.1f;
					bool bEnd = (ClosestToEndPos - L->GetEnd()).Size() < 0.1f;
					if (bStart && bEnd)
					{
						LInfo.DetailPoints.AddUnique(ClosestToStartPos);
						LInfo.DetailPoints.AddUnique(ClosestToEndPos);
						LInfo.IgoneLines.AddUnique(TPair<FVector, FVector>(ClosestToStartPos, ClosestToEndPos));

						AllRoomPoints.AddUnique(PointInfo(3, ClosestToStartPos, LInfo.RelationInnerRoom));
						AllRoomPoints.AddUnique(PointInfo(3, ClosestToEndPos, LInfo.RelationInnerRoom));

						RemoveInerArray.AddUnique(L);
					}
				}
			}
			TArray<InnerLineInfo> AddInnerLineInfoArray;
			for (auto TargetPillarInnerLine : TargetPillarInnerLineArray)
			{
				if (!RemoveInerArray.Contains(TargetPillarInnerLine))
				{
					InnerLineInfo mLInfo;
					mLInfo.MainLine = TargetPillarInnerLine;
					mLInfo.DetailPoints.AddUnique(TargetPillarInnerLine->GetStart());
					mLInfo.DetailPoints.AddUnique(TargetPillarInnerLine->GetEnd());
					mLInfo.RelationPillar = ObjIt.Pin();
					TArray<FVector> Vertices;
					ObjIt.Pin()->GetVertexes(Vertices);
					mLInfo.ComponentCenter = FBox(Vertices).GetCenter();
					AddInnerLineInfoArray.AddUnique(mLInfo);
				}
			}
			OtherInerArray.Append(AddInnerLineInfoArray);
		}

		TArray<TWeakPtr<FArmyHardware>> DetailObjects;
		FArmySceneData::Get()->GetAllHardWare(DetailObjects, ModelType);
		for (auto ObjIt : DetailObjects)
		{
			if (!ObjIt.IsValid()) continue;
			TSharedPtr<FArmyHardware> Obj = ObjIt.Pin();
			//如果该门窗洞在新建墙上，则在原始图上不展示出该门窗洞的标注
			if (Obj->InWallType == 1) continue;

			// @欧石楠 遇到在此处有空指针异常，请查找原因
			// 			if (Obj.IsValid())
			// 			{
			// 				switch (Obj->GetType())
			// 				{
			// 				case OT_Door:
			// 				case OT_SlidingDoor:
			// 				//case OT_SecurityDoor:
			// 					continue;
			// 					break;
			// 				default:
			// 					break;
			// 				}
			// 			}

			BoundBox += ObjIt.Pin()->GetBounds();

			int32 PointType = 3;

			FVector PStart = Obj->GetStartPos();
			FVector PEnd = Obj->GetEndPos();
			if (ModelType == E_LayoutModel && Obj->GetType() == OT_DoorHole)
			{
				FArmyPass* Pass = Obj->AsassignObj<FArmyPass>();
				if (Pass)
				{
					PStart = Pass->GetLastStart();
					PEnd = Pass->GetLastEnd();
				}
			}

			float W = Obj->GetWidth();
			FVector HardWareDir = (PEnd - PStart).GetSafeNormal();
			FVector WDir(-HardWareDir.Y, HardWareDir.X, 0);

			FVector StartNearP, EndNearP;
			for (auto & LInfo : OtherInerArray)//inner room line
			{
				float StartDis1 = FMath::PointDistToSegment(PStart + WDir * W / 2, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd());
				float EndDis1 = FMath::PointDistToSegment(PEnd + WDir * W / 2, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd());

				float StartDis2 = FMath::PointDistToSegment(PStart - WDir * W / 2, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd());
				float EndDis2 = FMath::PointDistToSegment(PEnd - WDir * W / 2, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd());

				if ((StartDis1 < 0.001 && EndDis1 < 0.001) || (StartDis2 < 0.001 && EndDis2 < 0.001))
				{
					StartNearP = FMath::ClosestPointOnLine(LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd(), PStart);
					EndNearP = FMath::ClosestPointOnLine(LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd(), PEnd);

					float MStartToObjStart = (LInfo.MainLine->GetStart() - StartNearP).Size();
					float MStartToObjEnd = (LInfo.MainLine->GetStart() - EndNearP).Size();

					LInfo.DetailPoints.AddUnique(StartNearP);
					LInfo.DetailPoints.AddUnique(EndNearP);

					AllRoomPoints.AddUnique(PointInfo(PointType, StartNearP, LInfo.RelationInnerRoom));
					AllRoomPoints.AddUnique(PointInfo(PointType, EndNearP, LInfo.RelationInnerRoom));
				}
			}
		}
		TY = BoundBox.Min.Y;
		BY = BoundBox.Max.Y;
		LX = BoundBox.Min.X;
		RX = BoundBox.Max.X;

		for (auto & P : AllRoomPoints)
		{
			FVector TP(P.Point - FVector(0, BoundBox.GetSize().Size(), 0));//top
			FVector BP(P.Point + FVector(0, BoundBox.GetSize().Size(), 0));//bottom
			FVector LP(P.Point - FVector(BoundBox.GetSize().Size(), 0, 0));//left
			FVector RP(P.Point + FVector(BoundBox.GetSize().Size(), 0, 0));//right

			int32 JudgeCount = P.StepType == 3 ? 2 : 3;

			for (auto & L : AllRoomLines)
			{
				FVector LDir = (L->GetEnd() - L->GetStart()).GetSafeNormal();
				//此处*6的意思是，为了去除其他内墙线在当前墙体内延伸出来的射线（去除标尺中的冗余小尺寸）
				FVector InterPos;
				if (P.TopInterCount < JudgeCount)
				{
					if (FMath::PointDistToSegment(P.Point, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6) < 0.001 || FArmyMath::CalculateLinesIntersection(P.Point, TP, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6, InterPos))
					{
						P.TopInterCount++;
					}
				}
				if (P.BottomInterCount < JudgeCount)
				{
					if (FMath::PointDistToSegment(P.Point, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6) < 0.001 || FArmyMath::CalculateLinesIntersection(P.Point, BP, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6, InterPos))
					{
						P.BottomInterCount++;
					}
				}
				if (P.LeftInterCount < JudgeCount)
				{
					if (FMath::PointDistToSegment(P.Point, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6) < 0.001 || FArmyMath::CalculateLinesIntersection(P.Point, LP, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6, InterPos))
					{
						P.LeftInterCount++;
					}
				}
				if (P.RightInterCount < JudgeCount)
				{
					if (FMath::PointDistToSegment(P.Point, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6) < 0.001 || FArmyMath::CalculateLinesIntersection(P.Point, RP, L->GetStart() - LDir * 6, L->GetEnd() + LDir * 6, InterPos))
					{
						P.RightInterCount++;
					}
				}
				if (P.TopInterCount > JudgeCount - 1 && P.BottomInterCount > JudgeCount - 1 && P.LeftInterCount > JudgeCount - 1 && P.RightInterCount > JudgeCount - 1)
				{
					//#ifdef DISCARD_REPEAT
					//				if (P.RelationInnerRoom.IsValid())
					//				{
					//					OtherInerArray.AddUnique(P);
					//				}
					//#endif // DISCARD_REPEAT
					break;
				}
			}
			if (P.TopInterCount < JudgeCount)
			{
				TopArray.AddUnique(PointInfo(P.StepType, P.Point, P.RelationInnerRoom));
			}
			if (P.BottomInterCount < JudgeCount)
			{
				BottomArray.AddUnique(PointInfo(P.StepType, P.Point, P.RelationInnerRoom));
			}
			if (P.LeftInterCount < JudgeCount)
			{
				LeftArray.AddUnique(PointInfo(P.StepType, P.Point, P.RelationInnerRoom));
			}
			if (P.RightInterCount < JudgeCount)
			{
				RightArray.AddUnique(PointInfo(P.StepType, P.Point, P.RelationInnerRoom));
			}
		}

		TopArray.Sort([&](const PointInfo& A, const PointInfo& B) {
			return A.Point.X < B.Point.X;
		});
		BottomArray.Sort([&](const PointInfo& A, const PointInfo& B) {
			return A.Point.X < B.Point.X;
		});
		LeftArray.Sort([&](const PointInfo& A, const PointInfo& B) {
			return A.Point.Y < B.Point.Y;
		});
		RightArray.Sort([&](const PointInfo& A, const PointInfo& B) {
			return A.Point.Y < B.Point.Y;
		});

		return MoveTemp(OtherInerArray);
	};

	TArray<InnerLineInfo> HomeOtherInerArray;  //原始数据
	TArray<InnerLineInfo> LayoutOtherInerArray;//拆改后的数据
	TopArray.Empty();
	BottomArray.Empty();
	LeftArray.Empty();
	RightArray.Empty();
	HomeOtherInerArray = { GetOtherInnerArray(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_HomeModel) };
	//原始数据标记内墙点（筛选出冗余的内墙点）
	SignInnerPoint(TopArray);
	SignInnerPoint(BottomArray);
	SignInnerPoint(LeftArray);
	SignInnerPoint(RightArray);

	//原始数据生成三层标尺
	GenerateStep3Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_HomeModel);
	GenerateStep2Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_HomeModel);
	GenerateStep1Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_HomeModel);

	TopArray.Empty();
	BottomArray.Empty();
	LeftArray.Empty();
	RightArray.Empty();
	LayoutOtherInerArray = { GetOtherInnerArray(TopArray, BottomArray, LeftArray, RightArray,EModelType::E_LayoutModel) };
	//拆改后数据标记内墙点（筛选出冗余的内墙点）
	SignInnerPoint(TopArray);
	SignInnerPoint(BottomArray);
	SignInnerPoint(LeftArray);
	SignInnerPoint(RightArray);

	//拆改后数据生成三层标尺
	GenerateStep3Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_LayoutModel);
	GenerateStep2Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_LayoutModel);
	GenerateStep1Dimensions(TopArray, BottomArray, LeftArray, RightArray, EModelType::E_LayoutModel);

	GenerateOtherInnerDimensions(HomeOtherInerArray);						//内墙标注使用原始户型数据
	GenerateModifyWallDimensions(HomeOtherInerArray, LayoutOtherInerArray); //新增墙使用拆改后的数据
	GenerateAutoObjectDimensions(LayoutOtherInerArray);						//使用拆改后的数据
	GenerateCeilingDimensions(LayoutOtherInerArray);						//使用拆改后的数据
}
//筛选出冗余的内墙点
void FArmyConstructionFunction::SignInnerPoint(TArray<FArmyConstructionFunction::PointInfo>& PointInfoList) const
{
	for (int32 i = 1; i < PointInfoList.Num() - 1; ++i)//注意这里从1开始索引
	{
		if (/*PointInfoList[i].RelationInnerRoom.IsValid()&&
			PointInfoList[i-1].RelationInnerRoom.IsValid()&&
			PointInfoList[i+1].RelationInnerRoom.IsValid()&&*/
			PointInfoList[i].RelationInnerRoom != PointInfoList[i - 1].RelationInnerRoom&&
			PointInfoList[i].RelationInnerRoom != PointInfoList[i + 1].RelationInnerRoom)
		{
			//该点是内墙点
			PointInfoList[i].bInnerPoint = true;
		}
	}
}
void FArmyConstructionFunction::GenerateStep3Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType /*= E_HomeModel*/)
{
	FString DismensionArgString = ModelType == E_HomeModel ? TEXT("_ORIGIN") : TEXT("_LAYOUT");

	float TY3 = TY - ThirdDimensionOffset;
	float BY3 = BY + ThirdDimensionOffset;
	float LX3 = LX - ThirdDimensionOffset;
	float RX3 = RX + ThirdDimensionOffset;

	for (int32 i = 0; i < TopArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(TopArray[i].Point.X - TopArray[i + 1].Point.X) < 0.1) continue;
		if (TopArray[i].bInnerPoint || TopArray[i + 1].bInnerPoint)
		{
			if (TopArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TopArray[StartIndex].Point, TopArray[EndIndex].Point,
			FVector(TopArray[StartIndex].Point.X, TY3, 0), FVector(TopArray[EndIndex].Point.X, TY3, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < BottomArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(BottomArray[i].Point.X - BottomArray[i + 1].Point.X) < 0.1) continue;
		if (BottomArray[i].bInnerPoint || BottomArray[i + 1].bInnerPoint)
		{
			if (BottomArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, BottomArray[StartIndex].Point, BottomArray[EndIndex].Point,
			FVector(BottomArray[StartIndex].Point.X, BY3, 0), FVector(BottomArray[EndIndex].Point.X, BY3, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < LeftArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(LeftArray[i].Point.Y - LeftArray[i + 1].Point.Y) < 0.1) continue;
		if (LeftArray[i].bInnerPoint || LeftArray[i + 1].bInnerPoint)
		{
			if (LeftArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, LeftArray[StartIndex].Point, LeftArray[EndIndex].Point,
			FVector(LX3, LeftArray[StartIndex].Point.Y, 0), FVector(LX3, LeftArray[EndIndex].Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < RightArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(RightArray[i].Point.Y - RightArray[i + 1].Point.Y) < 0.1) continue;
		if (RightArray[i].bInnerPoint || RightArray[i + 1].bInnerPoint)
		{
			if (RightArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, RightArray[StartIndex].Point, RightArray[EndIndex].Point,
			FVector(RX3, RightArray[StartIndex].Point.Y, 0), FVector(RX3, RightArray[EndIndex].Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}
}
void FArmyConstructionFunction::GenerateStep2Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType /*= E_HomeModel*/)
{
	FString DismensionArgString = ModelType == E_HomeModel ? TEXT("_ORIGIN") : TEXT("_LAYOUT");

	float TY2 = TY - SecondDimensionOffset;
	float BY2 = BY + SecondDimensionOffset;
	float LX2 = LX - SecondDimensionOffset;
	float RX2 = RX + SecondDimensionOffset;

	TArray<PointInfo> TempTopArray, TempBottomArray, TempLeftArray, TempRightArray;
	for (int32 i = 0; i < TopArray.Num(); ++i)
	{
		if (TopArray[i].StepType != 3)
		{
			TempTopArray.AddUnique(TopArray[i]);
		}
	}
	for (int32 i = 0; i < BottomArray.Num(); ++i)
	{
		if (BottomArray[i].StepType != 3)
		{
			TempBottomArray.AddUnique(BottomArray[i]);
		}
	}
	for (int32 i = 0; i < LeftArray.Num(); ++i)
	{
		if (LeftArray[i].StepType != 3)
		{
			TempLeftArray.AddUnique(LeftArray[i]);
		}
	}
	for (int32 i = 0; i < RightArray.Num(); ++i)
	{
		if (RightArray[i].StepType != 3)
		{
			TempRightArray.AddUnique(RightArray[i]);
		}
	}

	for (int32 i = 0; i < TempTopArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(TempTopArray[i].Point.X - TempTopArray[i + 1].Point.X) < 0.001) continue;
		if (TempTopArray[i].bInnerPoint || TempTopArray[i + 1].bInnerPoint)
		{
			if (TempTopArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TempTopArray[StartIndex].Point, TempTopArray[EndIndex].Point,
			FVector(TempTopArray[StartIndex].Point.X, TY2, 0), FVector(TempTopArray[EndIndex].Point.X, TY2, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < TempBottomArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(TempBottomArray[i].Point.X - TempBottomArray[i + 1].Point.X) < 0.001) continue;
		if (TempBottomArray[i].bInnerPoint || TempBottomArray[i + 1].bInnerPoint)
		{
			if (TempBottomArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TempBottomArray[StartIndex].Point, TempBottomArray[EndIndex].Point,
			FVector(TempBottomArray[StartIndex].Point.X, BY2, 0), FVector(TempBottomArray[EndIndex].Point.X, BY2, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < TempLeftArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(TempLeftArray[i].Point.Y - TempLeftArray[i + 1].Point.Y) < 0.001) continue;
		if (TempLeftArray[i].bInnerPoint || TempLeftArray[i + 1].bInnerPoint)
		{
			if (TempLeftArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TempLeftArray[StartIndex].Point, TempLeftArray[EndIndex].Point,
			FVector(LX2, TempLeftArray[StartIndex].Point.Y, 0), FVector(LX2, TempLeftArray[EndIndex].Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	for (int32 i = 0; i < TempRightArray.Num() - 1; ++i)
	{
		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		if (FMath::Abs(TempRightArray[i].Point.Y - TempRightArray[i + 1].Point.Y) < 0.001) continue;
		if (TempRightArray[i].bInnerPoint || TempRightArray[i + 1].bInnerPoint)
		{
			if (TempRightArray[i].bInnerPoint)
			{
				//画出内墙点两侧的线段
				StartIndex = i - 1;
				EndIndex = i + 1;
			}
			else
			{
				continue;
			}
		}

		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TempRightArray[StartIndex].Point, TempRightArray[EndIndex].Point,
			FVector(RX2, TempRightArray[StartIndex].Point.Y, 0), FVector(RX2, TempRightArray[EndIndex].Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}
}
void FArmyConstructionFunction::GenerateStep1Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType /*= E_HomeModel*/)
{
	FString DismensionArgString = ModelType == E_HomeModel ? TEXT("_ORIGIN") : TEXT("_LAYOUT");

	float TY1 = TY - FirstDimensionOffset;
	float BY1 = BY + FirstDimensionOffset;
	float LX1 = LX - FirstDimensionOffset;
	float RX1 = RX + FirstDimensionOffset;

	if (TopArray.Num() > 1)
	{
		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, TopArray.Top().Point, TopArray.HeapTop().Point,
			FVector(TopArray.Top().Point.X, TY1, 0), FVector(TopArray.HeapTop().Point.X, TY1, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	if (BottomArray.Num() > 1)
	{
		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, BottomArray.Top().Point, BottomArray.HeapTop().Point,
			FVector(BottomArray.Top().Point.X, BY1, 0), FVector(BottomArray.HeapTop().Point.X, BY1, 0), OT_OutSideWallDimensions, 10, 1, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	if (LeftArray.Num() > 1)
	{
		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, LeftArray.Top().Point, LeftArray.HeapTop().Point,
			FVector(LX1, LeftArray.Top().Point.Y, 0), FVector(LX1, LeftArray.HeapTop().Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}

	if (RightArray.Num() > 1)
	{
		TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationOutSide, RightArray.Top().Point, RightArray.HeapTop().Point,
			FVector(RX1, RightArray.Top().Point.Y, 0), FVector(RX1, RightArray.HeapTop().Point.Y, 0), OT_OutSideWallDimensions, 10, 2, true);
		if (CurrentDimensions.IsValid())
		{
			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO") + DismensionArgString).ArgUint32(E_ConstructionModel));
		}
	}
}
void FArmyConstructionFunction::GenerateOtherInnerDimensions(TArray<InnerLineInfo>& OtherArray)
{
	TArray<TSharedPtr<FArmyLine>> UseLines;
	for (auto & LInfo : OtherArray)
	{
		LInfo.SortPoints();

		//遍历细节顶点
		for (int32 i = 0; i < LInfo.DetailPoints.Num() - 1; ++i)
		{
			if ((LInfo.DetailPoints[i] - LInfo.DetailPoints[i + 1]).Size() < 0.1) continue;

			//排除需要忽略的线段
			TPair<FVector, FVector> P1(LInfo.DetailPoints[i], LInfo.DetailPoints[i + 1]);
			TPair<FVector, FVector> P2(LInfo.DetailPoints[i + 1], LInfo.DetailPoints[i]);

			if (LInfo.CompareIgone(P1) || LInfo.CompareIgone(P2)) continue;//包含忽略标注的两个点，则不进行标尺标注

			FVector LDir = (LInfo.DetailPoints[i + 1] - LInfo.DetailPoints[i]).GetSafeNormal();
			FVector OneSide = FRotator(0, 90, 0).RotateVector(LDir);
			OneSide.Normalize();

			if (LInfo.RelationInnerRoom.IsValid())
			{
				FVector LCenter = (LInfo.DetailPoints[i + 1] + LInfo.DetailPoints[i]) / 2;
				if (!LInfo.RelationInnerRoom->IsPointInRoom(LCenter + OneSide))
				{
					OneSide *= -1;
				}
			}
			else
			{
				FVector DirPos = FMath::ClosestPointOnInfiniteLine(LInfo.DetailPoints[i], LInfo.DetailPoints[i + 1], LInfo.ComponentCenter);
				OneSide = (DirPos - LInfo.ComponentCenter).GetSafeNormal();
			}

			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationInSide, LInfo.DetailPoints[i], LInfo.DetailPoints[i + 1],
				LInfo.DetailPoints[i] + OneSide * OffsetValue, LInfo.DetailPoints[i + 1] + OneSide * OffsetValue, OT_InSideWallDimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
	}
}
void FArmyConstructionFunction::GenerateModifyWallDimensions(const TArray<InnerLineInfo>& InInnerRoomOriginLines, const TArray<InnerLineInfo>& InInnerRoomLayoutLines)
{
	TArray<FObjectWeakPtr> AddWalls, ModifyWalls;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWalls);//新增墙
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWalls);//独立墙
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWalls);//拆除墙

																			  //拆除墙尺寸标注
	for (auto Obj : ModifyWalls)
	{
		TSharedPtr<FArmyModifyWall> W = StaticCastSharedPtr<FArmyModifyWall>(Obj.Pin());
		TSharedPtr<FArmyDimensions> CurrentDimensions = NULL;
		FVector WallCenter = W->GetBounds().GetCenter();
		WallCenter.Z = 0;
		FVector WDir = W->GetDir();
		WDir.Z = 0;
		WDir.Normalize();
		for (auto L : W->GetBorderLines())
		{
			FVector LStart = L->GetStart();
			FVector LEnd = L->GetEnd();
			LStart.Z = 0;
			LEnd.Z = 0;
			FVector LDir = (LEnd - LStart).GetSafeNormal();
			if ((LDir - WDir).Size() < 0.001 || (LDir + WDir).Size() < 0.001)
			{
				FVector TempPoint = FMath::ClosestPointOnInfiniteLine(LStart, LEnd, WallCenter);
				FVector Offset = (TempPoint - WallCenter).GetSafeNormal() * OffsetValue;

				if ((LStart - LEnd).Size() < 0.001) continue;

				/*TSharedPtr<FArmyDimensions> */CurrentDimensions = CreateObjectDismension(Class_AnnotationDeleteWall, LStart, LEnd,
					LStart + Offset, LEnd + Offset, OT_DeleteWallDimensions);
				if (CurrentDimensions.IsValid())
				{
					FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
				}
				break;
			}
		}

		if (CurrentDimensions.IsValid())
		{
			FVector LeftPos = CurrentDimensions->LeftStaticPoint->GetPos();
			FVector RightPos = CurrentDimensions->RightStaticPoint->GetPos();

			FVector LeftDir = (LeftPos - RightPos).GetSafeNormal();
			for (auto L : InInnerRoomOriginLines)
			{
				bool OnLine = FMath::PointDistToSegment(LeftPos, L.MainLine->GetStart(), L.MainLine->GetEnd()) < 0.001;
				if (OnLine && L.MainLine.IsValid() && (L.MainLine == W->GetWallCaptureInfo().FirstLine || L.MainLine == W->GetWallCaptureInfo().SecondLine))
				{
					float LeftDis = -1;
					FVector LeftNearest = LeftPos;
					for (auto & V : L.DetailPoints)
					{
						FVector LeftOffset = V - LeftPos;
						if ((LeftOffset.GetSafeNormal() - LeftDir).Size() < 0.001)
						{
							if (LeftDis < 0 || LeftOffset.Size() < LeftDis)
							{
								LeftNearest = V;
								LeftDis = LeftOffset.Size();
							}
						}
					}
					if (LeftNearest != LeftPos)
					{
						FVector Offset = (CurrentDimensions->LeftExtentPoint->GetPos() - LeftPos);

						if ((LeftNearest - LeftPos).Size() < 0.001) continue;

						TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationDeleteWall, LeftNearest, LeftPos,
							LeftNearest + Offset, LeftPos + Offset, OT_DeleteWallDimensions);
						if (CurrentDimensions.IsValid())
						{
							FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
						}
					}
					break;
				}
			}
		}
	}

	TArray<FObjectWeakPtr> PassArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, PassArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_DoorHole, PassArray);
	//门洞与入口标注
	for (auto Obj : PassArray)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(Obj.Pin());
		const TSharedPtr<FArmyRect> LeftAddWall = Pass->GetLeftAddWall();
		if (LeftAddWall.IsValid() && LeftAddWall->Width > 0)
		{
			FVector LStart = LeftAddWall->Pos;
			FVector LEnd = LeftAddWall->Pos + LeftAddWall->XDirection * LeftAddWall->Width;

			FVector Offset = -LeftAddWall->YDirection * OffsetValue;
			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, LStart, LEnd,
				LStart + Offset, LEnd + Offset, OT_AddWallDimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
		const TSharedPtr<FArmyRect> RightAddWall = Pass->GetRightAddWall();
		if (RightAddWall.IsValid() && RightAddWall->Width > 0)
		{
			FVector LStart = RightAddWall->Pos;
			FVector LEnd = RightAddWall->Pos + RightAddWall->XDirection * RightAddWall->Width;

			FVector Offset = -RightAddWall->YDirection * OffsetValue;
			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, LStart, LEnd,
				LStart + Offset, LEnd + Offset, OT_AddWallDimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
		const TSharedPtr<FArmyRect> LeftDeleteWall = Pass->GetLeftDeleteWall();
		if (LeftDeleteWall.IsValid() && LeftDeleteWall->Width > 0)
		{
			FVector LStart = LeftDeleteWall->Pos;
			FVector LEnd = LeftDeleteWall->Pos + LeftDeleteWall->XDirection * LeftDeleteWall->Width;

			FVector Offset = -LeftDeleteWall->YDirection * OffsetValue;

			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationDeleteWall, LStart, LEnd,
				LStart + Offset, LEnd + Offset, OT_DeleteWallDimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
		const TSharedPtr<FArmyRect> RightDeleteWall = Pass->GetRightDeleteWall();
		if (RightDeleteWall.IsValid() && RightDeleteWall->Width > 0)
		{
			FVector LStart = RightDeleteWall->Pos;
			FVector LEnd = RightDeleteWall->Pos + RightDeleteWall->XDirection * RightDeleteWall->Width;

			FVector Offset = -RightDeleteWall->YDirection * OffsetValue;

			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationDeleteWall, LStart, LEnd,
				LStart + Offset, LEnd + Offset, OT_DeleteWallDimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
	}
	struct NearPointInfo
	{
		NearPointInfo(FVector& InP) :BasePoint(InP) {}
		FVector BasePoint = FVector(ForceInitToZero);
		TArray<FVector> TargetPointArray;
		float Flag = 0;//用来做辅助判断（目前用最近两点距离的乘积来判断）

		void SortTargetPoint()
		{
			TargetPointArray.Sort([&](const FVector& A, const FVector& B)
			{
				return (BasePoint - A).Size() < (BasePoint - B).Size();
			});
		}
		void UpdateFlag()
		{
			if (TargetPointArray.Num() > 1)
			{
				Flag = (TargetPointArray[0] - BasePoint).Size() + (TargetPointArray[1] - BasePoint).Size();
			}
			else if (TargetPointArray.Num() == 1)
			{
				Flag = (TargetPointArray[0] - BasePoint).Size();
			}
		}

		bool operator==(const NearPointInfo& InOther) const
		{
			return BasePoint == InOther.BasePoint;
		}
	};

	//获取拆改后的户型数据
	TArray<TWeakPtr<FArmyHardware>> DetailObjects;
	FArmySceneData::Get()->GetAllHardWare(DetailObjects, EModelType::E_LayoutModel);
	//新建墙标注
	for (auto Obj : AddWalls)
	{
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(Obj.Pin());

		TArray<FVector> OutVertexes;
		AddWall->GetVertexes(OutVertexes);

		TSharedPtr<FArmyLine> OnWallLine = NULL;
		InnerLineInfo TheOnWall;

		TArray<TSharedPtr<FArmyLine>> WLines;
		TArray<InnerLineInfo> AddWallsLines;
		AddWall->GetLines(WLines);

		FVector StartNearP, EndNearP;
		bool HasDoorDetailObj = false;
		for (auto L : WLines)
		{
			InnerLineInfo LineInfo;
			LineInfo.MainLine = L;
			for (auto ObjIt : DetailObjects)
			{
				//如果是单扇门或者是推拉门并且该门在新建墙上
				if (/*(ObjIt.Pin()->GetType() == OT_Door || ObjIt.Pin()->GetType() == OT_SlidingDoor) && */ObjIt.Pin()->InWallType == 1)
				{
					FVector LStart = ObjIt.Pin()->GetStartPos();
					FVector LEnd = ObjIt.Pin()->GetEndPos();
					if (FArmyMath::IsPointInOrOnPolygon2D((LStart + LEnd) / 2, OutVertexes))
					{
						HasDoorDetailObj = true;

						TArray<FVector> BoxVertices;
						ObjIt.Pin()->GetVertexes(BoxVertices);
						//获取门的垂直向量
						FVector CrossDir = FVector::CrossProduct(LStart - LEnd, -FVector::UpVector).GetSafeNormal();
						//获取门的厚度
						float DoorThickness = 0.0f;
						for (uint16 Index = 0; Index < BoxVertices.Num(); ++Index)
						{
							float a = FVector::DotProduct(FVector(LEnd - LStart), FVector(BoxVertices[Index%BoxVertices.Num()] - BoxVertices[(Index + 1) % BoxVertices.Num()]));
							if (FMath::Abs(FVector::DotProduct(FVector(LEnd - LStart), FVector(BoxVertices[Index%BoxVertices.Num()] - BoxVertices[(Index + 1) % BoxVertices.Num()]))) < 0.001)
							{
								DoorThickness = FVector::Distance(BoxVertices[Index%BoxVertices.Num()], BoxVertices[(Index + 1) % BoxVertices.Num()]);
								break;
							}
						}
						//FMath::PointDistToSegment(CornerEndPos, InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd()) < 0.05)
						if (FMath::PointDistToSegment(LStart + CrossDir*DoorThickness*0.5f, L->GetStart(), L->GetEnd()) < 0.05&&
							FMath::PointDistToSegment(LEnd + CrossDir*DoorThickness*0.5f, L->GetStart(), L->GetEnd()) < 0.05)
						{
							LineInfo.DetailPoints.AddUnique(LStart + CrossDir*DoorThickness*0.5f);
							LineInfo.DetailPoints.AddUnique(LEnd + CrossDir*DoorThickness*0.5f);
						}
						else if (FMath::PointDistToSegment(LStart - CrossDir*DoorThickness*0.5f, L->GetStart(), L->GetEnd()) < 0.05&&
							FMath::PointDistToSegment(LEnd - CrossDir*DoorThickness*0.5f, L->GetStart(), L->GetEnd()) < 0.05)
						{
							LineInfo.DetailPoints.AddUnique(LStart - CrossDir*DoorThickness*0.5f);
							LineInfo.DetailPoints.AddUnique(LEnd - CrossDir*DoorThickness*0.5f);
						}
					}

				}
				else
				{
					TSharedPtr<FArmyHardware> Obj = ObjIt.Pin();
					const FVector center = Obj->GetBasePos();
					if (!FArmyMath::IsPointInOrOnPolygon2D(center, OutVertexes))
						continue;

					FVector PStart = Obj->GetStartPos();
					FVector PEnd = Obj->GetEndPos();

					float W = Obj->GetWidth();
					FVector HardWareDir = (PEnd - PStart).GetSafeNormal();
					FVector WDir(-HardWareDir.Y, HardWareDir.X, 0);

					float StartDis1 = FMath::PointDistToSegment(PStart + WDir * W / 2, L->GetStart(), L->GetEnd());
					float EndDis1 = FMath::PointDistToSegment(PEnd + WDir * W / 2, L->GetStart(), L->GetEnd());

					float StartDis2 = FMath::PointDistToSegment(PStart - WDir * W / 2, L->GetStart(), L->GetEnd());
					float EndDis2 = FMath::PointDistToSegment(PEnd - WDir * W / 2, L->GetStart(), L->GetEnd());

					if ((StartDis1 < 0.001 && EndDis1 < 0.001) || (StartDis2 < 0.001 && EndDis2 < 0.001))
					{
						StartNearP = FMath::ClosestPointOnLine(L->GetStart(), L->GetEnd(), PStart);
						EndNearP = FMath::ClosestPointOnLine(L->GetStart(), L->GetEnd(), PEnd);

						LineInfo.DetailPoints.AddUnique(StartNearP);
						LineInfo.DetailPoints.AddUnique(EndNearP);
					}
				}
			}

			LineInfo.DetailPoints.AddUnique(L->GetStart());
			LineInfo.DetailPoints.AddUnique(L->GetEnd());
			LineInfo.SortPoints();
			AddWallsLines.Add(LineInfo);
		}
		for (auto L : AddWallsLines)
		{
			for (int32 i = 0; i < L.DetailPoints.Num() - 1; ++i)
			{
				FVector LStart = L.DetailPoints[i];
				FVector LEnd = L.DetailPoints[i + 1];
				LStart.Z = 0;
				LEnd.Z = 0;
				TSharedPtr<FArmyLine> tempLine = MakeShareable(new FArmyLine(LStart, LEnd));

				bool NotOnline = true;
				for (auto & LInfo : InInnerRoomLayoutLines)
				{
					if (FMath::PointDistToSegment(LStart, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd()) < 0.001 &&
						FMath::PointDistToSegment(LEnd, LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd()) < 0.001)
					{
						if (((LStart - LInfo.MainLine->GetStart()).Size() < 0.001 && (LEnd - LInfo.MainLine->GetEnd()).Size() < 0.001) ||
							((LStart - LInfo.MainLine->GetEnd()).Size() < 0.001 && (LEnd - LInfo.MainLine->GetStart()).Size() < 0.001))
						{
							if (HasDoorDetailObj)
							{
								TheOnWall = LInfo;
								OnWallLine = tempLine;
							}
							continue;
						}
						NotOnline = false;
						break;
					}
				}
				if (NotOnline || HasDoorDetailObj)
				{
					if (FVector::Distance(LStart, LEnd) < 0.001) continue;
					FVector Center((LEnd + LStart) / 2);
					FVector Dir(LEnd - LStart);
					FVector OutDir(-Dir.Y, Dir.X, 0);
					OutDir.Normalize();
					if (FArmyMath::IsPointInOrOnPolygon2D(Center + OutDir * 0.1, OutVertexes))
					{
						OutDir *= -1;
					}

					FVector Offset = OutDir * OffsetValue;

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, LStart, LEnd,
						LStart + Offset, LEnd + Offset, OT_AddWallDimensions);
					if (CurrentDimensions.IsValid())
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
			}
		}

		//带出新增墙的定位尺寸
		{
			TArray<NearPointInfo> WallPointInfoArray;
			for (auto & V : OutVertexes)
			{
				int32 InfoIndex = INDEX_NONE;
				for (auto L : InInnerRoomLayoutLines)
				{
					//找出两个距离最近并且不与新建墙相交的线段
					FVector MainLineStart = L.MainLine->GetStart();
					FVector MainLineEnd = L.MainLine->GetEnd();
					MainLineStart.Z = 0;
					MainLineEnd.Z = 0;

					FVector InfP = FMath::ClosestPointOnInfiniteLine(MainLineStart, MainLineEnd, V);
					FVector NearP = FMath::ClosestPointOnLine(MainLineStart, MainLineEnd, V);
					if ((InfP - NearP).Size() < 0.001)//两点相等说明点到线段的垂足是在线段内部的
					{
						FVector TempOffset = InfP - V;

						//判断是否与新建墙有相交（如果有则抛弃掉）
						bool Inter = false;
						for (auto L : WLines)
						{
							FVector StartV = L->GetStart();
							FVector EndV = L->GetEnd();
							StartV.Z = 0;
							EndV.Z = 0;

							if (FArmyMath::CalculateLine2DIntersection(StartV, EndV, V + TempOffset.GetSafeNormal() * 0.1, InfP))
							{
								Inter = true;
								break;
							}

							//由于新建墙获取的是拆改后的数据
							//所以可能标注的目标是自身新建的墙
							//由于自身新建的墙和自身新增墙位置大小一致
							//所以目标的在自身线段上的话就丢弃该目标点
							if (((FMath::ClosestPointOnLine(StartV, EndV, V + TempOffset.GetSafeNormal() * 0.1) - (V + TempOffset.GetSafeNormal() * 0.1)).Size() < 0.05))
							{
								Inter = true;
								break;
							}
						}
						if (Inter)
						{
							continue;
						}
						else
						{
							NearPointInfo TempInfo(V);
							InfoIndex = WallPointInfoArray.Find(TempInfo);
							if (InfoIndex != INDEX_NONE)
							{
								WallPointInfoArray[InfoIndex].TargetPointArray.AddUnique(InfP);
							}
							else
							{
								TempInfo.TargetPointArray.AddUnique(InfP);
								InfoIndex = WallPointInfoArray.Add(TempInfo);
							}
						}
					}
				}

				if (InfoIndex != INDEX_NONE)
				{
					WallPointInfoArray[InfoIndex].SortTargetPoint();//对新建墙体顶点到所有原始内墙的距离点按距离排序
					WallPointInfoArray[InfoIndex].UpdateFlag();//取最短的两条线段按长度做乘积运算
				}
			}
			if (WallPointInfoArray.Num() > 0)
			{
				//按两标尺长度乘积的大小对新建墙所有点信息做排序
				WallPointInfoArray.Sort([&](const NearPointInfo& A, const NearPointInfo& B)
				{
					return A.Flag < B.Flag;
				});
				NearPointInfo& NearestInfo = WallPointInfoArray[0];

				//筛选线段（防止线段在同一方向重复标注）
				TArray<FVector> TargetPointList;
				FVector FirstPoint;
				FVector SecondPoint;
				bool FindFirstPoint = false;
				for (int32 Index = 0; Index < NearestInfo.TargetPointArray.Num(); ++Index)
				{
					//由于新建墙获取的是拆改后的数据
					//所以可能标注的目标是自身新建的墙
					//由于自身新建的墙和自身新增墙位置大小一致
					//所以目标的在自身线段上的话就丢弃该目标点
					bool IsContinue = false;
					for (TSharedPtr<FArmyLine> Line : WLines)
					{
						float dis = (FMath::ClosestPointOnLine(Line->GetStart(), Line->GetEnd(), NearestInfo.TargetPointArray[Index]) - NearestInfo.TargetPointArray[Index]).Size();
						if ((FMath::ClosestPointOnLine(Line->GetStart(), Line->GetEnd(), NearestInfo.TargetPointArray[Index]) - NearestInfo.TargetPointArray[Index]).Size() < 0.05)
						{
							IsContinue = true;
							break;
						}
					}
					if (IsContinue) continue;

					if (FVector::Distance(NearestInfo.BasePoint, NearestInfo.TargetPointArray[Index]) < 0.001)
					{
						continue;
					}
					if (!FindFirstPoint)
					{
						FirstPoint = NearestInfo.TargetPointArray[Index];
						TargetPointList.AddUnique(FirstPoint);

						FindFirstPoint = true;
					}
					else
					{
						const FVector& BaseToFirPos = (FirstPoint - NearestInfo.BasePoint).GetSafeNormal();
						const FVector& BaseToNearstPos = (NearestInfo.TargetPointArray[Index] - NearestInfo.BasePoint).GetSafeNormal();

						//如果方向和第一条线方向在同一方向，则舍弃
						if (FVector::Distance(BaseToFirPos, BaseToNearstPos) < 0.001 ||			//两个向量在同一方向
							(BaseToFirPos + BaseToNearstPos).Size() < 0.001)						//两个向量在相反方向
						{
							continue;
						}

						SecondPoint = NearestInfo.TargetPointArray[Index];
						TargetPointList.AddUnique(SecondPoint);
						//只需要标注两条线段，所以直接break掉
						break;
					}
				}

				//取经过排序后的前两个标尺点分别绘制标尺
				FVector DimensionDir1(0,0,0),DimensionDir2(0,0,0);
				for (int32 Index = 0; Index < TargetPointList.Num(); ++Index)
				{
					if (DimensionDir1.Size() == 0)
					{
						DimensionDir1 = (TargetPointList[Index] - NearestInfo.BasePoint).GetSafeNormal();
					}
					else if (DimensionDir2.Size() == 0)
					{
						DimensionDir2 = (TargetPointList[Index] - NearestInfo.BasePoint).GetSafeNormal();
					}
					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, NearestInfo.BasePoint, TargetPointList[Index],
						NearestInfo.BasePoint, TargetPointList[Index], OT_AddWallDimensions);
					if (CurrentDimensions.IsValid())
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}

				if (AddWall->GetIsHalfWall())
				{
					if (DimensionDir2.Size() == 0)
					{
						DimensionDir2.X = -DimensionDir1.Y;
						DimensionDir2.Y = DimensionDir1.X;
						DimensionDir2.Z = DimensionDir1.Z;

						if (!FArmyMath::IsPointInPolygon2D(NearestInfo.BasePoint - DimensionDir1 - DimensionDir2, OutVertexes))
						{
							DimensionDir2 *= -1;
						}
					}

					FVector LabelBasePos = NearestInfo.BasePoint - DimensionDir1 * 5 - DimensionDir2 * 5;
					FVector LabelLeadPos = NearestInfo.BasePoint - DimensionDir1 * 50 - DimensionDir2 * 50;

					FVector MainDir((-DimensionDir1 - DimensionDir2).X, 0, 0);
					FVector LastPos = LabelLeadPos + MainDir.GetSafeNormal() * 130;

					if (ClassTypeList.Contains(GetClassTypeByObject(AddWall, 2)) || ClassTypeList.Contains(Class_Other))
					{
						TSharedPtr<FArmyDownLeadLabel> HLabel = MakeShareable(new FArmyDownLeadLabel);
						HLabel->Init(GVC->ViewportOverlayWidget);
						//HLabel->SetTextSize(12);
						HLabel->SetObjTag(AddWall, -1);
						HLabel->SetWorldPosition(LabelBasePos);
						HLabel->SetLeadPoint(LabelLeadPos);
						HLabel->SetEndPoint(LastPos);
						HLabel->SetLabelContent(FText::FromString(TEXT("新建墙体高度:") + FString::FromInt(FMath::CeilToInt(AddWall->GetHeight() * 10)) + TEXT("mm")));
						FArmySceneData::Get()->Add(HLabel, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
			}
		}
	}

	//单扇门与拖拉门在没有连接门洞情况下生成的拆除墙标注
	TArray<FObjectWeakPtr> DoorWeakObjList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, DoorWeakObjList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, DoorWeakObjList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, DoorWeakObjList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_NewPass, DoorWeakObjList);
	auto DoorLocationFunc = [&](FVector LStart, FVector LEnd, const TArray<FVector>& BoxVertices, ObjectClassType ClassType, EObjectType ObjectType = OT_DeleteWallDimensions)
	{
		auto GetDismensionPosList = [&InInnerRoomOriginLines](const FVector& CornerStartPos, const FVector& CornerEndPos)->TPair<float, TArray<FVector>>
		{
			FVector MinDetailPos = CornerStartPos;//默认值
			float MinDistance = MAX_FLT;
			//筛选出与拆除墙边贴近的内墙线
			for (InnerLineInfo InnerLine : InInnerRoomOriginLines)
			{
				if (InnerLine.MainLine.IsValid())
				{
					float a = FMath::PointDistToSegment(CornerStartPos, InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd());
					//判断门的边是否在该内墙线上
					if (
						// (FMath::ClosestPointOnLine(InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd(), CornerStartPos) - CornerStartPos).Size() < 0.001 &&
						// (FMath::ClosestPointOnLine(InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd(), CornerEndPos) - CornerEndPos).Size() < 0.001
						FMath::PointDistToSegment(CornerStartPos, InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd()) < 0.05&&
						FMath::PointDistToSegment(CornerEndPos, InnerLine.MainLine->GetStart(), InnerLine.MainLine->GetEnd()) < 0.05)
					{
						//筛选最近的细节点
						for (FVector& DetailPos : InnerLine.DetailPoints)
						{
							//细节点在拆除墙边上
							if (FArmyMath::IsPointOnLine(DetailPos, CornerStartPos, CornerEndPos))
							{
								//如果细节点在拆除墙边上的两端，则直接设置该细节点为最近的标注点并break
								if (FVector::Distance(DetailPos, CornerStartPos) < 0.001 ||
									FVector::Distance(DetailPos, CornerEndPos) < 0.001)
								{
									MinDetailPos = DetailPos;
									MinDistance = 0;
									break;
								}
							}
							else
							{
								float Distance = FMath::PointDistToSegment(DetailPos, CornerStartPos, CornerEndPos);
								if (Distance < MinDistance)
								{
									MinDetailPos = DetailPos;
									MinDistance = Distance;
								}
							}
						}
						break;
					}
				}
			}
			//找到离MinDetailPos最远的端点作为起始点
			const FVector& FirstPos = FVector::Distance(CornerStartPos, MinDetailPos) > FVector::Distance(CornerEndPos, MinDetailPos) ? CornerStartPos : CornerEndPos;
			const FVector& SecondPos = FirstPos.Equals(CornerStartPos) ? CornerEndPos : CornerStartPos;
			return TPair<float, TArray<FVector>>(MinDistance, TArray<FVector>{ FirstPos, SecondPos, MinDetailPos });
		};

		//获取门的垂直向量
		FVector CrossDir = FVector::CrossProduct(LStart - LEnd, -FVector::UpVector).GetSafeNormal();
		//获取门的厚度
		float DoorThickness = 0.0f;
		for (uint16 Index = 0; Index < BoxVertices.Num(); ++Index)
		{
			float a = FVector::DotProduct(FVector(LEnd - LStart), FVector(BoxVertices[Index%BoxVertices.Num()] - BoxVertices[(Index + 1) % BoxVertices.Num()]));
			if (FMath::Abs(FVector::DotProduct(FVector(LEnd - LStart), FVector(BoxVertices[Index%BoxVertices.Num()] - BoxVertices[(Index + 1) % BoxVertices.Num()]))) < 0.001)
			{
				DoorThickness = FVector::Distance(BoxVertices[Index%BoxVertices.Num()], BoxVertices[(Index + 1) % BoxVertices.Num()]);
				break;
			}
		}
		//选中最近的顶点
		const TPair<float, TArray<FVector>>& Item1 = GetDismensionPosList(LStart + CrossDir*DoorThickness*0.5f, LEnd + CrossDir*DoorThickness*0.5f);
		const TPair<float, TArray<FVector>>& Item2 = GetDismensionPosList(LStart - CrossDir*DoorThickness*0.5f, LEnd - CrossDir*DoorThickness*0.5f);
		TArray<FVector> TargetDismension = Item1.Key > Item2.Key ? Item2.Value : Item1.Value;
		//获取标注的偏移量
		FVector Offset = FVector::ZeroVector;
		if (TargetDismension.Num() >= 2)
		{
			//获取拆除墙的边角点（TargetDismension前两个）
			Offset = ((TargetDismension[0] + TargetDismension[1])*0.5f - FBox(BoxVertices).GetCenter()).GetSafeNormal();
		}
		//带出标注
		for (uint8 Index = 0; Index < TargetDismension.Num() - 1; ++Index)
		{
			if (FVector::Distance(TargetDismension[Index], TargetDismension[Index + 1]) < 0.001) continue;


			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(ClassType, TargetDismension[Index], TargetDismension[Index + 1],
				TargetDismension[Index] + Offset * OffsetValue, TargetDismension[Index + 1] + Offset * OffsetValue, ObjectType);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
	};
	for (FObjectWeakPtr DoorWeakObj : DoorWeakObjList)
	{
		switch (DoorWeakObj.Pin()->GetType())
		{
		case OT_Door:
		{
			FArmySingleDoor* SingleDoor = DoorWeakObj.Pin()->AsassignObj<FArmySingleDoor>();
			if ((SingleDoor && !SingleDoor->bHasConnectPass&&SingleDoor->InWallType == 0))
			{
				TArray<FVector> OutVertices;
				SingleDoor->GetVertexes(OutVertices);
				DoorLocationFunc(SingleDoor->GetStartPos(), SingleDoor->GetEndPos(), OutVertices, Class_Pass);
			}
			break;
		}
		case  OT_SlidingDoor:
		{
			FArmySlidingDoor* SlidingDoor = DoorWeakObj.Pin()->AsassignObj<FArmySlidingDoor>();
			if ((SlidingDoor && !SlidingDoor->bHasConnectPass&&SlidingDoor->InWallType == 0))
			{
				TArray<FVector> OutVertices;
				SlidingDoor->GetVertexes(OutVertices);
				DoorLocationFunc(SlidingDoor->GetStartPos(), SlidingDoor->GetEndPos(), OutVertices, Class_Pass);
			}
			break;
		}
		case OT_Pass:
		{
			FArmyPass* PassWeakObj = DoorWeakObj.Pin()->AsassignObj<FArmyPass>();
			if ((PassWeakObj && PassWeakObj->GetIfFillPass()/*选择了填补门洞*/))
			{
				TArray<FVector> OutVertices;
				PassWeakObj->GetVertexes(OutVertices);
				DoorLocationFunc(PassWeakObj->GetStartPos(), PassWeakObj->GetEndPos(), OutVertices, Class_Pass, OT_AddWallDimensions);
			}
			break;
		}
		case OT_NewPass:
		{
			FArmyNewPass* NewPassWeakObj = DoorWeakObj.Pin()->AsassignObj<FArmyNewPass>();
			if (NewPassWeakObj)
			{
				TArray<FVector> OutVertices;
				NewPassWeakObj->GetVertexes(OutVertices);
				DoorLocationFunc(NewPassWeakObj->GetStartPos(), NewPassWeakObj->GetEndPos(), OutVertices, Class_Pass, NewPassWeakObj->InWallType == 1 ? OT_AddWallDimensions : OT_DeleteWallDimensions);
			}
			break;
		}
		default:
			break;
		}
	}

	//包立管的外侧标注
	TArray<InnerLineInfo> NewInnerRoomLine = InInnerRoomLayoutLines;
	TArray<FObjectWeakPtr> PackPipeList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, PackPipeList);
	for (FObjectWeakPtr PackPipeWeakObj : PackPipeList)
	{
		FArmyPackPipe* PackPipe = PackPipeWeakObj.Pin()->AsassignObj<FArmyPackPipe>();
		TArray<FVector> OutsideVertices;
		PackPipe->GetOutsideVertices(OutsideVertices);
		//构建包立管polygon
		TArray<FVector> AllVertices;
		PackPipe->GetVertexes(AllVertices);
		FArmyPolygon PackPipePolygon;
		PackPipePolygon.SetVertices(AllVertices);

		InnerLineInfo* FirstLineInfo = nullptr;
		FVector FirstInnerRoomLineVertex = FVector::ZeroVector;
		for (int Index = 0; Index < OutsideVertices.Num(); ++Index)
		{
			//找出该LInfo中需要Ignore的线段
			for (InnerLineInfo& LInfo : NewInnerRoomLine)
			{
				int32 PackPipeType = -1;
				FVector ClosestPoint = FMath::ClosestPointOnLine(LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd(), OutsideVertices[Index]);
				if ((ClosestPoint - OutsideVertices[Index]).Size() < 0.001)//点在直线上
				{
					if (FirstLineInfo == nullptr)
					{
						FirstLineInfo = &LInfo;
						FirstInnerRoomLineVertex = ClosestPoint;
					}
					else//判断获取该LineInfo的IgnoreLine
					{
						//判断该包立管的第一个与内墙线相交的外围点是否是在该LInfo上
						if (LInfo == *FirstLineInfo)
						{
							//包立管的起点和终点都是在一条线上(U形包立管)
							++LInfo.UPackPipeCount;
							PackPipeType = 0;
							LInfo.IgoneLines.AddUnique(TPair<FVector, FVector>(FirstInnerRoomLineVertex, ClosestPoint));
						}
						else
						{
							//LInfo与FirstLineInfo中的直线相交的点
							FVector IntersectionVec = FVector(ForceInitToZero);
							if (FArmyMath::CalculateLinesIntersection(LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd(), FirstLineInfo->MainLine->GetStart(), FirstLineInfo->MainLine->GetEnd(), IntersectionVec))
							{
								//相交
								//不是直角
								PackPipeType = 2;
								FirstLineInfo->IgoneLines.AddUnique(TPair<FVector, FVector>(FirstInnerRoomLineVertex, IntersectionVec));
								LInfo.IgoneLines.AddUnique(TPair<FVector, FVector>(ClosestPoint, IntersectionVec));
								//是否是直角
								float DotValue = FMath::Abs(FVector::DotProduct(LInfo.MainLine->GetStart() - LInfo.MainLine->GetEnd(), FirstLineInfo->MainLine->GetStart() - FirstLineInfo->MainLine->GetEnd()));
								if (DotValue < 0.01 && OutsideVertices.Num() == 3)
								{
									DotValue = FMath::Abs(FVector::DotProduct(OutsideVertices[0] - OutsideVertices[1], OutsideVertices[2] - OutsideVertices[1]));
									if (DotValue < 0.01)
									{
										PackPipeType = 1;//标注户型的直角包立管类型
														 // 										if (FirstLineInfo->PackPipePoints.Num() > 0)
														 // 										{
														 // 											//将前一段的FirstLineInfo包立管类型改变为直角类型
														 // 											FirstLineInfo->PackPipePoints.Top().Value = PackPipeType;
														 // 										}
									}
								}
							}
							else
							{
								//不想交
								//不需要做处理
							}
						}
						if (FirstLineInfo->PackPipePoints.Num() > 0)
						{
							//将前一段的FirstLineInfo包立管类型改变为直角类型
							FirstLineInfo->PackPipePoints.Top().Value = PackPipeType;
						}
					}
					LInfo.PackPipePoints.AddUnique(TPair<FVector, int32>(ClosestPoint, PackPipeType));
				}
			}

			//带出包立管的自身标注
			if (Index < OutsideVertices.Num() - 1)
			{
				FVector LStart = OutsideVertices[Index%OutsideVertices.Num()];
				FVector LEnd = OutsideVertices[(Index + 1) % OutsideVertices.Num()];

				//计算出线段的法线
				FVector CrossVec = FVector::CrossProduct(LStart - LEnd, FVector::UpVector);
				CrossVec = CrossVec.GetSafeNormal() * 0.5f;//0.5f是将单位向量长度减半
				if (PackPipePolygon.IsInside((LStart + LEnd) / 2 + CrossVec, true))
				{
					CrossVec *= -1;
				}
				CrossVec *= 2;//使得法线长度为1

				if (FVector::Distance(LStart, LEnd) < 0.001) continue;


				TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, LStart, LEnd,
					LStart + CrossVec * OffsetValue, LEnd + CrossVec * OffsetValue, OT_AddWallDimensions);
				if (CurrentDimensions.IsValid())
				{
					FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
				}
			}
		}
	}
	//带出包立管的定位标注（使用拆改后的数据）
	for (InnerLineInfo & LInfo : NewInnerRoomLine)
	{
		if (LInfo.PackPipePoints.Num() <= 0) continue;

		//将DetailPoint排序
		LInfo.DetailPoints.Sort([&](const FVector& A, const FVector& B) ->bool {
			if (FMath::Abs(A.X - B.X) < 0.001)//竖墙
			{
				return A.Y < B.Y;
			}
			else //if (FMath::Abs(A.Key.Y - B.Key.Y) < 0.001)//横墙
			{
				return A.X < B.X;
			}
		});

		//存储多个两端细节点对应端点之间的包立管外围点
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToPackPipeVertexMap;
		for (int Index = 0; Index < LInfo.DetailPoints.Num() - 1; ++Index)
		{
			TArray<TPair<FVector, int32>> PackPipeComponentList;
			for (TPair<FVector, int32>& ComponentPair : LInfo.PackPipePoints)
			{
				//判断该点是否在细节点之内
				FVector ResVec = (LInfo.DetailPoints[Index] - ComponentPair.Key).GetUnsafeNormal() + (LInfo.DetailPoints[Index + 1] - ComponentPair.Key).GetUnsafeNormal();
				if (ResVec.Size() < 0.001)
				{
					PackPipeComponentList.Emplace(ComponentPair);
				}
			}
			DetailToPackPipeVertexMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), PackPipeComponentList);
		}

		for (auto DetailToComponentIt = DetailToPackPipeVertexMap.CreateConstIterator(); DetailToComponentIt; ++DetailToComponentIt)
		{
			TSharedPtr<FArmyDimensions> TargetDismension;
			float TargetMinDistance = BIG_NUMBER;
			//在该LineInfo内U形包立管端点数量
			int UPackPipeVertices = 0;

			TPair<FVector, FVector> DetailPair = DetailToComponentIt.Key();
			TArray<TPair<FVector, int32>> PackPipeComponentList = DetailToComponentIt.Value();
			if (PackPipeComponentList.Num() <= 0 ||
				(DetailPair.Key - DetailPair.Value).Size() < 0.001)
			{
				continue;
			}
			else if (PackPipeComponentList.Num() > 0)
			{
				bool IsAllRightAngle = true;
				for (TPair<FVector, int32> PointPair : PackPipeComponentList)
				{
					if (PointPair.Value != 1)/*直角*/
					{
						++UPackPipeVertices;
						IsAllRightAngle = false;
					}
				}
				if (IsAllRightAngle)
				{
					continue;
				}
			}
			PackPipeComponentList.Append(TArray<TPair<FVector, int32>>{TPair<FVector, int32>(DetailPair.Key, -1), TPair<FVector, int32>(DetailPair.Value, -1)});

			//对细节点进行排序
			PackPipeComponentList.Sort([&](const TPair<FVector, int32>& A, const TPair<FVector, int32>& B) ->bool {
				if (FMath::Abs(A.Key.X - B.Key.X) < 0.001)//竖墙
				{
					return A.Key.Y < B.Key.Y;
				}
				else //if (FMath::Abs(A.Key.Y - B.Key.Y) < 0.001)//横墙
				{
					return A.Key.X < B.Key.X;
				}
			});

			for (int32 Index = 0; Index < PackPipeComponentList.Num() - 1; ++Index)
			{
				if (LInfo.CompareIgone(TPair<FVector, FVector>(PackPipeComponentList[Index].Key, PackPipeComponentList[Index + 1].Key)))
				{
					//IgnoreLine
					continue;
				}
				else if ((PackPipeComponentList[Index].Key - PackPipeComponentList[Index + 1].Key).Size() < 0.001)
				{
					//尺寸为零
					continue;
				}
				else
				{
					//计算出标尺法线
					FVector CrossVec = FVector::CrossProduct(PackPipeComponentList[Index].Key - PackPipeComponentList[Index + 1].Key, -FVector::UpVector).GetSafeNormal();
					CrossVec.Z = 0;
					FVector DismensionCenter = (PackPipeComponentList[Index].Key + PackPipeComponentList[Index + 1].Key) / 2;
					if (LInfo.RelationInnerRoom.IsValid() && !LInfo.RelationInnerRoom->IsPointInRoom(DismensionCenter + CrossVec))
					{
						CrossVec *= -1;
					}

					//带出标尺
					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationAddWall, PackPipeComponentList[Index].Key, PackPipeComponentList[Index + 1].Key,
						PackPipeComponentList[Index].Key + CrossVec*OffsetValue, PackPipeComponentList[Index + 1].Key + CrossVec*OffsetValue, OT_AddWallDimensions);
					const float TempMinDistance = FVector::Distance(PackPipeComponentList[Index].Key, PackPipeComponentList[Index + 1].Key);
					if (TempMinDistance < TargetMinDistance)
					{
						TargetMinDistance = TempMinDistance;
						TargetDismension = CurrentDimensions;
					}
					if (CurrentDimensions.IsValid() &&
						UPackPipeVertices > 2)
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
			}
			if (TargetDismension.IsValid() &&
				UPackPipeVertices <= 2)//1 or 2
			{
				FArmySceneData::Get()->Add(TargetDismension, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
		}
	}
}
void FArmyConstructionFunction::GenerateAutoObjectDimensions(const TArray<InnerLineInfo>& InInnerRoomLines)
{
	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);

	TArray<FObjectWeakPtr> ComponentArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, ComponentArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_ComponentBase, ComponentArray);
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_ComponentBase, ComponentArray);
	FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_ComponentBase, ComponentArray);
	struct PRulerInfo
	{
		PRulerInfo(FVector InP, FVector InTP, TSharedPtr<FArmyLine> InAttachLine, float InDis, TSharedPtr<FArmyFurniture> InFurniture = nullptr)
			:P(InP),
			TargetPoint(InTP),
			AttachLine(InAttachLine),
			Distance(InDis),
			Furniture(InFurniture)
		{
			BaseDir = AttachLine->GetEnd() - AttachLine->GetStart();
			ClassType = InFurniture.IsValid() ? ObjectClassType(InFurniture->BelongClass) : Class_Other;
		}
		FVector P = FVector(ForceInitToZero);
		FVector TargetPoint = FVector(ForceInitToZero);
		FVector BaseDir = FVector(ForceInitToZero);
		ObjectClassType ClassType = Class_Other;
		TSharedPtr<FArmyLine> AttachLine = nullptr;
		TSharedPtr<FArmyFurniture> Furniture = nullptr;
		float Distance = 9999999;

		bool operator==(const PRulerInfo& InOther) const
		{
			//同一直线也属于相等
			return P == InOther.P && ClassType == InOther.ClassType && ((BaseDir.GetSafeNormal() - InOther.BaseDir.GetSafeNormal()).Size() < 0.001f || (BaseDir.GetSafeNormal() + InOther.BaseDir.GetSafeNormal()).Size() < 0.001f);
		}
	};

	//存储全部的灯具构件
	//构件里面包含了距墙尺寸信息
	//PRulerInfo中有构件类型、距各边墙的起点与终点
	TMap<FObjectWeakPtr, TArray<PRulerInfo>> LampToRulerInfoMap;
	TArray<FObjectWeakPtr> LampDownList;
	TArray<InnerLineInfo> OnWallPointArray;
	for (auto Obj : ComponentArray)
	{
		if (Obj.Pin()->GetPropertyFlag(FArmyObject::FLAG_STOPPED))
		{
			continue;
		}
		TSharedPtr<FArmyFurniture> ComponentObj = NULL;
		if (Obj.Pin()->GetType() == OT_ComponentBase)
		{
			ComponentObj = StaticCastSharedPtr<FArmyFurniture>(Obj.Pin());
		}
		if (Obj.Pin()->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
		{

			FVector BasePos = Obj.Pin()->GetBasePos();
			BasePos.Z = 0;

			bool OnWall = false;
			TArray<PRulerInfo> NotOnWallPointInfos;

			for (auto & LInfo : InInnerRoomLines)
			{
				FVector LStart = LInfo.MainLine->GetStart();
				FVector LEnd = LInfo.MainLine->GetEnd();
				LStart.Z = 0;
				LEnd.Z = 0;

				float NearDistance = FMath::PointDistToSegment(BasePos, LStart, LEnd);
				//该点在墙上
				if (NearDistance < 0.05)
				{
					OnWall = true;
					int32 Index = OnWallPointArray.Find(LInfo);
					//将组件点的位置信息（包括在该点的组件类型）添加到内墙线中
					if (Index != INDEX_NONE)
					{
						InnerLineInfo& WInfo = OnWallPointArray[Index];
						//WInfo.DetailPoints.AddUnique(BasePos);
						WInfo.ComponentPoints.AddUnique(TPair<FVector, int32>(BasePos, ComponentObj.IsValid() ? ComponentObj->BelongClass : -1));
					}
					else
					{
						InnerLineInfo WInfo(LInfo);
						//WInfo.DetailPoints.AddUnique(BasePos);
						WInfo.ComponentPoints.AddUnique(TPair<FVector, int32>(BasePos, ComponentObj.IsValid() ? ComponentObj->BelongClass : -1));
						OnWallPointArray.AddUnique(WInfo);
					}
					continue;
				}
				//该点不在该墙上
				else if (!OnWall)
				{
					FVector ClosestPoint = FMath::ClosestPointOnInfiniteLine(LStart, LEnd, BasePos);
					FVector ClosestPointOnLine = FMath::ClosestPointOnLine(LStart, LEnd, BasePos);
					if ((ClosestPoint - ClosestPointOnLine).Size() < 0.001)
					{
						//存储该点到最近点信息
						PRulerInfo RInfo(BasePos, ClosestPoint, LInfo.MainLine, NearDistance, ComponentObj);
						int32 Index = NotOnWallPointInfos.Find(RInfo);
						if (Index != INDEX_NONE)
						{
							PRulerInfo& RInfo = NotOnWallPointInfos[Index];
							if (RInfo.Distance > NearDistance)
							{
								//更新最近点位与距离
								RInfo.TargetPoint = ClosestPoint;
								RInfo.Distance = NearDistance;
							}
						}
						else
						{
							NotOnWallPointInfos.Add(RInfo);
						}
					}
				}
			}
			//该点不与全部的内墙线相交
			if (!OnWall)
			{
				NotOnWallPointInfos.Sort([&](const PRulerInfo& A, const PRulerInfo& B) {
					return A.Distance < B.Distance;
				});

				//不在墙上的筒灯
				if (ComponentObj->BelongClass == Class_Light)
				{
					LampDownList.Emplace(ComponentObj);
					LampToRulerInfoMap.Emplace(ComponentObj, NotOnWallPointInfos);
					continue;
				}

				for (int32 i = 0; i < NotOnWallPointInfos.Num(); i++)
				{
					if (i > 1) break;//只取距离最短并且不在同一方向上的两个标尺

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(NotOnWallPointInfos[i].ClassType, NotOnWallPointInfos[i].P, NotOnWallPointInfos[i].TargetPoint,
						NotOnWallPointInfos[i].P, NotOnWallPointInfos[i].TargetPoint, OT_Dimensions);
					if (CurrentDimensions.IsValid())
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
			}
		}
		else
		{
			FVector Trans(ForceInitToZero), Scale(1, 1, 1);
			FRotator Rot(ForceInitToZero);
			Obj.Pin()->GetTransForm(Rot, Trans, Scale);
			FTransform ObjTransFrom(Rot, Trans, Scale);

			const FBox PreBox = Obj.Pin()->GetPreBounds();
			FVector LastSize = PreBox.GetSize() * Scale;

			FVector WidthStart((PreBox.Min + FVector(0, 10, 0)));
			FVector WidthEnd(PreBox.Max.X, WidthStart.Y, 0);

			FVector LengthStart((PreBox.Min + FVector(10, 0, 0)));
			FVector LengthEnd(LengthStart.X, PreBox.Max.Y, 0);
			if (LastSize.Y < 20)
			{
				WidthStart = FVector(PreBox.Min + FVector(0, PreBox.GetSize().Y / 2, 0));
				WidthEnd = FVector(PreBox.Max.X, WidthStart.Y, 0);
			}
			if (LastSize.X < 20)
			{
				LengthStart = FVector((PreBox.Min + FVector(PreBox.GetSize().X / 2, 0, 0)));
				LengthEnd = FVector(LengthStart.X, PreBox.Max.Y, 0);
			}
			WidthStart = Obj.Pin()->TranfromVertex(WidthStart);
			WidthEnd = Obj.Pin()->TranfromVertex(WidthEnd);

			LengthStart = Obj.Pin()->TranfromVertex(LengthStart);
			LengthEnd = Obj.Pin()->TranfromVertex(LengthEnd);


			TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)(ComponentObj.IsValid() ? ComponentObj->BelongClass : -1), WidthStart, WidthEnd,
				WidthStart, WidthEnd, OT_Dimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}
			CurrentDimensions = CreateObjectDismension((ObjectClassType)(ComponentObj.IsValid() ? ComponentObj->BelongClass : -1), LengthStart, LengthEnd,
				LengthStart, LengthEnd, OT_Dimensions);
			if (CurrentDimensions.IsValid())
			{
				FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
			}

			// 			CurrentDimensions = MakeShareable(new FArmyDimensions());
			// 			CurrentDimensions->SetArrowsType(FArmyDimensions::Diagonal);
			// 			CurrentDimensions->SetType(OT_Dimensions);
			// 			CurrentDimensions->AreaType = 0;
			// 			//CurrentDimensions->UseLeadLine = true;
			// 			CurrentDimensions->ClassType = ComponentObj.IsValid() ? ComponentObj->BelongClass : -1;
			// 			CurrentDimensions->SetBaseStart(LengthStart);
			// 			CurrentDimensions->SetBaseEnd(LengthEnd);
			// 			CurrentDimensions->SetDimensionsStart(LengthStart);
			// 			CurrentDimensions->SetDimensionsEnd(LengthEnd);
			// 			//CurrentDimensions->SetBaseColor(FLinearColor(0.5, 0.5, 0.5));
			// 			CurrentDimensions->SetTextSize(10);
			// 				
			// 			FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
		}
	}

	TArray<FObjectWeakPtr> PackPipeList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, PackPipeList);
	//遍历存在构件的墙
	for (InnerLineInfo & LInfo : OnWallPointArray)
	{
		//遍历每个包立管判断包立管的外围点是否是在墙线中
		//如果是则将该外围点
		for (FObjectWeakPtr PackPipeWeakObj : PackPipeList)
		{
			TArray<FVector> OutsideVertices;
			FArmyPackPipe* PackPipe = PackPipeWeakObj.Pin()->AsassignObj<FArmyPackPipe>();
			PackPipe->GetOutsideVertices(OutsideVertices);
			for (FVector OutsideVertex : OutsideVertices)
			{
				FVector ClosestPoint = FMath::ClosestPointOnLine(LInfo.MainLine->GetStart(), LInfo.MainLine->GetEnd(), OutsideVertex);
				if ((ClosestPoint - OutsideVertex).Size() < 0.001)//点在直线上
				{
					LInfo.DetailPoints.AddUnique(OutsideVertex);
				}
			}
		}

		//对细节点进行排序
		LInfo.DetailPoints.Sort([&](const FVector& A, const FVector& B) ->bool {
			if (FMath::Abs(A.X - B.X) < 0.001)//竖墙
			{
				return A.Y < B.Y;
			}
			else if (FMath::Abs(A.Y - B.Y) < 0.001)//横墙
			{
				return A.X < B.X;
			}
			else
			{
				//这里只对横墙和竖墙进行的细节点排序
				//如果产品需要对斜墙进行点位排序可以在此处加上排序方法
				return false;
			}
		});

		//存储开关与插座的数据结构
		TArray<TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>>> DetailToComponentMapList;
		//存储多个两端细节点对应端点之间的开关构件
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToSwitchComponentMap;
		//存储多个两端细节点对应端点之间的插座、电箱构件
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToElectricComponentMap;
		//存储多个两端细节点对应端点之间的灯具构件--->壁灯
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToLampComponentMap;
		//存储多个两端细节点对应端点之间的水路点位构件
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToWaterPosComponentMap;
		//存储多个两端细节点对应端点之间的燃气表点位构件
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToGasMeterComponentMap;
		//存储多个两端细节点对应端点之间的分集水器点位构件
		TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToDiversityWaterComponentMap;
		for (int Index = 0; Index < LInfo.DetailPoints.Num() - 1; ++Index)
		{
			TArray<TPair<FVector, int32>> SwitchComponentList;
			TArray<TPair<FVector, int32>> ElectricComponentList;
			TArray<TPair<FVector, int32>> LampComponentList;
			TArray<TPair<FVector, int32>> WaterPosComponentList;
			TArray<TPair<FVector, int32>> GasMeterComponentList;
			TArray<TPair<FVector, int32>> DiversityWaterComponentList;
			for (TPair<FVector, int32>& ComponentPair : LInfo.ComponentPoints)
			{
				//判断该点是否在细节点之内
				FVector ResVec = (LInfo.DetailPoints[Index] - ComponentPair.Key).GetUnsafeNormal() + (LInfo.DetailPoints[Index + 1] - ComponentPair.Key).GetUnsafeNormal();
				if (ResVec.Size() < 0.001)
				{
					switch (ComponentPair.Value)
					{
					case Class_Switch:
					{
						SwitchComponentList.Emplace(ComponentPair);
						break;
					}
					case Class_Socket:
					case Class_Socket_H:
					case Class_Socket_L:
					case Class_ElectricBoxH:
					case Class_ElectricBoxL:
					{
						ElectricComponentList.Emplace(ComponentPair);
						break;
					}
					case Class_Light:
					{
						LampComponentList.Emplace(ComponentPair);
						break;
					}
					case Class_HotWaterPoint://热水点位
					case Class_HotAndColdWaterPoint://冷热水点位
					case Class_ColdWaterPoint://冷水点位
					case Class_FloordRainPoint://地漏下水
					case Class_ClosestoolRainPoint://马桶下水
					case Class_WashBasinRainPoint://水盆下水
					case Class_RecycledWaterPoint://中水点位
					case Class_HomeEntryWaterPoint://进户水
					case Class_WaterRoutePoint:
					{
						WaterPosComponentList.Emplace(ComponentPair);
						break;
					}
					case Class_GasMeter:
					{
						GasMeterComponentList.Emplace(ComponentPair);
						break;
					}
					case Class_DiversityWater:
					{
						DiversityWaterComponentList.Emplace(ComponentPair);
						break;
					}
					default:
						break;
					}

				}
			}
			DetailToSwitchComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), SwitchComponentList);
			DetailToElectricComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), ElectricComponentList);
			DetailToLampComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), LampComponentList);
			DetailToWaterPosComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), WaterPosComponentList);
			DetailToGasMeterComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), GasMeterComponentList);
			DetailToDiversityWaterComponentMap.Emplace(TPair<FVector, FVector>(LInfo.DetailPoints[Index], LInfo.DetailPoints[Index + 1]), DiversityWaterComponentList);
		}
		DetailToComponentMapList = { DetailToSwitchComponentMap,DetailToElectricComponentMap, DetailToLampComponentMap,DetailToWaterPosComponentMap,DetailToGasMeterComponentMap/*,DetailToDiversityWaterComponentMap */ };

		for (TMap<TPair<FVector, FVector>, TArray<TPair<FVector, int32>>> DetailToComponentMap : DetailToComponentMapList)
		{
			for (auto DetailToComponentIt = DetailToComponentMap.CreateConstIterator(); DetailToComponentIt; ++DetailToComponentIt)
			{
				TPair<FVector, FVector> DetailPair = DetailToComponentIt.Key();
				TArray<TPair<FVector, int32>> ComponentList = DetailToComponentIt.Value();
				if (ComponentList.Num() > 0)
				{
					//最近与最远的端点
					FVector NearstDetailPoint, FarstDetailPoint;
					//筛选全部构件点中距离细节点最近的构建点
					float NearestDis = MAX_FLT;
					//找出距离构件点最近的细节点
					for (TPair<FVector, int32>& Component : ComponentList)
					{
						float Dis = FVector::Distance(Component.Key, DetailPair.Key);
						if (Dis < NearestDis)
						{
							NearstDetailPoint = DetailPair.Key;
							NearestDis = Dis;
						}

						Dis = FVector::Distance(Component.Key, DetailPair.Value);
						if (Dis < NearestDis)
						{
							NearstDetailPoint = DetailPair.Value;
							NearestDis = Dis;
						}
					}
					FarstDetailPoint = NearstDetailPoint == DetailPair.Key ? DetailPair.Value : DetailPair.Key;

					//已知最近的细节点，对其中的构件点进行排序
					//按到最近细节点的距离从小到大进行排序
					ComponentList.Sort([&](const TPair<FVector, int32>& A, const TPair<FVector, int32>& B)->bool {
						return FVector::Distance(NearstDetailPoint, A.Key) < FVector::Distance(NearstDetailPoint, B.Key);
					});

					//开始定位标注
					//对已经进行过排序的构件点进行排序
					//如果相邻的构件的距离大于构件到最远细节点的距离，则开始往反方向标注
					for (int Index = 0; Index < ComponentList.Num(); ++Index)
					{
						FVector DemensionStart, DemensionEnd;

						//遍历该构件后的全部构件之间的距离
						//如果之间的距离全部小于该构件到NearstDetailPoint的距离，则开始往反方向标注
						bool IsNormal = false;
						for (TPair<FVector, int32> ComponentIt : ComponentList)
						{
							if (ComponentIt == ComponentList[Index])
							{
								for (int Count = Index; Count < ComponentList.Num(); ++Count)
								{
									float Distance = 0;
									//判断Index索引后的每段线段之间的距离
									if (ComponentList[Count] == ComponentList[ComponentList.Num() - 1])
									{
										//最后一个构件
										Distance = FVector::Distance(ComponentList[Count].Key, FarstDetailPoint);
									}
									else
									{
										Distance = FVector::Distance(ComponentList[Count].Key, ComponentList[Count + 1].Key);
									}
									if (FVector::Distance(NearstDetailPoint, ComponentList[Index].Key) < Distance)
									{
										//正常
										IsNormal = true;
									}
								}
								break;
							}
						}

						//if (FVector::Distance(NearstDetailPoint, ComponentList[Index].Key) < FVector::Distance(FarstDetailPoint, ComponentList[Index].Key))
						if (IsNormal)
						{
							//开始正常标注
							DemensionStart = NearstDetailPoint;
							DemensionEnd = NearstDetailPoint = ComponentList[Index].Key;
						}
						else//开始往反方向标注
						{
							if (ComponentList[Index] == ComponentList[ComponentList.Num() - 1])
							{
								DemensionStart = ComponentList[Index].Key;
								DemensionEnd = FarstDetailPoint;
							}
							else
							{
								DemensionStart = ComponentList[Index].Key;
								DemensionEnd = ComponentList[Index + 1].Key;
							}
						}

						//计算标尺偏移方向
						FVector CrossVec = FVector::CrossProduct(DemensionStart - DemensionEnd, -FVector::UpVector).GetSafeNormal();
						FVector LCenter = (DemensionStart + DemensionEnd) / 2;

						if (LInfo.RelationInnerRoom.IsValid())
						{
							if (!LInfo.RelationInnerRoom->IsPointInRoom(LCenter + CrossVec))
							{
								CrossVec *= -1;
							}
						}
						else if (LInfo.RelationPillar.IsValid())//防止标注标注在风道、柱子内影响美观
						{
							TArray<FVector> Vertices;
							LInfo.RelationPillar->GetVertexes(Vertices);
							FArmyPolygon Polygon;
							Polygon.SetVertices(Vertices);
							if (Polygon.IsInside(LCenter + CrossVec))
							{
								CrossVec *= -1;
							}
						}
						//标注出最近的构件点到细节点
						TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ComponentList[Index].Value, DemensionStart, DemensionEnd,
							DemensionStart + CrossVec * OffsetValue, DemensionEnd + CrossVec * OffsetValue, OT_Dimensions);
						if (CurrentDimensions.IsValid())
						{
							FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
						}
					}
				}
			}
		}
	}

#pragma region 灯具的标注
	TMap<FObjectWeakPtr, TArray<FObjectWeakPtr>> RoomToLampMap;
	//一、遍历筒灯与房间之间的关系
	for (FObjectWeakPtr LampDownObj : LampDownList)
	{
		for (FObjectWeakPtr RoomWeakPtr : RoomList)
		{
			//获取房间polygon
			// 			TArray<FVector> RoomVertices;
			// 			TArray<TSharedPtr<FArmyLine>> RoomLineList;
			// 			RoomWeakPtr.Pin()->GetLines(RoomLineList);
			// 			for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
			// 			{
			// 				RoomVertices.Emplace(FVector(RoomLine->GetStart().X, RoomLine->GetStart().Y, 0));
			// 				RoomVertices.Emplace(FVector(RoomLine->GetEnd().X, RoomLine->GetEnd().Y, 0));
			// 			}
			// 			FArmyPolygon RoomPolygon;
			// 			RoomPolygon.SetVertices(RoomVertices);

			FArmyRoom * WD = RoomWeakPtr.Pin()->AsassignObj<FArmyRoom>();


			//如果该筒灯在该房间
			//if (RoomPolygon.IsInside(LampDownObj.Pin()->GetBasePos()))
			if (WD->IsPointInRoom(LampDownObj.Pin()->GetBasePos()))
			{
				if (RoomToLampMap.Contains(RoomWeakPtr))
				{
					TArray<FObjectWeakPtr>* ValueObjList = RoomToLampMap.Find(RoomWeakPtr);
					ValueObjList->Emplace(LampDownObj);
				}
				else
				{
					RoomToLampMap.Emplace(RoomWeakPtr, TArray<FObjectWeakPtr>{LampDownObj});
				}
				break;
			}
		}
	}
	//二、遍历每个房间的筒灯
	for (auto RoomToLampMapIt = RoomToLampMap.CreateConstIterator(); RoomToLampMapIt; ++RoomToLampMapIt)
	{
		//用于存储每个房间的灯具之间的标尺
		TArray<TSharedPtr<FArmyDimensions>> HomeDataDismensionList;

		auto ToWallDismensionFunc = [&](const FObjectWeakPtr& ObjWeakPtr) {
			//根据Map信息，寻找出距墙信息
			if (LampToRulerInfoMap.Contains(ObjWeakPtr))
			{
				const TArray<PRulerInfo>& NotOnWallPointInfos = *LampToRulerInfoMap.Find(ObjWeakPtr);
				for (int32 i = 0; i < (NotOnWallPointInfos.Num() > 1 ? NotOnWallPointInfos.Num() : -1); i++)
				{
					//只取距离最短并且不在同一方向上的两个标尺
					if (i > 1) break;
					bool IsHideDismension = false;
					//重复的标注取消显示并不需要标注该灯具距墙尺寸
					for (TSharedPtr<FArmyDimensions> TempDismension : HomeDataDismensionList)
					{
						if ((TempDismension->MainLine->GetStart() - NotOnWallPointInfos[i].P).Size() < 0.001 && (TempDismension->MainLine->GetEnd() - NotOnWallPointInfos[i].TargetPoint).Size() < 0.001 ||
							(TempDismension->MainLine->GetStart() - NotOnWallPointInfos[i].TargetPoint).Size() < 0.001 && (TempDismension->MainLine->GetEnd() - NotOnWallPointInfos[i].P).Size() < 0.001)
						{
							IsHideDismension = true;
						}
					}
					if (IsHideDismension) break;

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)NotOnWallPointInfos[i].ClassType, NotOnWallPointInfos[i].P, NotOnWallPointInfos[i].TargetPoint,
						NotOnWallPointInfos[i].P, NotOnWallPointInfos[i].TargetPoint, OT_Dimensions);

					//是斜边的情况下需要带出标注距离墙的尺寸
					if (FMath::Abs(FVector::DotProduct(NotOnWallPointInfos[i].BaseDir.GetSafeNormal(), FVector::RightVector)) > 0.001&&  //与Y轴不垂直
						FMath::Abs(FVector::DotProduct(NotOnWallPointInfos[i].BaseDir.GetSafeNormal(), FVector::ForwardVector)) > 0.001) //与X轴不垂直
					{
						const FVector& AttachLineStartPos = NotOnWallPointInfos[i].AttachLine->GetStart();
						const FVector& AttachLineEndPos = NotOnWallPointInfos[i].AttachLine->GetEnd();
						const FVector& AttachLineTargetPos = NotOnWallPointInfos[i].TargetPoint;
						const FVector& DimesionOffset = (NotOnWallPointInfos[i].P - NotOnWallPointInfos[i].TargetPoint).GetUnsafeNormal()*OffsetValue;

						const FVector& TargetPos = FVector::Distance(AttachLineStartPos, AttachLineTargetPos)
							< FVector::Distance(AttachLineEndPos, AttachLineTargetPos)
							? AttachLineStartPos : AttachLineEndPos;

						TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)NotOnWallPointInfos[i].ClassType, TargetPos, AttachLineTargetPos,
							TargetPos + DimesionOffset, AttachLineTargetPos + DimesionOffset, OT_Dimensions);

						if (CurrentDimensions.IsValid())
						{
							HomeDataDismensionList.AddUnique(CurrentDimensions);
							FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
						}
					}

					if (CurrentDimensions.IsValid())
					{
						HomeDataDismensionList.AddUnique(CurrentDimensions);
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
			}
		};

		FObjectWeakPtr Room = RoomToLampMapIt.Key();
		TArray<FObjectWeakPtr> LampList = RoomToLampMapIt.Value();

		TArray<TSharedPtr<FArmyLine>> RoomLineList;
		Room.Pin()->GetLines(RoomLineList);

		TArray<TPair<FVector, TArray<FObjectWeakPtr>>> LocalToObjsMap;
		//1、计算与墙平行的灯具列表
		for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
		{
			//计算灯具到墙的距离
			for (FObjectWeakPtr Lamp : LampList)
			{
				FVector OnInfinitePos = FMath::ClosestPointOnInfiniteLine(RoomLine->GetStart(), RoomLine->GetEnd(), Lamp.Pin()->GetBasePos());
				FVector SizeAndDirInfo = Lamp.Pin()->GetBasePos() - OnInfinitePos;
				bool IsCreateNewPair = true;
				for (TPair<FVector, TArray<FObjectWeakPtr>>& LacalToObjs : LocalToObjsMap)
				{
					if ((SizeAndDirInfo - LacalToObjs.Key).Size() < 0.1)
					{
						IsCreateNewPair = false;
						LacalToObjs.Value.Emplace(Lamp);
						break;
					}
				}
				if (IsCreateNewPair)
				{
					LocalToObjsMap.Emplace(TPair<FVector, TArray<FObjectWeakPtr>>(SizeAndDirInfo, TArray<FObjectWeakPtr> { Lamp }));
				}
			}
		}
		//2、对所有平行于墙的灯具列表进行排序
		for (TPair<FVector, TArray<FObjectWeakPtr>>& LocalToObjs : LocalToObjsMap)
		{
			if (LocalToObjs.Value.Num() >= 2)
			{
				LocalToObjs.Value.Sort([&](const FObjectWeakPtr& A, const FObjectWeakPtr& B)->bool {
					if (FMath::Abs(A.Pin()->GetBasePos().X - B.Pin()->GetBasePos().X) < 0.5)
					{
						//按Y轴排序
						return A.Pin()->GetBasePos().Y < B.Pin()->GetBasePos().Y;
					}
					else //(FMath::Abs(A.Pin()->GetBasePos().Y - B.Pin()->GetBasePos().Y) < 0.001)
					{
						//按X轴排序
						return A.Pin()->GetBasePos().X < B.Pin()->GetBasePos().X;
					}
					return false;
				});
			}
		}
		//3、标记出平行于墙的的灯具之间的距离标尺
		TArray<FObjectWeakPtr> AlreadyDismensionLampList;
		for (TPair<FVector, TArray<FObjectWeakPtr>> LocalToObjs : LocalToObjsMap)
		{
			if (LocalToObjs.Value.Num() >= 2)
			{
				bool IsHideToWallDismensions = false;
				//*****标注灯具之间的标尺*****//
				for (int32 Index = 1; Index < LocalToObjs.Value.Num(); ++Index)
				{
					FArmyFurniture* Furniture = LocalToObjs.Value[Index].Pin()->AsassignObj<FArmyFurniture>();

					const FVector& StartPos = LocalToObjs.Value[Index - 1].Pin()->GetBasePos();
					const FVector& EndPos = LocalToObjs.Value[Index].Pin()->GetBasePos();

					bool IsHideDismension = false;
					//1、距离为0的标尺取消显示
					if (FVector::Distance(StartPos, EndPos) < 0.001)
					{
						continue;
					}
					//2、重复的标注取消显示
					for (TSharedPtr<FArmyDimensions> TempDismension : HomeDataDismensionList)
					{
						//判断两条标注是后首尾一致
						if ((TempDismension->MainLine->GetStart() - StartPos).Size() < 0.001 && (TempDismension->MainLine->GetEnd() - EndPos).Size() < 0.001 ||
							(TempDismension->MainLine->GetStart() - EndPos).Size() < 0.001 && (TempDismension->MainLine->GetEnd() - StartPos).Size() < 0.001)
						{
							IsHideDismension = true;
							break;
						}
						// 						//判断两天标注是否是构成一条矩形
						// 						else if (FMath::Abs(FVector::Distance(TempDismension->MainLine->GetStart(), StartPos) - FVector::Distance(TempDismension->MainLine->GetEnd(), EndPos)) < 0.05&&
						// 							FMath::Abs(FVector::Distance(TempDismension->MainLine->GetEnd(), StartPos) - FVector::Distance(TempDismension->MainLine->GetStart(), EndPos)) < 0.05)
						// 						{
						// 							IsHideDismension = true;
						// 							break;
						// 						}
					}
					//3、如果标注已灯具列表的首尾长度一致，则取消标注
					for (TPair<FVector, TArray<FObjectWeakPtr>> TempLocalToObjs : LocalToObjsMap)
					{
						if (TempLocalToObjs.Value.Num() >= 2)
						{
							if (FMath::Abs(FVector::Distance(EndPos, StartPos) - FVector::Distance(TempLocalToObjs.Value.Top().Pin()->GetBasePos(), TempLocalToObjs.Value.HeapTop().Pin()->GetBasePos())) < 0.1)
							{
								//隐藏自身尺寸的同时也需要隐藏距墙尺寸
								IsHideToWallDismensions = true;
								IsHideDismension = true;
								break;
							}
						}
					}
					if (IsHideDismension)
					{
						continue;
					}

					AlreadyDismensionLampList.AddUnique(LocalToObjs.Value[Index - 1]);
					AlreadyDismensionLampList.AddUnique(LocalToObjs.Value[Index]);

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)(Furniture->BelongClass), StartPos, EndPos,
						StartPos, EndPos, OT_Dimensions);
					if (CurrentDimensions.IsValid())
					{
						HomeDataDismensionList.AddUnique(CurrentDimensions);
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}

				//****标注出灯具首/尾距墙的尺寸*****//
				if (!IsHideToWallDismensions)
				{
					const TArray<PRulerInfo>& TopPointInfos = *LampToRulerInfoMap.Find(LocalToObjs.Value.Top());
					const TArray<PRulerInfo>& HeapPointInfos = *LampToRulerInfoMap.Find(LocalToObjs.Value.HeapTop());
					//先找出平行灯具列表的首尾灯具哪个距墙尺寸最近
					auto GetDimensionSize = [](const TArray<PRulerInfo>& PointInfos)->float {
						return FVector::Distance(PointInfos[0].TargetPoint, PointInfos[0].P)*FVector::Distance(PointInfos[1].TargetPoint, PointInfos[1].P);
					};
					if (TopPointInfos.Num() > 1 && HeapPointInfos.Num() > 1)
					{
						//判断首尾灯具的尺寸(选取两端中最近的尺寸)
						GetDimensionSize(TopPointInfos) < GetDimensionSize(HeapPointInfos) ?
							ToWallDismensionFunc(LocalToObjs.Value.Top()) : ToWallDismensionFunc(LocalToObjs.Value.HeapTop());

						//两端都选择标注
						//ToWallDismensionFunc(LocalToObjs.Value.Top());
						//ToWallDismensionFunc(LocalToObjs.Value.HeapTop());
					}
				}
			}
		}
		//3、标记出和其他灯具构成的直线没有平行于墙体的灯具的距墙尺寸
		for (FObjectWeakPtr AlreadyDimesntion : AlreadyDismensionLampList)
		{
			if (LampList.Contains(AlreadyDimesntion))
			{
				LampList.Remove(AlreadyDimesntion);
			}
		}
		for (FObjectWeakPtr WithoutDimesntion : LampList)
		{
			ToWallDismensionFunc(WithoutDimesntion);
		}
	}
#pragma endregion 灯具的标注
}
void FArmyConstructionFunction::GenerateAutoObjectRuler()
{
	TArray<FObjectWeakPtr> OutRoomList, InternalRoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, OutRoomList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, InternalRoomList);

	//每个房间对应的该房间的线段哈希表
	TMap<FObjectWeakPtr, TArray<TSharedPtr<FArmyLine>>> InternalRoomLineMap;
	//获取内墙线
	for (auto TempObjIt : InternalRoomList)
	{
		TArray<TSharedPtr<FArmyLine>> TempLines;
		TempObjIt.Pin()->GetLines(TempLines);
		InternalRoomLineMap.Emplace(TempObjIt, TempLines);
	}

	//获取外墙线
	TArray<TSharedPtr<FArmyLine>> OutRoomLineList;
	for (auto TempObjIt : OutRoomList)
	{
		TArray<TSharedPtr<FArmyLine>> TempLines;
		TempObjIt.Pin()->GetLines(TempLines);
		OutRoomLineList.Append(TempLines);
	}

	TArray<TWeakPtr<FArmyObject>> AllObjectArray;
	AllObjectArray.Append(FArmySceneData::Get()->GetObjects(E_LayoutModel));
	AllObjectArray.Append(FArmySceneData::Get()->GetObjects(E_HardModel));
	AllObjectArray.Append(FArmySceneData::Get()->GetObjects(E_HydropowerModel));
	AllObjectArray.Append(FArmySceneData::Get()->GetObjects(E_ConstructionModel));

	for (auto ObjIt : AllObjectArray)
	{
		ObjectClassType ClassType = Class_Other;
		ObjectClassType HasLabel = Class_Other;
		switch (ObjIt.Pin()->GetType())
		{
		case OT_Door:
		case OT_SlidingDoor:
		{
			ClassType = Class_Door;
			HasLabel = Class_AnnotationDoor;
		}
		break;
		case OT_Pass://入口
		case OT_SecurityDoor:
		case OT_DoorHole:
		{
			ClassType = Class_Pass;
			HasLabel = Class_Pass;
		}
		break;
		case OT_InternalRoom:
		case OT_Beam: // 重构后的梁
		{
			ClassType = Class_BaseWall;
			HasLabel = Class_BaseWall;
		}
		break;
		case OT_Window:
		case OT_FloorWindow:
		case OT_RectBayWindow:
		case OT_TrapeBayWindow:
		case OT_CornerBayWindow:
		{
			ClassType = Class_Window;
			HasLabel = Class_AnnotationWindow;
		}
		break;
		case OT_ComponentBase:
		{
			TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ObjIt.Pin());
			if (Furniture.IsValid())
			{
				ClassType = ObjectClassType(Furniture->BelongClass);
				switch (ClassType)
				{
				case Class_ElectricBoxH:
					HasLabel = Class_AnnotationElectricBoxH;
					break;
				case Class_ElectricBoxL:
					HasLabel = Class_AnnotationElectricBoxL;
					break;
				case Class_AirConditionerPoint:
					HasLabel = Class_AnnotationAirConditionerPoint;
					break;
				case Class_Furniture:
					HasLabel = Class_AnnotationFurniture;
					break;
				case Class_Switch:
					HasLabel = Class_AnnotationSwitch;
					break;
				case Class_Socket:
					HasLabel = Class_AnnotationSocket;
					break;
				case Class_Socket_H:
					HasLabel = Class_AnnotationSocket_H;
					break;
				case Class_Socket_L:
					HasLabel = Class_AnnotationSocket_L;
					break;
				case Class_WaterRoutePoint:
					HasLabel = Class_WaterRoutePoint;
					break;
				case Class_HotWaterPoint://热水点位
					HasLabel = Class_AnnotationHotWaterPoint;
					break;
				case Class_HotAndColdWaterPoint://冷热水点位
					HasLabel = Class_AnnotationHotAndColdWaterPoint;
					break;
				case Class_ColdWaterPoint://冷水点位
					HasLabel = Class_AnnotationColdWaterPoint;
					break;
				case Class_FloordRainPoint://地漏下水
					HasLabel = Class_AnnotationFloordRainPoint;
					break;
				case Class_ClosestoolRainPoint://马桶下水
					HasLabel = Class_AnnotationClosestoolRainPoint;
					break;
				case Class_WashBasinRainPoint://水盆下水
					HasLabel = Class_AnnotationWashBasinRainPoint;
					break;
				case Class_RecycledWaterPoint://中水点位
					HasLabel = Class_AnnotationRecycledWaterPoint;
					break;
				case Class_HomeEntryWaterPoint://进户水
					HasLabel = Class_AnnotationHomeEntryWaterPoint;
					break;
				case Class_Light:
					HasLabel = Class_AnnotationLamp;
					break;
				default:
					break;
				}
			}
		}
		break;
		default:
			break;
		}

		if (ClassType == Class_Other) continue;

		if (ClassType == Class_Switch)//更新开关控制图
		{
			CreateLampControlLines(ObjIt.Pin());
		}
		if (HasLabel != Class_Other)
		{
			FText LabelText;
			switch (ObjIt.Pin()->GetType())
			{
			case OT_Pass:		  // 入口
			case OT_DoorHole:	  // 入口添加了防盗门后就是门洞了
			case OT_SecurityDoor: // 防盜门
			{
				FArmyHardware* XRPass = ObjIt.Pin()->AsassignObj<FArmyHardware>();

				FBox Box = XRPass->GetBounds();
				FVector TargetPos(Box.GetCenter().X, Box.GetCenter().Y, 0);

				//获取Box的四个点
				{
					FVector StartPos = XRPass->GetStartPos();
					FVector EndPos = XRPass->GetEndPos();

					FVector LinkLine = EndPos - StartPos;
					//获取垂直方向单位向量
					FVector SafeNormal = FVector::CrossProduct(LinkLine, FVector(0, 0, 1)).GetSafeNormal();
					FVector HalfThicknessVec = SafeNormal*(XRPass->GetWidth())*0.5f;
					FVector CornerPos1 = StartPos + HalfThicknessVec;
					FVector CornerPos2 = StartPos - HalfThicknessVec;
					FVector CornerPos3 = EndPos + HalfThicknessVec;
					FVector CornerPos4 = EndPos - HalfThicknessVec;

					//遍历全部外墙线段 是否与这四个点相交
					for (auto TempLine : OutRoomLineList)
					{
						//设置偏移量(入口图标默认偏移的长度为墙体的厚度)
						const float Distance = XRPass->GetWidth();
						float Dist1 = FMath::PointDistToSegment(CornerPos1, TempLine->GetStart(), TempLine->GetEnd());
						float Dist2 = FMath::PointDistToSegment(CornerPos2, TempLine->GetStart(), TempLine->GetEnd());
						float Dist3 = FMath::PointDistToSegment(CornerPos3, TempLine->GetStart(), TempLine->GetEnd());
						float Dist4 = FMath::PointDistToSegment(CornerPos4, TempLine->GetStart(), TempLine->GetEnd());
						if ((Dist1 < 0.05f) && (Dist3 < 0.05f))//此处误差较大，暂时先设定为0.05
						{
							const FVector ToInDirect = (CornerPos2 - CornerPos1).GetSafeNormal();
							const FVector TargetPosOffset = -ToInDirect*Distance;
							//创建入口图标-->位置与方向(由外指向内)
							CreateObjectEntrance(ObjIt.Pin(), TargetPos + TargetPosOffset, ToInDirect);
							break;
						}
						else if ((Dist2 < 0.05f) && (Dist4 < 0.05f))//此处误差较大，暂时先设定为0.05
						{
							const FVector ToInDirect = (CornerPos1 - CornerPos2).GetSafeNormal();
							FVector TargetPosOffset = -ToInDirect*Distance;
							CreateObjectEntrance(ObjIt.Pin(), TargetPos + TargetPosOffset, ToInDirect);
							break;
						}
					}
				}
				//入口和门洞不需要尺寸标签
				if (ObjIt.Pin()->GetType() == OT_Pass || ObjIt.Pin()->GetType() == OT_DoorHole)
				{
					break;
				}
			}
			case OT_Door:
			case OT_SlidingDoor:
			{
				FArmyHardware* WD = ObjIt.Pin()->AsassignObj<FArmyHardware>();
				if (WD)
				{
					//FVector PosDir = FRotator(0, -90, 0).RotateVector(WD->GetDirection());
					FVector PosDir = WD->GetDirection();

					float Offset = 10;
					if ((PosDir.GetSafeNormal() + FVector(1, 0, 0)).Size() < 0.5 || (PosDir.GetSafeNormal() + FVector(0, 1, 0)).Size() < 0.5)
					{
						Offset = 50;
					}
					FVector Pos = WD->GetPos() + PosDir.GetSafeNormal() * (WD->GetWidth() + Offset);
					FVector TestPoint = WD->GetPos() + PosDir * WD->GetWidth();
					TSharedPtr<FArmyRoom> Room = WD->FirstRelatedRoom.IsValid() ? WD->FirstRelatedRoom : WD->SecondRelatedRoom;
					if (Room.IsValid() && Room->IsPointInRoom(TestPoint))
					{
						Offset = 10;
						if ((PosDir.GetSafeNormal() - FVector(1, 0, 0)).Size() < 0.5 || (PosDir.GetSafeNormal() - FVector(0, 1, 0)).Size() < 0.5)
						{
							Offset = 50;
						}
						Pos = WD->GetPos() - PosDir.GetSafeNormal() * (WD->GetWidth() + Offset);
					}

					LabelText = FText::FromString(TEXT("H:" + FString::FromInt(FMath::CeilToInt(WD->GetHeight() * 10)) + "\nW:" + FString::FromInt(FMath::CeilToInt(WD->GetLength() * 10)) + "\nD:" + FString::FromInt(FMath::CeilToInt(WD->GetWidth() * 10))));
					CreateObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos);
				}
			}
			break;

			case OT_Beam: // 重构后的房梁
			{
				FArmyBeam* Beam = ObjIt.Pin()->AsassignObj<FArmyBeam>();
				if (Beam)
				{
					//获取梁标签的中心点
					TArray<FVector> BoundVertexes;
					Beam->GetVertexes(BoundVertexes);
					FVector TargetPos = FBox(BoundVertexes).GetCenter();

					FVector PosOffset(-15, -15, 0);
					TargetPos += PosOffset;

					LabelText = FText::FromString(FString::Printf(TEXT("LH:%.0f\nLW:%.0f"), Beam->GetHeight()*10.0f, Beam->GetWidth()*10.0f));

					CreateObjectTextLable(ObjIt.Pin(), -1, LabelText, TargetPos, FArmyTextLabel::LabelType::LT_Beam);
				}
			}
			break;
			case OT_ComponentBase://开关、插座、水位、强弱电箱的标高
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ObjIt.Pin());
				if (Furniture.IsValid())
				{
					ClassType = ObjectClassType(Furniture->BelongClass);
					switch (ClassType)
					{
					case Class_Light:  //壁灯
					{
						bool IsOnWallLamp = false;
						//根据该灯具是否靠墙判断该灯具是否是壁灯
						for (auto InnerRoomLineMapIt : InternalRoomLineMap)
						{
							for (TSharedPtr<FArmyLine> Line : InnerRoomLineMapIt.Value)
							{
								const FVector& PosOnLine = FMath::ClosestPointOnLine(Line->GetStart(), Line->GetEnd(), Furniture->GetBasePos());
								float dis = FVector::Distance(PosOnLine, Furniture->GetBasePos());
								if (FVector::Distance(PosOnLine, Furniture->GetBasePos()) < 0.05)//该点在墙上，是壁灯
								{
									IsOnWallLamp = true;
									break;
								}
							}
							if (IsOnWallLamp)
							{
								break;
							}
						}
						if (!IsOnWallLamp)
						{
							break;
						}
					}
					case Class_Switch:				//开关
					case Class_Socket:
					case Class_Socket_H:			//插座
					case Class_Socket_L:			//插座
					case Class_WaterRoutePoint:		//水位
					case Class_HotWaterPoint://热水点位
					case Class_HotAndColdWaterPoint://冷热水点位
					case Class_ColdWaterPoint://冷水点位
					case Class_FloordRainPoint://地漏下水
					case Class_ClosestoolRainPoint://马桶下水
					case Class_WashBasinRainPoint://水盆下水
					case Class_RecycledWaterPoint://中水点位
					case Class_HomeEntryWaterPoint://进户水
					case Class_ElectricBoxH:		//强弱电箱
					case Class_ElectricBoxL:
					{
						//获取组件位置
						FVector TargetPos = Furniture->GetBasePos();
						//遍历每个房间，再遍历该房间中的每条线段是否与该组件中心点相交
						TArray<FVector> InternalRoomPositionList;
						FVector InnerRoomCenter = FVector::ZeroVector;
						TSharedPtr<FArmyLine> TargetLine;
						for (auto MapIt = InternalRoomLineMap.CreateConstIterator(); MapIt; ++MapIt)
						{
							for (auto LineTt : MapIt.Value())
							{
								//点到直线距离
								const float Dist = FMath::PointDistToSegment(TargetPos, LineTt->GetStart(), LineTt->GetEnd());
								if (Dist < 0.05f)
								{
									//存储相交的线段
									TargetLine = LineTt;
									//该组件的点属于在该房间内
									//遍历获取该房间的线段中的所有点
									for (auto TempLine : MapIt.Value())
									{
										FArmyRoom* Room = MapIt.Key().Pin()->AsassignObj<FArmyRoom>();
										//获取房间的中心点
										FBox RoomBox = Room->GetBounds();
										InnerRoomCenter = RoomBox.GetCenter();
										FVector FirstProjPos = FVector::ZeroVector;
										FVector SecondProjPos = FVector::ZeroVector;
										if (!Room->IsPointInRoom(InnerRoomCenter))
										{
											Room->CalPointToRoomLineMinDis(InnerRoomCenter, FirstProjPos, SecondProjPos);
											InnerRoomCenter = (FirstProjPos + SecondProjPos) / 2;
										}

										InternalRoomPositionList.AddUnique(TempLine->GetStart());
										InternalRoomPositionList.AddUnique(TempLine->GetEnd());
									}
									break;
								}
							}
						}
						if (InternalRoomPositionList.Num() > 0)
						{
							//根据房间的所有点获取该房间的中心点
							//并确定偏移方向
							// 							FBox Box(InternalRoomPositionList);
							// 							const FVector& InnerRoomCenter = Box.GetCenter();
							const FVector& NearstPos = FMath::ClosestPointOnInfiniteLine(TargetLine->GetStart(), TargetLine->GetEnd(), InnerRoomCenter);
							FVector OffsetDir = (InnerRoomCenter - NearstPos).GetSafeNormal();
							//构建房建的polygon
							FArmyPolygon Polygon;
							Polygon.SetVertices(InternalRoomPositionList);
							if (!Polygon.IsInside(TargetPos + OffsetDir))
							{
								OffsetDir *= -1;
							}
							//设置偏移量
							FVector OffsetVec = OffsetDir * 20;
							LabelText = FText::FromString(FString::Printf(TEXT("H=%.0fmm"), Furniture->GetAltitude() * 10));
							const float FontSize = 10;
							//如果是水平方向，再偏移一段距离
							//if (FMath::Abs(FVector::DotProduct(OffsetDir, FVector::RightVector)) < 0.001f)
							{
								//向左偏移
								if (FVector::Distance(OffsetDir, FVector(-1, 0, 0)) < 0.001f)
								{
									//根据字符串的长度来计算偏移量
									OffsetVec = OffsetDir *  LabelText.ToString().GetCharArray().Num() * FontSize * 0.5f;
								}
								//向右偏移
								else if (FVector::Distance(OffsetDir, FVector(1, 0, 0)) < 0.001f)
								{
									OffsetVec = OffsetDir * 38;
								}
							}
							//垂直方向
							//else if ((FMath::Abs(FVector::DotProduct(OffsetDir, FVector::RightVector)) - 1) < 0.001)
							{
								//向上偏移
								if (FVector::Distance(OffsetDir, FVector(0, -1, 0)) < 0.001f)
								{
									OffsetVec = FVector(0, -1, 0) * 40;
								}
								//向下偏移
								else if (FVector::Distance(OffsetDir, FVector(0, 1, 0)) < 0.001f)
								{
									OffsetVec = OffsetDir * 30;
								}
							}

							TSharedPtr<FArmyTextLabel> TextLabel = CreateObjectTextLable(ObjIt.Pin(), -1, LabelText, TargetPos + OffsetVec, FArmyTextLabel::LabelType::LT_None);
							if (TextLabel.IsValid())
							{
								TextLabel->SetTextSize(FontSize);
							}
						}
						else
						{
							//该组件处于地面上获取偏差太大
							//地面上的插座或开关暂不需要标注高度
						}
						break;
					}
					default:
						break;
					}
				}
			}
			break;
			case OT_InternalRoom:	//@ 梁晓菲 房间标注
			{
				FBox Box = ObjIt.Pin()->GetBounds();
				FVector Pos(0, 0, 0);
				Pos.X = Box.GetCenter().X - 45;
				Pos.Y = Box.GetCenter().Y + 30;

				FArmyRoom * WD = ObjIt.Pin()->AsassignObj<FArmyRoom>();

				float TempArea = 0.f;
				float TempPerimeter = 0.f;
				WD->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);

				// 不生成无效的房间标注
				if (Pos.Equals(FVector::ZeroVector) || TempArea == 0.f || TempPerimeter == 0.f)
				{
					continue;
				}

				if (WD)
				{
					//计算出中心点的位置
					FBox RoomBox = WD->GetBounds();
					Pos = RoomBox.GetCenter();
					float MinDis = FLT_MAX;
					FVector FirstProjPos = FVector::ZeroVector;
					FVector SecondProjPos = FVector::ZeroVector;
					if (!WD->IsPointInRoom(Pos))
					{
						WD->CalPointToRoomLineMinDis(Pos, FirstProjPos, SecondProjPos);
						Pos = (FirstProjPos + SecondProjPos) / 2;
					}

					LabelText = FText::FromString(WD->GetSpaceName());
					TSharedPtr<FArmyTextLabel> SpaceNameLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(0, 0, 0), FArmyTextLabel::LT_SpaceName, TEXT("AUTO_LAYOUT"),true);
					if (SpaceNameLabel.IsValid())
					{
						SpaceNameLabel->SetTextSize(15);
					}
					LabelText = FText::FromString((TEXT("面积:") + FString::Printf(TEXT("%.1f"), TempArea) + TEXT("㎡")));
					TSharedPtr<FArmyTextLabel> AreaLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(15, 26, 0), FArmyTextLabel::LT_SpaceArea, TEXT("AUTO_LAYOUT"));
					if (AreaLabel.IsValid())
					{
						AreaLabel->SetTextSize(10);
					}
					LabelText = FText::FromString((TEXT("周长:") + FString::Printf(TEXT("%.1f"), TempPerimeter) + TEXT("m")));
					TSharedPtr<FArmyTextLabel> PerimeterText = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(15, 44, 0), FArmyTextLabel::LT_SpacePerimeter, TEXT("AUTO_LAYOUT"));
					if (PerimeterText.IsValid())
					{
						PerimeterText->SetTextSize(10);
					}
					LabelText = FText::FromString((TEXT("室内净高:") + FString::FromInt(FMath::CeilToInt(FArmySceneData::WallHeight * 10)) + TEXT("mm")));
					TSharedPtr<FArmyTextLabel> HeightLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(-10, 92, 0), FArmyTextLabel::LT_SpaceHeight, TEXT("AUTO_LAYOUT"));
					if (HeightLabel.IsValid())
					{
						HeightLabel->SetTextSize(10);
						HeightLabel->SetbUseEditPos(true);
					}
					LabelText = FText::FromString((TEXT("完成地面高度:") + FString::FromInt(FMath::CeilToInt(WD->GetFinishGroundHeight())) + TEXT("mm")));
					TSharedPtr<FArmyTextLabel> PostHeightLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(-10, 132, 0), FArmyTextLabel::LT_PostGroundHeight, TEXT("AUTO_LAYOUT"),true);
					if (PostHeightLabel.IsValid())
					{
						PostHeightLabel->SetTextSize(10);
						PostHeightLabel->SetbUseEditPos(true);
					}
				}
			}
			break;
			case OT_Window:
			case OT_FloorWindow:
			{
				FArmyWindow* WD = ObjIt.Pin()->AsassignObj<FArmyWindow>();
				if (WD)
				{
					float Offset = 10;
					if ((WD->GetDirection().GetSafeNormal() + FVector(1, 0, 0)).Size() < 0.5 || (WD->GetDirection().GetSafeNormal() + FVector(0, 1, 0)).Size() < 0.5)
					{
						Offset = 20;
					}
					FVector Pos = WD->GetPos() + WD->GetDirection().GetSafeNormal() * (WD->GetWidth() + Offset);
					FVector TestPoint = WD->GetPos() + WD->GetDirection().GetSafeNormal() * WD->GetWidth();
					TSharedPtr<FArmyRoom> Room = WD->FirstRelatedRoom.IsValid() ? WD->FirstRelatedRoom : WD->SecondRelatedRoom;
					if (Room.IsValid() && Room->IsPointInRoom(TestPoint))
					{
						Offset = 10;
						if ((WD->GetDirection().GetSafeNormal() - FVector(1, 0, 0)).Size() < 0.5 || (WD->GetDirection().GetSafeNormal() - FVector(0, 1, 0)).Size() < 0.5)
						{
							Offset = 20;
						}
						Pos = WD->GetPos() - WD->GetDirection().GetSafeNormal() * (WD->GetWidth() + Offset);
					}
					LabelText = FText::FromString(TEXT("CH:" + FString::FromInt(FMath::CeilToInt(WD->GetHeight() * 10)) + "\nTH:" + FString::FromInt(FMath::CeilToInt(WD->GetHeightToFloor() * 10)) /*+ "\nCW:" + FString::FromInt(FMath::CeilToInt(WD->GetLength() * 10))*/ + "\nCD:" + FString::FromInt(FMath::CeilToInt(WD->GetWindowDeep() * 10))));
					CreateObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos);
				}
			}
			break;
			case OT_RectBayWindow:
			case OT_TrapeBayWindow:
			{
				FArmyRectBayWindow* WD = ObjIt.Pin()->AsassignObj<FArmyRectBayWindow>();
				if (WD)
				{
					float Offset = WD->GetWindowDeep() + 50;
					if ((WD->GetDirection().GetSafeNormal() + FVector(1, 0, 0)).Size() < 0.5 || (WD->GetDirection().GetSafeNormal() + FVector(0, 1, 0)).Size() < 0.5)
					{
						Offset = WD->GetWindowDeep() + 50;
					}
					FVector Pos = WD->GetPos() + WD->GetDirection().GetSafeNormal() * (WD->GetWidth() + Offset);
					FVector TestPoint = WD->GetPos() + WD->GetDirection().GetSafeNormal() * WD->GetWidth();
					TSharedPtr<FArmyRoom> Room = WD->FirstRelatedRoom.IsValid() ? WD->FirstRelatedRoom : WD->SecondRelatedRoom;
					if (Room.IsValid() && Room->IsPointInRoom(TestPoint))
					{
						Offset = WD->GetWindowDeep() + 50;
						if ((WD->GetDirection().GetSafeNormal() - FVector(1, 0, 0)).Size() < 0.5 || (WD->GetDirection().GetSafeNormal() - FVector(0, 1, 0)).Size() < 0.5)
						{
							Offset = WD->GetWindowDeep() + 50;
						}
						Pos = WD->GetPos() - WD->GetDirection().GetSafeNormal() * (WD->GetWidth() + Offset);
					}

					LabelText = FText::FromString(TEXT("CH:" + FString::FromInt(FMath::CeilToInt(WD->GetHeight() * 10)) + "\nTH:" + FString::FromInt(FMath::CeilToInt((WD->GetHeightToFloor()/* + WD->GetWindowBottomBoardTickness()*/) * 10)) /*+ "\nCW:" + FString::FromInt(FMath::CeilToInt(WD->GetWidth() * 10))*/ + "\nCD:" + FString::FromInt(FMath::CeilToInt(WD->GetWindowHoleDepth() * 10))));
					CreateObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos);
				}
			}
			break;
			case OT_CornerBayWindow:
			{
				FArmyCornerBayWindow* WD = ObjIt.Pin()->AsassignObj<FArmyCornerBayWindow>();
				if (WD)
				{
					FVector LeftPos = (WD->LeftUpPointPos + WD->GetPos()) / 2 - WD->RightDirection.GetSafeNormal() * WD->GetWindowHoleDepth();
					FVector RightPos = (WD->RightUpPointPos + WD->GetPos()) / 2 - WD->LeftDirection.GetSafeNormal() * WD->GetWindowHoleDepth();

					LabelText = FText::FromString(TEXT("CH:" + FString::FromInt(FMath::CeilToInt(WD->GetHeight() * 10)) + "\nTH:" + FString::FromInt(FMath::CeilToInt((WD->GetHeightToFloor() + WD->GetWindowBottomBoardHeight()) * 10)) /*+ "\nCW:" + FString::FromInt(FMath::CeilToInt(WD->GetLeftWindowLength() * 10))*/ + "\nCD:" + FString::FromInt(FMath::CeilToInt(WD->GetWindowHoleDepth() * 10))));
					CreateObjectTextLable(ObjIt.Pin(), 1, LabelText, LeftPos);

					LabelText = FText::FromString(TEXT("CH:" + FString::FromInt(FMath::CeilToInt(WD->GetHeight() * 10)) + "\nTH:" + FString::FromInt(FMath::CeilToInt((WD->GetHeightToFloor() + WD->GetWindowBottomBoardHeight()) * 10)) /*+ "\nCW:" + FString::FromInt(FMath::CeilToInt(WD->GetRightWindowLength() * 10))*/ + "\nCD:" + FString::FromInt(FMath::CeilToInt(WD->GetWindowHoleDepth() * 10))));
					CreateObjectTextLable(ObjIt.Pin(), 2, LabelText, RightPos);
				}
			}
			break;
			default:
				break;
			}
		}
	}


	TArray<TWeakPtr<FArmyObject>> HomeModelObjArray;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, HomeModelObjArray);
	for (auto ObjIt : HomeModelObjArray)
	{
		FText LabelText;
		switch (ObjIt.Pin()->GetType())
		{
		case OT_InternalRoom:
		{
			FBox Box = ObjIt.Pin()->GetBounds();
			FVector Pos(0, 0, 0);
			Pos.X = Box.GetCenter().X - 45;
			Pos.Y = Box.GetCenter().Y + 30;

			FArmyRoom * WD = ObjIt.Pin()->AsassignObj<FArmyRoom>();

			float TempArea = 0.f;
			float TempPerimeter = 0.f;
			WD->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);

			// 不生成无效的房间标注
			if (Pos.Equals(FVector::ZeroVector) || TempArea == 0.f || TempPerimeter == 0.f)
			{
				continue;
			}

			if (WD)
			{
				//获取房间的中心点
				FBox RoomBox = WD->GetBounds();
				Pos = RoomBox.GetCenter();
				float MinDis = FLT_MAX;
				FVector FirstProjPos = FVector::ZeroVector;
				FVector SecondProjPos = FVector::ZeroVector;
				if (!WD->IsPointInRoom(Pos))
				{
					WD->CalPointToRoomLineMinDis(Pos, FirstProjPos, SecondProjPos);
					Pos = (FirstProjPos + SecondProjPos) / 2;
				}

				LabelText = FText::FromString(WD->GetSpaceName());
				TSharedPtr<FArmyTextLabel> SpaceNameLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(0, -40, 0), FArmyTextLabel::LT_SpaceName, TEXT("AUTO_ORIGIN"),true);
				if (SpaceNameLabel.IsValid())
				{
					SpaceNameLabel->SetTextSize(18);
				}
				LabelText = FText::FromString((TEXT("面积:") + FString::Printf(TEXT("%.1f"), TempArea) + TEXT("㎡")));
				TSharedPtr<FArmyTextLabel> AreaLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(15, -14, 0), FArmyTextLabel::LT_SpaceArea, TEXT("AUTO_ORIGIN"));
				if (AreaLabel.IsValid())
				{
					AreaLabel->SetTextSize(10);
				}
				LabelText = FText::FromString((TEXT("周长:") + FString::Printf(TEXT("%.1f"), TempPerimeter) + TEXT("m")));
				TSharedPtr<FArmyTextLabel> PerimeterText = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(15, 4, 0), FArmyTextLabel::LT_SpacePerimeter, TEXT("AUTO_ORIGIN"));
				if (PerimeterText.IsValid())
				{
					PerimeterText->SetTextSize(10);
				}
				LabelText = FText::FromString((TEXT("室内净高:") + FString::FromInt(FMath::CeilToInt(FArmySceneData::WallHeight * 10)) + TEXT("mm")));
				TSharedPtr<FArmyTextLabel> HeightLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(-10, 52, 0), FArmyTextLabel::LT_SpaceHeight, TEXT("AUTO_ORIGIN"));
				if (HeightLabel.IsValid())
				{
					HeightLabel->SetTextSize(10);
					HeightLabel->SetbUseEditPos(true);
				}
				LabelText = FText::FromString((TEXT("原始地面高度:") + FString::FromInt(FMath::CeilToInt(WD->GetOriginGroundHeight())) + TEXT("mm")));
				TSharedPtr<FArmyTextLabel> OriginHeightLabel = CreateOriginObjectTextLable(ObjIt.Pin(), -1, LabelText, Pos + FVector(-10, 92, 0), FArmyTextLabel::LT_BaseGroundHeight, TEXT("AUTO_ORIGIN"),true);
				if (OriginHeightLabel.IsValid())
				{
					OriginHeightLabel->SetTextSize(10);
					OriginHeightLabel->SetbUseEditPos(true);
				}
			}
			break;
		}
		}
	}

	TArray<FObjectWeakPtr> LampControlObj;
	FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_LampControlLine, LampControlObj);
	for (auto & Obj : LampControlObj)
	{
		if (Obj.Pin().IsValid())
		{
			Obj.Pin()->Refresh();
		}
	}
}
void FArmyConstructionFunction::GenerateCeilingDimensions(const TArray<InnerLineInfo>& InInnerRoomLines)
{
	TArray<FObjectWeakPtr> InternalRoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, InternalRoomList);
	//获取吊顶
	TArray<FObjectWeakPtr> BaseAreaArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CurtainBox, BaseAreaArray);//窗帘盒和吊顶类似

																				//获取圆形吊顶
	TArray<FObjectWeakPtr> CircleAreaArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_CircleArea, CircleAreaArray);

	//获取该房间的全部吊顶、梁、窗帘盒线段
	TArray<FObjectWeakPtr> ObjArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Beam, ObjArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CurtainBox, ObjArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, ObjArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_CircleArea, ObjArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, ObjArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, ObjArray);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, ObjArray);//在拆改中模式下获取墙体
#pragma region 矩形吊顶
																		//判断矩形吊顶是在哪个房间内
	TArray<TPair<TSharedPtr<FArmyLine>, EAreaEnum>> AllDemensionList;
	for (FObjectWeakPtr RectAreaWeakPtr : BaseAreaArray)
	{
		EAreaEnum AreaType = EAreaEnum::NoneArea;
		ObjectClassType ClassType = Class_Other;
		for (FObjectWeakPtr RoomWeakPtr : InternalRoomList)
		{
			TArray<FVector> RoomVertices;
			TArray<TSharedPtr<FArmyLine>> RoomLineList;
			RoomWeakPtr.Pin()->GetLines(RoomLineList);
			for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
			{
				RoomVertices.Emplace(FVector(RoomLine->GetStart().X, RoomLine->GetStart().Y, 0));
				RoomVertices.Emplace(FVector(RoomLine->GetEnd().X, RoomLine->GetEnd().Y, 0));
			}
			FArmyPolygon RoomPolygon;
			RoomPolygon.SetVertices(RoomVertices);

			FArmyRoom * WD = RoomWeakPtr.Pin()->AsassignObj<FArmyRoom>();

			TArray<TSharedPtr<FArmyLine>> BaseAreaLineList;
			TArray<FVector> RectAreaVertices;
			FVector CenterPosition = FVector(MAX_FLT, MAX_FLT, MAX_FLT);
			switch (RectAreaWeakPtr.Pin()->GetType())
			{
			case OT_CurtainBox:
			{
				AreaType = EAreaEnum::Ceiling;
				ClassType = Class_AnnotationCurtainBox;

				//获取窗帘盒点集
				FArmyFurniture* Furniture = RectAreaWeakPtr.Pin()->AsassignObj<FArmyFurniture>();
				RectAreaVertices = Furniture->GetBottomVerticeList(OT_CurtainBox);
				//获取中心点
				CenterPosition = FBox(RectAreaVertices).GetCenter();
				CenterPosition.Z = 0;
				//窗帘盒线集
				int Number = RectAreaVertices.Num();
				for (int Index = 0; Index < Number; ++Index)
				{
					FVector Start = RectAreaVertices[Index%Number];
					FVector End = RectAreaVertices[(Index + 1) % Number];
					TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Start, End));
					BaseAreaLineList.Emplace(Line);
				}

				break;
			}
			case OT_RectArea:
			{
				FArmyRectArea* BaseArea = RectAreaWeakPtr.Pin()->AsassignObj< FArmyRectArea>();
				BaseArea->GetOutlineCenter(CenterPosition);
				CenterPosition.Z = 0;
				switch (BaseArea->SurfaceType)
				{
				case 0:
					AreaType = EAreaEnum::Floor;
					ClassType = Class_Floorlayout;
					break;
				case 1:
					AreaType = EAreaEnum::Wall;
					ClassType = Class_Walllayout;
					break;
				case 2:
					AreaType = EAreaEnum::Ceiling;
					ClassType = Class_AnnotationCeilingObj;
					break;
				default:
					break;
				}
				//判断该区域是否有波打线
				//如果有波打线，则只需要显示获取外面一圈的线段
				//获取区域的各条边
				BaseArea->GetConstructionOutLines(BaseAreaLineList);
				//构建区域的闭合polygon
				for (TSharedPtr<FArmyLine> Line : BaseAreaLineList)
				{
					RectAreaVertices.Emplace(FVector(Line->GetStart().X, Line->GetStart().Y, 0));
					RectAreaVertices.Emplace(FVector(Line->GetEnd().X, Line->GetEnd().Y, 0));
				}
				break;
			}
			case OT_PolygonArea:
			case OT_FreePolygonArea:
			{
				FArmyBaseArea* BaseArea = RectAreaWeakPtr.Pin()->AsassignObj< FArmyBaseArea>();
				BaseArea->GetOutlineCenter(CenterPosition);
				CenterPosition.Z = 0;
				switch (BaseArea->SurfaceType)
				{
				case 0:
					AreaType = EAreaEnum::Floor;
					ClassType = Class_Floorlayout;
					break;
				case 1:
					AreaType = EAreaEnum::Wall;
					ClassType = Class_Walllayout;
					break;
				case 2:
					AreaType = EAreaEnum::Ceiling;
					ClassType = Class_AnnotationCeilingObj;
					break;
				default:
					break;
				}
				//获取区域的各条边
				BaseArea->GetLines(BaseAreaLineList, true);
				//构建区域的闭合polygon
				for (TSharedPtr<FArmyLine> Line : BaseAreaLineList)
				{
					RectAreaVertices.Emplace(FVector(Line->GetStart().X, Line->GetStart().Y, 0));
					RectAreaVertices.Emplace(FVector(Line->GetEnd().X, Line->GetEnd().Y, 0));
				}
				break;
			}
			default:
				check(false);
				break;
			}


			//这里只判断区域的点是否在房间矩形内
			//可以判断整个区域时候在房间矩形呢--->见FBox::IsInside(XXX)的另外一个重载
			//if (RoomPolygon.IsInside(CenterPosition))//开始计算区域的定位尺寸
			if (WD->IsPointInRoom(CenterPosition))
			{
				FArmyPolygon BaseAreaPolygon;
				BaseAreaPolygon.SetVertices(RectAreaVertices);

				TArray<TSharedPtr<FArmyLine>> OtherObjLineList;//吊顶、梁、窗帘盒边线
				TArray<TPair<FVector, float>> OtherCircleAreaList;//圆形吊顶的位置与半径
				TArray<FArmyPolygon> OtherPolygonList;
				for (FObjectWeakPtr ObjWeakObj : ObjArray)
				{
					if (ObjWeakObj == RectAreaWeakPtr) continue;

					//获取包围点
					switch (ObjWeakObj.Pin()->GetType())
					{
					case OT_CircleArea:
					{
						FArmyCircleArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyCircleArea>();

						EAreaEnum OtherType = EAreaEnum::NoneArea;
						switch (TempArea->SurfaceType)
						{
						case 0:
							OtherType = EAreaEnum::Floor;
							break;
						case 1:
							OtherType = EAreaEnum::Wall;
							break;
						case 2:
							OtherType = EAreaEnum::Ceiling;
							break;
						default:
							break;
						}
						//相同图层下的类型才相互标注
						if (OtherType != AreaType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						if (WD->IsPointInRoom(OutlineCenter))
						{
							//存储圆形吊顶的圆心位置与板件
							OtherCircleAreaList.Emplace(TPair<FVector, float>(FVector(OutlineCenter.X, OutlineCenter.Y, 0), TempArea->GetRadius()));
						}
						break;
					}
					case OT_Beam:
						//如果需求需要在地面铺装图中不需要标注距墙尺寸，则取消下行注释
						if (EAreaEnum::Ceiling != AreaType) continue;
					case OT_AddWall:
					{
						TArray<FVector> TargetBoundVertexes;
						ObjWeakObj.Pin()->GetVertexes(TargetBoundVertexes);
						FVector ObjCenterPos = FBox(TargetBoundVertexes).GetCenter();
						if (WD->IsPointInRoom(ObjCenterPos))
						{
							//存储polygon
							FArmyPolygon BeamPolygon;
							BeamPolygon.SetVertices(TargetBoundVertexes);
							OtherPolygonList.Emplace(BeamPolygon);

							//存储线
							int Number = TargetBoundVertexes.Num();
							for (int Index = 0; Index < Number; ++Index)
							{
								FVector Start = TargetBoundVertexes[Index%Number];
								FVector End = TargetBoundVertexes[(Index + 1) % Number];
								TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Start, End));
								OtherObjLineList.Emplace(Line);
							}
						}
						break;
					}
					case OT_CurtainBox:
					{
						if (EAreaEnum::Ceiling != AreaType) continue;

						TArray<FVector> TargetBoundVertexes;
						FArmyFurniture* Furniture = ObjWeakObj.Pin()->AsassignObj<FArmyFurniture>();
						TargetBoundVertexes = Furniture->GetBottomVerticeList(OT_CurtainBox);
						FVector ObjCenterPos = FBox(TargetBoundVertexes).GetCenter();
						if (WD->IsPointInRoom(ObjCenterPos))
						{
							//存储polygon
							FArmyPolygon CutainBoxPolygon;
							CutainBoxPolygon.SetVertices(TargetBoundVertexes);
							OtherPolygonList.Emplace(CutainBoxPolygon);

							//存储线
							int Number = TargetBoundVertexes.Num();
							for (int Index = 0; Index < Number; ++Index)
							{
								FVector Start = TargetBoundVertexes[Index%Number];
								FVector End = TargetBoundVertexes[(Index + 1) % Number];
								TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Start, End));
								OtherObjLineList.Emplace(Line);
							}
						}
						break;
					}
					case OT_RectArea:
					{
						FArmyRectArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyRectArea>();
						EAreaEnum OtherType = EAreaEnum::NoneArea;
						switch (TempArea->SurfaceType)
						{
						case 0:
							OtherType = EAreaEnum::Floor;
							break;
						case 1:
							OtherType = EAreaEnum::Wall;
							break;
						case 2:
							OtherType = EAreaEnum::Ceiling;
							break;
						default:
							break;
						}
						if (OtherType != AreaType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						if (WD->IsPointInRoom(OutlineCenter))
						{
							TArray<TSharedPtr<FArmyLine>> RectAreaLineList;
							TempArea->GetConstructionOutLines(RectAreaLineList);
							TArray<FVector> mRectAreaVertices;
							for (TSharedPtr<FArmyLine> Line : RectAreaLineList)
							{
								mRectAreaVertices.AddUnique(Line->GetStart());
								mRectAreaVertices.AddUnique(Line->GetEnd());
							}
							FArmyPolygon RectAreaPolygon;
							RectAreaPolygon.SetVertices(mRectAreaVertices);

							OtherPolygonList.Emplace(RectAreaPolygon);
							ObjWeakObj.Pin()->GetLines(OtherObjLineList, true);
						}
						break;
					}
					case OT_PolygonArea:
					case OT_FreePolygonArea:
					{
						FArmyBaseArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyBaseArea>();
						EAreaEnum OtherType = EAreaEnum::NoneArea;
						switch (TempArea->SurfaceType)
						{
						case 0:
							OtherType = EAreaEnum::Floor;
							break;
						case 1:
							OtherType = EAreaEnum::Wall;
							break;
						case 2:
							OtherType = EAreaEnum::Ceiling;
							break;
						default:
							break;
						}
						if (OtherType != AreaType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						//获取区域的各条边
						if (WD->IsPointInRoom(OutlineCenter))
						{
							ObjWeakObj.Pin()->GetLines(OtherObjLineList, true);
						}
						break;
					}
					default:
						break;
					}

				}
				RoomLineList.Append(OtherObjLineList);

				TArray<TPair<FVector, FVector>> RoomInsideMap, RectAreaInsideMap;
				//计算各条边到区域中心的最近点
				for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
				{
					FVector StartPos = RoomLine->GetStart();
					FVector EndPos = RoomLine->GetEnd();
					StartPos.Z = EndPos.Z = 0;

					//判断点是否在区域的边的上（主要在易形区域情况下出现这种情况）
					FVector ClosestPoint = FMath::ClosestPointOnInfiniteLine(StartPos, EndPos, CenterPosition);
					FVector ClosestPointOnLine = FMath::ClosestPointOnLine(StartPos, EndPos, CenterPosition);
					if ((ClosestPointOnLine - ClosestPoint).Size() < 0.001)
					{
						RoomInsideMap.Emplace(TPair<FVector, FVector>((CenterPosition - ClosestPoint).GetSafeNormal(), ClosestPoint));
					}
				}

				//由于规则多边形只需要标记其中一条边，所以标记是否标记
				bool PolygonFlag = false;

				for (TSharedPtr<FArmyLine> RectAreaLine : BaseAreaLineList)
				{
					FVector StartPos = RectAreaLine->GetStart();
					FVector EndPos = RectAreaLine->GetEnd();
					StartPos.Z = EndPos.Z = 0;

					//判断点是否在区域的边的上（主要在易形区域情况下出现这种情况）
					FVector ClosestPoint = FMath::ClosestPointOnInfiniteLine(StartPos, EndPos, CenterPosition);
					FVector ClosestPointOnLine = FMath::ClosestPointOnLine(StartPos, EndPos, CenterPosition);
					if ((ClosestPointOnLine - ClosestPoint).Size() < 0.001)
					{
						RectAreaInsideMap.Emplace(TPair<FVector, FVector>((CenterPosition - ClosestPoint).GetSafeNormal(), ClosestPoint));
					}

					//标注区域自身尺寸 
					switch (RectAreaWeakPtr.Pin()->GetType())
					{
					case OT_CurtainBox:
					case OT_RectArea:
					{
						FVector OffsetVec = FVector::ZeroVector;
						//标注矩形区域自身尺寸
						if (((CenterPosition - ClosestPoint).GetSafeNormal() - FVector::ForwardVector).Size() < 0.001)
						{
							PolygonFlag = false;
							OffsetVec = FVector(OffsetValue, 0, 0);
						}
						else if (((CenterPosition - ClosestPoint).GetSafeNormal() - FVector::RightVector).Size() < 0.001)
						{
							PolygonFlag = false;
							OffsetVec = FVector(0, OffsetValue, 0);
						}
						else
						{
							PolygonFlag = true;
						}
						if (!PolygonFlag)
						{
							const FVector& DesimenStartPos = RectAreaLine->GetStart();
							const FVector& DesimenEndPos = RectAreaLine->GetEnd();

							TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, DesimenStartPos, DesimenEndPos,
								DesimenStartPos + OffsetVec, DesimenEndPos + OffsetVec, OT_Dimensions);
							if (CurrentDimensions.IsValid())
							{
								FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
							}
						}
					}
					break;
					case OT_PolygonArea:
					{
						//以及标记了无需标记
						if (PolygonFlag)
							continue;
						PolygonFlag = true;

						//计算出该条边的垂线（向内标注）
						const FVector& CrossVec = FVector::CrossProduct(StartPos - EndPos, -FVector::UpVector).GetSafeNormal();
						//标出每条线段尺寸
						TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, StartPos, EndPos,
							StartPos + CrossVec*OffsetValue, EndPos + CrossVec*OffsetValue, OT_Dimensions);
						if (CurrentDimensions.IsValid())
						{
							FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
						}
					}
					break;
					case OT_FreePolygonArea:
					{
						//计算出该条边的垂线（向内标注）
						const FVector& CrossVec = FVector::CrossProduct(StartPos - EndPos, -FVector::UpVector).GetSafeNormal();
						//标出每条线段尺寸
						TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, StartPos, EndPos,
							StartPos + CrossVec*OffsetValue, EndPos + CrossVec*OffsetValue, OT_Dimensions);
						if (CurrentDimensions.IsValid())
						{
							FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
						}
					}
					break;
					default:
						break;
					}

				}

				//根据Map的方向遍历出相邻边，取出邻边的距离
				for (auto RectAreaInsideMapIt : RectAreaInsideMap)
				{
					TSharedPtr<FArmyDimensions> TargetDimensions;
					float MinDistance = MAX_FLT;
					for (auto RoomInsideMapIt : RoomInsideMap)
					{
						//方向一致,标注出尺寸
						if ((RoomInsideMapIt.Key - RectAreaInsideMapIt.Key).Size() < 0.05)
						{
							if ((RoomInsideMapIt.Value - RectAreaInsideMapIt.Value).Size() < 0.5) continue;
							if (BaseAreaPolygon.IsInside(RoomInsideMapIt.Value, true)) continue;
							if (!RoomPolygon.IsInside(RoomInsideMapIt.Value, true)) continue;

							//刷选出距离最短的尺寸
							float CurrentDistance = FVector::Distance(RoomInsideMapIt.Value, RectAreaInsideMapIt.Value);
							if (CurrentDistance < MinDistance)
							{
								TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, RoomInsideMapIt.Value, RectAreaInsideMapIt.Value,
									RoomInsideMapIt.Value, RectAreaInsideMapIt.Value, OT_Dimensions);

								MinDistance = CurrentDistance;
								TargetDimensions = CurrentDimensions;
							}
						}
					}
					if (TargetDimensions.IsValid())
					{
						//1、遍历已有的标注，若两条标注平行且相等长度也continue
						//注意：这种方法可以解决并排吊顶、回形吊顶以及避免两个相邻吊顶相互标注问题
						//but ：不显示两条相互平行且相等的直线中的其中一条
						bool IsHide = false;
						FVector CurrentDismensDirect;
						float CurrentDismensLength;
						(TargetDimensions->MainLine->GetStart() - TargetDimensions->MainLine->GetEnd()).ToDirectionAndLength(CurrentDismensDirect, CurrentDismensLength);
						for (TPair<TSharedPtr<FArmyLine>, EAreaEnum> TempLine : AllDemensionList)
						{
							FVector TempLineDirect;
							float TempLineLength;
							(TempLine.Key->GetStart() - TempLine.Key->GetEnd()).ToDirectionAndLength(TempLineDirect, TempLineLength);
							if (TempLine.Value == AreaType &&//类型一致
								FMath::Abs(CurrentDismensLength - TempLineLength) < 0.001    && //长度相等
																								//((CurrentDismensDirect + TempLineDirect).Size() < 0.001 )) || //平行
																								//(CurrentDismensDirect - TempLineDirect).Size() < 0.01)     ||
								FMath::Abs(FVector::Distance(TargetDimensions->MainLine->GetStart(), TempLine.Key->GetEnd()) - //相互相对平行，两条线段构成一个矩形，对角线长度相等
									FVector::Distance(TargetDimensions->MainLine->GetEnd(), TempLine.Key->GetStart())) < 0.001&&
								FMath::Abs(FVector::Distance(TargetDimensions->MainLine->GetStart(), TempLine.Key->GetStart()) - //相互相对平行，两条线段构成一个矩形，对角线长度相等
									FVector::Distance(TargetDimensions->MainLine->GetEnd(), TempLine.Key->GetEnd())) < 0.001)
							{
								IsHide = true;
								break;

							}
						}
						//2、遍历该房间的其余梁、窗帘盒、矩形吊顶，判断标注是否标注到其里面
						//里面的标注是需要筛选剔除的
						//判断标注是否在一个矩形内，判断标注的两个顶点是否都在矩形内部
						for (FArmyPolygon OtherPolygon : OtherPolygonList)
						{
							if (OtherPolygon.IsInside(TargetDimensions->MainLine->GetStart(), true) &&
								OtherPolygon.IsInside(TargetDimensions->MainLine->GetEnd(), true))
							{
								IsHide = true;
								break;
							}
						}
						//3、遍历该房间的所有圆形吊顶，判断该标注时候穿过圆形吊顶，如果是，则剔除这条标注
						for (TPair<FVector, float> CircleArea : OtherCircleAreaList)
						{
							//如果圆形到直线距离小于半径，则线段穿过圆，则需要隐藏
							const FVector ClosestOnLine = FMath::ClosestPointOnLine(TargetDimensions->MainLine->GetStart(), TargetDimensions->MainLine->GetEnd(), CircleArea.Key);
							const FVector ClosestOnInfinite = FMath::ClosestPointOnInfiniteLine(TargetDimensions->MainLine->GetStart(), TargetDimensions->MainLine->GetEnd(), CircleArea.Key);
							if ((ClosestOnInfinite - ClosestOnLine).Size() < 0.001f)
							{
								//可待优化
								//if (CircleArea.Value > FVector::Distance(ClosestOnLine, CircleArea.Key))
								{
									IsHide = true;
									break;
								}
							}
						}
						if (IsHide) continue;

						AllDemensionList.Emplace(TargetDimensions->MainLine, AreaType);
						FArmySceneData::Get()->Add(TargetDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}
				break;
			}
		}
	}
#pragma endregion 矩形吊顶

#pragma region 圆形吊顶
	//判断圆形吊顶是在哪个房间内
	for (FObjectWeakPtr CircleAreaWeakPtr : CircleAreaArray)
	{
		ObjectClassType ClassType = Class_Other;
		for (FObjectWeakPtr RoomWeakPtr : InternalRoomList)
		{
			//户型polygon
			TArray<FVector> RoomVertices;
			TArray<TSharedPtr<FArmyLine>> RoomLineList;
			RoomWeakPtr.Pin()->GetLines(RoomLineList);
			for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
			{
				RoomVertices.Emplace(FVector(RoomLine->GetStart().X, RoomLine->GetStart().Y, 0));
				RoomVertices.Emplace(FVector(RoomLine->GetEnd().X, RoomLine->GetEnd().Y, 0));
			}
			FArmyPolygon RoomPolygon;
			RoomPolygon.SetVertices(RoomVertices);

			//获取户型中心点
			FArmyCircleArea* CircleArea = CircleAreaWeakPtr.Pin()->AsassignObj<FArmyCircleArea>();
			FVector CenterPosition;
			CircleArea->GetOutlineCenter(CenterPosition);
			CenterPosition.Z = 0;
			//这里只判断区域的点是否在房间矩形内
			if (RoomPolygon.IsInside(CenterPosition))//开始计算区域的定位尺寸
			{
				switch (CircleArea->SurfaceType)
				{
				case 0:
					ClassType = Class_Floorlayout;
					break;
				case 1:
					ClassType = Class_Walllayout;
					break;
				case 2:
					ClassType = Class_Ceilinglayout;
					break;
				default:
					break;
				}

				TArray<TSharedPtr<FArmyLine>> OtherObjLineList;//存储梁、窗帘盒、吊顶边线
				TArray<TPair<FVector, float>> OtherCircleAreaList;//圆形吊顶的位置与半径
				TArray<FArmyPolygon> OtherPolygonList;
				for (FObjectWeakPtr ObjWeakObj : ObjArray)
				{
					if (ObjWeakObj == CircleAreaWeakPtr) continue;

					//获取包围点
					switch (ObjWeakObj.Pin()->GetType())
					{
					case OT_CircleArea:
					{
						FArmyCircleArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyCircleArea>();
						ObjectClassType mClassType = Class_Other;
						switch (TempArea->SurfaceType)
						{
						case 0:
							mClassType = Class_Floorlayout;
							break;
						case 1:
							mClassType = Class_Walllayout;
							break;
						case 2:
							mClassType = Class_Ceilinglayout;
							break;
						default:
							break;
						}
						if (mClassType != ClassType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						if (RoomPolygon.IsInside(OutlineCenter))
						{
							//存储圆形吊顶的圆心位置与板件
							OtherCircleAreaList.Emplace(TPair<FVector, float>(FVector(OutlineCenter.X, OutlineCenter.Y, 0), TempArea->GetRadius()));
						}
						break;
					}
					case OT_Beam:
					case OT_AddWall:
					{
						TArray<FVector> BoundVertexes;
						ObjWeakObj.Pin()->GetVertexes(BoundVertexes);
						FVector ObjCenterPos = FBox(BoundVertexes).GetCenter();
						if (RoomPolygon.IsInside(ObjCenterPos))
						{
							//存储polygon
							FArmyPolygon BeamPolygon;
							BeamPolygon.SetVertices(BoundVertexes);
							OtherPolygonList.Emplace(BeamPolygon);

							int Number = BoundVertexes.Num();
							for (int Index = 0; Index < Number; ++Index)
							{
								FVector Start = BoundVertexes[Index%Number];
								FVector End = BoundVertexes[(Index + 1) % Number];
								TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Start, End));
								OtherObjLineList.Emplace(Line);
							}
						}
						break;
					}
					case OT_CurtainBox:
					{
						if (Class_AnnotationCeilingObj != ClassType) continue;

						TArray<FVector> TargetBoundVertexes;
						FArmyFurniture* Furniture = ObjWeakObj.Pin()->AsassignObj<FArmyFurniture>();
						TargetBoundVertexes = Furniture->GetBottomVerticeList(OT_CurtainBox);
						FVector ObjCenterPos = FBox(TargetBoundVertexes).GetCenter();
						if (RoomPolygon.IsInside(ObjCenterPos))
						{
							//存储polygon
							FArmyPolygon CutainBoxPolygon;
							CutainBoxPolygon.SetVertices(TargetBoundVertexes);
							OtherPolygonList.Emplace(CutainBoxPolygon);

							//存储线
							int Number = TargetBoundVertexes.Num();
							for (int Index = 0; Index < Number; ++Index)
							{
								FVector Start = TargetBoundVertexes[Index%Number];
								FVector End = TargetBoundVertexes[(Index + 1) % Number];
								TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Start, End));
								OtherObjLineList.Emplace(Line);
							}
						}
						break;
					}
					case OT_RectArea:
					{
						FArmyRectArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyRectArea>();
						ObjectClassType mClassType = Class_Other;
						switch (TempArea->SurfaceType)
						{
						case 0:
							mClassType = Class_Floorlayout;
							break;
						case 1:
							mClassType = Class_Walllayout;
							break;
						case 2:
							mClassType = Class_Ceilinglayout;
							break;
						default:
							break;
						}
						if (mClassType != ClassType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						if (RoomPolygon.IsInside(OutlineCenter))
						{
							TArray<TSharedPtr<FArmyLine>> RectAreaLineList;
							TempArea->GetConstructionOutLines(RectAreaLineList);
							TArray<FVector> mRectAreaVertices;
							for (TSharedPtr<FArmyLine> Line : RectAreaLineList)
							{
								mRectAreaVertices.AddUnique(Line->GetStart());
								mRectAreaVertices.AddUnique(Line->GetEnd());
							}
							FArmyPolygon RectAreaPolygon;
							RectAreaPolygon.SetVertices(mRectAreaVertices);

							OtherPolygonList.Emplace(RectAreaPolygon);
						}
					}
					case OT_PolygonArea:
					case OT_FreePolygonArea:
					{
						FArmyBaseArea* TempArea = ObjWeakObj.Pin()->AsassignObj<FArmyBaseArea>();
						ObjectClassType mClassType = Class_Other;
						switch (TempArea->SurfaceType)
						{
						case 0:
							mClassType = Class_Floorlayout;
							break;
						case 1:
							mClassType = Class_Walllayout;
							break;
						case 2:
							mClassType = Class_Ceilinglayout;
							break;
						default:
							break;
						}
						if (mClassType != ClassType) continue;

						FVector OutlineCenter = FVector::ZeroVector;
						TempArea->GetOutlineCenter(OutlineCenter);
						//获取区域的各条边
						if (RoomPolygon.IsInside(OutlineCenter))
						{
							ObjWeakObj.Pin()->GetLines(OtherObjLineList, true);
						}
						break;
					}
					default:
						break;
					}

				}
				RoomLineList.Append(OtherObjLineList);


				//获取区域的半径
				float Radius = CircleArea->GetRadius();

				/******
				* 注意：这里只会在每个方向各存储一条尺寸
				* 如若有需求，可待优化
				*******/
				TMap<FVector, FVector> RoomInsideMap;

				//计算各条边到区域中心的最近点
				for (TSharedPtr<FArmyLine> RoomLine : RoomLineList)
				{
					FVector StartPos = RoomLine->GetStart();
					FVector EndPos = RoomLine->GetEnd();
					StartPos.Z = EndPos.Z = 0;

					FVector ClosestPoint = FMath::ClosestPointOnInfiniteLine(StartPos, EndPos, CenterPosition);
					FVector NearPoint = FMath::ClosestPointOnLine(StartPos, EndPos, CenterPosition);

					FVector Direct = (CenterPosition - ClosestPoint).GetSafeNormal();
					if ((Direct - FVector::ForwardVector).Size() < 0.001)
						Direct = FVector::ForwardVector;
					else if ((Direct + FVector::ForwardVector).Size() < 0.001)
						Direct = -FVector::ForwardVector;
					else if ((Direct - FVector::RightVector).Size() < 0.001)
						Direct = FVector::RightVector;
					else if ((Direct + FVector::RightVector).Size() < 0.001)
						Direct = -FVector::RightVector;

					if ((ClosestPoint - NearPoint).Size() < 0.001 &&       //两点相等说明点到线段的垂足是在线段内部的
						(Direct.Equals(FVector::ForwardVector) ||       //或者在上下左右方向
							Direct.Equals(-FVector::ForwardVector) ||
							Direct.Equals(FVector::RightVector) ||
							Direct.Equals(-FVector::RightVector)))
					{
						//比较最近点，将最近的点添加进去
						if (RoomInsideMap.Contains(Direct))
						{
							if (FVector::Distance(ClosestPoint, CenterPosition) < FVector::Distance(*RoomInsideMap.Find(Direct), CenterPosition))
							{
								//更新最近点
								*RoomInsideMap.Find(Direct) = ClosestPoint;
							}
						}
						else
						{
							RoomInsideMap.Emplace(Direct, ClosestPoint);
						}
					}
				}

				//标注圆形区域自身尺寸
				{
					const FVector& StartPos = CenterPosition;
					const FVector& EndPos = StartPos + FVector::ForwardVector * Radius;

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, StartPos, EndPos,
						StartPos, EndPos, OT_Dimensions);
					if (CurrentDimensions.IsValid())
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}


				for (auto RoomInsideMapIt = RoomInsideMap.CreateConstIterator(); RoomInsideMapIt; ++RoomInsideMapIt)
				{
					const FVector& EndPosition = CenterPosition - RoomInsideMapIt.Key()*Radius;

					if ((RoomInsideMapIt.Value() - EndPosition).Size() < 0.001) continue;
					if (Radius > FVector::Distance(CenterPosition, RoomInsideMapIt.Value())) continue;
					if (!RoomPolygon.IsInside(RoomInsideMapIt.Value(), true)) continue;

					//1、遍历该房间的其余梁、窗帘盒、矩形吊顶，判断标注是否标注到其里面
					//里面的标注是需要筛选剔除的
					//判断标注是否在一个矩形内，判断标注的两个顶点是否都在矩形内部
					bool IsHide = false;
					for (FArmyPolygon OtherPolygon : OtherPolygonList)
					{
						if (OtherPolygon.IsInside(RoomInsideMapIt.Value(), true) &&
							OtherPolygon.IsInside(EndPosition, true))
						{
							IsHide = true;
							break;
						}
					}
					//2、遍历该房间的所有圆形吊顶，判断该标注时候穿过圆形吊顶，如果是，则剔除这条标注
					for (TPair<FVector, float> CircleArea : OtherCircleAreaList)
					{
						//如果圆形到直线距离小于半径，则线段穿过圆，则需要隐藏
						const FVector ClosestOnLine = FMath::ClosestPointOnLine(RoomInsideMapIt.Value(), EndPosition, CircleArea.Key);
						const FVector ClosestOnInfinite = FMath::ClosestPointOnInfiniteLine(RoomInsideMapIt.Value(), EndPosition, CircleArea.Key);
						if ((ClosestOnInfinite - ClosestOnLine).Size() < 0.001f)
						{
							//if (CircleArea.Value > FVector::Distance(ClosestOnLine, CircleArea.Key))
							{
								IsHide = true;
								break;
							}
						}
					}

					if (IsHide) continue;

					TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension((ObjectClassType)ClassType, RoomInsideMapIt.Value(), EndPosition,
						RoomInsideMapIt.Value(), EndPosition, OT_Dimensions);
					if (CurrentDimensions.IsValid())
					{
						FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
					}
				}

				break;
			}
		}
	}
#pragma endregion 圆形吊顶

#pragma region 顶角线
	TArray<FObjectWeakPtr> VertexAngleAreaArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, VertexAngleAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, VertexAngleAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, VertexAngleAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, VertexAngleAreaArray);

	for (FObjectWeakPtr VertexAngleArea : VertexAngleAreaArray)
	{
		FArmyBaseArea* BaseArea = VertexAngleArea.Pin()->AsassignObj<FArmyBaseArea>();
		if (BaseArea&&BaseArea->SurfaceType == 2)////顶角线需要标注自身尺寸，踢脚线不需要标注
		{
			TArray<TSharedPtr<FArmyLine>> OutVertexAngleLineList;//区域线（外线）
			TArray<TSharedPtr<FArmyLine>> InnerVertexAngleLineList;//顶角线（内线）
			TMap<int32, TArray< TSharedPtr<FArmyLine> >> ExtrusionLinesMap;
			BaseArea->GetExtrusionLines(ExtrusionLinesMap, ExtrusionType::RoofExtrusionLine);
			if (ExtrusionLinesMap.Num() > 0)//有顶角线
			{
				OutVertexAngleLineList = *ExtrusionLinesMap.Find(0);
				InnerVertexAngleLineList = *ExtrusionLinesMap.Find(1);

				//获取区域的中心点
				FVector CenterPosition, InPos, OutPos;
				BaseArea->GetOutlineCenter(CenterPosition);
				for (TSharedPtr<FArmyLine> VertexAngleLine : InnerVertexAngleLineList)
				{
					FVector OnInfinetePos = FMath::ClosestPointOnInfiniteLine(VertexAngleLine->GetStart(), VertexAngleLine->GetEnd(), CenterPosition);
					FVector OnLinePos = FMath::ClosestPointOnLine(VertexAngleLine->GetStart(), VertexAngleLine->GetEnd(), CenterPosition);
					if ((OnLinePos - OnInfinetePos).Size() < 0.001)
					{
						InPos = OnLinePos;
						const FVector& DemensionDir = (CenterPosition - InPos).GetSafeNormal();
						for (TSharedPtr<FArmyLine> AreaLine : OutVertexAngleLineList)
						{
							FVector OutPos = FMath::ClosestPointOnLine(AreaLine->GetStart(), AreaLine->GetEnd(), CenterPosition);
							if (((CenterPosition - OutPos).GetSafeNormal() - DemensionDir).Size() < 0.001)//方向和内线到中心点方向相同
							{
								//开始标注顶角线自身尺寸
								TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_AnnotationCeilingObj, InPos, OutPos,
									InPos, OutPos, OT_Dimensions);
								if (CurrentDimensions.IsValid())
								{
									FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
								}
								break;
							}
						}
						break;
					}
				}
			}
		}
	}
#pragma endregion 顶角线

#pragma region 波打线
	TArray<FObjectWeakPtr> BodaRectAreaObjArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, BodaRectAreaObjArray);
	for (FObjectWeakPtr BodaRectAreaWeakObj : BodaRectAreaObjArray)
	{
		FArmyRectArea* BodaRectArea = BodaRectAreaWeakObj.Pin()->AsassignObj<FArmyRectArea>();
		if (BodaRectArea&&BodaRectArea->HasBodaThickness())
		{
			//获取波打线段和矩形区域线段
			TArray<TSharedPtr<FArmyLine>> OutAreaLineList, InnerAreaLineList;
			BodaRectArea->GetConstructionOutLines(OutAreaLineList);
			BodaRectArea->GetConstructionInnerLines(InnerAreaLineList);

			//获取区域的中心点
			FVector CenterPosition, InPos, OutPos;
			BodaRectArea->GetOutlineCenter(CenterPosition);
			for (TSharedPtr<FArmyLine> VertexAngleLine : InnerAreaLineList)
			{
				FVector OnInfinetePos = FMath::ClosestPointOnInfiniteLine(VertexAngleLine->GetStart(), VertexAngleLine->GetEnd(), CenterPosition);
				FVector OnLinePos = FMath::ClosestPointOnLine(VertexAngleLine->GetStart(), VertexAngleLine->GetEnd(), CenterPosition);
				if ((OnLinePos - OnInfinetePos).Size() < 0.001)
				{
					InPos = OnLinePos;
					const FVector& DemensionDir = (CenterPosition - InPos).GetSafeNormal();
					for (TSharedPtr<FArmyLine> AreaLine : OutAreaLineList)
					{
						FVector OutPos = FMath::ClosestPointOnLine(AreaLine->GetStart(), AreaLine->GetEnd(), CenterPosition);
						if (((CenterPosition - OutPos).GetSafeNormal() - DemensionDir).Size() < 0.001)//方向和内线到中心点方向相同
						{
							//开始标注顶角线自身尺寸
							TSharedPtr<FArmyDimensions> CurrentDimensions = CreateObjectDismension(Class_Floorlayout, InPos, OutPos,
								InPos, OutPos, OT_Dimensions);
							if (CurrentDimensions.IsValid())
							{
								FArmySceneData::Get()->Add(CurrentDimensions, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
							}
							break;
						}
					}
					break;
				}
			}
		}
	}
#pragma endregion 波打线
}
void FArmyConstructionFunction::CreateLampControlLines(FObjectPtr InBindObj)
{
	TArray<FObjectWeakPtr> LampControlObj;
	FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_LampControlLine, LampControlObj);

	TSharedPtr<FArmyLampControlLines> LampControlLine;
	if (LampControlObj.Num() == 0)
	{
		LampControlLine = MakeShareable(new FArmyLampControlLines);
		FArmySceneData::Get()->Add(LampControlLine, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
	}
	else
	{
		LampControlLine = StaticCastSharedPtr<FArmyLampControlLines>(LampControlObj[0].Pin());
	}

	{
		FArmyLampControlLines::LampControlInfo& LampControlInfo = LampControlLine->GetLampControlInfo(InBindObj, true);
		TSharedPtr<FArmyFurniture> ComponentObj = StaticCastSharedPtr<FArmyFurniture>(InBindObj);
		for (auto & Obj : ComponentObj->RelatedFurnitureMap)
		{
			int32 Index = LampControlInfo.ControlLampLineArray.AddUnique(FArmyLampControlLines::FControlLine(Obj.Key));
			FArmyLampControlLines::FControlLine& Info = LampControlInfo.ControlLampLineArray[Index];

			FObjectWeakPtr Lamp = FArmySceneData::Get()->GetObjectByGuid(E_ConstructionModel, Obj.Value);
			if (Lamp.IsValid())
			{
				Info.ContrlLampArray.AddUnique(Lamp);
			}
			else
			{
				Lamp = FArmySceneData::Get()->GetObjectByGuid(E_HardModel, Obj.Value);
				if (Lamp.IsValid())
				{
					Info.ContrlLampArray.AddUnique(Lamp);
				}
			}
		}
	}
}
void FArmyConstructionFunction::CreateObjectRuler(FObjectPtr InBindObj)
{
	TSharedPtr<FArmyObjAutoRuler> ObjRuler = MakeShareable(new FArmyObjAutoRuler());
	ObjRuler->SetBoundObj(InBindObj);
	ObjRuler->SetAutoCheck(true);
	FArmySceneData::Get()->Add(ObjRuler, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
}
void FArmyConstructionFunction::CreateObjectEntrance(FObjectPtr InBindObj, const FVector& Pos, const FVector& Rotate)
{
	if (ClassTypeList.Contains(GetClassTypeByObject(InBindObj, 2)) || ClassTypeList.Contains(Class_Other))
	{
		//绘制进入图标
		TSharedPtr<FArmyEntrance> ObjEntrance = MakeShareable(new FArmyEntrance());
		ObjEntrance->SetWorldTransform(Pos, Rotate);

		//将本数据加入到数据HOME中
		FArmySceneData::Get()->Add(ObjEntrance, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
		SCOPE_TRANSACTION(TEXT("生成入口标签"));
		ObjEntrance->Modify();
	}
}
TSharedPtr<FArmyTextLabel> FArmyConstructionFunction::CreateObjectTextLable(FObjectPtr InObj, int32 InTag, const FText& InText, const FVector& Pos, FArmyTextLabel::LabelType InLabelType)
{
	if (ClassTypeList.Contains(GetClassTypeByObject(InObj, 2)) || ClassTypeList.Contains(Class_Other))
	{
		TSharedPtr<FArmyTextLabel> TextLabel = MakeShareable(new FArmyTextLabel());
		TextLabel->SetbUseEditPos(false);
		TextLabel->SetWorldPosition(Pos);
		TextLabel->SetLabelContent(InText);
		//TextLabel->SetTextColor(FLinearColor(0.5,0.5,0.5));
		TextLabel->SetObjTag(InObj, InTag);
		TextLabel->SetLabelType(InLabelType);

		FArmySceneData::Get()->Add(TextLabel, XRArgument(1).ArgString(TEXT("AUTO")).ArgUint32(E_ConstructionModel));
		//AllObjectTextLabelList.Add(TPair<ObjectClassType, TSharedPtr<FArmyTextLabel>>(InClassType, TextLabel));
		SCOPE_TRANSACTION(TEXT("变换"));
		TextLabel->Modify();
		return TextLabel;
	}

	return nullptr;
}

TSharedPtr<FArmyTextLabel> FArmyConstructionFunction::CreateOriginObjectTextLable(FObjectPtr InObj, int32 InTag, const FText& InText, const FVector& Pos, FArmyTextLabel::LabelType InLabelType, FString ArgString, bool IgnoreCompare)
{
	if (ClassTypeList.Contains(GetClassTypeByObject(InObj, 2)) || ClassTypeList.Contains(Class_Other))
	{
		TSharedPtr<FArmyTextLabel> TextLabel = MakeShareable(new FArmyTextLabel());
		TextLabel->SetbUseEditPos(false);
		TextLabel->SetWorldPosition(Pos);
		TextLabel->SetLabelContent(InText);
		//TextLabel->SetTextColor(FLinearColor(0.5,0.5,0.5));
		TextLabel->SetObjTag(InObj, InTag);
		TextLabel->SetLabelType(InLabelType);

		FArmySceneData::Get()->Add(TextLabel, XRArgument(1).ArgString(ArgString).ArgUint32(E_ConstructionModel));
		//AllObjectTextLabelList.Add(TPair<ObjectClassType, TSharedPtr<FArmyTextLabel>>(InClassType, TextLabel));
		SCOPE_TRANSACTION(TEXT("变换"));
		TextLabel->Modify();

		return TextLabel;
	}

	// 如果忽略比较，则从数据库中取
	if (IgnoreCompare)
	{
		// 获得所有的textlable
		TArray<FObjectWeakPtr> TextLableArray;
		FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_TextLabel, TextLableArray);
		for (auto It : TextLableArray)
		{
			if (!It.IsValid())
			{
				continue;
			}

			TSharedPtr<FArmyTextLabel> TextLabelObj = StaticCastSharedPtr<FArmyTextLabel>(It.Pin());
			if (!TextLabelObj.IsValid())
			{
				continue;
			}

			if (TextLabelObj->Args._ArgString != ArgString)
			{
				continue;
			}

			if (TextLabelObj->GetLabelType() == InLabelType && TextLabelObj->GetBindObjGuid() == InObj->GetUniqueID())
			{
				if (TextLabelObj->GetLabelContent().ToString() != InText.ToString())
				{
					TextLabelObj->SetbUseEditPos(false);
					TextLabelObj->SetWorldPosition(Pos);
					TextLabelObj->SetLabelContent(InText);
					TextLabelObj->SetObjTag(InObj, InTag);

					return TextLabelObj;
				}
				else
				{
					return nullptr;
				}
				
			}
		}
	}
	
	return nullptr;
}

TSharedPtr<FArmyDimensions> FArmyConstructionFunction::CreateObjectDismension(ObjectClassType mClassType, const FVector& mBaseStart, const FVector& mBaseEnd, const FVector& mDismensionStart, const FVector& mDismensionEnd, EObjectType mObjType, int32 mTextSize, int32 mAreaType, bool mUseLeadLine)
{
	if (ClassTypeList.Contains(GetAnnotationClassTypeByObject(mClassType)) || ClassTypeList.Contains(Class_Other))
	{
		TSharedPtr<FArmyDimensions> CurrentDimensions = MakeShareable(new FArmyDimensions());
		CurrentDimensions->SetArrowsType(FArmyDimensions::Diagonal);
		CurrentDimensions->SetType(mObjType);
		CurrentDimensions->AreaType = mAreaType;
		CurrentDimensions->UseLeadLine = mUseLeadLine;
		CurrentDimensions->ClassType = mClassType;
		CurrentDimensions->SetBaseStart(mBaseStart);
		CurrentDimensions->SetBaseEnd(mBaseEnd);
		CurrentDimensions->SetDimensionsStart(mDismensionStart);
		CurrentDimensions->SetDimensionsEnd(mDismensionEnd);
		CurrentDimensions->SetTextSize(mTextSize);

		return CurrentDimensions;
	}
	return nullptr;
}
void FArmyConstructionFunction::AddLayerFrameProperty(const FName& InLayerName, const FObjectPtr InAddObj)
{
	FArmyWHCTableObject* TabelObj = InAddObj->AsassignObj<FArmyWHCTableObject>();
	if (TabelObj)
	{
		int32 Index = CupboardTableLayerArray.Find(FConstructionPropertyInfo(InLayerName));
		if (Index == INDEX_NONE)
		{
			Index = CupboardTableLayerArray.AddUnique(FConstructionPropertyInfo(InLayerName));
		}

		{
			FConstructionPropertyInfo& PInfo = CupboardTableLayerArray[Index];
			PInfo.AreaPropertyMap.Add(1004, TEXT(""));//套餐风格暂时没有
			PInfo.AreaPropertyMap.Add(1005, TabelObj->TableInfo.FrontEdgeName);//+TabelObj->TableInfo.FrontEdgeHeight

			FString StoveTypeStr = TEXT("");
			for (auto& Str : TabelObj->TableInfo.StoveTypeArray)
			{
				if (!StoveTypeStr.IsEmpty())
				{
					StoveTypeStr += TEXT("\\");
				}
				StoveTypeStr += Str;
			}
			PInfo.AreaPropertyMap.Add(1006, StoveTypeStr);


			PInfo.AreaPropertyMap.Add(1010, TabelObj->TableInfo.BoardMaterial);//+height
			PInfo.AreaPropertyMap.Add(1011, TabelObj->TableInfo.BackEdgeName);//+height

			FString SinkTypeStr = TEXT("");
			for (auto& Str : TabelObj->TableInfo.SinkTypeArray)
			{
				if (!SinkTypeStr.IsEmpty())
				{
					SinkTypeStr += TEXT("\\");
				}
				SinkTypeStr += Str;
			}
			PInfo.AreaPropertyMap.Add(1012, SinkTypeStr);
		}
	}
}
void FArmyConstructionFunction::GenerateCupboardPlanar(const FName& InLayerName)
{
	auto ClipSingleRoom = [&](const TSharedPtr<FArmyRoom> InRoom, const FVector& Offset) {

		TArray<TSharedPtr<FArmyHardware>> ObjectArray;
		TArray<TSharedPtr<FArmyLine>> LineArray;
		InRoom->GetLines(LineArray);
		InRoom->GetHardwareListRelevance(ObjectArray);

		TArray<TSharedPtr<FArmyLine>> ScaleLineArray;

		struct FDirectionOffset
		{
			FVector Value;

			FDirectionOffset(const FVector& InV) :Value(InV) {}
			bool operator==(const FDirectionOffset& InOther) const
			{
				return (Value.GetSafeNormal() + InOther.Value.GetSafeNormal()).Size() < 0.001 ||
					(Value.GetSafeNormal() - InOther.Value.GetSafeNormal()).Size() < 0.001;
			}
		};
		TMap<TSharedPtr<FArmyPoint>, TArray<FDirectionOffset>> OffsetMap;

		for (auto HObj : ObjectArray)
		{
			if (HObj->GetType() == OT_DoorHole) continue;//如果是门洞，用门来判断，而不用门洞（涉及拆改问题）
			FObjectPtr CopyObj = HObj->CopySelf();
			FArmyHardware* HWObj = CopyObj->AsassignObj<FArmyHardware>();
			
			if (HWObj)
			{
				// 临时解决崩溃问题
				if (!(HWObj->LinkFirstLine.IsValid()) || !(HWObj->LinkSecondLine.IsValid()))
				{
					continue;
				}
				HWObj->Point->SetPos(HWObj->GetPos() + Offset);
				HWObj->Update();
				FArmySceneData::Get()->Add(CopyObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
			}

			FVector CenterPos = (HObj->GetEndPos() + HObj->GetStartPos()) / 2;
			FVector ClosePoint1 = FMath::ClosestPointOnInfiniteLine(HWObj->LinkFirstLine->GetStartPointer()->Pos, HWObj->LinkFirstLine->GetEndPointer()->Pos, CenterPos);
			FVector ClosePoint2 = FMath::ClosestPointOnInfiniteLine(HWObj->LinkSecondLine->GetStartPointer()->Pos, HWObj->LinkSecondLine->GetEndPointer()->Pos, CenterPos);

			if (LineArray.Contains(HWObj->LinkFirstLine))
			{
				ScaleLineArray.AddUnique(HWObj->LinkFirstLine);

				FVector TempScale = ClosePoint2 - ClosePoint1;

				if (OffsetMap.Contains(HWObj->LinkFirstLine->GetStartPointer()))
				{
					OffsetMap[HWObj->LinkFirstLine->GetStartPointer()].AddUnique(FDirectionOffset(TempScale));
				}
				else
				{
					OffsetMap.Add(HWObj->LinkFirstLine->GetStartPointer(), TArray<FDirectionOffset>{FDirectionOffset(TempScale)});
				}

				if (OffsetMap.Contains(HWObj->LinkFirstLine->GetEndPointer()))
				{
					OffsetMap[HWObj->LinkFirstLine->GetEndPointer()].AddUnique(FDirectionOffset(TempScale));
				}
				else
				{
					OffsetMap.Add(HWObj->LinkFirstLine->GetEndPointer(), TArray<FDirectionOffset>{FDirectionOffset(TempScale)});
				}
			}
			else
			{
				ScaleLineArray.AddUnique(HWObj->LinkSecondLine);

				FVector TempScale = ClosePoint1 - ClosePoint2;

				if (OffsetMap.Contains(HWObj->LinkSecondLine->GetStartPointer()))
				{
					OffsetMap[HWObj->LinkSecondLine->GetStartPointer()].AddUnique(FDirectionOffset(TempScale));
				}
				else
				{
					OffsetMap.Add(HWObj->LinkSecondLine->GetStartPointer(), TArray<FDirectionOffset>{FDirectionOffset(TempScale)});
				}

				if (OffsetMap.Contains(HWObj->LinkSecondLine->GetEndPointer()))
				{
					OffsetMap[HWObj->LinkSecondLine->GetEndPointer()].AddUnique(FDirectionOffset(TempScale));
				}
				else
				{
					OffsetMap.Add(HWObj->LinkSecondLine->GetEndPointer(), TArray<FDirectionOffset>{FDirectionOffset(TempScale)});
				}
			}
		}
		for (auto L : LineArray)
		{
			if (!ScaleLineArray.Contains(L))
			{
				FVector Dir = (L->GetEnd() - L->GetStart()).GetSafeNormal();
				FVector OutDir(Dir.Y, -Dir.X, 0);
				FVector Center = (L->GetEnd() + L->GetStart()) / 2;

				if (InRoom->IsPointInRoom(Center + OutDir))
				{
					OutDir *= -1;
				}
				OutDir *= 24;

				if (OffsetMap.Contains(L->GetStartPointer()))
				{
					OffsetMap[L->GetStartPointer()].AddUnique(FDirectionOffset(OutDir));
				}
				else
				{
					OffsetMap.Add(L->GetStartPointer(), TArray<FDirectionOffset>{FDirectionOffset(OutDir)});
				}

				if (OffsetMap.Contains(L->GetEndPointer()))
				{
					OffsetMap[L->GetEndPointer()].AddUnique(FDirectionOffset(OutDir));
				}
				else
				{
					OffsetMap.Add(L->GetEndPointer(), TArray<FDirectionOffset>{FDirectionOffset(OutDir)});
				}
			}
		}

		FObjectPtr CopyRoom = InRoom->CopySelf();
		FArmyRoom* Room = CopyRoom->AsassignObj<FArmyRoom>();
		if (Room)
		{
			for (auto It : Room->GetPoints())
			{
				It->SetPos(It->GetPos() + Offset);
			}
			Room->UpdateSpacing();
		}
		TSharedPtr<FArmyRoom> OutRoom = MakeShareable(new FArmyRoom);
		OutRoom->SetType(OT_OutRoom);
		for (auto L : LineArray)
		{
			FVector StartOffset(ForceInitToZero), EndOffset(ForceInitToZero);
			for (auto& V : OffsetMap.FindRef(L->GetStartPointer()))
			{
				StartOffset += V.Value;
			}
			for (auto& V : OffsetMap.FindRef(L->GetEndPointer()))
			{
				EndOffset += V.Value;
			}
			FVector StartP = L->GetStartPointer()->Pos + StartOffset + Offset;
			FVector EndP = L->GetEndPointer()->Pos + EndOffset + Offset;
			OutRoom->AddLine(MakeShareable(new FArmyWallLine(StartP, EndP)));
		}

		FArmySceneData::Get()->Add(CopyRoom, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
		FArmySceneData::Get()->Add(OutRoom, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));

		TArray<FObjectWeakPtr> RelevanceObjArray;
		InRoom->GetObjectsRelevance(RelevanceObjArray);
		for (auto Obj : RelevanceObjArray)
		{
			FObjectPtr CopyObj = Obj.Pin()->CopySelf();
			FArmySimpleComponent* SimpleObj = CopyObj->AsassignObj<FArmySimpleComponent>();
			if (SimpleObj)
			{
				SimpleObj->SetPos(SimpleObj->GetPos() + Offset);
				FArmySceneData::Get()->Add(CopyObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
			}
		}
	};

	TMultiMap<FVector, TSharedPtr<FCabinetFacadeData>> FacadeDataMap;

	FObjectWeakPtr RoomPtr;
	FVector Offset(ForceInitToZero);
	for (auto S : FArmyWHCabinetMgr::Get()->GetPlaceShapes())
	{
		switch ((ECabinetType)S->Shape->GetShapeCategory())
		{
		case ECabinetType::EType_HangCab :
		case ECabinetType::EType_OnGroundCab :
		case ECabinetType::EType_TallCab :
		case ECabinetType::EType_OnCabCab :
			break;
		default:
			continue;
		}

		if (!RoomPtr.IsValid())
		{
			RoomPtr = FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, S->RoomAttachedIndex);
			FVector CenterPos = RoomPtr.Pin()->GetBasePos();
			Offset = BoundBox.GetCenter() - CenterPos;
			ClipSingleRoom(StaticCastSharedPtr<FArmyRoom>(RoomPtr.Pin()), Offset);
		}
		if (S->CabinetActor.IsValid())
		{
			if (!S->CabinetActor->Have(S->CabinetActor->HashClassType<XRWHCabinetFurnitureComponent>()))
			{
				auto ICabinetComponent = S->CabinetActor->GenCabinetComponents(
					FShapeInRoom::ECC_TYPE::CCT_FArmyFURNITURE, S.Get());
			}
			XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = S->CabinetActor;
			TSharedPtr<class FArmyFurniture> ShapeComponent = FurnitureComponent->Get();
			if (ShapeComponent.IsValid()) {

				FObjectPtr CopyShape = ShapeComponent->CopySelf();
				FArmyFurniture* ShapeObj = CopyShape->AsassignObj<FArmyFurniture>();
				if (ShapeObj)
				{
					ShapeObj->ApplyTransform(FTransform(Offset));
					ShapeObj->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, true);
				}
				FArmySceneData::Get()->Add(CopyShape, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
			}
		}
		// @zengy TODO: 见光板图纸部分需要重新写
		// for (auto SunBoard : S->mSunBoard)
		// {
		// 	if (SunBoard)
		// 	{
		// 		FVector MinV, MaxV;
		// 		SunBoard->GetStaticMeshComponent()->GetLocalBounds(MinV, MaxV);

		// 		FVector Center = (MaxV + MinV) / 2;
		// 		FVector MinOffset = MinV - Center;
		// 		FVector MaxOffset = MaxV - Center;

		// 		FTransform Trans = SunBoard->GetTransform();
		// 		Center = Trans.TransformPosition(Center);
		// 		FVector Pos = Trans.GetLocation();
		// 		Pos.Z = 0;
		// 		Center.Z = 0;
		// 		Trans.SetLocation(Center + Offset);

		// 		TSharedPtr<FArmyCustomDefine> GapObject = MakeShareable(new FArmyCustomDefine);
		// 		GapObject->SetBelongClass(Class_SCTCabinet);
		// 		GapObject->ApplyTransform(Trans);

		// 		TArray<FSimpleElementVertex> VArray{
		// 			FSimpleElementVertex(FVector4(MinOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MaxOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MaxOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MaxOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MaxOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MinOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MinOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 			FSimpleElementVertex(FVector4(MinOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		// 		};
		// 		GapObject->SetBaseVertexArray(VArray);

		// 		FArmySceneData::Get()->Add(GapObject, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
		// 	}
		// }

		if (!S->CabinetActor.IsValid())
			return;
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = S->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Component.IsValid())
		{
			FObjectPtr AccessoryObj = Accessory->Component->CopySelf();
			AccessoryObj->ApplyTransform(FTransform(Offset));
			AccessoryObj->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, true);
			FArmySceneData::Get()->Add(AccessoryObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
		}
		///////////////////////////////////////////////////////////立面索引符号
		FRotator Rot = S->ShapeFrame->GetActorRotation();
		FVector FaceDir = Rot.RotateVector(FVector(0, 1, 0));
		FVector RDir(FaceDir.Y, -FaceDir.X, 0);

		TArray<FVector> FaceDirArray;
		FacadeDataMap.GetKeys(FaceDirArray);
		bool ExsitKeyF = false;
		for (auto& V : FaceDirArray)
		{
			if ((V + FaceDir).Size() > 1.5)
			{
				FacadeDataMap.AddUnique(V, MakeShareable(new FCabinetFacadeData(S)));
				ExsitKeyF = true;
				break;
			}
		}

		if (!ExsitKeyF)
		{
			FacadeDataMap.AddUnique(FaceDir, MakeShareable(new FCabinetFacadeData(S)));
		}
	}

	TArray<FObjectWeakPtr> CupboardTableArray;
	FArmySceneData::Get()->GetObjects(E_WHCModel, OT_CupboardTable, CupboardTableArray);
	for (auto TablePtr : CupboardTableArray)
	{
		FObjectPtr NewTable = TablePtr.Pin()->CopySelf();
		FRotator R;
		FVector T;
		FVector S;
		NewTable->GetTransForm(R, T, S);
		NewTable->ApplyTransform(FTransform(R, T + Offset, S));
		FArmyWHCTableObject* TabelObj = NewTable->AsassignObj<FArmyWHCTableObject>();
		if (TabelObj)
		{
			TabelObj->SetDrawComponent(false);
			TabelObj->SetDrawFrontOutLine(false);
			TabelObj->SetDrawBackOutLine(false);
		}
		FArmySceneData::Get()->Add(NewTable, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
	}

	TArray<AXRWhcGapActor*> GapArray;
	FArmyWHCabinetMgr::Get()->GetGapActors(-1, GapArray);
	for (auto GapActor : GapArray)
	{
		FVector MinV, MaxV;
		GapActor->GetStaticMeshComponent()->GetLocalBounds(MinV, MaxV);


		FVector Center = (MaxV + MinV) / 2;
		FVector MinOffset = MinV - Center;
		FVector MaxOffset = MaxV - Center;

		FTransform Trans = GapActor->GetTransform();
		Center = Trans.TransformPosition(Center);
		FVector Pos = Trans.GetLocation();
		Pos.Z = 0;
		Center.Z = 0;
		Trans.SetLocation(Center + Offset);

		TSharedPtr<FArmyCustomDefine> GapObject = MakeShareable(new FArmyCustomDefine);
		GapObject->SetBelongClass(Class_SCTCabinet);
		GapObject->ApplyTransform(Trans);

		TArray<FSimpleElementVertex> VArray{
			FSimpleElementVertex(FVector4(MinOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MaxOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MaxOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MaxOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MaxOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MinOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MinOffset.X,MaxOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
			FSimpleElementVertex(FVector4(MinOffset.X,MinOffset.Y,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
		};
		GapObject->SetBaseVertexArray(VArray);

		FArmySceneData::Get()->Add(GapObject, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
	}

	TArray<FVector> FaceDirArray;
	FacadeDataMap.GetKeys(FaceDirArray);
	for (auto S : FArmyWHCabinetMgr::Get()->GetPlaceShapes())
	{
		bool ExsitKeyL = false, ExsitKeyR = false;

		FRotator Rot = S->ShapeFrame->GetActorRotation();
		FVector FaceDir = Rot.RotateVector(FVector(0, 1, 0));
		FVector RDir(FaceDir.Y, -FaceDir.X, 0);

		for (auto& V : FaceDirArray)
		{
			if ((V + RDir).Size() > 1.5)
			{
				FVector CurrentPos = S->ShapeFrame->GetActorLocation();
				float D = S->Shape->GetShapeDepth();
				float W = S->Shape->GetShapeWidth();
				FVector CurrentFaceL = CurrentPos + FaceDir * D / 10 - RDir * W / 20;
				FVector CurrentFaceR = CurrentPos + FaceDir * D / 10 + RDir * W / 20;

				TArray<TSharedPtr<FCabinetFacadeData>> FaceDataArray;
				FacadeDataMap.MultiFind(V, FaceDataArray);
				for (auto FaceData : FaceDataArray)
				{
					FVector TempPos = FaceData->Shape->ShapeFrame->GetActorLocation();
					float TempW = FaceData->Shape->Shape->GetShapeWidth();
					float TempD = FaceData->Shape->Shape->GetShapeDepth();
					FVector BackRP = TempPos + FVector(V.Y, -V.X, 0) * TempW / 20;
					FVector FRontRP = BackRP + V * TempD / 10;
					FVector FRontLP = FRontRP - FVector(V.Y, -V.X, 0) * TempW / 10;

					if (FMath::PointDistToSegment(CurrentFaceL, BackRP, FRontRP) < 15 || FMath::PointDistToSegment(CurrentFaceR, BackRP, FRontRP) < 15)//两个柜子间距小于15cm则判断为相邻
					{
						FacadeDataMap.AddUnique(V, MakeShareable(new FCabinetFacadeData(S, FCabinetFacadeData::EA_RIGHT)));
						ExsitKeyR = true;
						break;
					}
					if (FMath::PointDistToSegment(CurrentFaceL, FRontLP, FRontRP) < 15 || FMath::PointDistToSegment(CurrentFaceR, FRontLP, FRontRP) < 15)//两个柜子间距小于15cm则判断为相邻
					{
						FacadeDataMap.AddUnique(V, MakeShareable(new FCabinetFacadeData(S, FCabinetFacadeData::EA_RIGHT)));
						ExsitKeyL = true;
						break;
					}
				}
			}
			else if ((V - RDir).Size() > 1.5)
			{
				FVector CurrentPos = S->ShapeFrame->GetActorLocation();
				float D = S->Shape->GetShapeDepth();
				float W = S->Shape->GetShapeWidth();
				FVector CurrentFaceL = CurrentPos + FaceDir * D / 10 - RDir * W / 20;
				FVector CurrentFaceR = CurrentPos + FaceDir * D / 10 + RDir * W / 20;

				TArray<TSharedPtr<FCabinetFacadeData>> FaceDataArray;
				FacadeDataMap.MultiFind(V, FaceDataArray);
				for (auto FaceData : FaceDataArray)
				{
					FVector TempPos = FaceData->Shape->ShapeFrame->GetActorLocation();
					float TempW = FaceData->Shape->Shape->GetShapeWidth();
					float TempD = FaceData->Shape->Shape->GetShapeDepth();
					FVector BackLP = TempPos - FVector(V.Y, -V.X, 0) * TempW / 20;
					FVector FRontLP = BackLP + V * TempD / 10;
					FVector FRontRP = FRontLP + FVector(V.Y, -V.X, 0) * TempW / 10;

					if (FMath::PointDistToSegment(CurrentFaceL, BackLP, FRontLP) < 15 || FMath::PointDistToSegment(CurrentFaceR, BackLP, FRontLP) < 15)//两个柜子间距小于15cm则判断为相邻
					{
						FacadeDataMap.AddUnique(V, MakeShareable(new FCabinetFacadeData(S, FCabinetFacadeData::EA_LEFT)));
						ExsitKeyL = true;
						break;
					}
					if (FMath::PointDistToSegment(CurrentFaceL, FRontLP, FRontRP) < 15 || FMath::PointDistToSegment(CurrentFaceR, FRontLP, FRontRP) < 15)//两个柜子间距小于15cm则判断为相邻
					{
						FacadeDataMap.AddUnique(V, MakeShareable(new FCabinetFacadeData(S, FCabinetFacadeData::EA_LEFT)));
						ExsitKeyL = true;
						break;
					}
				}
			}

			if (ExsitKeyL && ExsitKeyR) break;
		}
	}

	GenerateCupboardFacade(InLayerName, FacadeDataMap, Offset);
}
void FArmyConstructionFunction::GenerateCupboardFacade(const FName& InLayerName, TMultiMap<FVector, TSharedPtr<FCabinetFacadeData>>& InFacadeDataMap, const FVector& InOffset)
{
	FName LayerName(*(FString(TEXT("橱柜安装立面图"))));//暂时没有分空间，所以名称暂时不加空间名称了
	DelegateNewLayer.Execute(LayerName, false, FArmyLayer::LT_D_CUPBOARDTABLE);
	int32 ShapeIndex = 0;

	TArray<FVector> FaceDirArray;
	InFacadeDataMap.GetKeys(FaceDirArray);
	int32 KeyIndex = 0;
	float VOffsetValue = 0;
	for (auto V : FaceDirArray)
	{
		TArray<TSharedPtr<FCabinetFacadeData>> FaceDataArray;
		InFacadeDataMap.MultiFind(V, FaceDataArray);

		if (FaceDataArray.Num() > 0)
		{
			FVector RDir(V.Y, -V.X, 0);

			FVector TempStartPos = FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() - RDir * 100;
			FVector TempEndPos = FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() + RDir * 100;

			FaceDataArray.Sort([&](const TSharedPtr<FCabinetFacadeData> A, const TSharedPtr<FCabinetFacadeData> B) {
				FVector APos = A->Shape->ShapeFrame->GetActorLocation();
				FVector BPos = B->Shape->ShapeFrame->GetActorLocation();
				FVector NA = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, APos);
				FVector NB = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, BPos);

				bool SameDir = ((NB - NA).GetSafeNormal() + RDir).Size() > 1.5;//同向
				if (APos.Z < BPos.Z && SameDir)//把吊柜往后排
				{
					return true;
				}
				else if (APos.Z < BPos.Z)
				{
					return true;
				}
				else
				{
					return SameDir;
				}
			});

			TSharedPtr<FCabinetFacadeData> PreFaceData = nullptr;

			float MaxDepth = 0, MaxHeight = 0;
			FBox MaxBox(ForceInitToZero);
			FVector MaxFacePoint(ForceInitToZero);
			FPlane TempPlane(FVector(0, 0, 0), V);

			for (auto FaceData : FaceDataArray)
			{
				float W = FaceData->Shape->Shape->GetShapeWidth();
				float D = FaceData->Shape->Shape->GetShapeDepth();
				float H = FaceData->Shape->Shape->GetShapeHeight();

				FVector Location = FaceData->Shape->ShapeFrame->GetActorLocation();

				if (FaceData->Shape->Shape->GetShapeCategory() == int32(ECabinetType::EType_OnGroundCab))//地柜才有台面
				{
					FVector CurrentPoint = Location + V * (D / 10 + 5);
					FVector MaxNP = FVector::PointPlaneProject(MaxFacePoint, TempPlane);
					FVector CurrentNP = FVector::PointPlaneProject(CurrentPoint, TempPlane);

					FVector MaxOffset = MaxNP - MaxFacePoint;
					FVector CurrentOffset = CurrentNP - CurrentPoint;

					float MaxLen = (MaxOffset.GetSafeNormal() - V).Size() < 0.01 ? MaxOffset.Size() : MaxOffset.Size() * -1;
					float CurrentLen = (CurrentOffset.GetSafeNormal() - V).Size() < 0.01 ? CurrentOffset.Size() : CurrentOffset.Size() * -1;

					if (CurrentLen > MaxLen && CurrentLen > 0)
					{
						MaxFacePoint = CurrentPoint;
					}

					if (PreFaceData.IsValid())
					{
						FVector PreLocation = PreFaceData->Shape->ShapeFrame->GetActorLocation();
						float PreW = PreFaceData->Shape->Shape->GetShapeWidth();
						float PreH = PreLocation.Z + PreFaceData->Shape->Shape->GetShapeHeight();

						bool AlignChange = false;

						if (PreFaceData->AlignType == FCabinetFacadeData::EA_LEFT)
						{
							PreW = PreFaceData->Shape->Shape->GetShapeDepth();
							PreLocation = PreLocation + RDir * PreW / 20;
							AlignChange = true;
						}
						else if (PreFaceData->AlignType == FCabinetFacadeData::EA_RIGHT)
						{
							PreW = PreFaceData->Shape->Shape->GetShapeDepth();
							PreLocation = PreLocation - RDir * PreW / 20;
							AlignChange = true;
						}

						if (FaceData->AlignType == FCabinetFacadeData::EA_LEFT || FaceData->AlignType == FCabinetFacadeData::EA_RIGHT)
						{
							AlignChange = true;
						}

						float CurrentH = Location.Z + H / 10;

						FVector PreNearP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, PreLocation);
						FVector CurrentNearP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, Location);

						PreFaceData = FaceData;
					}
					else
					{
						PreFaceData = FaceData;
					}
				}


				MaxBox += Location;
				MaxBox += (Location + FVector(0, 0, H / 10));

				D > MaxDepth ? MaxDepth = D : MaxDepth;
			}
			MaxBox.GetSize().Z > MaxHeight ? MaxHeight = MaxBox.GetSize().Z : MaxHeight;

			float StartW = FaceDataArray[0]->Shape->Shape->GetShapeWidth();
			TempStartPos = FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() - RDir * StartW / 20;
			TempEndPos = FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() + RDir * StartW / 20;
			TempStartPos.Z = MaxBox.Min.Z;
			TempEndPos.Z = MaxBox.Min.Z;

			VOffsetValue += (MaxHeight + 70);
			FVector VOffset = FVector(0, 1, 0) * VOffsetValue;

			FVector CenterPos = (FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() + FaceDataArray.Last()->Shape->ShapeFrame->GetActorLocation()) / 2 + (MaxDepth / 10 + 10) * V;
			//添加索引符号到CenterPos ，-V
			KeyIndex++;
			TSharedPtr<FArmySymbol> Symbol = MakeShareable(new FArmySymbol);
			FString Text = TEXT("");
			switch (KeyIndex)
			{
			case 1:
				Text = TEXT("A");
				break;
			case 2:
				Text = TEXT("B");
				break;
			case 3:
				Text = TEXT("C");
				break;
			case 4:
				Text = TEXT("D");
				break;
			case 5:
				Text = TEXT("E");
				break;
			case 6:
				Text = TEXT("F");
				break;
			case 7:
				Text = TEXT("G");
				break;
			case 8:
				Text = TEXT("H");
				break;
			default:
				break;
			}

			Symbol->SetText(Text);
			Symbol->ApplyTransform(FTransform(FQuat::FindBetween(FVector(0, -1, 0), -V), CenterPos + InOffset));
			FArmySceneData::Get()->Add(Symbol, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
			///////////////////////////////////////////////////////////////////////添加柜体立面
			FVector FaceStartPos = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, FaceDataArray[0]->Shape->ShapeFrame->GetActorLocation() - RDir * FaceDataArray[0]->Shape->Shape->GetShapeWidth() / 20);
			for (auto FaceData : FaceDataArray)
			{
				if (FaceData->AlignType == FCabinetFacadeData::EA_FACE)
				{
					float W = FaceData->Shape->Shape->GetShapeWidth() / 10;
					float H = FaceData->Shape->Shape->GetShapeHeight() / 10;
					float D = FaceData->Shape->Shape->GetShapeDepth() / 10;

					FVector ShapePos = FaceData->Shape->ShapeFrame->GetActorLocation() - RDir * W / 2;
					FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, ShapePos);
					float VValue = ShapePos.Z - NP.Z;
					float  LocalOffset = (NP - FaceStartPos).Size();

					TSharedPtr<FArmyCustomDefine> ShapeFace = MakeShareable(new FArmyCustomDefine);
					ShapeFace->SetType(OT_CabinetFace);
					ShapeFace->SetBelongClass(Class_SCTCabinet);
					ShapeFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					TArray<FSimpleElementVertex> VArray{
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(W,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(W,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(W,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(W,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					};
					ShapeFace->SetBaseVertexArray(VArray);
					FArmySceneData::Get()->Add(ShapeFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

					//见光板
					// @zengy TODO: 见光板部分需要重新写
					// for (auto SunBoard : FaceData->Shape->mSunBoard)
					// {
					// 	if (SunBoard)
					// 	{
					// 		FVector RightDir = SunBoard->GetActorForwardVector();//rotate FVector(1,0,0)
					// 		if ((RightDir - V).Size() > 1.5)
					// 		{
					// 			UStaticMesh* pMesh = FArmyWHCabinetMgr::Get()->GetStaticMesh("WHC/400_600_418_sunboard.pak");
					// 			FVector MeshSize = pMesh->GetBoundingBox().GetExtent();
					// 			FVector SunBoardSize = MeshSize * SunBoard->GetActorScale3D() * 2;

					// 			FVector BoardPos = SunBoard->GetActorLocation();

					// 			FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, BoardPos);
					// 			float VValue = BoardPos.Z - NP.Z;

					// 			FVector TrimOffset = NP - FaceStartPos;
					// 			int32 Flag = (TrimOffset.GetSafeNormal() - RDir).Size() < 0.01 ? 1 : -1;
					// 			float  LocalOffset = TrimOffset.Size() * Flag;

					// 			TSharedPtr<FArmyCustomDefine> BoardFace = MakeShareable(new FArmyCustomDefine);
					// 			ShapeFace->SetType(OT_SunBoardFace);
					// 			BoardFace->SetBelongClass(Class_SCTCabinet);
					// 			BoardFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					// 			TArray<FSimpleElementVertex> VArray{
					// 				FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(SunBoardSize.Y,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(SunBoardSize.Y,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(SunBoardSize.Y,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(SunBoardSize.Y,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 			};
					// 			BoardFace->SetBaseVertexArray(VArray);
					// 			FArmySceneData::Get()->Add(BoardFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
					// 		}
					// 		else if ((RightDir + V).Size() > 1.5)
					// 		{
					// 			UStaticMesh* pMesh = FArmyWHCabinetMgr::Get()->GetStaticMesh("WHC/400_600_418_sunboard.pak");
					// 			FVector MeshSize = pMesh->GetBoundingBox().GetExtent();
					// 			FVector SunBoardSize = MeshSize * SunBoard->GetActorScale3D() * 2;

					// 			FVector BoardPos = SunBoard->GetActorLocation();

					// 			FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, BoardPos);
					// 			float VValue = BoardPos.Z - NP.Z;
					// 			FVector TrimOffset = NP - FaceStartPos;
					// 			int32 Flag = (TrimOffset.GetSafeNormal() - RDir).Size() < 0.01 ? 1 : -1;
					// 			float  LocalOffset = TrimOffset.Size() * Flag;

					// 			TSharedPtr<FArmyCustomDefine> BoardFace = MakeShareable(new FArmyCustomDefine);
					// 			ShapeFace->SetType(OT_SunBoardFace);
					// 			BoardFace->SetBelongClass(Class_SCTCabinet);
					// 			BoardFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					// 			TArray<FSimpleElementVertex> VArray{
					// 				FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(-SunBoardSize.Y,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(-SunBoardSize.Y,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(-SunBoardSize.Y,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(-SunBoardSize.Y,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,-SunBoardSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 				FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					// 			};
					// 			BoardFace->SetBaseVertexArray(VArray);
					// 			FArmySceneData::Get()->Add(BoardFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

					// 		}
					// 	}
					// }

					//////////////////////////////////////////添加柜体序号start//////////////////////////////////////////////////
					TSharedPtr<FArmySymbol> IndexSymbol = MakeShareable(new FArmySymbol);
					IndexSymbol->SetText(FString::FromInt(++ShapeIndex));
					IndexSymbol->SetDrawArrows(false);
					IndexSymbol->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * (LocalOffset + W / 2) + FVector(0, -1, 0) * (VValue + H / 2 + 15/*symbol 内部的偏移值*/) + VOffset));
					FArmySceneData::Get()->Add(IndexSymbol, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
					//////////////////////////////////////////添加柜体序号end//////////////////////////////////////////////////

					//////////////////////////////////////////添加隔板投影数据start//////////////////////////////////////////////////
					TArray<FSimpleElementVertex> BoardVArray;
					TArray<FBox2D> SheetBoxArray;
					if (GenerateBoardFaceOutline(FaceData->Shape->Shape->GetShapeActor(), FaceData->Shape->Shape, FaceStartPos + V * (D + 10), FPlane(FaceStartPos + V * (D + 10), V), LayerName, TempStartPos, TempEndPos, InOffset + VOffset, SheetBoxArray, BoardVArray))
					{
						if (BoardVArray.Num() > 1)
						{
							TSharedPtr<FArmyCustomDefine> BoardShapeFace = MakeShareable(new FArmyCustomDefine);
							ShapeFace->SetType(OT_CabinetFace);
							BoardShapeFace->SetBelongClass(Class_SCTCabinet);
							BoardShapeFace->SetBaseVertexArray(BoardVArray);
							BoardShapeFace->ApplyTransform(FTransform(InOffset + VOffset));
							FArmySceneData::Get()->Add(BoardShapeFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
						}
					}
					//////////////////////////////////////////添加隔板投影数据end//////////////////////////////////////////////////

					//////////////////////////////////////////添加灶具水盆start//////////////////////////////////////////////////
					struct FAccessoryFaceOption
					{
						void operator()(const TSharedPtr<FShapeAccessory> InAccessory, const TSharedPtr<FCabinetWholeComponent> InWholeComponent,
							const FVector& InTempStartPos, const FVector& InTempEndPos, const FVector& InFaceStartPos, const FVector& InGlobalOffset,
							const FName& InFaceLayerName) {
							if (InAccessory.IsValid() && InAccessory->Actor)
							{
								//FaceData->Shape->Accessory->Component
								FVector ComponentPos = InAccessory->Actor->GetActorLocation();

								FString AccessoryComponentFile = TEXT("");

								if (InWholeComponent.IsValid())
								{
									for (auto AccessoryComponent : InWholeComponent->Accessorys)
									{
										if (AccessoryComponent->Id == InAccessory->CabAccInfo->Id)
										{
											AccessoryComponentFile = AccessoryComponent->GetCachePath(2);
											break;
										}
									}
								}
								if (AccessoryComponentFile.IsEmpty())
								{
									//switch (EMetalsType(InAccessory->CabAccInfo->Type))
									//{
									//case EMetalsType::MT_WATER_CHANNE:
									//{
									//	AccessoryComponentFile = FPaths::ProjectContentDir() + "Assets/" + "face_water_channe.dxf";
									//}
									//break;
									//case EMetalsType::MT_BIBCOCK:
									//{
									//	AccessoryComponentFile = FPaths::ProjectContentDir() + "Assets/" + "face_bibcock.dxf";
									//}
									//break;
									//case EMetalsType::MT_KITCHEN_RANGE:
									//{
									//	AccessoryComponentFile = FPaths::ProjectContentDir() + "Assets/" + "face_kitchen_range.dxf";
									//}
									//break;
									//case EMetalsType::MT_FLUE_GAS_TURBINE:
									//{
									//	AccessoryComponentFile = FPaths::ProjectContentDir() + "Assets/" + "face_flue_gas_turbine.dxf";
									//}
									//break;
									//default:
									//	break;
									//}
								}

								TArray<TSharedPtr<FArmyFurniture>> CADLayers = FArmyToolsModule::Get().ParseDXF(AccessoryComponentFile);
								if (CADLayers.Num() > 0)
								{
									TSharedPtr<FArmyFurniture> FaceObj = CADLayers[0];
									if (FaceObj.IsValid())
									{
										FVector CNP = FMath::ClosestPointOnInfiniteLine(InTempStartPos, InTempEndPos, ComponentPos);
										float CVValue = ComponentPos.Z - CNP.Z;
										float CLocalOffset = (CNP - InFaceStartPos).Size();
										FaceObj->SetType(OT_ComponentBase);
										FaceObj->BelongClass = Class_SCTCabinet;
										FaceObj->ApplyTransform(FTransform(InGlobalOffset + FVector(1, 0, 0) * CLocalOffset + FVector(0, -1, 0) * CVValue));
										FArmySceneData::Get()->Add(FaceObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InFaceLayerName).ArgUint32(E_ConstructionModel));
									}
								}

								for (auto SubAccessory : InAccessory->AttachedSubAccessories)
								{
									this->operator()(SubAccessory, InWholeComponent, InTempStartPos, InTempEndPos, InFaceStartPos, InGlobalOffset, InFaceLayerName);
								}
							}
						}
					};
					if (FaceData->Shape->CabinetActor.IsValid())
					{
						XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = FaceData->Shape->CabinetActor;
						TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
						if (Accessory.IsValid())
						{
							TSharedPtr<FCabinetWholeComponent> Result = FWHCModeGlobalData::CabinetComponentMap.FindRef(FaceData->Shape->Shape->GetShapeId());
							FAccessoryFaceOption AccessoryFaceOption;
							AccessoryFaceOption(Accessory, Result, TempStartPos, TempEndPos, FaceStartPos, InOffset + VOffset, LayerName);
						}
					}
					//////////////////////////////////////////添加灶具水盆end//////////////////////////////////////////////////
				}
				else if (FaceData->AlignType == FCabinetFacadeData::EA_RIGHT)
				{
					float D = FaceData->Shape->Shape->GetShapeDepth() / 10;
					float H = FaceData->Shape->Shape->GetShapeHeight() / 10;

					FVector ShapePos = FaceData->Shape->ShapeFrame->GetActorLocation() - RDir * D;
					FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, ShapePos);
					float VValue = ShapePos.Z - NP.Z;
					float  LocalOffset = (NP - FaceStartPos).Size();

					TSharedPtr<FArmyCustomDefine> ShapeFace = MakeShareable(new FArmyCustomDefine);
					ShapeFace->SetType(OT_CabinetFace);
					ShapeFace->SetBelongClass(Class_SCTCabinet);
					ShapeFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					float DoorThickness = FaceData->ExistDoor ? 2 : 0;
					TArray<FSimpleElementVertex> VArray{
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),

						FSimpleElementVertex(FVector4(DoorThickness,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(DoorThickness,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					};
					if (FaceData->ExistDoor)
					{
						VArray.Add(FSimpleElementVertex(FVector4(DoorThickness, 0, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						VArray.Add(FSimpleElementVertex(FVector4(DoorThickness, -H, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
					ShapeFace->SetBaseVertexArray(VArray);
					FArmySceneData::Get()->Add(ShapeFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
				}
				else if (FaceData->AlignType == FCabinetFacadeData::EA_LEFT)
				{
					float D = FaceData->Shape->Shape->GetShapeDepth() / 10;
					float H = FaceData->Shape->Shape->GetShapeHeight() / 10;

					FVector ShapePos = FaceData->Shape->ShapeFrame->GetActorLocation();
					FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, ShapePos);
					float VValue = ShapePos.Z - NP.Z;
					float  LocalOffset = (NP - FaceStartPos).Size();

					TSharedPtr<FArmyCustomDefine> ShapeFace = MakeShareable(new FArmyCustomDefine);
					ShapeFace->SetType(OT_CabinetFace);
					ShapeFace->SetBelongClass(Class_SCTCabinet);
					ShapeFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					float DoorThickness = FaceData->ExistDoor ? 2 : 0;
					TArray<FSimpleElementVertex> VArray{
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),

						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D - DoorThickness,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(D - DoorThickness,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-H,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					};
					if (FaceData->ExistDoor)
					{
						VArray.Add(FSimpleElementVertex(FVector4(D - DoorThickness, 0, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						VArray.Add(FSimpleElementVertex(FVector4(D - DoorThickness, -H, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
					ShapeFace->SetBaseVertexArray(VArray);
					FArmySceneData::Get()->Add(ShapeFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
				}

				//////////////////////////////////////////添加门把手立面图start//////////////////////////////////////////////////
				TArray<FAccessoryShape*> DoorHanleAccessArray;
				FSCTShapeUtilityTool::GetCabinetAllDoorHanle(FaceData->Shape->Shape.Get(), DoorHanleAccessArray);
				for (auto Accessory : DoorHanleAccessArray)
				{
					FVector Scale3D(1, 1, 1);
					FVector ComponentPos = Accessory->GetShapeActor()->GetActorLocation();
					FString AccessoryComponentFile = TEXT("");
					TSharedPtr<FCabinetWholeComponent> Result = FWHCModeGlobalData::CabinetComponentMap.FindRef(FaceData->Shape->Shape->GetShapeId());
					if (Result.IsValid())
					{
						for (auto AccessoryComponent : Result->Accessorys)
						{
							if (AccessoryComponent->Id == Accessory->GetShapeId())
							{
								if (FaceData->AlignType == FCabinetFacadeData::EA_FACE)
								{
									AccessoryComponentFile = AccessoryComponent->GetCachePath(2);
								}
								else if (FaceData->AlignType == FCabinetFacadeData::EA_LEFT)
								{
									AccessoryComponentFile = AccessoryComponent->GetCachePath(3);
									Scale3D = FVector(-1, 1, 1);
								}
								else if (FaceData->AlignType == FCabinetFacadeData::EA_RIGHT)
								{
									AccessoryComponentFile = AccessoryComponent->GetCachePath(3);
								}
								break;
							}
						}
					}

					TArray<TSharedPtr<FArmyFurniture>> CADLayers = FArmyToolsModule::Get().ParseDXF(AccessoryComponentFile);
					if (CADLayers.Num() > 0)
					{
						TSharedPtr<FArmyFurniture> FaceObj = CADLayers[0];
						if (FaceObj.IsValid())
						{
							FVector CNP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, ComponentPos);
							float CVValue = ComponentPos.Z - CNP.Z;
							FVector TrimOffset = CNP - FaceStartPos;
							int32 Flag = (TrimOffset.GetSafeNormal() - RDir).Size() < 0.01 ? 1 : -1;
							float  CLocalOffset = TrimOffset.Size() * Flag;
							FTransform Trans(InOffset + FVector(1, 0, 0) * CLocalOffset + FVector(0, -1, 0) * CVValue + VOffset);
							Trans.SetScale3D(Scale3D);
							FaceObj->SetType(OT_ComponentBase);
							FaceObj->BelongClass = Class_SCTCabinet;
							FaceObj->ApplyTransform(Trans);
							FArmySceneData::Get()->Add(FaceObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
						}
					}
				}
				//////////////////////////////////////////添加门把手立面图end//////////////////////////////////////////////////
			}

			//////////////////////////////////////////////////////////////图纸名称start
			FVector CenterNP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, CenterPos);
			float  LocalOffset = (CenterNP - FaceStartPos).Size();

			FVector FaceNamePos = InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, 1, 0) * 20 + VOffset;

			TSharedPtr<FArmyTextLabel> TextLabel = MakeShareable(new FArmyTextLabel());
			TextLabel->SetbUseEditPos(false);
			TextLabel->SetWorldPosition(FaceNamePos);
			TextLabel->SetLabelContent(FText::FromString(Text + TEXT("面正视图")));
			TextLabel->SetTextSize(20);
			//TextLabel->SetTextColor(FLinearColor(0.5,0.5,0.5));
			TextLabel->SetLabelType(FArmyTextLabel::LT_None);

			FArmySceneData::Get()->Add(TextLabel, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
			//////////////////////////////////////////////////////////////图纸名称end

			/////////////////////////////////////生成调整板start//////////////////////////////////////////
			TArray<AXRWhcGapActor*> GapArray;
			gFArmyWHCabinMgr->GetGapActors(-1, GapArray);
			FVector GapBaseSize = gFArmyWHCabinMgr->GetSizeGapMesh();
			for (auto Gap : GapArray)
			{
				FVector FaceDir = Gap->GetActorRotation().RotateVector(FVector(0, 1, 0));
				if ((V + FaceDir).Size() > 1.5)//同向
				{
					FVector GapSize = GapBaseSize * Gap->GetActorScale3D() * 2;

					FVector GapPos = Gap->GetActorLocation();
					FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, GapPos);
					float VValue = GapPos.Z - NP.Z;

					FVector TrimOffset = NP - FaceStartPos;
					int32 Flag = (TrimOffset.GetSafeNormal() - RDir).Size() < 0.01 ? 1 : -1;
					float  LocalOffset = (NP - FaceStartPos).Size() * Flag;

					TSharedPtr<FArmyCustomDefine> ShapeFace = MakeShareable(new FArmyCustomDefine);
					ShapeFace->SetType(OT_ResizeFace);
					ShapeFace->SetBelongClass(Class_SCTCabinet);
					ShapeFace->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));

					TArray<FSimpleElementVertex> VArray{
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(GapSize.X,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(GapSize.X,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(GapSize.X,-GapSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(GapSize.X,-GapSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-GapSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,-GapSize.Z,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
						FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
					};
					ShapeFace->SetBaseVertexArray(VArray);
					FArmySceneData::Get()->Add(ShapeFace, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

				}
			}
			/////////////////////////////////////生成调整板end////////////////////////////////////////

			/////////////////////////////////////台面start
			//2d方案
			struct FacePointInfo
			{
				enum FacePointType
				{
					FP_None,
					FP_Left,
					FP_Right
				};
				FacePointInfo(const FVector& InConnerPoint, const FVector InProfileFaceDir, FacePointType InFaceType) :ConnerPoint(InConnerPoint), ProfileFaceDir(InProfileFaceDir), ProfileType(InFaceType) {};
				FacePointType ProfileType = FP_None;
				FVector ConnerPoint;
				FVector ProfileFaceDir;
			};
			FPlane MaxFacePlane(MaxFacePoint + V * 20, V);
			FVector XDir(MaxFacePlane.Y, -MaxFacePlane.X, 0);
			FVector YDir(0, 0, -1);

			//上条线，台面，踢脚线
			auto TrimFaceFunction = [&](const TArray<FArmyWHCabinetMgr::FTrimInfo>& InPath, const FArmySplineRes* InSplineRes, bool bBack) {
				for (auto& Path : InPath)
				{
					bool NoCabinet = true;
					TArray<FVector> PolygonVArray;
					for (int32 i = 0; i < Path.TrimPath.Num() - 1; ++i)
					{
						int32 Index1 = bBack ? i + 1 : i;
						int32 Index2 = bBack ? i : i + 1;

						FVector2D V1 = Path.TrimPath[Index1];
						FVector2D V2 = Path.TrimPath[Index2];
						FVector VV1(V1, 0);
						FVector VV2(V2, 0);

						if (NoCabinet)
						{
							for (auto FaceDataPtr : FaceDataArray)
							{
								float FaceD = bBack ? 0 : FaceDataPtr->Shape->Shape->GetShapeDepth() / 10;
								float FaceW = FaceDataPtr->Shape->Shape->GetShapeWidth() / 10;

								if (FaceDataPtr->AlignType == FCabinetFacadeData::EA_FACE)
								{
									FVector ShapeFaceP(FaceDataPtr->Shape->ShapeFrame->GetActorLocation() + V * FaceD);
									ShapeFaceP.Z = 0;

									if (FMath::PointDistToSegment(ShapeFaceP, VV1, VV2) < 10)
									{
										NoCabinet = false;
										break;
									}
								}
								else
								{
									FVector ShapeFaceRP = FaceDataPtr->Shape->ShapeFrame->GetActorLocation() + V * FaceW / 2;
									ShapeFaceRP.Z = 0;

									if (FMath::PointDistToSegment(ShapeFaceRP, VV1, VV2) < 10)
									{
										NoCabinet = false;
										break;
									}

									FVector ShapeFaceLP = FaceDataPtr->Shape->ShapeFrame->GetActorLocation() - V * FaceW / 2;
									ShapeFaceLP.Z = 0;

									if (FMath::PointDistToSegment(ShapeFaceLP, VV1, VV2) < 10)
									{
										NoCabinet = false;
										break;
									}
								}
							}
						}

						PolygonVArray.Add(VV1);
						PolygonVArray.Add(VV2);
						FVector2D Dir = (V2 - V1).GetSafeNormal();
						FVector2D VOutDir(-Dir.Y, Dir.X);
						FVector2D MinInterPoint1(V1 - VOutDir * 10000);
						FVector2D MinInterPoint2(V2 - VOutDir * 10000);

						bool ExsistL1 = false, ExsistL2 = false;
						for (int32 j = 0; j < Path.TrimPath.Num() - 1; ++j)
						{
							if (i == j) continue;
							FVector2D V21 = Path.TrimPath[j];
							FVector2D V22 = Path.TrimPath[j + 1];
							FVector VV21(V21, 0);
							FVector VV22(V22, 0);
							if (!ExsistL1 && (V21 == V1 || V22 == V1))
							{
								ExsistL1 = true;
							}
							else
							{
								FVector NearP1(VV1);

								if (FMath::SegmentIntersection2D(VV21, VV22, VV1, FVector(V1 - VOutDir * 10000, 0), NearP1))
								{
									FVector2D NearP2D(NearP1);
									if ((MinInterPoint1 - V1).Size() > (NearP2D - V1).Size())
									{
										MinInterPoint1 = NearP2D;
									}
								}
							}
							if (!ExsistL2 && (V21 == V2 || V22 == V2))
							{
								ExsistL2 = true;
							}
							else
							{
								FVector NearP2(VV2);

								if (FMath::SegmentIntersection2D(VV21, VV22, VV2, FVector(V2 - VOutDir * 10000, 0), NearP2))
								{
									FVector2D NearP2D(NearP2);
									if ((MinInterPoint2 - V2).Size() > (NearP2D - V2).Size())
									{
										MinInterPoint2 = NearP2D;
									}
								}
							}

							if (ExsistL1 && ExsistL2)
							{
								break;
							}
						}
						if (!ExsistL1)
						{
							PolygonVArray.Add(FVector(MinInterPoint1, 0));
							PolygonVArray.Add(VV1);
						}
						if (!ExsistL2)
						{
							PolygonVArray.Add(FVector(MinInterPoint2, 0));
							PolygonVArray.Add(VV2);
						}
					}

					if (NoCabinet)
					{
						continue;//该线条下没有当前要计算的柜体，则不需要生成该线条的立面
					}

					TArray<TSharedPtr<FArmyLine>> LastLines;
					TArray<FacePointInfo> LastPoints;

					for (int32 i = 0; i < Path.TrimPath.Num() - 1; ++i)
					{
						int32 Index1 = bBack ? i + 1 : i;
						int32 Index2 = bBack ? i : i + 1;
						FVector V1(Path.TrimPath[Index1], 0);
						FVector V2(Path.TrimPath[Index2], 0);
						FVector Dir = (V2 - V1).GetSafeNormal();

						FVector VOutDir(-Dir.Y, Dir.X, 0);
						FVector VInnerDir = VOutDir * -1;

						FVector VPStart = FVector::PointPlaneProject(V1, MaxFacePlane);
						FVector VPEnd = FVector::PointPlaneProject(V2, MaxFacePlane);
						//与面垂直
						bool VerticalPlane = (Dir + V).Size() < 0.01 || (Dir - V).Size() < 0.01;

						FVector StartOffset = VPStart - V1;
						FVector EndOffset = VPEnd - V2;
						bool StartInBack = (StartOffset.GetSafeNormal() + V).Size() < 0.001;
						bool EndInBack = (EndOffset.GetSafeNormal() + V).Size() < 0.001;
						if (StartInBack && EndInBack)//方向相反
						{
							continue;
						}
						else if (StartInBack || (!StartInBack && !EndInBack && (EndOffset.Size() - StartOffset.Size()) > 0.1))
						{
							if (!VerticalPlane)
							{
								LastLines.AddUnique(MakeShareable(new FArmyLine(VPStart, VPEnd)));
							}

							bool ExsistNextLine = false;
							for (int32 j = 0; j < Path.TrimPath.Num() - 1; ++j)
							{
								if (i == j) continue;
								FVector V21(Path.TrimPath[j], 0);
								FVector V22(Path.TrimPath[j + 1], 0);

								if (V21 == V1 || V22 == V1)
								{
									ExsistNextLine = true;
									break;
								}
							}
							if (!ExsistNextLine && !bBack)
							{
								LastPoints.Add(FacePointInfo(VPEnd, -Dir, FacePointInfo::FP_None));
							}
							else
							{
								if ((VOutDir + RDir).Size() < 0.5)
								{
									LastPoints.Add(FacePointInfo(VPStart, -Dir, bBack ? FacePointInfo::FP_Right : FacePointInfo::FP_Left));
								}
								else if ((VOutDir - RDir).Size() < 0.5)
								{
									LastPoints.Add(FacePointInfo(VPStart, -Dir, bBack ? FacePointInfo::FP_Left : FacePointInfo::FP_Right));
								}
							}

							if (StartInBack && !bBack)//后档不需要相交点
							{
								FVector MidPoint = (V1 + V2) / 2;
								for (int32 j = 0; j < PolygonVArray.Num() - 1; j += 2)
								{
									FVector VS = PolygonVArray[j];
									FVector VE = PolygonVArray[j + 1];

									FVector P;
									if (FArmyMath::SegmentIntersection2D(VS, VE, MidPoint + VInnerDir, MidPoint + VInnerDir * 250, P))//相交
									{
										LastLines.AddUnique(MakeShareable(new FArmyLine(MidPoint, P)));
										LastPoints.Add(FacePointInfo(P, -Dir, FacePointInfo::FP_None));
										break;
									}
								}
							}
						}
						else if (EndInBack || (!StartInBack && !EndInBack && (StartOffset.Size() - EndOffset.Size()) > 0.1))
						{
							if (!VerticalPlane)
							{
								LastLines.AddUnique(MakeShareable(new FArmyLine(VPStart, VPEnd)));
							}
							bool ExsistNextLine = false;
							for (int32 j = 0; j < Path.TrimPath.Num() - 1; ++j)
							{
								if (i == j) continue;
								FVector V21(Path.TrimPath[j], 0);
								FVector V22(Path.TrimPath[j + 1], 0);
								{
									if (V21 == V2 || V22 == V2)
									{
										ExsistNextLine = true;
										break;
									}
								}
							}
							if (!ExsistNextLine && !bBack)
							{
								LastPoints.Add(FacePointInfo(VPStart, Dir, FacePointInfo::FP_None));
							}
							else
							{
								if ((VOutDir + RDir).Size() < 0.5)
								{
									LastPoints.Add(FacePointInfo(VPStart, -Dir, bBack ? FacePointInfo::FP_Right : FacePointInfo::FP_Left));
								}
								else if ((VOutDir - RDir).Size() < 0.5)
								{
									LastPoints.Add(FacePointInfo(VPStart, -Dir, bBack ? FacePointInfo::FP_Left : FacePointInfo::FP_Right));
								}
							}

							if (EndInBack && !bBack)
							{
								FVector MidPoint = (V1 + V2) / 2;
								for (int32 j = 0; j < PolygonVArray.Num() - 1; j += 2)
								{
									FVector VS = PolygonVArray[j];
									FVector VE = PolygonVArray[j + 1];

									FVector P;
									if (FArmyMath::SegmentIntersection2D(VS, VE, MidPoint + VInnerDir, MidPoint + VInnerDir * 250, P))//相交
									{
										LastLines.AddUnique(MakeShareable(new FArmyLine(MidPoint, P)));
										LastPoints.Add(FacePointInfo(P, -Dir, FacePointInfo::FP_None));
										break;
									}
								}
							}
						}
						else if (!VerticalPlane)
						{
							LastLines.AddUnique(MakeShareable(new FArmyLine(VPStart, VPEnd)));
							if (bBack)
							{
								LastPoints.Add(FacePointInfo(VPStart, -Dir, FacePointInfo::FP_None));
								LastPoints.Add(FacePointInfo(VPEnd, -Dir, FacePointInfo::FP_None));
							}
							else
							{
								bool ExsistNextLine1 = false, ExsistNextLine2 = false;
								for (int32 j = 0; j < Path.TrimPath.Num() - 1; ++j)
								{
									if (i == j) continue;
									FVector V21(Path.TrimPath[j], 0);
									FVector V22(Path.TrimPath[j + 1], 0);

									if (V21 == V1 || V22 == V1)
									{
										ExsistNextLine1 = true;
									}
									else if (V21 == V2 || V22 == V2)
									{
										ExsistNextLine2 = true;
									}
									if (ExsistNextLine1 && ExsistNextLine2) break;
								}
								if (!ExsistNextLine1)
								{
									LastPoints.Add(FacePointInfo(VPStart, -Dir, FacePointInfo::FP_None));
								}
								if (!ExsistNextLine2)
								{
									LastPoints.Add(FacePointInfo(VPEnd, -Dir, FacePointInfo::FP_None));
								}
							}
						}
					}

					TArray<FSimpleElementVertex> TabelObjVertexArray;
					float InBaseH = Path.TrimActor->GetActorLocation().Z;
					float InTargetH = InSplineRes->mSize.Y;
					for (auto Line : LastLines)
					{
						FVector VPStart = FVector::PointPlaneProject(Line->GetStart(), MaxFacePlane);
						FVector VPEnd = FVector::PointPlaneProject(Line->GetEnd(), MaxFacePlane);
						VPStart.Z = InBaseH;
						VPEnd.Z = InBaseH;

						FVector RelativeStart = VPStart - FaceStartPos;
						FVector RelativeEnd = VPEnd - FaceStartPos;

						float x1 = FVector::DotProduct(RelativeStart, XDir.GetSafeNormal());
						float y1 = FVector::DotProduct(RelativeStart, YDir.GetSafeNormal());
						float z1 = FVector::DotProduct(RelativeStart, MaxFacePlane.GetSafeNormal());

						float x2 = FVector::DotProduct(RelativeEnd, XDir.GetSafeNormal());
						float y2 = FVector::DotProduct(RelativeEnd, YDir.GetSafeNormal());
						float z2 = FVector::DotProduct(RelativeEnd, MaxFacePlane.GetSafeNormal());

						FVector RelativeV1(x1, y1, z1);
						FVector RelativeV2(x2, y2, z2);

						TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV2), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));

						TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV1 + FVector(0, -InTargetH, 0)), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV2 + FVector(0, -InTargetH, 0)), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}

					for (auto& Point : LastPoints)
					{
						FVector ConnerPoint = FVector::PointPlaneProject(Point.ConnerPoint, MaxFacePlane);
						FVector VPConner = FVector::PointPlaneProject(ConnerPoint, MaxFacePlane);
						VPConner.Z = InBaseH;
						FVector RelativeConner = VPConner - FaceStartPos;

						float x1 = FVector::DotProduct(RelativeConner, XDir.GetSafeNormal());
						float y1 = FVector::DotProduct(RelativeConner, YDir.GetSafeNormal());
						float z1 = FVector::DotProduct(RelativeConner, MaxFacePlane.GetSafeNormal());

						FVector RelativeV1(x1, y1, z1);

						if (Point.ProfileType == FacePointInfo::FP_None || !InSplineRes)
						{
							TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
							TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV1 + FVector(0, -InTargetH, 0)), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						}
						else if (Point.ProfileType == FacePointInfo::FP_Left)
						{
							for (int32 i = 0; i < InSplineRes->mPoints.Num() - 1; ++i)
							{
								FVector2D VStart = InSplineRes->mPoints[i];
								FVector2D VEnd = InSplineRes->mPoints[i + 1];
								if ((VStart == FVector2D(0, 0) && VEnd == FVector2D(0, InSplineRes->mSize.Y)) ||
									(VEnd == FVector2D(0, 0) && VStart == FVector2D(0, InSplineRes->mSize.Y)))
								{
									continue;
								}
								TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(FVector(VStart * FVector2D(-1, -1), 0) + RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
								TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(FVector(VEnd * FVector2D(-1, -1), 0) + RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
							}
						}
						else if (Point.ProfileType == FacePointInfo::FP_Right)
						{
							for (int32 i = 0; i < InSplineRes->mPoints.Num() - 1; ++i)
							{
								FVector2D VStart = InSplineRes->mPoints[i];
								FVector2D VEnd = InSplineRes->mPoints[i + 1];
								if ((VStart == FVector2D(0, 0) && VEnd == FVector2D(0, InSplineRes->mSize.Y)) ||
									(VEnd == FVector2D(0, 0) && VStart == FVector2D(0, InSplineRes->mSize.Y)))
								{
									continue;
								}
								TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(FVector(VStart * FVector2D(1, -1), 0) + RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
								TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(FVector(VEnd * FVector2D(1, -1), 0) + RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
							}
						}
					}


					TSharedPtr<FArmyCustomDefine> TableFaceObj = MakeShareable(new FArmyCustomDefine);
					TableFaceObj->SetType(OT_TopLineFace);
					TableFaceObj->SetBelongClass(Class_SCTCabinet);
					TableFaceObj->ApplyTransform(FTransform(InOffset + VOffset));
					TableFaceObj->SetBaseVertexArray(TabelObjVertexArray);

					FArmySceneData::Get()->Add(TableFaceObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
				}
			};
			FArmySplineRes* pTopSplineRes = nullptr;
			FTableInfo* pTopInfo = gFArmyWHCabinMgr->GetTopInfo();
			if (pTopInfo)
			{
				pTopSplineRes = gSplineResMgr.GetSplineRes(pTopInfo->GetCachedPath());
			}
			else
			{
				pTopSplineRes = gSplineResMgr.GetSplineRes(GDefaultTopDxf);
			}
			TArray<FArmyWHCabinetMgr::FTrimInfo> TopPath;
			gFArmyWHCabinMgr->GetTrimPath(3, TopPath);

			TrimFaceFunction(TopPath, pTopSplineRes, false);

			FArmySplineRes* pFrontSplineRes = nullptr;
			FTableInfo* pFrontInfo = gFArmyWHCabinMgr->GetFrontInfo();
			if (pFrontInfo)
			{
				pFrontSplineRes = gSplineResMgr.GetSplineRes(pFrontInfo->GetCachedPath());
			}
			else
			{
				pFrontSplineRes = gSplineResMgr.GetSplineRes(GDefaultFrontDxf);
			}

			TArray<FArmyWHCabinetMgr::FTrimInfo> FrontPath;
			gFArmyWHCabinMgr->GetTrimPath(1, FrontPath);
			TrimFaceFunction(FrontPath, pFrontSplineRes, false);

			FArmySplineRes* pBackSplineRes = gSplineResMgr.GetSplineRes(GDefaultBackDxf);

			TArray<FArmyWHCabinetMgr::FTrimInfo> BackPath;
			gFArmyWHCabinMgr->GetTrimPath(2, BackPath);
			TrimFaceFunction(BackPath, pBackSplineRes, true);


			FArmySplineRes* pFootSplineRes = nullptr;
			FTableInfo* pFootInfo = gFArmyWHCabinMgr->GetToeInfo();
			if (pFootInfo)
			{
				pFootSplineRes = gSplineResMgr.GetSplineRes(pFootInfo->GetCachedPath());
			}
			else
			{
				pFootSplineRes = gSplineResMgr.GetSplineRes(GDefaultToeDxf);
			}

			TArray<FArmyWHCabinetMgr::FTrimInfo> FootPath;
			gFArmyWHCabinMgr->GetTrimPath(0, FootPath);
			TrimFaceFunction(FootPath, pFootSplineRes, false);
			//struct VertexIndexPair
			//{
			//	VertexIndexPair(uint16 InIndex1, uint16 InIndex2):IndexStart(InIndex1), IndexEnd(InIndex2){};
			//	uint16 IndexStart;
			//	uint16 IndexEnd;

			//	bool operator==(const VertexIndexPair& InOther) const
			//	{
			//		return (IndexStart == InOther.IndexStart && IndexEnd == InOther.IndexEnd)|| (IndexStart == InOther.IndexEnd && IndexEnd == InOther.IndexStart);
			//	}
			//};
			//FCollisionQueryParams TraceParams(FName(TEXT("Pick")), true);
			//TraceParams.bTraceComplex = false;

			//FPlane MaxFacePlane(MaxFacePoint + V * 20, V);
			//TArray<AXRShapeTableActor*> TrimActors;
			//TrimActors.Append(gFArmyWHCabinMgr->GetTrimActors(FArmyWHCabinetMgr::ETrim_PlatformFront));
			//TrimActors.Append(gFArmyWHCabinMgr->GetTrimActors(FArmyWHCabinetMgr::ETrim_PlatformBack));
			//TrimActors.Append(gFArmyWHCabinMgr->GetTrimActors(FArmyWHCabinetMgr::ETrim_Top));
			//TrimActors.Append(gFArmyWHCabinMgr->GetTrimActors(FArmyWHCabinetMgr::ETrim_Toe));
			//for (auto FA : TrimActors)
			//{
			//	if (FA->GetRootComponent()->GetClass() == UPNTUVPrimitive::StaticClass())
			//	{
			//		//TArray<FPUVVertex>& GetVertices() { return Vertices; }
			//		const TArray<FPUVVertex>& VertexArray = ((UPNTUVPrimitive*)(FA->GetRootComponent()))->GetVertices();
			//		const TArray<uint16>& IndexArray = ((UPNTUVPrimitive*)(FA->GetRootComponent()))->GetIndices();

			//		TArray<VertexIndexPair> LastIndexArray;
			//		TArray<VertexIndexPair> DiscardIndexArray;

			//		for (int32 i = 0;i <= IndexArray.Num() - 3 ; i += 3)
			//		{
			//			uint16 Index1 = IndexArray[i];
			//			uint16 Index2 = IndexArray[i + 1];
			//			uint16 Index3 = IndexArray[i + 2];

			//			VertexIndexPair VP1(Index1, Index2);
			//			VertexIndexPair VP2(Index1, Index3);
			//			VertexIndexPair VP3(Index2, Index3);
			//			
			//			if (!DiscardIndexArray.Contains(VP1))
			//			{
			//				LastIndexArray.AddUnique(VP1);
			//			}
			//			if (!DiscardIndexArray.Contains(VP2))
			//			{
			//				LastIndexArray.AddUnique(VP2);
			//			}
			//			if (!DiscardIndexArray.Contains(VP3))
			//			{
			//				LastIndexArray.AddUnique(VP3);
			//			}

			//			FVector Vertext1 = VertexArray[Index1].Position;
			//			FVector Vertext2 = VertexArray[Index2].Position;
			//			FVector Vertext3 = VertexArray[Index3].Position;

			//			FPlane PrePlane = FPlane(Vertext1, Vertext2, Vertext3);

			//			for (int32 j = i + 3; j <= IndexArray.Num() - 3; j += 3)
			//			{
			//				uint16 Index4 = IndexArray[j];
			//				uint16 Index5 = IndexArray[j + 1];
			//				uint16 Index6 = IndexArray[j + 2];

			//				FVector Vertext4 = VertexArray[Index4].Position;
			//				FVector Vertext5 = VertexArray[Index5].Position;
			//				FVector Vertext6 = VertexArray[Index6].Position;

			//				TArray<uint16> TempIndexArray{ Index1 ,Index2 ,Index3 };
			//				TArray<uint16> PostIndexArray{ Index1 ,Index2 ,Index3 };

			//				TempIndexArray.Remove(Index4);
			//				TempIndexArray.Remove(Index5);
			//				TempIndexArray.Remove(Index6);
			//				if (TempIndexArray.Num() == 1)//共线
			//				{
			//					uint16 AloneIndex = TempIndexArray[0];
			//					PostIndexArray.Remove(AloneIndex);

			//					bool SamePlane = FPlane(Vertext4, Vertext5, Vertext6) == PrePlane;//共面
			//					if (SamePlane)
			//					{
			//						LastIndexArray.Remove(VertexIndexPair(PostIndexArray[0], PostIndexArray[1]));
			//						DiscardIndexArray.AddUnique(VertexIndexPair(PostIndexArray[0], PostIndexArray[1]));
			//					}
			//				}
			//			}
			//		}
			//		//

			//		FVector Pos = FA->GetActorLocation();
			//		FVector NP = FMath::ClosestPointOnInfiniteLine(TempStartPos, TempEndPos, Pos);
			//		float VValue = Pos.Z - NP.Z;
			//		FVector TrimOffset = NP - FaceStartPos;
			//		int32 Flag = (TrimOffset.GetSafeNormal() - RDir).Size() < 0.01 ? 1 : -1;
			//		float  LocalOffset = (NP - FaceStartPos).Size() * Flag;

			//		FVector XDir(MaxFacePlane.Y, -MaxFacePlane.X, 0);
			//		FVector YDir(0, 0, -1);

			//		TArray<FSimpleElementVertex> TabelObjVertexArray;
			//		FTransform CurrentTrans = FA->GetActorTransform();
			//		FVector OriginPos = FVector::PointPlaneProject(Pos, MaxFacePlane);
			//		for (auto& VIndex : LastIndexArray)
			//		{
			//			FHitResult OutHit(ForceInit);
			//			const FVector BaseV1 = CurrentTrans.TransformPosition(VertexArray[VIndex.IndexStart].Position);
			//			const FVector BaseV2 = CurrentTrans.TransformPosition(VertexArray[VIndex.IndexEnd].Position);
			//			
			//			FVector TransV1 = BaseV1 * FVector(1, 1, 0.98) + FVector(0, 0, 0.01) + V * 0.01;
			//			FVector TransV2 = BaseV2 * FVector(1, 1, 0.98) + FVector(0, 0, 0.01) + V * 0.01;

			//			FVector VP1 = FVector::PointPlaneProject(TransV1, MaxFacePlane);
			//			FVector VP2 = FVector::PointPlaneProject(TransV2, MaxFacePlane);
			//			if (!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV1, VP1, ECC_Visibility, TraceParams) &&
			//				!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV2, VP2, ECC_Visibility, TraceParams) &&
			//				!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP1, TransV1, ECC_Visibility, TraceParams) &&
			//				!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP2, TransV2, ECC_Visibility, TraceParams))
			//			{
			//				FVector BaseVP1 = FVector::PointPlaneProject(BaseV1, MaxFacePlane);
			//				FVector BaseVP2 = FVector::PointPlaneProject(BaseV2, MaxFacePlane);

			//				float x1 = FVector::DotProduct((BaseVP1 - OriginPos), XDir.GetSafeNormal());
			//				float y1 = FVector::DotProduct((BaseVP1 - OriginPos), YDir.GetSafeNormal());
			//				float z1 = FVector::DotProduct((BaseVP1 - OriginPos), MaxFacePlane.GetSafeNormal());

			//				float x2 = FVector::DotProduct((BaseVP2 - OriginPos), XDir.GetSafeNormal());
			//				float y2 = FVector::DotProduct((BaseVP2 - OriginPos), YDir.GetSafeNormal());
			//				float z2 = FVector::DotProduct((BaseVP2 - OriginPos), MaxFacePlane.GetSafeNormal());

			//				FVector RelativeV1(x1, y1, z1);
			//				FVector RelativeV2(x2, y2, z2);
			//				TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
			//				TabelObjVertexArray.Add(FSimpleElementVertex(FVector4(RelativeV2), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
			//			}
			//			//else
			//			//{
			//			//	int32 a = 1;
			//			//}
			//		}
			//		TSharedPtr<FArmyCustomDefine> TableFaceObj = MakeShareable(new FArmyCustomDefine);
			//		TableFaceObj->SetBelongClass(Class_SCTCabinet);
			//		TableFaceObj->ApplyTransform(FTransform(InOffset + FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + VOffset));
			//		TableFaceObj->SetBaseVertexArray(TabelObjVertexArray);

			//		FArmySceneData::Get()->Add(TableFaceObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

			//	}
			//}
			///////////////////////////////////////台面end
		}
	}
}
bool FArmyConstructionFunction::GenerateBoardFaceOutline(AActor* InShapeActor, const TSharedPtr<FSCTShape> InShape, const FVector& InBasePos, const FPlane& InPlane, const FName& InLayerName, const FVector InStartPos, const FVector InEndPos, const FVector InOffset, TArray<FBox2D>& OutSheetBoxArray, TArray<FSimpleElementVertex>& OutVArray)
{
	FCollisionQueryParams TraceParams(FName(TEXT("Pick")), true);
	TraceParams.bTraceComplex = false;

	FVector XDir(InPlane.Y, -InPlane.X, 0);
	FVector YDir(0, 0, -1);

	//InShape->GetShapeType();
	TArray<AActor*> AttachedActors;
	InShapeActor->GetAttachedActors(AttachedActors);
	for (auto AShape : AttachedActors)
	{
		FTransform CurrentTrans = AShape->GetActorTransform();

		//ASCTShapeActor* CShape = Cast<ASCTShapeActor>(AShape);
		//if (CShape)
		//{
		//	if (CShape->GetShape()->GetShapeType() == ST_SideHungDoor)
		//	{
		//		FSideHungDoor* DoorShape = ((FSideHungDoor*)CShape->GetShape());
		//		VGap = DoorShape->GetUpDownGapValue() / 10;
		//		HGap = DoorShape->GetLeftRightGapValue() / 10;
		//	}
		//	else if (CShape->GetShape()->GetShapeType() == ST_DrawerDoor)
		//	{
		//		FDrawerDoorShape* DoorShape = ((FDrawerDoorShape*)CShape->GetShape());
		//		VGap = DoorShape->GetUpDownGapValue() / 10;
		//		HGap = DoorShape->GetLeftRightGapValue() / 10;
		//	}
		//}
		ASCTShapeActor* CShape = Cast<ASCTShapeActor>(AShape);
		// @zengy 增加判断Actor是否关联Shape的判断
		if (CShape && CShape->GetShape() != nullptr)
		{
			if (CShape->GetShape()->GetShapeType() == ST_SideHungDoor ||
				CShape->GetShape()->GetShapeType() == ST_SlidingDoor ||
				CShape->GetShape()->GetShapeType() == ST_DrawerDoor)
			{
				//return false;
				FString SheetComponentFile = TEXT("");
				TSharedPtr<FCabinetWholeComponent> Result = FWHCModeGlobalData::CabinetComponentMap.FindRef(InShape->GetShapeId());
				if (Result.IsValid())
				{
					for (auto SheetComponent : Result->DoorSheets)
					{
						if (SheetComponent->Id == CShape->GetShape()->GetShapeId())
						{
							SheetComponentFile = SheetComponent->GetCachePath(2);
							break;
						}
					}
				}
				//if (SheetComponentFile.IsEmpty()) SheetComponentFile = TEXT("../../../XR/Content/Assets/DefaultSheet.dxf"); 不用本地默认门板图例

				TArray<TSharedPtr<FArmyFurniture>> ObjectLayers = FArmyToolsModule::ParseDXF(SheetComponentFile);
				if (ObjectLayers.Num() > 0)
				{
					TMap<TSharedPtr<FVariableAreaDoorSheet>, FVector2D> SheetGapMap;

					TArray<TSharedPtr<FVariableAreaDoorSheet>> DoorSheetArray;
					if (CShape->GetShape()->GetShapeType() == ST_SideHungDoor)
					{
						FSideHungDoor* SideHungDoor = StaticCast<FSideHungDoor*>(CShape->GetShape());
						DoorSheetArray.Append(SideHungDoor->GetDoorSheets());
						float VGap = SideHungDoor->GetUpDownGapValue();
						float HGap = SideHungDoor->GetLeftRightGapValue();
						for (auto DoorSheet : SideHungDoor->GetDoorSheets())
						{
							SheetGapMap.Add(DoorSheet, FVector2D(HGap, VGap));
						}
					}
					else if (CShape->GetShape()->GetShapeType() == ST_DrawerDoor)
					{
						FDrawerDoorShape* DrawerDoorShape = StaticCast<FDrawerDoorShape*>(CShape->GetShape());
						DoorSheetArray.Add(DrawerDoorShape->GetDrawDoorSheet());
						float VGap = DrawerDoorShape->GetUpDownGapValue();
						float HGap = DrawerDoorShape->GetLeftRightGapValue();
						SheetGapMap.Add(DrawerDoorShape->GetDrawDoorSheet(), FVector2D(HGap, VGap));
					}

					TSharedPtr<FArmyFurniture>CurrentObj = ObjectLayers[0];
					FVector ObjBaseSize = CurrentObj->GetBounds().GetSize();

					for (auto Sheet : DoorSheetArray)
					{
						FVector ShapePos = Sheet->GetShapeActor()->GetActorLocation();
						FVector NP = FMath::ClosestPointOnInfiniteLine(InStartPos, InEndPos, ShapePos);
						NP = FVector::PointPlaneProject(NP, InPlane);

						float VValue = ShapePos.Z - NP.Z;
						float  LocalOffset = (NP - InBasePos).Size();

						TArray<FSCTShapeUtilityTool::FVariableAreaMeta> DividBlocks;
						FSCTShapeUtilityTool::GetDoorSheetDividBlocks(Sheet.Get(), ObjBaseSize.X * 10, ObjBaseSize.Y * 10, DividBlocks);

						TArray<TSharedPtr<FArmyWHCDoorSheet::FDxfSpliteBlock>> BlockArray;
						for (auto& Block : DividBlocks)
						{
							TSharedPtr<FArmyWHCDoorSheet::FDxfSpliteBlock> FaceBlock = MakeShareable(new FArmyWHCDoorSheet::FDxfSpliteBlock());
							FaceBlock->BlockBoxBase = FBox2D(Block.DividBlock.Min / 10, Block.DividBlock.Max / 10);
							FaceBlock->BlockBoxPost = FaceBlock->BlockBoxBase;
							switch (Block.ScaleType)
							{
							case FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType::E_None:
								FaceBlock->BlockModel = FArmyWHCDoorSheet::FDxfSpliteBlock::BM_STATIC;
								break;
							case FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType::E_X_Scale:
								FaceBlock->BlockModel = FArmyWHCDoorSheet::FDxfSpliteBlock::BM_XSCALE;
								break;
							case FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType::E_Z_Scale:
								FaceBlock->BlockModel = FArmyWHCDoorSheet::FDxfSpliteBlock::BM_YSCALE;
								break;
							case FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType::E_XZ_Scale:
								FaceBlock->BlockModel = FArmyWHCDoorSheet::FDxfSpliteBlock::BM_XYSCALE;
								break;
							default:
								FaceBlock->BlockModel = FArmyWHCDoorSheet::FDxfSpliteBlock::BM_STATIC;
								break;
							}
							BlockArray.Add(FaceBlock);
						}

						TSharedPtr<FArmyWHCDoorSheet> DoorSheetObj = MakeShareable(new FArmyWHCDoorSheet);
						FVector2D XYGap = SheetGapMap.FindRef(Sheet);
						float LastSheetWidth = (Sheet->GetShapeWidth() + XYGap.X * 2) / 10;
						float LastSheetHeight = (Sheet->GetShapeHeight() + XYGap.Y * 2) / 10;
						DoorSheetObj->SetCurrentSize(LastSheetWidth, LastSheetHeight);
						DoorSheetObj->SetDataAddBlocks(CurrentObj, BlockArray);
						DoorSheetObj->ApplyTransform(FTransform(FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * (VValue + LastSheetHeight) + InOffset + FVector(-XYGap.X / 10, XYGap.Y / 10, 0)));
						FArmySceneData::Get()->Add(DoorSheetObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));

						TSharedPtr<FArmyCustomDefine> DoorDirectionObj = MakeShareable(new FArmyCustomDefine);
						DoorDirectionObj->SetBelongClass(Class_SCTCabinet);
						DoorDirectionObj->ApplyTransform(FTransform(FVector(1, 0, 0) * LocalOffset + FVector(0, -1, 0) * VValue + InOffset + FVector(-XYGap.X / 10, XYGap.Y / 10, 0)));

						switch (FVariableAreaDoorSheet::EDoorOpenDirection(Sheet->GetOpenDoorDirectionValue()))
						{
						case FVariableAreaDoorSheet::EDoorOpenDirection::E_Left:
						{
							TArray<FSimpleElementVertex> VArray{
								FSimpleElementVertex(FVector4(LastSheetWidth,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(0,-LastSheetHeight / 2,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(0,-LastSheetHeight / 2,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth,-LastSheetHeight,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
							};
							DoorDirectionObj->SetBaseVertexArray(VArray);
							FArmySceneData::Get()->Add(DoorDirectionObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
						}
						break;
						case FVariableAreaDoorSheet::EDoorOpenDirection::E_Right:
						{
							TArray<FSimpleElementVertex> VArray{
								FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth,-LastSheetHeight / 2,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth,-LastSheetHeight / 2,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(0,-LastSheetHeight,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
							};
							DoorDirectionObj->SetBaseVertexArray(VArray);
							FArmySceneData::Get()->Add(DoorDirectionObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
						}
						break;
						case FVariableAreaDoorSheet::EDoorOpenDirection::E_Top:
						{
							TArray<FSimpleElementVertex> VArray{
								FSimpleElementVertex(FVector4(0,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth / 2,-LastSheetHeight,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth / 2,-LastSheetHeight,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
								FSimpleElementVertex(FVector4(LastSheetWidth,0,0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()),
							};
							DoorDirectionObj->SetBaseVertexArray(VArray);
							FArmySceneData::Get()->Add(DoorDirectionObj, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
						}
						break;
						default:
							break;
						}

						////////////////////////////////remove cover point/////////////////////////////////////////
						FVector SP = FVector::PointPlaneProject(ShapePos, InPlane);
						float MinX = FVector::DotProduct((SP - InBasePos), XDir.GetSafeNormal()) - XYGap.X / 10;
						float MinY = FVector::DotProduct((SP - InBasePos), YDir.GetSafeNormal()) - XYGap.Y / 10;

						FBox2D SheetBox(FVector2D(MinX, MinY - LastSheetHeight), FVector2D(MinX + LastSheetWidth, MinY));
						OutSheetBoxArray.Add(SheetBox);

						if (OutVArray.Num() > 1)
						{
							for (int32 i = OutVArray.Num() - 1; i > 0; i -= 2)
							{
								auto& V1 = OutVArray[i];
								auto& V2 = OutVArray[i - 1];

								if (SheetBox.IsInside(FVector2D(V1.Position.X, V1.Position.Y)) ||
									SheetBox.IsInside(FVector2D(V2.Position.X, V2.Position.Y)))
								{
									OutVArray.RemoveAt(i, 1);
									OutVArray.RemoveAt(i - 1, 1);
								}
							}
						}
					}
				}
			}
		}
		if (AShape->GetClass() == ASCTBoardActor::StaticClass())
		{
			TArray<TArray<FVector>> EdgeVertexArray;
			TArray<FVector> OutLineVertexArray, LastOutLineVertexArray;
			FBoardShape* BoradShape = (FBoardShape*)((ASCTBoardActor*)AShape)->GetShape();
			BoradShape->CalcBoardPoint(EdgeVertexArray, OutLineVertexArray);
			float W = BoradShape->GetShapeWidth() / 10;
			float D = BoradShape->GetShapeDepth() / 10;
			float H = BoradShape->GetShapeHeight() / 10;

			float HGap = 0.01, VGap = 0.01, DGap = 0.01;

			float XScale = (W - HGap * 2) / W;
			float YScale = (D + DGap * 2) / D;
			float ZScale = (H - VGap * 2) / H;

			if (OutLineVertexArray.Num() > 0)
			{
				FVector FirstV = OutLineVertexArray[0];
				OutLineVertexArray.Add(FirstV);
			}
			for (int32 i = 0; i < OutLineVertexArray.Num() - 1; ++i)
			{
				const FVector& V1 = OutLineVertexArray[i] / 10;
				const FVector& V2 = OutLineVertexArray[i + 1] / 10;
				const FVector& V1H = V1 + FVector(0, 0, H);
				const FVector& V2H = V2 + FVector(0, 0, H);

				const FVector& _V1 = V1 * FVector(XScale, YScale, ZScale) + FVector(HGap, -DGap, VGap);
				const FVector& _V2 = V2 * FVector(XScale, YScale, ZScale) + FVector(HGap, -DGap, VGap);
				const FVector& _V1H = V1H * FVector(XScale, YScale, ZScale) + FVector(HGap, -DGap, VGap);
				const FVector& _V2H = V2H * FVector(XScale, YScale, ZScale) + FVector(HGap, -DGap, VGap);

				const FVector TransV1 = CurrentTrans.TransformPosition(_V1);
				const FVector TransV2 = CurrentTrans.TransformPosition(_V2);

				const FVector TransV1H = CurrentTrans.TransformPosition(_V1H);
				const FVector TransV2H = CurrentTrans.TransformPosition(_V2H);

				const FVector TransVV1 = CurrentTrans.TransformPosition(_V1);
				const FVector TransVV1H = CurrentTrans.TransformPosition(_V1H);

				const FVector TransVV2 = CurrentTrans.TransformPosition(_V2);
				const FVector TransVV2H = CurrentTrans.TransformPosition(_V2H);

				FHitResult OutHit(ForceInit);
				FVector VP1 = FVector::PointPlaneProject(TransV1, InPlane);
				FVector VP2 = FVector::PointPlaneProject(TransV2, InPlane);
				if (!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV1, VP1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV2, VP2, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP1, TransV1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP2, TransV2, ECC_Visibility, TraceParams))
				{
					//add
					FVector V1Project = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V1), InPlane);
					FVector V2Project = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V2), InPlane);

					float x1 = FVector::DotProduct((V1Project - InBasePos), XDir.GetSafeNormal());
					float y1 = FVector::DotProduct((V1Project - InBasePos), YDir.GetSafeNormal());
					float z1 = FVector::DotProduct((V1Project - InBasePos), InPlane.GetSafeNormal());

					float x2 = FVector::DotProduct((V2Project - InBasePos), XDir.GetSafeNormal());
					float y2 = FVector::DotProduct((V2Project - InBasePos), YDir.GetSafeNormal());
					float z2 = FVector::DotProduct((V2Project - InBasePos), InPlane.GetSafeNormal());

					FVector RelativeV1(x1, y1, z1);
					FVector RelativeV2(x2, y2, z2);

					bool Ignore = false;
					for (auto& Box : OutSheetBoxArray)
					{
						if (Box.IsInside(FVector2D(RelativeV1)) ||
							Box.IsInside(FVector2D(RelativeV2))/*||
															   Box.IsInside((FVector2D(RelativeV1) + FVector2D(RelativeV2)) / 2)*/)
						{
							Ignore = true;
							break;
						}
					}
					if (!Ignore)
					{
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV2), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
				}

				VP1 = FVector::PointPlaneProject(TransV1H, InPlane);
				VP2 = FVector::PointPlaneProject(TransV2H, InPlane);
				if (!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV1H, VP1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransV2H, VP2, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP1, TransV1H, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP2, TransV2H, ECC_Visibility, TraceParams))
				{
					//add
					FVector V1HProject = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V1H), InPlane);
					FVector V2HProject = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V2H), InPlane);

					float x1 = FVector::DotProduct((V1HProject - InBasePos), XDir.GetSafeNormal());
					float y1 = FVector::DotProduct((V1HProject - InBasePos), YDir.GetSafeNormal());
					float z1 = FVector::DotProduct((V1HProject - InBasePos), InPlane.GetSafeNormal());

					float x2 = FVector::DotProduct((V2HProject - InBasePos), XDir.GetSafeNormal());
					float y2 = FVector::DotProduct((V2HProject - InBasePos), YDir.GetSafeNormal());
					float z2 = FVector::DotProduct((V2HProject - InBasePos), InPlane.GetSafeNormal());

					FVector RelativeV1H(x1, y1, z1);
					FVector RelativeV2H(x2, y2, z2);

					bool Ignore = false;
					for (auto& Box : OutSheetBoxArray)
					{
						if (Box.IsInside(FVector2D(RelativeV1H)) ||
							Box.IsInside(FVector2D(RelativeV2H))/*||
																Box.IsInside((FVector2D(RelativeV1H) + FVector2D(RelativeV2H)) / 2)*/)
						{
							Ignore = true;
							break;
						}
					}
					if (!Ignore)
					{
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV1H), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV2H), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
				}

				VP1 = FVector::PointPlaneProject(TransVV1, InPlane);
				VP2 = FVector::PointPlaneProject(TransVV1H, InPlane);
				if (!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransVV1, VP1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransVV1H, VP2, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP1, TransVV1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP2, TransVV1H, ECC_Visibility, TraceParams))
				{
					//add
					FVector V1Project = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V1), InPlane);
					FVector V1HProject = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V1H), InPlane);

					float x1 = FVector::DotProduct((V1Project - InBasePos), XDir.GetSafeNormal());
					float y1 = FVector::DotProduct((V1Project - InBasePos), YDir.GetSafeNormal());
					float z1 = FVector::DotProduct((V1Project - InBasePos), InPlane.GetSafeNormal());

					float x2 = FVector::DotProduct((V1HProject - InBasePos), XDir.GetSafeNormal());
					float y2 = FVector::DotProduct((V1HProject - InBasePos), YDir.GetSafeNormal());
					float z2 = FVector::DotProduct((V1HProject - InBasePos), InPlane.GetSafeNormal());

					FVector RelativeV1(x1, y1, z1);
					FVector RelativeV1H(x2, y2, z2);

					bool Ignore = false;
					for (auto& Box : OutSheetBoxArray)
					{
						if (Box.IsInside(FVector2D(RelativeV1)) ||
							Box.IsInside(FVector2D(RelativeV1H)) /*||
																 Box.IsInside((FVector2D(RelativeV1) + FVector2D(RelativeV1H))/2)*/)
						{
							Ignore = true;
							break;
						}
					}
					if (!Ignore)
					{
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV1), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV1H), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
				}

				VP1 = FVector::PointPlaneProject(TransVV2, InPlane);
				VP2 = FVector::PointPlaneProject(TransVV2H, InPlane);
				if (!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransVV2, VP1, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, TransVV2H, VP2, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP1, TransVV2, ECC_Visibility, TraceParams) &&
					!GVC->GetWorld()->LineTraceSingleByChannel(OutHit, VP2, TransVV2H, ECC_Visibility, TraceParams))
				{
					//add
					FVector V2Project = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V2), InPlane);
					FVector V2HProject = FVector::PointPlaneProject(CurrentTrans.TransformPosition(V2H), InPlane);

					float x1 = FVector::DotProduct((V2Project - InBasePos), XDir.GetSafeNormal());
					float y1 = FVector::DotProduct((V2Project - InBasePos), YDir.GetSafeNormal());
					float z1 = FVector::DotProduct((V2Project - InBasePos), InPlane.GetSafeNormal());

					float x2 = FVector::DotProduct((V2HProject - InBasePos), XDir.GetSafeNormal());
					float y2 = FVector::DotProduct((V2HProject - InBasePos), YDir.GetSafeNormal());
					float z2 = FVector::DotProduct((V2HProject - InBasePos), InPlane.GetSafeNormal());

					FVector RelativeV2(x1, y1, z1);
					FVector RelativeV2H(x2, y2, z2);

					bool Ignore = false;
					for (auto& Box : OutSheetBoxArray)
					{
						if (Box.IsInside(FVector2D(RelativeV2)) ||
							Box.IsInside(FVector2D(RelativeV2H))/* ||
																Box.IsInside((FVector2D(RelativeV2) + FVector2D(RelativeV2H)) / 2)*/)
						{
							Ignore = true;
							break;
						}
					}
					if (!Ignore)
					{
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV2), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
						OutVArray.Add(FSimpleElementVertex(FVector4(RelativeV2H), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
					}
				}
			}
		}
		else
		{
			if (!GenerateBoardFaceOutline(AShape, InShape, InBasePos, InPlane, InLayerName, InStartPos, InEndPos, InOffset, OutSheetBoxArray, OutVArray))
			{
				return false;
			}
		}
	}
	return true;
}
void FArmyConstructionFunction::GenerateAutoLayers()
{
	if (ClassTypeList.Contains(Class_SCTCabinet) || ClassTypeList.Contains(Class_Other))
	{
		TArray<FObjectWeakPtr> CupboardTableArray;
		FArmySceneData::Get()->GetObjects(E_WHCModel, OT_CupboardTable, CupboardTableArray);
		for (auto TablePtr : CupboardTableArray)
		{
			//FName LayerName(*(FString(TEXT("*橱柜台面-")) + TablePtr.Pin()->GetName()));
			FName LayerName(*(FString(TEXT("橱柜台面"))));//暂时没有分空间，所以名称暂时不加空间名称了
			DelegateNewLayer.Execute(LayerName, false, FArmyLayer::LT_D_CUPBOARDTABLE);

			FObjectPtr NewTable = TablePtr.Pin()->CopySelf();
			FVector Offset = NewTable->GetBounds().GetExtent();
			Offset = BoundBox.GetCenter() + FVector(-Offset.X, Offset.Y, Offset.Z);
			NewTable->ApplyTransform(FTransform(Offset));
			FArmyWHCTableObject* TabelObj = NewTable->AsassignObj<FArmyWHCTableObject>();
			if (TabelObj)
			{
				TabelObj->SetDrawComponent(true);
			}
			FArmySceneData::Get()->Add(NewTable, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER/LAYERTYPE_CUPBOARDTABLE")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
		}
		/////////////////////////////////////////////////////////////
		FName LayerName(*(FString(TEXT("橱柜安装平面图"))));//暂时没有分空间，所以名称暂时不加空间名称了
		DelegateNewLayer.Execute(LayerName, false, FArmyLayer::LT_NONE);
		GenerateCupboardPlanar(LayerName);
	}
}

// 花恩最后修改，常远暂时注释
//void FArmyConstructionFunction::GenerateWallFacadeLayer(const FName& InLayerName, const TSharedPtr<FArmyRoom> InRoom, const FPlane& InFacePlane)
//{
//	TArray<FObjectWeakPtr> WallFaceObjectArray;
//
//	TArray<FObjectWeakPtr> RoomSpaceObjArray;
//
//	TArray<FString> CurrentRoomObjIDArray{ InRoom->GetUniqueID().ToString() };
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, RoomSpaceObjArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, RoomSpaceObjArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_IndependentWall, RoomSpaceObjArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, RoomSpaceObjArray);
//
//	for (auto& Obj : RoomSpaceObjArray)
//	{
//		TArray<TSharedPtr<FArmyLine>> LineArray;
//		Obj.Pin()->GetLines(LineArray);
//		for (auto L : LineArray)
//		{
//			if (InRoom->IsPointInRoom((L->GetStart() + L->GetEnd()) / 2))
//			{
//				CurrentRoomObjIDArray.Add(Obj.Pin()->GetUniqueID().ToString());
//				break;
//			}
//		}
//	}
//
//	TArray<FObjectWeakPtr> RoomSpaceComponentArray;
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_HardModel, OT_ComponentBase, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_ComponentBase, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_DoorHole, RoomSpaceComponentArray);
//
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CornerBayWindow, RoomSpaceComponentArray);
//	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Punch, RoomSpaceComponentArray);
//
//	//FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_ComponentBase, RoomSpaceComponentArray);
//
//	//该代码是做深度遮挡判断的，经测试没有成功，需要再调试验证
//	TArray<FObjectWeakPtr> RoomSpaceAreaArray;
//	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, RoomSpaceAreaArray);
//	struct AreaLinkGroup
//	{
//		int32 Depth = -1;
//		float AreaSize = 0;
//		TArray<FVector> AllVertexArray;
//		TArray<FObjectWeakPtr> SpaceAreaArray;
//
//		void operator+=(const AreaLinkGroup& InOther)
//		{
//			Depth < InOther.Depth ? Depth = InOther.Depth : true;
//			AreaSize += InOther.AreaSize;
//			AllVertexArray.Append(InOther.AllVertexArray);
//			SpaceAreaArray.Append(InOther.SpaceAreaArray);
//		}
//	};
//	TArray<AreaLinkGroup> AreaLinkGroupArray;
//	for (auto WallArea : RoomSpaceAreaArray)
//	{
//		bool NoLink = true;
//		TArray<FVector> OutVertexes;
//		WallArea.Pin()->GetVertexes(OutVertexes);
//		FBox Box(OutVertexes);
//		int32 LinkIndex = -1;
//		for (auto& V : OutVertexes)
//		{
//			for (int32 i = 0; i < AreaLinkGroupArray.Num(); ++i)
//			{
//				auto& LinkGroup = AreaLinkGroupArray[i];
//				if (LinkGroup.AllVertexArray.Contains(V))
//				{
//					NoLink = false;
//					if (LinkIndex != i && LinkIndex >= 0)
//					{
//						auto& PreLinkGroup = AreaLinkGroupArray[LinkIndex];
//						PreLinkGroup += LinkGroup;//每个面最多只有两面连接
//						AreaLinkGroupArray.RemoveAt(i);
//						break;
//					}
//					else
//					{
//						LinkGroup.SpaceAreaArray.AddUnique(WallArea);
//						LinkGroup.AllVertexArray.Append(OutVertexes);
//						LinkGroup.AreaSize += Box.GetSize().Size();
//						LinkIndex = i;
//					}
//				}
//			}
//			if (!NoLink)
//			{
//				break;
//			}
//		}
//		if (NoLink)
//		{
//			int32 Index = AreaLinkGroupArray.Add(AreaLinkGroup());
//			AreaLinkGroupArray[Index].SpaceAreaArray.AddUnique(WallArea);
//			AreaLinkGroupArray[Index].AllVertexArray.Append(OutVertexes);
//			AreaLinkGroupArray[Index].AreaSize += Box.GetSize().Size();
//		}
//	}
//
//	AreaLinkGroupArray.Sort([](const AreaLinkGroup& A, const AreaLinkGroup& B) {
//		return A.AreaSize < B.AreaSize;
//	});
//	if (AreaLinkGroupArray.Num() > 0)
//	{
//		WallFaceObjectArray.Append(AreaLinkGroupArray.Last().SpaceAreaArray);
//	}
//
//	GenerateWallFacadeData(InLayerName, WallFaceObjectArray, InFacePlane);
//}

TArray<FObjectWeakPtr> SelectRoomSpaceAreas(const TArray<FObjectWeakPtr>& InWallObjectArray, const FPlane &InFacePlane)
{
	struct WallProjectionInfo
	{
		FVector Start;
		FVector End;
		TArray<int32> WallIndices;
		float Len;
		WallProjectionInfo(const FVector &Start_, const FVector &End_, const TArray<int32> WallIndices_, float Len_)
		: Start(Start_), End(End_), WallIndices(WallIndices_), Len(Len_) {}
	};
	auto IsOverlapped = [](const WallProjectionInfo &SrcInfo, const WallProjectionInfo &DestInfo, const FPlane &FacePlane)
	{
		FVector SrcInfoProjStart = FVector::PointPlaneProject(SrcInfo.Start, FacePlane);
		FVector SrcInfoProjEnd = FVector::PointPlaneProject(SrcInfo.End, FacePlane);
		FVector DestInfoProjStart = FVector::PointPlaneProject(DestInfo.Start, FacePlane);
		FVector DestInfoProjEnd = FVector::PointPlaneProject(DestInfo.End, FacePlane);

		FVector InfoProjDir = SrcInfoProjEnd - SrcInfoProjStart;
		FVector InfoProjNormDir = InfoProjDir.GetSafeNormal();
		float InfoProjLen = InfoProjDir.Size();

		if (FVector::DotProduct(DestInfoProjEnd - SrcInfoProjStart, InfoProjNormDir) < 0.01f)
			return false;
		else if (FVector::DotProduct(DestInfoProjStart - SrcInfoProjEnd, InfoProjNormDir) > -0.01f)
			return false;
		else
			return true;
	};
	FVector PlaneDir = FVector::CrossProduct(FVector::UpVector, InFacePlane.GetSafeNormal());
	TArray<WallProjectionInfo> WallProjectionArr;
	for (int32 i = 0; i < InWallObjectArray.Num(); ++i)
	{
		TSharedPtr<FArmyRoomSpaceArea> Area = StaticCastSharedPtr<FArmyRoomSpaceArea>(InWallObjectArray[i].Pin());
		FVector Center = Area->GetPlaneCenter();
		FVector XDir = Area->GetXDir();
		FVector BoundExt = Area->GetBounds().GetExtent();
		FVector LeftCenter = Center - PlaneDir * BoundExt.X;
		FVector RightCenter = Center + PlaneDir * BoundExt.X;
		if (WallProjectionArr.Num() == 0)
		{
			WallProjectionArr.Emplace(
				LeftCenter,
				RightCenter,
				TArray<int32>{i},
				(RightCenter - LeftCenter).Size()
			);
		}
		else
		{
			bool bFoundLinked = false;

			// 查找相连接的Area
			// 构建临时的当前Area的ProjectionInfo
			WallProjectionInfo TempInfo(LeftCenter, RightCenter, TArray<int32>(), 0.0f);
			// 遍历所有已存在的ProjectInfo，查找当前的Area是否和某个Project连接
			for (auto &WallProjection : WallProjectionArr)
			{
				// 首先要判定当前Area是否和已存在的Projection相交
				if (!IsOverlapped(WallProjection, TempInfo, InFacePlane))
				{
					// 不相交则检查当前Area和Projection是否首尾相连
					if (FMath::IsNearlyZero(
						(FVector::PointPlaneProject(WallProjection.Start, InFacePlane) - FVector::PointPlaneProject(RightCenter, InFacePlane)).Size(), 
						0.01f))
					{
						WallProjection.Start = LeftCenter;
						WallProjection.WallIndices.Emplace(i);
						WallProjection.Len += (RightCenter - LeftCenter).Size();
						bFoundLinked = true;
						break;
					}
					else if (FMath::IsNearlyZero(
						(FVector::PointPlaneProject(WallProjection.End, InFacePlane) - FVector::PointPlaneProject(LeftCenter, InFacePlane)).Size(), 
						0.01f))
					{
						WallProjection.End = RightCenter;
						WallProjection.WallIndices.Emplace(i);
						WallProjection.Len += (RightCenter - LeftCenter).Size();
						bFoundLinked = true;
						break;
					}
				}
			}

			// // 户型线是按顺序给出的
			// if (FMath::IsNearlyZero(
			// 	(FVector::PointPlaneProject(WallProjectionArr[0].Start, InFacePlane) - FVector::PointPlaneProject(RightCenter, InFacePlane)).Size(), 
			// 	0.01f))
			// {
			// 	WallProjectionArr[0].Start = LeftCenter;
			// 	WallProjectionArr[0].WallIndices.Emplace(i);
			// 	WallProjectionArr[0].Len += (RightCenter - LeftCenter).Size();
			// 	bFoundLinked = true;
			// }
			// else if (FMath::IsNearlyZero(
			// 	(FVector::PointPlaneProject(WallProjectionArr[WallProjectionArr.Num() - 1].End, InFacePlane) - FVector::PointPlaneProject(LeftCenter, InFacePlane)).Size(), 
			// 	0.01f))
			// {
			// 	WallProjectionArr[WallProjectionArr.Num() - 1].End = RightCenter;
			// 	WallProjectionArr[WallProjectionArr.Num() - 1].WallIndices.Emplace(i);
			// 	WallProjectionArr[WallProjectionArr.Num() - 1].Len += (RightCenter - LeftCenter).Size();
			// 	bFoundLinked = true;
			// }

			if (!bFoundLinked)
			{
				WallProjectionArr.Emplace(
					LeftCenter,
					RightCenter,
					TArray<int32>{i},
					(RightCenter - LeftCenter).Size()
				);
			}
		}
	}

	int32 IterCount = WallProjectionArr.Num();
	// Flags数组中的0表示索引信息可用，1表示不可用
	TArray<int32> Flags; Flags.SetNumZeroed(IterCount);
	for (int32 i = 0; i < IterCount; ++i)
	{
		if (Flags[i] == 1)
			continue;
		const auto &SrcInfo = WallProjectionArr[i];
		for (int32 j = i + 1; j < IterCount; ++j)
		{
			if (Flags[j] == 0)
			{
				if (IsOverlapped(SrcInfo, WallProjectionArr[j], InFacePlane)) // 两组墙投影信息有相交情况
				{
					if (SrcInfo.Len > WallProjectionArr[j].Len)
						Flags[j] = 1; 
					else
						Flags[i] = 1;
				}
			}
		}
	}
	
	TArray<FObjectWeakPtr> RetWallObjectArr;
	for (int32 i = 0; i < Flags.Num(); ++i)
	{
		if (Flags[i] == 0) // 只取出标记为0的墙投影信息
		{
			for (int32 j = 0; j < WallProjectionArr[i].WallIndices.Num(); ++j)
			{
				int32 Index = WallProjectionArr[i].WallIndices[j];
				RetWallObjectArr.Emplace(InWallObjectArray[Index]);
			}
		}
	}

	struct FSortPredicate
	{
		FVector Dir;
		FPlane Plane;
		FSortPredicate(const FVector &InDir, const FPlane &InPlane) : Dir(InDir), Plane(InPlane) {}
		bool operator() (const FObjectWeakPtr Left, const FObjectWeakPtr Right) const
		{
			TSharedPtr<FArmyRoomSpaceArea> AreaLeft = StaticCastSharedPtr<FArmyRoomSpaceArea>(Left.Pin());
			TSharedPtr<FArmyRoomSpaceArea> AreaRight = StaticCastSharedPtr<FArmyRoomSpaceArea>(Right.Pin());
			FVector ProjLeftCenter = FVector::PointPlaneProject(AreaLeft->GetPlaneCenter(), Plane);
			FVector ProjRightCenter = FVector::PointPlaneProject(AreaRight->GetPlaneCenter(), Plane);
			return FVector::DotProduct((ProjRightCenter - ProjLeftCenter), Dir) > 0.0f;
		}
 	};
	RetWallObjectArr.Sort(FSortPredicate(PlaneDir, InFacePlane));
	return MoveTemp(RetWallObjectArr);
}

void FArmyConstructionFunction::GenerateWallFacadeLayer(const FName& InLayerName, const TSharedPtr<FArmyRoom> InRoom, const FPlane& InFacePlane,const FBox& InRegionBox)
{
	FVector PlaneNormal = InFacePlane.GetSafeNormal();
	FVector SortDir = FVector::CrossProduct(FVector::UpVector,PlaneNormal).GetSafeNormal();

	TArray<FObjectWeakPtr> WallFaceObjectArray;
	TArray<TSharedPtr<FAreaWithDepthInfo>> ValidWallFaceObjectArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, WallFaceObjectArray);

	TArray<TSharedPtr<FArmyRoomSpaceArea>> ProjectAreaArray;//吊顶、梁、地台等

	FBox TempBox(InRegionBox);
	TempBox.Min.Z = -10;
	TempBox.Max.Z = FArmySceneData::WallHeight + 10;
	for (auto& Obj : WallFaceObjectArray)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(Obj.Pin());
		FVector WallNormal = RoomSpaceArea->GetPlaneNormal();
		FVector ProjectNormal = WallNormal.ProjectOnTo(PlaneNormal).GetSafeNormal();

		if ((ProjectNormal - PlaneNormal).Size() < PlaneNormal.Size()/*背向面*/) continue;

		switch (RoomSpaceArea->SurfaceType)
		{
		case 1://墙
		{
			if (RoomSpaceArea->GenerateFromObjectType == OT_Beam)
			{
				ProjectAreaArray.Add(RoomSpaceArea);
			}
		}
			break;
		case 0://地
		case 2://顶
		{
			if (RoomSpaceArea->GenerateFromObjectType == OT_InternalRoom)
			{
				ProjectAreaArray.Add(RoomSpaceArea);
			}
		}
		default:
			continue;
		}


		TSharedPtr<FArmyWallLine> WallLine = RoomSpaceArea->GetAttachLine();

		bool ValidWall = false;
		bool Intersection = false;

		float MinDepth = FLT_MAX;
		FVector MaxStart(ForceInitToZero), MaxEnd(ForceInitToZero);

		if (WallLine.IsValid())
		{
			MaxStart = WallLine->GetCoreLine()->GetStart();
			MaxEnd = WallLine->GetCoreLine()->GetEnd();
		}
		else if(RoomSpaceArea->GenerateFromObjectType != OT_AddWall && RoomSpaceArea->GenerateFromObjectType != OT_IndependentWall)
		{
			continue;//将非墙体的面剔除
		}

		TArray<TSharedPtr<FArmyLine>> LineArray;
		RoomSpaceArea->GetLines(LineArray,true);
		for (auto L : LineArray)
		{
			Intersection = FMath::LineBoxIntersection(TempBox, L->GetStart(), L->GetEnd(), (L->GetEnd() - L->GetStart()).GetSafeNormal());

			if (!ValidWall && (TempBox.IsInside(L->GetStart()) || TempBox.IsInside(L->GetEnd()) || Intersection))
			{
				ValidWall = true;

				//FMath::SegmentPlaneIntersection()
			}

			FVector TempStart = L->GetStart();
			FVector TempEnd = L->GetEnd();
			TempStart.Z = 0;
			TempEnd.Z = 0;

			if (!WallLine.IsValid() && (MaxEnd - MaxStart).Size() < (TempEnd - TempStart).Size())
			{
				MaxStart = TempStart;
				MaxEnd = TempEnd;
			}

			float TempMin = FMath::Min(FPlane::PointPlaneDist(TempStart, PlaneNormal * InFacePlane.W, PlaneNormal),
				FPlane::PointPlaneDist(TempEnd, PlaneNormal * InFacePlane.W, PlaneNormal));
			if (TempMin < MinDepth)
			{
				MinDepth = TempMin;
			}
		}
		if (ValidWall)
		{
			TSharedPtr<FAreaWithDepthInfo> TempAreaWithDepth = MakeShareable(new FAreaWithDepthInfo(RoomSpaceArea, MinDepth));
			TempAreaWithDepth->VerticalToPlane = ProjectNormal.IsZero();
			TempAreaWithDepth->Intersection = Intersection;
			TempAreaWithDepth->SectionBox = TempBox;
			TempAreaWithDepth->StartPos = MaxStart;
			TempAreaWithDepth->EndPos = MaxEnd;
			ValidWallFaceObjectArray.AddUnique(TempAreaWithDepth);
		}
	}
	ValidWallFaceObjectArray.Sort([&](const TSharedPtr<FAreaWithDepthInfo> A, const TSharedPtr<FAreaWithDepthInfo> B) {

		FVector TempDir = (B->AreaObj->GetPlaneCenter() - A->AreaObj->GetPlaneCenter()).GetSafeNormal();

		FVector ProjectDir = TempDir.ProjectOnTo(SortDir).GetSafeNormal();

		return (ProjectDir - SortDir).Size() <= SortDir.Size();
	});


	TArray<FObjectWeakPtr> RoomSpaceComponentArray;
	TArray<FObjectWeakPtr> LastValidObjArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_ComponentBase, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_ComponentBase, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_DoorHole, RoomSpaceComponentArray);

	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CornerBayWindow, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Punch, RoomSpaceComponentArray);

	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, RoomSpaceComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, RoomSpaceComponentArray);

	for (auto& Obj : RoomSpaceComponentArray)
	{
		switch (Obj.Pin()->GetType())
		{
		case OT_ComponentBase:
		{
			FVector BasePos = Obj.Pin()->GetBasePos();
			FArmyFurniture* ComponentObj = Obj.Pin()->AsassignObj<FArmyFurniture>();
			if (ComponentObj)
			{
				BasePos.Z = ComponentObj->GetAltitude();
			}
			
			if (TempBox.IsInside(BasePos))
			{
				for (auto WallObj : ValidWallFaceObjectArray)
				{
					if (FVector::PointPlaneDist(BasePos, WallObj->AreaObj->GetPlaneCenter(), WallObj->AreaObj->GetPlaneNormal()) <= 0.1)
					{
						if (WallObj->AreaObj->IsPointIn(WallObj->AreaObj->GetPlaneLocalFromWorld(BasePos), false))
						{
							WallObj->OwnerObjectArray.AddUnique(Obj);
							break;
						}
					}
				}
			}
		}
			break;

		case OT_Pass:
		case OT_NewPass:
		case OT_Door:
		case OT_DoorHole:
		case OT_SecurityDoor:
		case OT_SlidingDoor:
		case OT_Window:
		case OT_FloorWindow:
		case OT_RectBayWindow:
		case OT_TrapeBayWindow:
		case OT_CornerBayWindow:
		case OT_Punch:
		{
			FArmyHardware* HardwareObj = Obj.Pin()->AsassignObj<FArmyHardware>();
			if (HardwareObj)
			{
				float H = HardwareObj->GetHeight();
				TArray<TSharedPtr<FArmyPoint>> AlonePoints;
				HardwareObj->GetAlonePoints(AlonePoints);
				for (auto P : AlonePoints)
				{
					bool Exist = false;
					FVector TempPoint(P->Pos.X, P->Pos.Y, H);
					for (auto WallObj : ValidWallFaceObjectArray)
					{
						float Dis = FVector::PointPlaneDist(TempPoint, WallObj->AreaObj->GetPlaneCenter(), WallObj->AreaObj->GetPlaneNormal());
						if (FMath::Abs(Dis) <= 0.1)
						{
							if (WallObj->AreaObj->IsPointIn(WallObj->AreaObj->GetPlaneLocalFromWorld(TempPoint), false))
							{
								WallObj->OwnerObjectArray.AddUnique(Obj);
								Exist = true;
								break;
							}
						}
					}
					if (Exist)break;
				}
			}
		}
			break;
		case OT_Pillar:
		case OT_AirFlue:
			break;
		case OT_PackPipe:
			break;
		default:
			break;
		}
	}

	auto LineBoxIntersection = [](const FBox& InBox,const FVector& InV1,const FVector& InV2,FVector& OutV1,FVector& OutV2) {
		
		TArray<FVector> VArray;
		if (InBox.IsInside(InV1) && InBox.IsInside(InV2))
		{
			OutV1 = InV1;
			OutV2 = InV2;
			return true;
		}
		else if (InBox.IsInside(InV1))
		{
			VArray.Add(InV1);
		}
		else if (InBox.IsInside(InV2))
		{
			VArray.Add(InV2);
		}

		FVector InterPos;
		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Min, FVector(1, 0, 0)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Min, FVector(0, 1, 0)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Min, FVector(0, 0, 1)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Max, FVector(-1, 0, 0)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Max, FVector(0, -1, 0)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (FMath::SegmentPlaneIntersection(InV1, InV2, FPlane(InBox.Max, FVector(0, 0, -1)), InterPos))
		{
			if (InBox.IsInsideOrOn(InterPos))
			{
				VArray.AddUnique(InterPos);
			}
		}

		if (VArray.Num() > 1)
		{
			OutV1 = VArray[0];
			OutV2 = VArray[1];
			return true;
		}
		return false;
	};

	TArray<FVector> ProjectValidVArray;
	TMap<FVector, FVector2D> ClinderMap;//圆心，半径，高度
	//吊顶、地台、梁等数据的投影数据
	for (auto SpaceArea : ProjectAreaArray)
	{
		FVector SpaceNormal = SpaceArea->GetPlaneNormal();
		FVector SpaceCenter = SpaceArea->GetPlaneCenter() + SpaceNormal * SpaceArea->GetExtrusionHeight();
		
		TArray<TSharedPtr<FArmyBaseArea>> VAreaArray{};
		TArray<TSharedPtr<FArmyBaseArea>> ChildAreaArray = SpaceArea->GetEditAreas();
		if (SpaceArea->SurfaceType == 1)
		{
			VAreaArray.Add(SpaceArea);
		}
		for (auto AreaPtr : ChildAreaArray)
		{
			if (FArmyRectArea* RectArea = AreaPtr->AsassignObj<FArmyRectArea>())
			{
				VAreaArray.Append(RectArea->GetInnearHoleSurface());
			}
			else if (FArmyRegularPolygonArea* RegularPolygonArea = AreaPtr->AsassignObj<FArmyRegularPolygonArea>())
			{
				VAreaArray.Append(RegularPolygonArea->GetInnearHoleSurface());
			}
			else if (FArmyCircleArea* CircleArea = AreaPtr->AsassignObj<FArmyCircleArea>())
			{
				TSharedPtr<class FArmyClinderArea> VArea = CircleArea->GetInnearHoleSurface();

				if (VArea.IsValid())
				{
					FVector Center;
					float R = 0, H = 0;

					VArea->GetClinderInfo(Center, R, H);
					FVector TempCenter(Center.X, Center.Y, 0);

					FVector Min(TempBox.Min.X, TempBox.Min.Y, 0), Max(TempBox.Max.X, TempBox.Max.Y, 0);
					FVector V1(Min.X, Max.Y, 0);
					FVector V2(Max.X, Min.Y, 0);

					bool Inter = false;

					if (FMath::PointDistToSegment(TempCenter, Min, V1) < R)
					{
						Inter = true;
					}
					if (FMath::PointDistToSegment(TempCenter, Min, V2) < R)
					{
						Inter = true;
					}
					if (FMath::PointDistToSegment(TempCenter, V1, Max) < R)
					{
						Inter = true;
					}
					if (FMath::PointDistToSegment(TempCenter, V2, Max) < R)
					{
						Inter = true;
					}

					if (!Inter && TempBox.IsInside(Center))
					{

					}
				}
			}
		}

		{
			FPlane SpacePlane(SpaceCenter, SpaceNormal);

			for (auto WallObj : ValidWallFaceObjectArray)
			{
				FVector WallCenter = WallObj->AreaObj->GetPlaneCenter();
				FVector WallNormal = WallObj->AreaObj->GetPlaneNormal();

				for (auto HoleArea : VAreaArray)
				{
					FVector AreaNormal = HoleArea->GetPlaneNormal();
					FVector ProjectNormal = AreaNormal.ProjectOnTo(PlaneNormal).GetSafeNormal();

					if ((ProjectNormal - PlaneNormal).Size() < PlaneNormal.Size()/*背向面*/) continue;

					TArray<TSharedPtr<FArmyLine>> LineArray;
					HoleArea->GetLines(LineArray, true);
					for (auto L : LineArray)
					{
						FVector V1, V2;
						if (LineBoxIntersection(TempBox, L->GetStart(), L->GetEnd(), V1, V2))
						{
							FVector PV1 = FVector::PointPlaneProject(V1, WallCenter, WallNormal);
							FVector PV2 = FVector::PointPlaneProject(V2, WallCenter, WallNormal);

							FVector LocalP1 = WallObj->AreaObj->GetPlaneLocalFromWorld(PV1);
							FVector LocalP2 = WallObj->AreaObj->GetPlaneLocalFromWorld(PV2);

							bool ValidV1 = WallObj->AreaObj->IsPointIn(LocalP1, false);
							bool ValidV2 = WallObj->AreaObj->IsPointIn(LocalP2, false);
							if (ValidV1)
							{
								WallObj->ProjectOtherVArray.Add(LocalP1);
							}
							if (ValidV2)
							{
								WallObj->ProjectOtherVArray.Add(LocalP2);
							}
							if (!ValidV1 || !ValidV2)
							{
								TArray<FVector> InterVArray;
								if (FArmyMath::CalLinePolygonIntersection(LocalP1, LocalP2, WallObj->AreaObj->GetVertices(), InterVArray))
								{
									WallObj->ProjectOtherVArray.Append(InterVArray);
								}
							}
						}
					}
				}

				if (SpaceArea->SurfaceType != 1)
				{
					TArray<FVector> WallSpaceInterVArray;
					TArray<TSharedPtr<FArmyLine>> WallLines;
					WallObj->AreaObj->GetLines(WallLines, true);
					for (auto L : WallLines)
					{
						FVector InterPos;
						if (FMath::SegmentPlaneIntersection(L->GetStart(), L->GetEnd(), SpacePlane, InterPos))
						{
							WallSpaceInterVArray.Add(InterPos);
						}
					}

					if (WallSpaceInterVArray.Num() > 1)
					{
						WallSpaceInterVArray.Sort([&](const FVector& A, const FVector& B) {
							FVector TempDir = (B - A).GetSafeNormal();

							FVector ProjectDir = TempDir.ProjectOnTo(SortDir).GetSafeNormal();

							return (ProjectDir - SortDir).Size() <= SortDir.Size();
						});

						WallObj->ProjectOtherVArray.Add(WallObj->AreaObj->GetPlaneLocalFromWorld(WallSpaceInterVArray.Last()));
						WallObj->ProjectOtherVArray.Add(WallObj->AreaObj->GetPlaneLocalFromWorld(WallSpaceInterVArray.HeapTop()));
					}
				}
			}
		}
	}

	auto GroupForLinkArea = [](TArray<TSharedPtr<FAreaWithDepthInfo>>& InAreaWithDepthArray, TArray<AreaLinkGroup>& OutLinkGroupArray) 
	{
		for (auto RoomSpaceArea : InAreaWithDepthArray)
		{
			bool NoLink = true;
			TArray<FVector> OutVertexes{ RoomSpaceArea->StartPos,RoomSpaceArea->EndPos };

			int32 LinkIndex = -1;
			for (auto& V : OutVertexes)
			{
				for (int32 i = 0; i < OutLinkGroupArray.Num(); ++i)
				{
					auto& LinkGroup = OutLinkGroupArray[i];
					if (LinkGroup.AllVertexArray.Contains(V))
					{
						NoLink = false;
						if (LinkIndex != i && LinkIndex >= 0)
						{
							auto& PreLinkGroup = OutLinkGroupArray[LinkIndex];
							PreLinkGroup += LinkGroup;//每个面最多只有两面连接
							OutLinkGroupArray.RemoveAt(i);
							break;
						}
						else
						{
							LinkGroup.SpaceAreaArray.AddUnique(RoomSpaceArea);
							LinkGroup.AllVertexArray.Append(OutVertexes);
							LinkGroup.AreaSize += (RoomSpaceArea->EndPos - RoomSpaceArea->StartPos).Size();
							if (LinkGroup.Depth > RoomSpaceArea->Depth)
							{
								LinkGroup.Depth = RoomSpaceArea->Depth;
							}
							LinkIndex = i;
						}
					}
				}
				if (!NoLink)
				{
					break;
				}
			}
			if (NoLink)
			{
				int32 Index = OutLinkGroupArray.Add(AreaLinkGroup());
				OutLinkGroupArray[Index].SpaceAreaArray.AddUnique(RoomSpaceArea);
				OutLinkGroupArray[Index].AllVertexArray.Append(OutVertexes);
				OutLinkGroupArray[Index].AreaSize += (RoomSpaceArea->EndPos - RoomSpaceArea->StartPos).Size();
				if (OutLinkGroupArray[Index].Depth > RoomSpaceArea->Depth)
				{
					OutLinkGroupArray[Index].Depth = RoomSpaceArea->Depth;
				}
			}
		}
		OutLinkGroupArray.Sort([](const AreaLinkGroup& A, const AreaLinkGroup& B) {
			return A.AreaSize < B.AreaSize;
		});
	};

	TArray<AreaLinkGroup> AreaLinkGroupArray;
	GroupForLinkArea(ValidWallFaceObjectArray,AreaLinkGroupArray);

	TArray<TSharedPtr<FAreaWithDepthInfo>> UnOverlabWallArray, DiscardWallArray;
	//2019-06-21 by huaen 由于需求有变动  不需要自动筛选最长的墙体（改为：只筛选出没有被遮挡的墙体）
	//if (AreaLinkGroupArray.Num() > 0)
	//{
	//	UnOverlabWallArray.Append(AreaLinkGroupArray.Last().SpaceAreaArray);
	//}

	for (int32 i = 0;i < AreaLinkGroupArray.Num(); ++i)
	{
		const AreaLinkGroup& CurrentLinkGroup = AreaLinkGroupArray[i];
		for (int32 k = 0; k < CurrentLinkGroup.SpaceAreaArray.Num(); ++k)
		{
			const TSharedPtr<FAreaWithDepthInfo> CurrentAreaDepth = CurrentLinkGroup.SpaceAreaArray[k];

			if (UnOverlabWallArray.Contains(CurrentAreaDepth)) continue;

			FVector ProjectStart = FVector::PointPlaneProject(CurrentAreaDepth->StartPos, InFacePlane);
			FVector ProjectEnd = FVector::PointPlaneProject(CurrentAreaDepth->EndPos, InFacePlane);

			for (int32 j = 0; j < AreaLinkGroupArray.Num(); ++j)
			{
				if (i == j) continue;

				const AreaLinkGroup& NextLinkGroup = AreaLinkGroupArray[j];
				for (int32 n = 0; n < NextLinkGroup.SpaceAreaArray.Num(); ++n)
				{
					const TSharedPtr<FAreaWithDepthInfo> NextAreaDepth = NextLinkGroup.SpaceAreaArray[n];

					FVector NextProjectStart = FVector::PointPlaneProject(NextAreaDepth->StartPos,InFacePlane);
					FVector NextProjectEnd = FVector::PointPlaneProject(NextAreaDepth->EndPos, InFacePlane);

					if ((ProjectStart - ProjectEnd).Size() - FMath::PointDistToSegment(ProjectStart, NextProjectStart, NextProjectEnd) > 0.01 &&
						(ProjectStart - ProjectEnd).Size() - FMath::PointDistToSegment(ProjectEnd, NextProjectStart, NextProjectEnd) > 0.01)//重叠
					{
						if (CurrentAreaDepth->Depth > NextAreaDepth->Depth)
						{
							if (UnOverlabWallArray.Contains(CurrentAreaDepth))//遮挡住了不应该被遮挡的墙，则舍弃
							{
								DiscardWallArray.AddUnique(NextAreaDepth);
							}
							CurrentAreaDepth->CoverAreaArray.AddUnique(NextAreaDepth->AreaObj);
						}
						else if (UnOverlabWallArray.Contains(NextAreaDepth))
						{
							DiscardWallArray.AddUnique(CurrentAreaDepth);
						}
					}
					else
					{
						continue;
					}
				}
			}

			if (CurrentAreaDepth->CoverAreaArray.Num() == 0 && !DiscardWallArray.Contains(CurrentAreaDepth))
			{
				UnOverlabWallArray.Add(CurrentAreaDepth);
			}
		} 
	}

	TArray<AreaLinkGroup> ValidAreaLinkGroupArray;
	GroupForLinkArea(UnOverlabWallArray, ValidAreaLinkGroupArray);

	GenerateWallFacadeData(InLayerName, ValidAreaLinkGroupArray, LastValidObjArray, InFacePlane);
}

void FArmyConstructionFunction::GenerateWallFacadeData(const FName& InLayerName,TArray<AreaLinkGroup>& InWallLinkGroupArray, const TArray<FObjectWeakPtr>& InWallObjectArray, const FPlane &InProjectPlane)
{
	//static const float cDrawPlaneAngleThreshold = 40.0f;

	FVector LayerCenterPos = BoundBox.GetCenter();
	FVector OriginalPos;//这里的坐标原点是立面模式下取一个点（世界坐标）作为所有投影数据的相对原点
	TArray<TSharedPtr<FArmyCustomDefine>> FacadeObjectList;

	FVector PlaneNormal(static_cast<const FVector&>(InProjectPlane));
	FVector PlaneRight = FVector::CrossProduct(FVector::UpVector, PlaneNormal);


	for (auto LinkGroup : InWallLinkGroupArray)
	{
		LinkGroup.SpaceAreaArray.Sort([&](const TSharedPtr<FAreaWithDepthInfo> A, const TSharedPtr<FAreaWithDepthInfo> B) {
			FVector TempDir = (B->AreaObj->GetPlaneCenter() - A->AreaObj->GetPlaneCenter()).GetSafeNormal();

			FVector ProjectDir = TempDir.ProjectOnTo(PlaneRight).GetSafeNormal();

			return (ProjectDir - PlaneRight).Size() <= PlaneRight.Size();
		});
	}

	InWallLinkGroupArray.Sort([&](const AreaLinkGroup& A, const AreaLinkGroup& B) {
		const TSharedPtr<FAreaWithDepthInfo> AFirst = A.SpaceAreaArray.HeapTop();
		const TSharedPtr<FAreaWithDepthInfo> ALast = A.SpaceAreaArray.Top();

		const TSharedPtr<FAreaWithDepthInfo> BFirst = B.SpaceAreaArray.HeapTop();
		const TSharedPtr<FAreaWithDepthInfo> BLast = B.SpaceAreaArray.Top();

		const FVector ACenterPos = (AFirst->StartPos + AFirst->EndPos + ALast->StartPos + ALast->EndPos) / 2;
		const FVector BCenterPos = (BFirst->StartPos + BFirst->EndPos + BLast->StartPos + BLast->EndPos) / 2;

		FVector TempDir = (BCenterPos - ACenterPos).GetSafeNormal();

		FVector ProjectDir = TempDir.ProjectOnTo(PlaneRight).GetSafeNormal();

		return (ProjectDir - PlaneRight).Size() <= PlaneRight.Size();
	});
	///////////////////////////////////////////////////////////////

	auto GenerateGapObj = [&](const FVector& InMin,const FVector& InMax, TArray<FVector>& OutWallVArray)
	{
		FVector BreakPos = InMin + (InMax - InMin) / 3;
		OutWallVArray.Add(FVector(InMin.X, InMin.Y, 0));
		OutWallVArray.Add(FVector(InMin.X, InMax.Y, 0));

		OutWallVArray.Add(FVector(InMin.X, InMax.Y, 0));
		OutWallVArray.Add(FVector(InMax.X, InMax.Y, 0));

		OutWallVArray.Add(FVector(InMax.X, InMax.Y, 0));
		OutWallVArray.Add(FVector(InMax.X, InMin.Y, 0));

		OutWallVArray.Add(FVector(InMax.X, InMin.Y, 0));
		OutWallVArray.Add(FVector(InMin.X, InMin.Y, 0));

		OutWallVArray.Add(FVector(InMin.X, InMax.Y, 0));
		OutWallVArray.Add(BreakPos);

		OutWallVArray.Add(BreakPos);
		OutWallVArray.Add(FVector(InMax.X, InMin.Y, 0));
	};

	TArray<FObjectPtr> NewComponentArray;
	auto GenerateComponentObj = [&](const TSharedPtr<FAreaWithDepthInfo> InWall,const FVector& Offset,const FName& InLayerName, TArray<FVector>& OutWallVArray) {
		for (auto Obj : InWall->OwnerObjectArray)
		{
			switch (Obj.Pin()->GetType())
			{
			case OT_ComponentBase:
			{
				FVector BasePos = Obj.Pin()->GetBasePos();
				FArmyFurniture* ComponentObj = Obj.Pin()->AsassignObj<FArmyFurniture>();
				if (ComponentObj)
				{
					TSharedPtr<FArmyFurniture> FrontObj = ComponentObj->GetFurniturePro()->GetItemLegend(E_Dxf_Front);
					if (FrontObj.IsValid())
					{
						TSharedPtr<FArmyFurniture> NewComponentObj = StaticCastSharedPtr<FArmyFurniture>(FrontObj->CopySelf());
						BasePos.Z = ComponentObj->GetAltitude();

						FVector LocalPos = InWall->AreaObj->GetPlaneLocalFromWorld(BasePos) + Offset;
						NewComponentObj->SetTransform(FTransform(LocalPos));
						NewComponentArray.Add(NewComponentObj);
						FArmySceneData::Get()->Add(NewComponentObj, XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
					}
					else
					{
						TSharedPtr<FArmyFurniture> NewComponentObj = StaticCastSharedPtr<FArmyFurniture>(ComponentObj->CopySelf());
						BasePos.Z = ComponentObj->GetAltitude();

						FVector LocalPos = InWall->AreaObj->GetPlaneLocalFromWorld(BasePos) + Offset;
						NewComponentObj->SetTransform(FTransform(LocalPos));
						NewComponentArray.Add(NewComponentObj);
						FArmySceneData::Get()->Add(NewComponentObj, XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
					}
				}
			}
			break;
			case OT_Pass:
			case OT_NewPass:
			{
				TArray<struct FLinesInfo> LinesList = GetObjFacadeBoxes(Obj, InWall->AreaObj);
				if (LinesList.Num() > 0)
				{
					FBox PassBox(ForceInitToZero);
					for (auto Lines : LinesList)
					{
						FLinearColor InColor = Lines.Color;

						int32 NCount = Lines.Vertices.Num();
						for (int n = 0; n < NCount; ++n)
						{
							FVector LocalStart = Lines.Vertices[n] + Offset;
							PassBox += LocalStart;
						}
					}
					GenerateGapObj(FVector(PassBox.Min.X, PassBox.Min.Y, 0), FVector(PassBox.Max.X, PassBox.Max.Y, 0), OutWallVArray);
				}
			}
			break;
			case OT_Door:
			case OT_DoorHole:
			case OT_SecurityDoor:
			case OT_SlidingDoor:
			case OT_Window:
			case OT_FloorWindow:
			case OT_RectBayWindow:
			case OT_TrapeBayWindow:
			case OT_CornerBayWindow:
			case OT_Punch:
			{
				TArray<struct FLinesInfo> LinesList = GetObjFacadeBoxes(Obj, InWall->AreaObj);
				if (LinesList.Num() > 0)
				{
					TArray<FSimpleElementVertex> HardwareVArray;
					for (auto Lines : LinesList)
					{
						FLinearColor InColor = Lines.Color;

						int32 NCount = Lines.Vertices.Num();
						int32 NMax = Lines.DrawType == 0 ? NCount : NCount - 1;

						for (int n = 0; n < NMax; ++n)
						{
							FVector LocalStart = Lines.Vertices[n] + Offset;
							FVector LocalEnd = Lines.Vertices[(n + 1) % NCount] + Offset;

							HardwareVArray.Add(FSimpleElementVertex(FVector4(LocalStart, 0), FVector2D(0, 0), InColor, FHitProxyId()));
							HardwareVArray.Add(FSimpleElementVertex(FVector4(LocalEnd, 0), FVector2D(0, 0), InColor, FHitProxyId()));
						}
						TSharedPtr<FArmyCustomDefine> HardWare = MakeShareable(new FArmyCustomDefine(HardwareVArray, FTransform(), GetClassTypeByObject(Obj.Pin())));
						NewComponentArray.Add(HardWare);
						FArmySceneData::Get()->Add(HardWare, XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
					}
				}
			}
			break;
			case OT_Pillar:
			case OT_AirFlue:
			break;
			case OT_PackPipe:
			break;
			default:
			break;
			}
		}
	};

	auto GenerateWallFacedObj = [](const TSharedPtr<FAreaWithDepthInfo> InWall, const FVector& Offset, TArray<FVector>& OutWallVArray)
	{
		/////////////////////  绘制墙砖开始/////////////////////////////////////
		TArray<struct FLinesInfo> TileLinesList = InWall->AreaObj->GetFacadeBox();
		TArray<FSimpleElementVertex> TileVArray;
		// 进行投影的绘制
		for (auto Lines : TileLinesList)
		{
			int32 NCount = Lines.Vertices.Num();
			FLinearColor InColor = Lines.Color;
			for (int n = 0; n < NCount; n++)
			{
				FVector CurrentProjRelVertex = InWall->AreaObj->GetPlaneLocalFromWorld(Lines.Vertices[n]) + Offset;
				FVector NextProjRelVertex = InWall->AreaObj->GetPlaneLocalFromWorld(Lines.Vertices[(n + 1) % NCount]) + Offset;

				OutWallVArray.Add(CurrentProjRelVertex);
				OutWallVArray.Add(NextProjRelVertex);
			}
		}
		/////////////////////  绘制墙砖结束///////////////////////////////////////

		////吊顶，柱子投影
		for (int32 i = 0;i < InWall->ProjectOtherVArray.Num() - 1; i += 2)
		{
			OutWallVArray.Add(InWall->ProjectOtherVArray[i] + Offset);
			OutWallVArray.Add(InWall->ProjectOtherVArray[i + 1] + Offset);
		}
	};

	TArray<FVector> WallProjectVArray,PreVerticalWallVArray;

	FVector PreProject,PrePlaneCenter,CurrentOffset(ForceInitToZero);
	bool ProjectStart = false;
	FBox PreBox(ForceInitToZero);
	
	for (auto LinkGroup : InWallLinkGroupArray)
	{
		bool CurrentProjectStart = false;

		if (ProjectStart)
		{
			for (auto AreaWithDepth : LinkGroup.SpaceAreaArray)
			{
				TArray<FVector> TempVerticalWallVArray;

				FVector StartProject = FVector::PointPlaneProject(AreaWithDepth->StartPos, InProjectPlane);
				FVector EndProject = FVector::PointPlaneProject(AreaWithDepth->EndPos, InProjectPlane);

				FVector TempDir = EndProject - StartProject;

				FVector ProjectDir = TempDir.ProjectOnTo(PlaneRight).GetSafeNormal();

				FVector CurrentPlaneCenter = AreaWithDepth->AreaObj->GetPlaneCenter();
				float HOffset = (CurrentPlaneCenter - PrePlaneCenter).Z * -1;
				PrePlaneCenter = CurrentPlaneCenter;

				CurrentOffset.Y += HOffset;

				FVector GapOffset(ForceInitToZero);
				FVector GapStart = CurrentOffset;
				if ((ProjectDir - PlaneRight).Size() <= PlaneRight.Size())
				{
					if (!CurrentProjectStart)
					{
						GapOffset = FVector((StartProject - PreProject).Size(), 0, 0);
						CurrentOffset += GapOffset;
					}
					PreProject = EndProject;
				}
				else
				{
					if (!CurrentProjectStart)
					{
						GapOffset = FVector((EndProject - PreProject).Size(), 0, 0);
						CurrentOffset += GapOffset;
					}
					PreProject = StartProject;
				}

				if (!CurrentProjectStart)
				{
					CurrentProjectStart = true;
				}

				const  TArray<FVector>& AreaVArray = AreaWithDepth->AreaObj->GetVertices();

				{
					FBox CurrentBox(AreaVArray);

					if (AreaWithDepth->VerticalToPlane)//
					{
						CurrentBox.Min.X = 0;
						CurrentBox.Max.X = 0;

						TempVerticalWallVArray.Add(CurrentBox.Min + CurrentOffset);
						TempVerticalWallVArray.Add(CurrentBox.Max + CurrentOffset);
					}

					if (GapOffset.Size() > 0.01 && FMath::Abs(PreBox.Min.Y - CurrentBox.Min.Y) < 0.01 && FMath::Abs(PreBox.Max.Y - CurrentBox.Max.Y) < 0.01)
					{
						WallProjectVArray.Append(PreVerticalWallVArray);//上一个垂直墙面
						GenerateGapObj(FVector(GapStart.X, PreBox.Min.Y, 0), FVector(GapStart.X + GapOffset.Size(), PreBox.Max.Y, 0), WallProjectVArray);
					}

					PreBox = CurrentBox;
					PreVerticalWallVArray = TempVerticalWallVArray;
				}

				if (!AreaWithDepth->VerticalToPlane)
				{
					CurrentOffset += FVector((AreaWithDepth->EndPos - AreaWithDepth->StartPos).Size() / 2, 0, 0);

					int32 VNumber = AreaVArray.Num();
					for (int32 i = 0; i < VNumber; ++i)
					{
						WallProjectVArray.Add(AreaVArray[i] + CurrentOffset);
						WallProjectVArray.Add(AreaVArray[(i + 1) % VNumber] + CurrentOffset);
					}
					GenerateWallFacedObj(AreaWithDepth, CurrentOffset, WallProjectVArray);
					GenerateComponentObj(AreaWithDepth, CurrentOffset, InLayerName, WallProjectVArray);

					CurrentOffset += FVector((AreaWithDepth->EndPos - AreaWithDepth->StartPos).Size() / 2, 0, 0);
				}
			}
		}
		else
		{
			for (auto AreaWithDepth : LinkGroup.SpaceAreaArray)
			{
				TArray<FVector> TempVerticalWallVArray;

				FVector StartProject = FVector::PointPlaneProject(AreaWithDepth->StartPos, InProjectPlane);
				FVector EndProject = FVector::PointPlaneProject(AreaWithDepth->EndPos, InProjectPlane);

				FVector TempDir = EndProject - StartProject;

				FVector ProjectDir = TempDir.ProjectOnTo(PlaneRight).GetSafeNormal();

				FVector CurrentPlaneCenter = AreaWithDepth->AreaObj->GetPlaneCenter();
				
				if (CurrentProjectStart)
				{
					float HOffset = (CurrentPlaneCenter - PrePlaneCenter).Z * -1;
					CurrentOffset.Y += HOffset;
				}
				PrePlaneCenter = CurrentPlaneCenter;

				FVector GapOffset(ForceInitToZero);
				if ((ProjectDir - PlaneRight).Size() <= PlaneRight.Size())
				{
					PreProject = EndProject;
				}
				else
				{
					PreProject = StartProject;
				}

				const  TArray<FVector>& AreaVArray = AreaWithDepth->AreaObj->GetVertices();

				{
					FBox CurrentBox(AreaVArray);

					if (AreaWithDepth->VerticalToPlane)
					{
						CurrentBox.Min.X = 0;
						CurrentBox.Max.X = 0;

						TempVerticalWallVArray.Add(CurrentBox.Min + CurrentOffset);
						TempVerticalWallVArray.Add(CurrentBox.Max + CurrentOffset);
					}

					if (GapOffset.Size() > 0 && FMath::Abs(PreBox.Min.Y - CurrentBox.Min.Y) < 0.01 && FMath::Abs(PreBox.Max.Y - CurrentBox.Max.Y) < 0.01)
					{
						WallProjectVArray.Append(PreVerticalWallVArray);//上一个垂直墙面
						GenerateGapObj(FVector(CurrentOffset.X, PreBox.Min.Y, 0), FVector(CurrentOffset.X + GapOffset.Size(), PreBox.Max.Y, 0), WallProjectVArray);
					}

					PreBox = CurrentBox;
					PreVerticalWallVArray = TempVerticalWallVArray;
				}

				if(CurrentProjectStart && !AreaWithDepth->VerticalToPlane)
				{
					CurrentOffset += FVector((AreaWithDepth->EndPos - AreaWithDepth->StartPos).Size() / 2, 0, 0);
				}
				else
				{
					CurrentProjectStart = true;
				}

				if (!AreaWithDepth->VerticalToPlane)
				{
					int32 VNumber = AreaVArray.Num();
					for (int32 i = 0; i < VNumber; ++i)
					{
						WallProjectVArray.Add(AreaVArray[i] + CurrentOffset);
						WallProjectVArray.Add(AreaVArray[(i + 1) % VNumber] + CurrentOffset);
					}
					GenerateWallFacedObj(AreaWithDepth, CurrentOffset, WallProjectVArray);
					GenerateComponentObj(AreaWithDepth, CurrentOffset, InLayerName, WallProjectVArray);

					CurrentOffset += FVector((AreaWithDepth->EndPos - AreaWithDepth->StartPos).Size() / 2, 0, 0);
				}
			}
		}

		ProjectStart = true;
	}

	TArray<FSimpleElementVertex> WallVArray;

	FVector LastOffset(CurrentOffset.X / 2,0 , 0);
	int32 VNumber = WallProjectVArray.Num();
	for (int32 i = 0;i < (VNumber - 1) ; i+=2)
	{
		FVector V1 = WallProjectVArray[i] - LastOffset;
		FVector V2 = WallProjectVArray[i + 1] - LastOffset;
		WallVArray.Add(FSimpleElementVertex(FVector4(V1, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
		WallVArray.Add(FSimpleElementVertex(FVector4(V2, 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
	}
	if (WallVArray.Num() > 0)
	{
		FacadeObjectList.Add(MakeShareable(new FArmyCustomDefine(WallVArray, FTransform(LayerCenterPos), Class_Walllayout)));
	}
	for (auto NewComponentObj : NewComponentArray)
	{
		NewComponentObj->ApplyTransform(FTransform(LayerCenterPos - LastOffset));
	}
	///////////////////////////////////////////////////////////////
	// 判断是否需要在图纸中绘制此面墙
	//auto NeedDrawPlane = [&PlaneNormal](const FVector &WallNormal) -> bool {
	//	float Dot = FVector::DotProduct(PlaneNormal, WallNormal);
	//	float Angle = FMath::Acos(Dot) / PI * 180.0f;
	//	Angle = 180.0f - Angle;
	//	if (Dot < -KINDA_SMALL_NUMBER)
	//		return Angle > -KINDA_SMALL_NUMBER && Angle < cDrawPlaneAngleThreshold + KINDA_SMALL_NUMBER;
	//	else
	//		return false;
	//};

	//// 严格判定是否需要投影此面墙
	//auto NeedDrawPlaneRestrict = [](const FVector &ProjPlaneNormal, const FVector &AreaNormal) -> bool {
	//	float Dot = FVector::DotProduct(ProjPlaneNormal, AreaNormal);
	//	return FMath::IsNearlyZero(Dot + 1.0f, KINDA_SMALL_NUMBER);
	//};

	//// 获取局部坐标系的投影平面
	//auto GetLocalProjectPlane = [&InProjectPlane](const FVector &WallNormal) -> FPlane {
	//	return FPlane(-WallNormal, InProjectPlane.W);
	//};

	//// 判断两个物体间的相对位置
	//auto LeftOrRight = [](const FVector &InSrcCenter, const FVector &InDestCenter, const FPlane &InProjectPlane, const FVector &InPlaneRight) -> bool {
	//	FVector SrcProj = FVector::PointPlaneProject(InSrcCenter, InProjectPlane);
	//	FVector DestProj = FVector::PointPlaneProject(InDestCenter, InProjectPlane);
	//	float SrcProjDis = FVector::DotProduct(SrcProj, InPlaneRight);
	//	float DestProjDis = FVector::DotProduct(DestProj, InPlaneRight);
	//	return SrcProjDis < DestProjDis;
	//};

	//// 将RoomSpaceArea分组，分成内墙面和其它面
	//TArray<TSharedPtr<FArmyRoomSpaceArea>> WallAreas;
	//TArray<TSharedPtr<FArmyRoomSpaceArea>> OtherAreas;
	//// 门窗等构件数组
	//TArray<FObjectWeakPtr> ObjectArray;
	//for (const auto &Object : InWallObjectArray)
	//{
	//	if (Object.Pin()->GetType() == OT_RoomSpaceArea)
	//	{
	//		TSharedPtr<FArmyRoomSpaceArea> AreaPtr = StaticCastSharedPtr<FArmyRoomSpaceArea>(Object.Pin());
	//		if (AreaPtr->GenerateFromObjectType == OT_WallLine)
	//			WallAreas.Emplace(AreaPtr);
	//		else
	//			OtherAreas.Emplace(AreaPtr);
	//	}
	//	else
	//	{
	//		ObjectArray.Emplace(Object);
	//	}
	//}

	//// 进行
	//// 没有可投影的墙面则退出
	//if (WallAreas.Num() == 0)
	//	return;
	//OriginalPos = WallAreas[0]->GetPlaneCenter();
	//FVector ProjectOriginal = FVector::PointPlaneProject(OriginalPos, InProjectPlane);
	//int32 FirstDrawWallIndex = 0;

	///*
	//整体思路：
	//1、确定哪些内墙是需要在当前方向上投影
	//2、每面内墙单独投影，投影面为墙面法线反方向的平面，投影包括墙和墙面内的所有对象，投影中心点为墙的中心点
	//3、将所有单独投影后的结果连接起来，每个单独投影结果都要根据OriginalPos再做偏移
	//*/
	//TSharedPtr<FArmyRoomSpaceArea> &FirstDrawingWallPtr = WallAreas[FirstDrawWallIndex];
	//for (int32 i = FirstDrawWallIndex; i < WallAreas.Num(); ++i)
	//{
	//	TSharedPtr<FArmyRoomSpaceArea> &WallPtr = WallAreas[i];
	//	FVector WallNormal = WallPtr->GetPlaneNormal();
	//	TArray<TArray<FVector>> LocalProjVerticesArr;
	//	LocalProjVerticesArr.Emplace(WallPtr->GetVertices());

	//	// 计算当前墙到第一面需要绘制的墙的水平距离
	//	float Distance = 0.0f;
	//	if (FirstDrawWallIndex < i)
	//	{
	//		TSharedPtr<FArmyRoomSpaceArea> &CurrentWallPtr = WallAreas[i];
	//		float Sign = 0.0f;
	//		bool CurrentIsRight = LeftOrRight(CurrentWallPtr->GetPlaneCenter(), FirstDrawingWallPtr->GetPlaneCenter(), InProjectPlane, PlaneRight);
	//		for (int32 j = FirstDrawWallIndex + 1; j < i; ++j)
	//		{
	//			TSharedPtr<FArmyRoomSpaceArea> &WallBetweenFirstAndCurrentPtr = WallAreas[j];
	//			if (LeftOrRight(WallBetweenFirstAndCurrentPtr->GetPlaneCenter(), FirstDrawingWallPtr->GetPlaneCenter(), InProjectPlane, PlaneRight) == CurrentIsRight)
	//			{
	//				Sign = LeftOrRight(WallBetweenFirstAndCurrentPtr->GetPlaneCenter(), FirstDrawingWallPtr->GetPlaneCenter(), InProjectPlane, PlaneRight) ? -1.0f : 1.0f;
	//				Distance += WallBetweenFirstAndCurrentPtr->GetBounds().GetSize().X * Sign;
	//			}
	//		}
	//		
	//		Sign = CurrentIsRight ? -1.0f : 1.0f;
	//		Distance += FirstDrawingWallPtr->GetBounds().GetExtent().X * Sign;
	//		Distance += CurrentWallPtr->GetBounds().GetExtent().X * Sign;
	//	}

	//	for (int32 j = 0; j < LocalProjVerticesArr.Num(); ++j)
	//	{
	//		//OriginalPos = WallPtr->GetPlaneCenter();
	//		const TArray<FVector> &LocalProjVertices = LocalProjVerticesArr[j];
	//		int32 NumVerts = LocalProjVertices.Num();
	//		for (int32 k = 0; k < NumVerts; ++k)
	//		{
	//			// 计算墙面顶点相对于OriginalPos的偏移
	//			FVector CurrentProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight * LocalProjVertices[k].X + FVector::UpVector * LocalProjVertices[k].Y;
	//			FVector NextProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight * LocalProjVertices[(k + 1) % NumVerts].X + FVector::UpVector * LocalProjVertices[(k + 1) % NumVerts].Y;
	//			// 将墙面顶点投影到平面上
	//			CurrentProjRelVertex = FVector::PointPlaneProject(CurrentProjRelVertex, InProjectPlane) - ProjectOriginal;
	//			NextProjRelVertex = FVector::PointPlaneProject(NextProjRelVertex, InProjectPlane) - ProjectOriginal;
	//			WallVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(CurrentProjRelVertex, PlaneRight), FVector::DotProduct(CurrentProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
	//			WallVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(NextProjRelVertex, PlaneRight), FVector::DotProduct(NextProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
	//		}
	//	}

	//	/////////////////////  绘制墙砖开始/////////////////////////////////////
	//	TArray<struct FLinesInfo> TileLinesList = WallPtr->GetFacadeBox();
	//	TArray<FSimpleElementVertex> TileVArray;
	//	// 进行投影的绘制
	//	for (auto Lines : TileLinesList)
	//	{
	//		int32 NCount = Lines.Vertices.Num();
	//		FLinearColor InColor = Lines.Color;
	//		for (int n = 0; n < NCount; n++)
	//		{
	//			
	//			FVector CurrentPos = GetLocalPosByWallSpace(WallPtr, Lines.Vertices[n]);
	//			FVector NextPos = GetLocalPosByWallSpace(WallPtr, Lines.Vertices[(n + 1) % NCount]);
	//			// 计算墙面顶点相对于OriginalPos的偏移
	//			FVector CurrentProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight * CurrentPos.X + FVector::UpVector * CurrentPos.Y;
	//			FVector NextProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight * NextPos.X + FVector::UpVector * NextPos.Y;
	//			// 将墙面顶点投影到平面上
	//			CurrentProjRelVertex = FVector::PointPlaneProject(CurrentProjRelVertex, InProjectPlane)-ProjectOriginal;
	//			NextProjRelVertex = FVector::PointPlaneProject(NextProjRelVertex, InProjectPlane) - ProjectOriginal;
	//			TileVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(CurrentProjRelVertex, PlaneRight), FVector::DotProduct(CurrentProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//			TileVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(NextProjRelVertex, PlaneRight), FVector::DotProduct(NextProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//		}
	//	}
	//	if (TileVArray.Num() > 0)
	//	{
	//		FacadeObjectList.Add(MakeShareable(new FArmyCustomDefine(TileVArray, FTransform(LayerCenterPos), Class_Walllayout)));
	//	}
	//	/////////////////////  绘制墙砖结束///////////////////////////////////////

	//	/////////////////////  绘制门窗风道柱子开始////////////////////////////////
	//	for (auto It : ObjectArray)
	//	{
	//		TArray<struct FLinesInfo> LinesList = GetObjFacadeBoxes(It, WallPtr);
	//		if (LinesList.Num() == 0)
	//		{
	//			continue;
	//		}

	//		TArray<FSimpleElementVertex> HardwareVArray;
	//		// 进行门窗风道柱子投影的绘制
	//		for (auto Lines : LinesList)
	//		{
	//			int32 NCount = Lines.Vertices.Num();
	//			FLinearColor InColor = Lines.Color;
	//			// 首位相连进行绘制
	//			if (Lines.DrawType == 0)
	//			{
	//				for (int n = 0; n < NCount; n++)
	//				{
	//					// 计算墙面顶点相对于OriginalPos的偏移
	//					FVector CurrentProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight *  Lines.Vertices[n].X + FVector::UpVector *  Lines.Vertices[n].Y;
	//					FVector NextProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight *  Lines.Vertices[(n + 1) % NCount].X + FVector::UpVector *  Lines.Vertices[(n + 1) % NCount].Y;
	//					// 将墙面顶点投影到平面上
	//					CurrentProjRelVertex = FVector::PointPlaneProject(CurrentProjRelVertex, InProjectPlane) - ProjectOriginal;
	//					NextProjRelVertex = FVector::PointPlaneProject(NextProjRelVertex, InProjectPlane) - ProjectOriginal;
	//					HardwareVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(CurrentProjRelVertex, PlaneRight), FVector::DotProduct(CurrentProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//					HardwareVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(NextProjRelVertex, PlaneRight), FVector::DotProduct(NextProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//				}
	//			}
	//			else if (Lines.DrawType == 1) // 首位不相连进行绘制
	//			{
	//				for (int n = 0; n < NCount - 1; n++)
	//				{
	//					// 计算墙面顶点相对于OriginalPos的偏移
	//					FVector CurrentProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight *  Lines.Vertices[n].X + FVector::UpVector *  Lines.Vertices[n].Y;
	//					FVector NextProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight *  Lines.Vertices[(n + 1) % NCount].X + FVector::UpVector *  Lines.Vertices[(n + 1) % NCount].Y;
	//					// 将墙面顶点投影到平面上
	//					CurrentProjRelVertex = FVector::PointPlaneProject(CurrentProjRelVertex, InProjectPlane) - ProjectOriginal;
	//					NextProjRelVertex = FVector::PointPlaneProject(NextProjRelVertex, InProjectPlane) - ProjectOriginal;
	//					HardwareVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(CurrentProjRelVertex, PlaneRight), FVector::DotProduct(CurrentProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//					HardwareVArray.Add(FSimpleElementVertex(FVector4(FVector::DotProduct(NextProjRelVertex, PlaneRight), FVector::DotProduct(NextProjRelVertex, FVector::UpVector), 0), FVector2D(0, 0), InColor, FHitProxyId()));
	//				}
	//			}
	//		}

	//		if (HardwareVArray.Num() > 0)
	//		{
	//			FacadeObjectList.Add(MakeShareable(new FArmyCustomDefine(HardwareVArray, FTransform(LayerCenterPos), GetClassTypeByObject(It.Pin()))));
	//		}

	//		/////////////////////  绘制门窗风道柱子结束////////////////////////////////
	//	}

	//	/////////////////////  绘制DXF构件开始////////////////////////////////
	//	// 进行dxf构件的投影计算
	//	for (auto It : ObjectArray)
	//	{
	//		FVector CenterPos;
	//		if (GetComponentCenter(It, WallPtr, CenterPos))
	//		{
	//			TArray<FSimpleElementVertex> DxfVArray;
	//			// 计算墙面顶点相对于OriginalPos的偏移
	//			FVector CurrentProjRelVertex = OriginalPos + PlaneRight * Distance + PlaneRight * CenterPos.X + FVector::UpVector * CenterPos.Y;
	//			// 将墙面顶点投影到平面上
	//			CurrentProjRelVertex = FVector::PointPlaneProject(CurrentProjRelVertex, InProjectPlane) - ProjectOriginal;
	//			FVector NewCenterPos = FVector(FVector::DotProduct(CurrentProjRelVertex, PlaneRight), FVector::DotProduct(CurrentProjRelVertex, FVector::UpVector), 0);
	//			TSharedPtr<FArmyFurniture> FArmyObj = StaticCastSharedPtr<FArmyFurniture>(It.Pin())->GetFurniturePro()->GetItemLegend(E_Dxf_Front);
	//			if (FArmyObj.IsValid())
	//			{
	//				FArmyObj->SetPosition(NewCenterPos);
	//				DxfVArray.Append(FArmyObj->GetElementVertex());
	//			}
	//			if (DxfVArray.Num() > 0)
	//			{
	//				FacadeObjectList.Add(MakeShareable(new FArmyCustomDefine(DxfVArray, FTransform(LayerCenterPos), GetClassTypeByObject(It.Pin()))));
	//			}
	//		}
	//	}
	//	/////////////////////  绘制DXF构件结束////////////////////////////////
	//}

	//// 绘制墙体
	//if (WallVArray.Num() > 0)
	//{
	//	FacadeObjectList.Add(MakeShareable(new FArmyCustomDefine(WallVArray, FTransform(LayerCenterPos), Class_Walllayout)));
	//}

	// 开始自定义绘制
	for (auto Facade : FacadeObjectList)
	{
		if (Facade.IsValid())
		{
            FArmySceneData::Get()->Add(Facade, XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));
		}
		
	}
	//TSharedPtr<FArmyCustomDefine> WallFaceObject = MakeShareable(new FArmyCustomDefine);
	//WallFaceObject->SetBelongClass(Class_Walllayout);
	//WallFaceObject->ApplyTransform(FTransform(LayerCenterPos));
	//WallFaceObject->SetBaseVertexArray(VArray);
	//FArmySceneData::Get()->Add(WallFaceObject, XRArgument(1).ArgString(TEXT("AUTO_DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(InLayerName).ArgUint32(E_ConstructionModel));

}

// 计算不同构件的立面box
TArray<struct FLinesInfo> FArmyConstructionFunction::GetObjFacadeBoxes(FObjectWeakPtr InObj, TSharedPtr<FArmyRoomSpaceArea> &InWallPtr)
{

	// 判断门窗是否投影到该墙体
	auto NeedDrawPlane = [](const FObjectWeakPtr InObjPtr, const TSharedPtr<FArmyWallLine>& InWallLine) -> bool {
		// 获得与这个构件相关的墙体
		TArray<TSharedPtr<FArmyWallLine>> attWallLines;
		bool IsEffective = false;
		FArmySceneData::Get()->GetHardwareRelateWalllines(InObjPtr.Pin(), attWallLines);
		for (auto It : attWallLines)
		{
			if (It->GetUniqueID() == InWallLine->GetUniqueID())
			{
				return true;
			}
		}
		return false;
	};


	TArray<struct FLinesInfo> OutLinesArray;
	TArray<struct FLinesInfo> TmpLinesArray;
	if (!InObj.Pin().IsValid() || !InWallPtr.IsValid())
	{
		return OutLinesArray;
	}

	// 获得墙体对应的墙线  此处有可能是在新建墙上  这里不需要再加墙体限制
	//TSharedPtr<FArmyWallLine> WallLine = InWallPtr->GetAttachLine();
	//if (!WallLine.IsValid())
	//{
	//	return OutLinesArray;
	//}

	switch (InObj.Pin()->GetType())
	{
	case OT_Window:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyWindow> FArmyObj = StaticCastSharedPtr<FArmyWindow>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_FloorWindow:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyFloorWindow> FArmyObj = StaticCastSharedPtr<FArmyFloorWindow>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_RectBayWindow:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyRectBayWindow> FArmyObj = StaticCastSharedPtr<FArmyRectBayWindow>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_TrapeBayWindow:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyTrapeBayWindow> FArmyObj = StaticCastSharedPtr<FArmyTrapeBayWindow>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_CornerBayWindow:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyCornerBayWindow> FArmyObj = StaticCastSharedPtr<FArmyCornerBayWindow>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}


	case OT_Door:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmySingleDoor> FArmyObj = StaticCastSharedPtr<FArmySingleDoor>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_SecurityDoor:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmySecurityDoor> FArmyObj = StaticCastSharedPtr<FArmySecurityDoor>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	/** 推拉门 */
	case OT_SlidingDoor:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmySlidingDoor> FArmyObj = StaticCastSharedPtr<FArmySlidingDoor>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}


	case OT_Pass:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyPass> FArmyObj = StaticCastSharedPtr<FArmyPass>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	case OT_NewPass:
	{
		//if (NeedDrawPlane(InObj, WallLine))
		{
			TSharedPtr<FArmyNewPass> FArmyObj = StaticCastSharedPtr<FArmyNewPass>(InObj.Pin());
			TmpLinesArray = FArmyObj->GetFacadeBox();
		}
		break;
	}
	//case OT_DoorHole:
	//{
	//	if (NeedDrawPlane(InObj, WallLine))
	//	{
	//		TSharedPtr<FArmyPass> FArmyObj = StaticCastSharedPtr<FArmyPass>(InObj.Pin());
	//		TmpLinesArray = FArmyObj->GetFacadeBox();
	//	}
	//	break;
	//}
	/*case OT_Pillar:
	{
		TSharedPtr<FArmyPillar> FArmyObj = StaticCastSharedPtr<FArmyPillar>(InObj.Pin());
		TmpLinesArray = FArmyObj->GetFacadeBox(WallLine->GetCoreLine());
		break;
	}*/
	//case OT_AirFlue:
	//{
	//	TSharedPtr<FArmyAirFlue> FArmyObj = StaticCastSharedPtr<FArmyAirFlue>(InObj.Pin());
	//	TmpLinesArray = FArmyObj->GetFacadeBox(WallLine->GetCoreLine());
	//	break;
	//}
	//case OT_PackPipe:
	//{
	//	TSharedPtr<FArmyPackPipe> FArmyObj = StaticCastSharedPtr<FArmyPackPipe>(InObj.Pin());
	//	TmpLinesArray = FArmyObj->GetFacadeBox(WallLine->GetCoreLine());
	//	break;
	//}
	}

	// 世界坐标系装换为相对坐标
	for (auto Lines : TmpLinesArray)
	{
		TArray<FVector> OutVectors;
		for (auto It : Lines.Vertices)
		{
			OutVectors.Push(GetLocalPosByWallSpace(InWallPtr, It));
		}
		if (OutVectors.Num() > 0)
		{
			OutLinesArray.Add(FLinesInfo(OutVectors, Lines.Color, Lines.LineType, Lines.DrawType));
		}
	}

	return OutLinesArray;
}

// 判断点位是否可以投影到当面墙，如果可以计算出中心点
bool FArmyConstructionFunction::GetComponentCenter(FObjectWeakPtr InObj, TSharedPtr<FArmyRoomSpaceArea> &InWallPtr, FVector &OutCenter)
{
	if (!InObj.Pin().IsValid() || !InWallPtr.IsValid())
	{
		return false;
	}

	// 获得墙体对应的墙线
	TSharedPtr<FArmyWallLine> WallLine = InWallPtr->GetAttachLine();
	if (!WallLine.IsValid())
	{
		return false;
	}

	// 如果不是构件则不进行绘制
	if (InObj.Pin()->GetType() != OT_ComponentBase)
	{
		return false;
	}

	TSharedPtr<FArmyFurniture> FArmyObj = StaticCastSharedPtr<FArmyFurniture>(InObj.Pin());
	// 判定该构件是否有立面投影
	if (!(FArmyObj->GetFurniturePro()->GetItemLegend(E_Dxf_Front)).IsValid())
	{
		return false;
	}

	FVector CenterPos = FArmyObj->LocalTransform.GetLocation();

	// 点是否在墙线上
	if (FArmyMath::Distance(CenterPos, WallLine->GetCoreLine()->GetStart(), WallLine->GetCoreLine()->GetEnd()) > 0.1f)
	{
		return false;
	}

	// 点是否在墙线上
	if (!FArmyMath::IsPointOnLine(CenterPos, WallLine->GetCoreLine()->GetStart(), WallLine->GetCoreLine()->GetEnd(),0.001f))
	{
		return false;
	}

	CenterPos.Z = CenterPos.Z + FArmyObj->GetFurniturePro()->GetAltitude();
	// 世界坐标系装换为相对坐标
	OutCenter = GetLocalPosByWallSpace(InWallPtr, CenterPos);

	return true;
}

// 获得点位相对于墙面的本地坐标
FVector FArmyConstructionFunction::GetLocalPosByWallSpace(TSharedPtr<FArmyRoomSpaceArea> InWallSpace,FVector InSrcPos)
{
	if (!InWallSpace.IsValid())
	{
		return FVector::ZeroVector;
	}

	FVector LocalOriginPos = InWallSpace->GetPlaneCenter();
	FVector LocalXDir = InWallSpace->GetXDir();
	FVector LocalYDir = InWallSpace->GetYDir();
	float x = FVector::DotProduct((InSrcPos - LocalOriginPos), LocalXDir);
	float y = FVector::DotProduct((InSrcPos - LocalOriginPos), LocalYDir);
	
	return FVector(x, y, 0.0f);
}
