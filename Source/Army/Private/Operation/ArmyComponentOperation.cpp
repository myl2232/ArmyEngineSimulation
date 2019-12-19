#include "ArmyComponentOperation.h"
//#include "ObjParser.h"
#include "ArmyDxfModel.h"
#include "dxfFile.h"
#include "ArmyToolsModule.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmyTypes.h"
#include "ArmyAutoCad.h"

FArmyComponentOperation::FArmyComponentOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
{
	SelectedComponent = NULL;
}

FArmyComponentOperation::~FArmyComponentOperation()
{
}

void FArmyComponentOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (!IsModifying && SelectedComponent.IsValid())
	{
		SelectedComponent->Draw(PDI, View);
	}
}

void FArmyComponentOperation::Reset()
{
	if (SelectOperationPoint.IsValid())
	{
		SelectOperationPoint->SetState(FArmyPrimitive::OPS_Normal);
		//SelectOperationPoint->SetPropertyFlag(FArmyObject::FLAG_POINTCAPTUREABLE, IsSelectCaptruePoint);
	}
	SelectOperationPoint = NULL;
	SelectedComponent = NULL;
	IsModifying = false;
	IsModified = false;
}

void FArmyComponentOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	static int32 OldX = X, OldY = Y;
	if (OldX == X && OldY == Y)
	{
		return;
	}
	else
	{
		OldX = X;
		OldY = Y;
	}

	if (SelectOperationPoint.IsValid() && SelectedComponent.IsValid())
	{
		IsModified = true;
		FVector WorldPos = GVC->PixelToWorld(X, Y, 0);
		WorldPos.Z = 0;

		if (SelectOperationPoint->OperationType == XROP_MOVE
			|| SelectOperationPoint->OperationType == XROP_NONE)
		{
			bool bSetPos = true;
			bool inRoom = false;
			bool bAutoAttach = false;
			TSharedPtr<FArmyLine> CaptureLine;
			TSharedPtr<FArmyRoom> CaptureRoom;
			if (!IsModifying)
			{
				//@ 如果构件在墙外，不显示标尺
				TArray<TWeakPtr<FArmyObject>> WallList;
				FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, WallList);
				for (auto It : WallList)
				{
					TSharedPtr<FArmyRoom> WallList = StaticCastSharedPtr<FArmyRoom>(It.Pin());
					if (WallList.IsValid() && WallList->IsPointInRoom(WorldPos))
					{
						if (SelectedComponent->GetPlaceLocationFlag() == EPLF_ROOMSPACE)
						{
							bAutoAttach = false;
							inRoom = true;

						}
						else if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
						{
							//FVector CapturePos;
							if (CaptureClosestPoint(WorldPos, WorldPos, CaptureLine, CaptureRoom))
							{
								LastPos = WorldPos;
							}
							//墙线点位不开启四轴标尺，但需要捕捉
							//WorldPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
							bAutoAttach = false;
							inRoom = false;
						}
						break;
					}
				}

				ShowAxisRulerDelegate.ExecuteIfBound(SelectedComponent, inRoom, bAutoAttach);
				SelectedComponent->SetPosition(WorldPos);
			}

			if (SelectedComponent->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ) && SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
			{
				PositionSelfAdaption(WorldPos,CaptureLine,CaptureRoom);
			}
		}
	}
}

void FArmyComponentOperation::MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key)
{
	if (!IsModifying && Key != EKeys::LeftMouseButton)
		return;

	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;
	SelectedComponent->SetbDragModify(true);


	if (SelectOperationPoint.IsValid() && SelectedComponent.IsValid())
	{
		IsModified = true;
		if (SelectOperationPoint->OperationType == XROP_MOVE)
		{
			//@ 修改点位时，点位仅能在所在房间移动
			TWeakPtr<FArmyRoom> RelateRoom = SelectedComponent->GetRelateRoom();
			TSharedPtr<FArmyLine> CaptureLine;
			TSharedPtr<FArmyRoom> CaptureRoom;
			if (RelateRoom.IsValid())
			{
				bool bSetPos = true;
				bool inRoom = false;
				bool bAutoAttach = false;

				const FBox ComBox = SelectedComponent->GetBounds();
				FVector BoxSize = ComBox.GetSize();
				FVector LeftTop(WorldPos.X - BoxSize.X / 2, WorldPos.Y - BoxSize.Y / 2, 0);
				FVector RightTop(WorldPos.X + BoxSize.X / 2, WorldPos.Y - BoxSize.Y / 2, 0);
				FVector RightBottom(WorldPos.X + BoxSize.X / 2, WorldPos.Y + BoxSize.Y / 2, 0);
				FVector LeftBottom(WorldPos.X - BoxSize.X / 2, WorldPos.Y + BoxSize.Y / 2, 0);

				//放置在空间的点位，移动时外包几何不能超出所在房间
				if (SelectedComponent->GetPlaceLocationFlag() == EPLF_ROOMSPACE
					&& RelateRoom.Pin()->IsPointInRoom(LeftTop)
					&& RelateRoom.Pin()->IsPointInRoom(RightTop)
					&& RelateRoom.Pin()->IsPointInRoom(RightBottom)
					&& RelateRoom.Pin()->IsPointInRoom(LeftBottom))
				{
					bSetPos = true;
					bAutoAttach = false;
					inRoom = true;
				}
				else if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE
					&& RelateRoom.Pin()->IsPointInRoom(WorldPos))
				{
					//WorldPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
					
					if (CaptureClosestPoint(WorldPos, WorldPos, CaptureLine, CaptureRoom))
					{
						LastPos = WorldPos;
					}
					
					bSetPos = true;
					bAutoAttach = false;
					inRoom = false;
				}
				else
				{
					bSetPos = false;
					bAutoAttach = false;
					inRoom = false;
				}

				ShowAxisRulerDelegate.ExecuteIfBound(SelectedComponent, inRoom, bAutoAttach);
				if (bSetPos)
				{
					SelectedComponent->SetPosition(WorldPos);
				}
			}

			if (SelectedComponent->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ) && SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
			{
				PositionSelfAdaption(WorldPos, CaptureLine, CaptureRoom);
			}
		}
		else if (SelectOperationPoint->OperationType == XROP_ROTATE)
		{
			FVector BasePoint = SelectedComponent->GetBasePos();
			FVector2D BaseLine = FVector2D(SelectOperationPoint->GetPos() - BasePoint);
			if (BaseLine.Size() < 0.0001) return;
			FVector2D CurrentLine = FVector2D(WorldPos - BasePoint);
			float flag = BaseLine.GetSafeNormal().operator^(CurrentLine.GetSafeNormal());
			flag = flag < 0 ? -1 : 1;
			float TempAngle = FVector2D::DotProduct(BaseLine.GetSafeNormal(), CurrentLine.GetSafeNormal());

			TempAngle = FMath::Acos(TempAngle)*flag;
			TempAngle *= (180.f / PI);
			FRotator Rot(0, TempAngle, 0);
			//SelectedComponent->SetRotation(Rot);
			SelectedComponent->ApplyTransform(FTransform(Rot));
			/*SelectedComponent->SetIsRotating(true);
			SelectedComponent->HideRuleLineInputBox(true);*/
		}
		else if (SelectOperationPoint->OperationType == XROP_SCALE)
		{			
			FVector TempScale(1, 1, 1);
			uint32 SelectIndex = SelectedComponent->GetOperationIndex(SelectOperationPoint);
			uint32 BaseIndex = 0, BaseLeftIndex = 0, BaseRightIndex = 0;
			bool FreeScale = false;
			switch (SelectIndex)
			{
			case 1:
				BaseIndex = 9;
				BaseLeftIndex = 3;
				BaseRightIndex = 7;
				FreeScale = true;
				break;
			case 2:
				BaseIndex = 8;
				break;
			case 3:
				BaseIndex = 7;
				BaseLeftIndex = 9;
				BaseRightIndex = 1;
				FreeScale = true;
				break;
			case 4:
				BaseIndex = 6;
				break;
			case 6:
				BaseIndex = 4;
				break;
			case 7:
				BaseIndex = 3;
				BaseLeftIndex = 1;
				BaseRightIndex = 9;
				FreeScale = true;
				break;
			case 8:
				BaseIndex = 2;
				break;
			case 9:
				BaseIndex = 1;
				BaseLeftIndex = 7;
				BaseRightIndex = 3;
				FreeScale = true;
				break;
			default:
				break;
			}
			if (BaseIndex == 0)return;
			TSharedPtr<FArmyEditPoint> OBasePoint = SelectedComponent->GetOperationPoint(BaseIndex);
			if (!OBasePoint.IsValid()) return;
			FVector BasePoint = OBasePoint->GetPos();
			if (FreeScale)
			{
				TSharedPtr<FArmyEditPoint> OBaseLeftPoint = SelectedComponent->GetOperationPoint(BaseLeftIndex);
				TSharedPtr<FArmyEditPoint> OBaseRightPoint = SelectedComponent->GetOperationPoint(BaseRightIndex);
				if (!OBaseLeftPoint.IsValid() || !OBaseRightPoint.IsValid()) return;
				FVector2D LeftLine = FVector2D(OBaseLeftPoint->GetPos() - BasePoint);
				FVector2D RightLine = FVector2D(OBaseRightPoint->GetPos() - BasePoint);

				FVector LeftNearPoint = FMath::ClosestPointOnInfiniteLine(BasePoint, OBaseLeftPoint->GetPos(), WorldPos);
				FVector RightNearPoint = FMath::ClosestPointOnInfiniteLine(BasePoint, OBaseRightPoint->GetPos(), WorldPos);

				FVector2D LeftNearLine = FVector2D(LeftNearPoint) - FVector2D(BasePoint);
				FVector2D RightNearLine = FVector2D(RightNearPoint) - FVector2D(BasePoint);

				int32 LeftFlag = 1, RightFlag = 1;
				if ((LeftLine.GetSafeNormal() + LeftNearLine.GetSafeNormal()).Size() < 0.0001)
				{
					LeftFlag = -1;
				}
				if ((RightLine.GetSafeNormal() + RightNearLine.GetSafeNormal()).Size() < 0.0001)
				{
					RightFlag = -1;
				}

				float LeftScale = LeftFlag * LeftNearLine.Size() / LeftLine.Size();
				float RightScale = RightFlag * RightNearLine.Size() / RightLine.Size();
				if (SelectIndex == 3 || SelectIndex == 7)
				{
					TempScale = FVector(LeftScale, RightScale, 1);
				}
				else
				{
					TempScale = FVector(RightScale, LeftScale, 1);
				}
			}
			else
			{
				FVector NearPoint = FMath::ClosestPointOnInfiniteLine(BasePoint, SelectOperationPoint->GetPos(), WorldPos);
				FVector2D CurrentLine(NearPoint - BasePoint);
				FVector2D BaseLine(SelectOperationPoint->GetPos() - BasePoint);

				float ScaleV = CurrentLine.Size() / BaseLine.Size();
				if ((CurrentLine.GetSafeNormal() + BaseLine.GetSafeNormal()).Size() < 0.0001)
				{
					ScaleV *= -1;
				}

				if (SelectIndex == 2 || SelectIndex == 8)
				{
					TempScale = FVector(1, ScaleV, 1);
				}
				else
				{
					TempScale = FVector(ScaleV, 1, 1);
				}
			}
			if (FMath::Abs(TempScale.X) < 0.0001 || FMath::Abs(TempScale.Y) < 0.0001)
			{
				return;
			}
			FTransform TempTran;
			TempTran.SetScale3D(TempScale);
			SelectedComponent->ApplyTransform(TempTran);
			FVector Offset = BasePoint - OBasePoint->GetPos();
			SelectedComponent->ApplyTransform(FTransform(Offset));			
		}
	}
}

bool FArmyComponentOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Event == IE_Pressed && bCreatFurniture)
	{
		if (Key == EKeys::LeftMouseButton && !IsModifying && SelectedComponent.IsValid())
		{
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			SelectedComponent->SetState(OS_Normal);
			SCOPE_TRANSACTION(TEXT("添加构件"));
			if (SelectedComponent->GetType() == OT_CurtainBox)
			{
				SelectedComponent->SetBeamWidth(10);
				SelectedComponent->SetThickness(15);
			}
			
			bool bInRoom = false;
			if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE
				|| SelectedComponent->GetPlaceLocationFlag() == EPLF_ROOMSPACE)
			{
				//@ 如果构件在墙外，不创建构件，主下水管需放置在空间内，开关需放置在墙线上
				FVector MouseWorldPos = InViewPortClient->PixelToWorld(InViewPort->GetMouseX(),InViewPort->GetMouseY(),0);
				MouseWorldPos.Z = 0;
				bInRoom = CalPositionIllegal(MouseWorldPos);

				//不在房间内，直接返回，此时不结束绘制
				if (!bInRoom)
				{
					if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
					{
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("请放置在墙线上"));
						SelectedComponent->SetPosition(LastPos);
					}
					else
					{
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("请放置在房间内"));

					}
					return false;
				}
				SelectedComponent->CalcRelatedRoom();
			}

			SelectedComponent->SetbCreateOperation(true);
			TSharedPtr<FArmyFurniture> ResultFurniture = SelectedComponent->CopyFurniture();

            //@郭子阳 断点改造
			ResultFurniture->SetBreakPointType(EBreakPointType::NativePoint);


			if (ResultFurniture->GetType() == OT_CurtainBox)
			{
				FArmySceneData::Get()->Add(ResultFurniture, XRArgument(1).ArgUint32(E_ModifyModel).ArgFName(FName(TEXT("构建"))));
			}
			else
			{
				FArmySceneData::Get()->Add(ResultFurniture, XRArgument(1).ArgUint32(E_HomeModel).ArgFName(FName(TEXT("构建"))));
			}			
			/*EndOperation();
			FArmyToolsModule::Get().GetRectSelectTool()->Clear();*/
			
		}
		else if (Key == EKeys::RightMouseButton || Key == EKeys::Escape)
		{
			//滚动后右键需判断点位位置
			bool bInRoom = false;
			if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE
				|| SelectedComponent->GetPlaceLocationFlag() == EPLF_ROOMSPACE)
			{
				FVector MouseWorldPos = InViewPortClient->PixelToWorld(InViewPort->GetMouseX(), InViewPort->GetMouseY(), 0);
				MouseWorldPos.Z = 0;
				bInRoom = CalPositionIllegal(MouseWorldPos);

				if (!bInRoom)
				{
					if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
					{
						SelectedComponent->SetPosition(LastPos);
					}
				}
			}
			
			SelectedComponent->SetbCreateOperation(false);
			ShowAxisRulerDelegate.ExecuteIfBound(SelectedComponent, false, false);
			EndOperation();
			FArmyToolsModule::Get().GetRectSelectTool()->Clear();
		}
	}
	else if (Key == EKeys::LeftMouseButton && Event == IE_Released && !bCreatFurniture)
	{
		//开关等放置在墙线上的点位修改操作必须放置在墙线上
		if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
		{
			SelectedComponent->CalcRelatedRoom();

			TWeakPtr<FArmyRoom> RelateRoom = SelectedComponent->GetRelateRoom();

			if (!RelateRoom.IsValid())
				return false;

			SelectedComponent->SetState(OS_Selected);

			TArray<TSharedPtr<FArmyLine> > Lines;
			RelateRoom.Pin()->GetLines(Lines);
			FVector ComPos = SelectedComponent->GetBaseEditPoint()->GetPos();
			bool bInLine = false;
			for (auto L : Lines)
			{
				if (L.IsValid() && FArmyMath::IsPointOnLine(ComPos, L->GetStart(), L->GetEnd()))
				{
					bInLine = true;
					break;
				}
			}
			if (!bInLine)
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("请放置在墙线上"));
				SelectedComponent->SetPosition(LastPos);
				return false;
			}
		}
		SelectedComponent->SetbDragModify(false);
		//SelectedComponent->SetState(OS_Selected);
		SCOPE_TRANSACTION(TEXT("拖拽修改构件"));
		SelectedComponent->Modify();
		EndOperation();
	}
	return false;
}

void FArmyComponentOperation::BeginOperation(XRArgument InArg)
{
	OperationArg = InArg;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FString FileDirDxf = "";
	FString FileDirPAK = "";
	FString FacadeDirDxf = "";
	switch (InArg._ArgInt32)
	{
	case 0://创建
		bCreatFurniture = true;
		FileDirDxf = InArg._ArgUint8 == 1 ? TEXT("Assets/PointPosition/") : TEXT("Assets/");
		FileDirDxf = FPaths::ProjectContentDir() + FileDirDxf + InArg._ArgString;
		ReadFromFile(FileDirDxf, OT_ComponentBase/*InArg._ArgUint32*/);
		// 正视图例路径
		FacadeDirDxf = FPaths::GetPath(FileDirDxf) + TEXT("/") + FPaths::GetBaseFilename(FileDirDxf) + TEXT("_Front.dxf");

		//获取构件关联的pak路径
		FileDirPAK = InArg._ArgUint8 == 1 ? TEXT("DBJCache/Common/PointPosition/") : TEXT("");
		FileDirPAK = FPaths::ProjectContentDir() + FileDirPAK + InArg._ArgString;
		FileDirPAK = FPaths::SetExtension(FileDirPAK, TEXT(".pak"));

		if (SelectedComponent.IsValid())
		{
			//SelectedComponent->SetState(OS_Selected);
			if (SelectedComponent->GetType() == OT_CurtainBox)
			{
				SelectedComponent->SetBeamWidth(10);
				SelectedComponent->SetThickness(15);
			}

			SelectedComponent->GetFurniturePro()->SetComponentPath(FileDirDxf);
			// 添加正视图图例
			TSharedPtr<FArmyFurniture> FrontFurniture = FArmyToolsModule::GetFurnitureByDXF(FacadeDirDxf);
			if (FrontFurniture.IsValid())
			{
				SelectedComponent->GetFurniturePro()->AddItemLegend(E_Dxf_Front, FrontFurniture);
			}
			
			switch (InArg._ArgUint32)
			{
			case EC_Drain_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_ROOMSPACE);
				SelectedComponent->GetFurniturePro()->SetRadius(11);
				SelectedComponent->GetFurniturePro()->SetHeight(FArmySceneData::WallHeight);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(false);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Drain_Point);
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_WaterPipe);
				SelectedComponent->BelongClass = Class_WaterPipe;
				SelectedComponent->ComponentType = EC_Drain_Point;
				break;
			case EC_Switch1O2_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetRadius(11);
				SelectedComponent->GetFurniturePro()->SetHeight(FArmySceneData::WallHeight);
				SelectedComponent->ComponentType = EC_Switch1O2_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_Switch);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Switch1O2_Point);
				SelectedComponent->BelongClass = Class_Switch;
				break;
			case EC_Closestool:
				SelectedComponent->SetPlaceLocationFlag(EPLF_ROOMSPACE);
				SelectedComponent->GetFurniturePro()->SetRadius(11);
				SelectedComponent->GetFurniturePro()->SetHeight(0/*20*/);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->ComponentType = EC_Closestool;
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Closestool);
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_ClosestoolRainPoint);
				SelectedComponent->BelongClass = Class_ClosestoolRainPoint;
				break;
			/*case EC_CurtainBox:
				SelectedComponent->SetPlaceLocationFlag(EPLF_ROOMSPACE);
				break;*/
			case EC_EleBoxH_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetLength(30.5);
				SelectedComponent->GetFurniturePro()->SetWidth(9);
				SelectedComponent->GetFurniturePro()->SetHeight(23);
				SelectedComponent->GetFurniturePro()->SetAltitude(180);
				SelectedComponent->ComponentType = EC_EleBoxH_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_ElectricBoxH);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_EleBoxH_Point);
				SelectedComponent->BelongClass = Class_ElectricBoxH;
				break;
			case EC_Gas_Meter:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetLength(21.7);
				SelectedComponent->GetFurniturePro()->SetWidth(16.3);
				SelectedComponent->GetFurniturePro()->SetHeight(26.1);
				SelectedComponent->GetFurniturePro()->SetAltitude(180);
				SelectedComponent->ComponentType = EC_Gas_Meter;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_GasMeter);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Gas_Meter);
				SelectedComponent->BelongClass = Class_GasMeter;
				break;
			case EC_Gas_MainPipe:
				SelectedComponent->SetPlaceLocationFlag(EPLF_ROOMSPACE);
				SelectedComponent->GetFurniturePro()->SetRadius(7);
				SelectedComponent->GetFurniturePro()->SetHeight(FArmySceneData::WallHeight);
				SelectedComponent->GetFurniturePro()->SetAltitude(0);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(false);
				SelectedComponent->ComponentType = EC_Gas_MainPipe;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_GasPipe);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Gas_MainPipe);
				SelectedComponent->BelongClass = Class_GasPipe;
				break;
			case EC_Water_Chilled_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetRadius(2);
				SelectedComponent->GetFurniturePro()->SetAltitude(30);
				SelectedComponent->ComponentType = EC_Water_Chilled_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_ColdWaterPoint);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Water_Chilled_Point);
				SelectedComponent->BelongClass = Class_ColdWaterPoint;
				break;
			case EC_Water_Normal_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetRadius(2);
				SelectedComponent->GetFurniturePro()->SetAltitude(30);
				SelectedComponent->ComponentType = EC_Water_Normal_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_RecycledWaterPoint);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Water_Normal_Point);
				SelectedComponent->BelongClass = Class_RecycledWaterPoint;
				break;
			case EC_Water_Hot_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetRadius(2);
				SelectedComponent->GetFurniturePro()->SetAltitude(30);
				SelectedComponent->ComponentType = EC_Water_Hot_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_HotWaterPoint);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Water_Hot_Point);
				SelectedComponent->BelongClass = Class_HotWaterPoint;
				break;
			case EC_Water_Separator_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetLength(96.2);
				SelectedComponent->GetFurniturePro()->SetWidth(17);
				SelectedComponent->GetFurniturePro()->SetHeight(61);
				SelectedComponent->GetFurniturePro()->SetAltitude(0);
				SelectedComponent->ComponentType = EC_Water_Separator_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_DiversityWater);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Water_Separator_Point);
				SelectedComponent->BelongClass = Class_DiversityWater;
				break;
			case EC_Basin:
				SelectedComponent->SetPlaceLocationFlag(EPLF_ROOMSPACE);
				// @常远 地漏管径由75mm改为50mm
				//SelectedComponent->GetFurniturePro()->SetRadius(7.5); 
				SelectedComponent->GetFurniturePro()->SetRadius(5.0f);
				SelectedComponent->GetFurniturePro()->SetHeight(0);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->ComponentType = EC_Basin;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_FloordRainPoint);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Basin);
				SelectedComponent->BelongClass = Class_FloordRainPoint;
				break;
			case EC_EleBoxL_Point:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetLength(30.5);
				SelectedComponent->GetFurniturePro()->SetWidth(0.7);
				SelectedComponent->GetFurniturePro()->SetHeight(23);
				SelectedComponent->GetFurniturePro()->SetAltitude(30);
				SelectedComponent->ComponentType = EC_EleBoxL_Point;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_ElectricBoxL);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_EleBoxL_Point);
				SelectedComponent->BelongClass = Class_ElectricBoxL;
				break;
			case EC_Water_Supply:
				SelectedComponent->SetPlaceLocationFlag(EPLF_INNERNALWALLLINE);
				SelectedComponent->GetFurniturePro()->SetRadius(2.5);
				SelectedComponent->GetFurniturePro()->SetAltitude(30);
				SelectedComponent->ComponentType = EC_Water_Supply;
				SelectedComponent->GetFurniturePro()->SetBelongClass(ObjectClassType::Class_HomeEntryWaterPoint);
				SelectedComponent->GetFurniturePro()->SetbIsPakModel(true);
				SelectedComponent->GetFurniturePro()->SetObjectType(OT_Water_Supply);
				SelectedComponent->BelongClass = Class_HomeEntryWaterPoint;
				break;
			default:
				break;
			}
			SelectedComponent->SetPropertyFlag(FArmyObject::FLAG_ORIGINALPOINT, true);
			if(FPaths::FileExists(FileDirPAK))
			{
				SelectedComponent->GetFurniturePro()->SetModelPath(FileDirPAK);

				//设置材质参数
				FString ParamPath = FileDirPAK;
				ParamPath = FPaths::SetExtension(ParamPath, TEXT(".json"));
				if (FPaths::FileExists(ParamPath))
				{
					SelectedComponent->GetFurniturePro()->SetOptimizeParam(ParamPath);
				}
			}

			// 获取点位dxf生成CAD的块文件
			//FArmyAutoCad::Get()->AddComponent(InArg._ArgUint32, FPaths::ProjectContentDir() + FileDirDxf + InArg._ArgString);
			FArmyAutoCad::Get()->AddComponent(SelectedComponent->ComponentType, SelectedComponent);
		}
		break;
	case 1://编辑修改
		IsModifying = true;
		bCreatFurniture = false;
		break;
	default:
		break;
	}
}

void FArmyComponentOperation::EndOperation()
{
	/*SelectedComponent->SetIsRotating(false);*/
	Reset();
	FArmyOperation::EndOperation();
}

void FArmyComponentOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	SelectedComponent = StaticCastSharedPtr<FArmyFurniture>(Object);
	if (SelectedComponent.IsValid() && InOperationPrimitive.IsValid())
	{
		SelectOperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);;
		//IsSelectCaptruePoint = SelectOperationPoint->GetPropertyFlag(FArmyObject::FLAG_POINTCAPTUREABLE);
		//SelectOperationPoint->SetPropertyFlag(FArmyObject::FLAG_POINTCAPTUREABLE, false);
	}

}

void FArmyComponentOperation::ReadFromFile(const FString& InFilePath, uint32 InObjType)
{
	std::string FilePath(TCHAR_TO_UTF8(*InFilePath));
	dxfFile DxfFile(FilePath);
	TSharedPtr<FArmyDxfModel> DxfModel = NULL;
	if (DxfFile.parseFile())
	{
		DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
		TArray<TSharedPtr<FArmyFurniture>> TempObjs = DxfModel->GetAllLayers();
		if (TempObjs.Num() > 0)
		{
			SelectedComponent = TempObjs[0];
			SelectedComponent->SetType(EObjectType(InObjType));
			SelectedComponent->SetName(OperationArg._ArgFName.ToString());
			SelectOperationPoint = SelectedComponent->GetBaseEditPoint();
			switch (InObjType)
			{
			case OT_Switch3O2_Point://三联双控开关
			case OT_Switch3O1_Point://三联单控开关
			case OT_Switch2O2_Point://双联双控开关
			case OT_Switch2O1_Point://双联单控开关
			case OT_Switch1O2_Point://单连双控开关
			case OT_Switch1O1_Point://单连单控开关
			case OT_Socket_Five_Point:
			case OT_Socket_Five_On_Point:
			case OT_Socket_TP:
			case OT_Socket_TV:
				SelectedComponent->SetAltitude(135);//默认开关1350毫米的高度
				break;
			default:
				break;
			}
		}
	}
}

bool FArmyComponentOperation::CaptureClosestPoint(FVector InputPos, FVector & OutCapturePos, TSharedPtr<FArmyLine> & OutLine, TSharedPtr<FArmyRoom> & OutRoom)
{

	TArray<TWeakPtr<FArmyObject>> WallList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, WallList);
	FVector MinPos;
	float MinDis = FLT_MAX;
	TSharedPtr<FArmyLine> MinLine;
	TSharedPtr<FArmyRoom> MinRoom;

	for (auto It : WallList)
	{
		TSharedPtr<FArmyRoom> Wall = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TSharedPtr<FArmyRoom> RelatedWall;;
		if (Wall.IsValid())
		{
			if (SelectedComponent.IsValid() && SelectedComponent->GetbCreateOperation() == false)
			{
				RelatedWall = SelectedComponent->GetRelateRoom().Pin();
				if (RelatedWall.IsValid())
				{
					Wall = RelatedWall;
				}
			}

			//计算最近的点
			TArray<TSharedPtr<FArmyLine> > Lines;
			Wall->GetLines(Lines);
			for (auto & L : Lines)
			{
				float dis = FArmyMath::CalcPointToLineDistance(InputPos, L->GetStart(), L->GetEnd());
				if (dis < MinDis && FArmyMath::IsPointProjectionOnLineSegment2D(FVector2D(InputPos), FVector2D(L->GetStart()), FVector2D(L->GetEnd())))
				{
					MinPos = FArmyMath::GetProjectionPoint(InputPos, L->GetStart(), L->GetEnd());
					MinLine = L;
					MinDis = dis;
					MinRoom = Wall;
				}
			}

			//如果有关联房间，仅计算关联房间
			if (RelatedWall.IsValid())
			{
				break;
			}
		}
	}

	if (MinDis < 100 && MinPos.X > -1000000 && MinPos.X < 1000000 && MinPos.Y > -1000000 && MinPos.Y < 1000000)
	{
		OutCapturePos = MinPos;
		OutLine = MinLine;
		OutRoom = MinRoom;
		return true;
	}
	else
	{
		OutCapturePos = InputPos;
		return false;
	}
	return false;
}

bool FArmyComponentOperation::CalPositionIllegal(const FVector & CurrentPos)
{
	TArray<TWeakPtr<FArmyObject>> WallList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, WallList);
	FVector ComPos = SelectedComponent->GetBaseEditPoint()->GetPos();
	bool bInRoom = false;
	for (auto It : WallList)
	{
		TSharedPtr<FArmyRoom> WallList = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (WallList.IsValid() && WallList->IsPointInRoom(ComPos))
		{
			if (SelectedComponent->GetPlaceLocationFlag() == EPLF_INNERNALWALLLINE)
			{
				TArray<TSharedPtr<FArmyLine> > Lines;
				WallList->GetLines(Lines);
				for (auto L : Lines)
				{
					if (L.IsValid() && FArmyMath::IsPointOnLine(ComPos, L->GetStart(), L->GetEnd()))
						//if (L.IsValid() && FArmyMath::CalcPointToLineDistance(MouseWorldPos, L->GetStart(), L->GetEnd()) < 0.5)
					{
						//开关等部分点位需要放置在墙线上
						bInRoom = true;
						break;
					}
					else
					{
						bInRoom = false;
					}
				}
			}
			else
			{
				const FBox ComBox = SelectedComponent->GetBounds();
				FVector BoxSize = ComBox.GetSize();
				FVector LeftTop(CurrentPos.X - BoxSize.X / 2, CurrentPos.Y - BoxSize.Y / 2, 0);
				FVector RightTop(CurrentPos.X + BoxSize.X / 2, CurrentPos.Y - BoxSize.Y / 2, 0);
				FVector RightBottom(CurrentPos.X + BoxSize.X / 2, CurrentPos.Y + BoxSize.Y / 2, 0);
				FVector LeftBottom(CurrentPos.X - BoxSize.X / 2, CurrentPos.Y + BoxSize.Y / 2, 0);

				//放置在空间的点位，外包几何不能超出所在房间
				if (SelectedComponent->GetPlaceLocationFlag() == EPLF_ROOMSPACE
					&& WallList->IsPointInRoom(LeftTop)
					&& WallList->IsPointInRoom(RightTop)
					&& WallList->IsPointInRoom(RightBottom)
					&& WallList->IsPointInRoom(LeftBottom))
				{
					bInRoom = true;
					break;
				}
				else
				{
					bInRoom = false;
				}
			}
		}
	}
	return bInRoom;
}

bool FArmyComponentOperation::PositionSelfAdaption(const FVector & WorldPos, const TSharedPtr<FArmyLine> & CaptureLine, const TSharedPtr<FArmyRoom>& CaptureRoom)
{
	//@ 点位旋转相关
	//TSharedPtr<FArmyLine> CaptureLine = CaptureLine/*FArmyToolsModule::Get().GetMouseCaptureTool()->GetCaptureLine()*/;
	if (CaptureLine.IsValid())
	{
		FVector2D TempLine(1, 0);
		FVector2D MouseLine(1, 0);
		//FVector MousePos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(),0);
		if (CaptureLine->GetEnd().Y > CaptureLine->GetStart().Y)
		{
			TempLine = FVector2D(CaptureLine->GetEnd() - CaptureLine->GetStart());
			MouseLine = FVector2D(WorldPos - CaptureLine->GetStart());
		}
		else if (CaptureLine->GetEnd().Y < CaptureLine->GetStart().Y)
		{
			TempLine = FVector2D(CaptureLine->GetStart() - CaptureLine->GetEnd());
			MouseLine = FVector2D(WorldPos - CaptureLine->GetEnd());
		}
		else if (CaptureLine->GetEnd().X > CaptureLine->GetStart().X)
		{
			MouseLine = FVector2D(WorldPos - CaptureLine->GetStart());
		}
		else
		{
			MouseLine = FVector2D(WorldPos - CaptureLine->GetEnd());
		}

		//确保构件在所在房间内
		FVector2D Normal = MouseLine.GetSafeNormal();
		FVector2D NormalDir = Normal.GetRotated(90);
		float AngleAdjust = 0;
		if (!CaptureRoom->IsPointInRoom(WorldPos + FVector(NormalDir * 5, 0)))
		{
			NormalDir *= -1;
			AngleAdjust = 180;
		}


		float flag = FVector2D(1, 0).operator^(TempLine.GetSafeNormal());
		flag = flag < 0 ? -1 : 1;

		float TempAngle = FVector2D::DotProduct(FVector2D(1, 0), TempLine.GetSafeNormal());
		TempAngle = FMath::Acos(TempAngle)*flag;
		TempAngle *= (180.f / PI);


		float mflag = TempLine.operator^(MouseLine.GetSafeNormal());
		mflag = mflag > 0 ? -1 : 1;
		//if (mflag < 0)
		{
			//TempAngle > 0 ? TempAngle -= 180 : TempAngle += 180;
			FRotator Rot(0, TempAngle + AngleAdjust, 0);
			SelectedComponent->SetRotation(Rot);
		}
		//else
		{
			//SelectedComponent->SetRotation(FRotator(0, 0, 0));
		}
		SelectedComponent->SetPosition(WorldPos);

	}
	return true;
}
