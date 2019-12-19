#include "ArmySceneData.h"
#include "ArmyEditorEngine.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyReferenceImage.h"
#include "ArmyHardware.h"
#include "ArmyRoom.h"
#include "ArmyMath.h"
#include "CSGInterface.h"
#include "ArmyRectBayWindow.h"
#include "ArmyFurniture.h"
#include "ArmyModifyWall.h"
#include "ArmyPass.h"
#include "ArmySimpleComponent.h"
#include "ArmyPackPipe.h"
#include "ArmyWallLine.h"
#include "ArmyPunch.h"
#include "ArmyBaseEditStyle.h"
#include "ArmyWallActor.h"
#include "ArmyEngineModule.h"
#include "../../HardModeData/Mathematics/BspPolygon2d.h"
#include "ArmySceneData/Public/Data/HardModeData/Mathematics/IntersectSegment2dBox2d.h"
#include "ArmySceneData/Private/Data/HardModeData/Mathematics/EarcutTesselator.h"
#include "ArmyGameInstance.h"
#include "ArmySplitLine.h"
#include "ArmyRegion.h"
#include "EngineUtils.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "ArmyActorConstant.h"
#include "ArmyActorConstant.h"
#include "ArmyUser.h"
#include "ArmyExtrusionActor.h"
#include "ArmyAutoCad.h"
#include "SArmyMulitCategory.h"
#include "ArmyHydropowerModeController.h"
#include "ArmyConstructionManager.h"
//@打扮家 XRLightmass
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyHardModeController.h"
#include "ArmyConstructionManager.h"
#include "ArmyAirLouver.h"

DEFINE_LOG_CATEGORY(LogHomeData);

float FArmySceneData::WallHeight = 280.f;
float FArmySceneData::OutWallThickness = 240.f;
float FArmySceneData::MaxWallCaptureThickness = 80.f;

float FArmySceneData::FinishWallThick = 0.01f;

FSimpleMulticastDelegate FArmySceneData::HomeMode_ModifyMultiDelegate;
FSimpleMulticastDelegate FArmySceneData::ModifyMode_ModifyMultiDelegate;
FSimpleMulticastDelegate FArmySceneData::LayOutMode_ModifyMultiDelegate;

void FArmySceneData::PushModifyDataMap(TSharedPtr<class FArmyObject> InObject, uint8 InModifyTpye)
{
	FModifyConnectedData ConnectedData;
	if (InModifyTpye == 0)
		ConnectedData.ModifyType = MT_Create;
	else if (InModifyTpye == 1)
		ConnectedData.ModifyType = MT_Delete;
	else if (InModifyTpye == 2)
		ConnectedData.ModifyType = MT_Modify;

	TArray<FString> CurrentRoomIdList;
	/*FArmyHardware* HardwareObj = InObject->AsassignObj<FArmyHardware>();
	FArmySimpleComponent* SimpleComponentObj = InObject->AsassignObj<FArmySimpleComponent>();
	FArmyWallLine* WallLineObj = InObject->AsassignObj<FArmyWallLine>();
	FArmyRoom* RoomObj = InObject->AsassignObj<FArmyRoom>();
	if (HardwareObj)
	{
		CurrentRoomIdList.Push(HardwareObj->FirstRelatedRoom->GetUniqueID().ToString());
		CurrentRoomIdList.Push(HardwareObj->SecondRelatedRoom->GetUniqueID().ToString());
	}
	else if (SimpleComponentObj)
	{
		CurrentRoomIdList.Push(SimpleComponentObj->GetRelatedRoom().Pin()->GetUniqueID().ToString());
	}
	else if (WallLineObj)
	{
		CurrentRoomIdList.Push(WallLineObj->GetRelatedRoom().Pin()->GetUniqueID().ToString());
	}
	else if (RoomObj)
	{

	}*/
	//if (HardwareObj)
	//{
	//	CurrentRoomIdList.Push(HardwareObj->FirstRelatedRoom->GetUniqueID().ToString());
	//	CurrentRoomIdList.Push(HardwareObj->SecondRelatedRoom->GetUniqueID().ToString());
	//}
	//else if (SimpleComponentObj)
	//{
	//	CurrentRoomIdList.Push(SimpleComponentObj->GetRelatedRoom().Pin()->GetUniqueID().ToString());
	//}
	//else if (WallLineObj)
	//{
	//	CurrentRoomIdList.Push(WallLineObj->GetRelatedRoom().Pin()->GetUniqueID().ToString());
	//}
	//else if (RoomObj)
	//{

	//}

	ConnectedData.ConnectRoomIdList = CurrentRoomIdList;
	ModifyDataMap.Add(InObject->GetUniqueID().ToString(), ConnectedData);
}

void FArmySceneData::PushModifyDataMapFromGUID(FGuid InID, uint8 InModifyTpye)
{
	FModifyConnectedData ConnectedData;
	if (InModifyTpye == 0)
		ConnectedData.ModifyType = MT_Create;
	else if (InModifyTpye == 1)
		ConnectedData.ModifyType = MT_Delete;
	else if (InModifyTpye == 2)
		ConnectedData.ModifyType = MT_Modify;

	ModifyDataMap.Add(InID.ToString(), ConnectedData);
}

void FArmySceneData::ClearModifyMap()
{
	ModifyDataMap.Empty();
}

bool FArmySceneData::CreateSpaceNameToRoomRelated(EModelType InDesData)
{
	TArray<TWeakPtr<FArmyObject>> InnerRoomObjs;
	TArray<TWeakPtr<FArmyObject>> TextLabelObjs;
	FArmySceneData::Get()->GetObjects(InDesData, OT_InternalRoom, InnerRoomObjs);
	FArmySceneData::Get()->GetObjects(InDesData, OT_TextLabel, TextLabelObjs);

	for (auto & RoomIt : InnerRoomObjs)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomIt.Pin());
		for (auto & TxtIt : TextLabelObjs)
		{
			TSharedPtr<FArmyTextLabel> TxtLabel = StaticCastSharedPtr<FArmyTextLabel>(TxtIt.Pin());
			if (Room.IsValid() && TxtLabel.IsValid()
				&& TxtLabel->GetLabelType() == FArmyTextLabel::LT_SpaceName
				&& Room->IsPointInRoom(TxtLabel->GetWorldPosition()))
			{
				Room->SetSpaceNameLabel(TxtLabel.ToSharedRef());
				TxtLabel->SetObjTag(Room, -1);
				break;
			}
		}
	}
	return true;
}

bool FArmySceneData::GenerateNewRoomSpaceNameLabel(TSharedPtr<FArmyRoom> InRoom, EModelType InModelType)
{
	if (InRoom.IsValid())
	{
        InRoom->GenerateSpaceNameLabel(InModelType);
	}
	return true;
}

TMap<int32, FObjectWeakPtr>  FArmySceneData::GetSortDataFromTArrayToTMap(TArray<FObjectWeakPtr> & InRoomArr)
{
	TMap<int32, FObjectWeakPtr > RoomMap;
	for (auto & RoomIt : InRoomArr)
	{
		TSharedPtr<FArmyRoom> RoomTemp = StaticCastSharedPtr<FArmyRoom>(RoomIt.Pin());
		if (RoomTemp.IsValid() && RoomTemp->GetSpaceNameLabel().IsValid() && !RoomTemp->GetSpaceName().Equals(TEXT("未命名")))
		{
			FString SpaceName = RoomTemp->GetSpaceName();
			int32 RoomIdex = SpaceName.Right(1).IsNumeric() ? FCString::Atoi(*SpaceName.Right(1)) : 0;

			RoomMap.Add(RoomIdex, RoomTemp);
		}
	}

	//根据房间序号对房间进行排序
	RoomMap.KeySort(TLess<int32>());

	return RoomMap;
}

void FArmySceneData::CalculateSimpleComponent(TSharedPtr<FArmyObject> InSimpleComponent, const FString &InSuffix, const TArray<FVector> &InOverridedBoundaryVertices)
{
	TArray<FVector> BoundaryVerts;
	if (InOverridedBoundaryVertices.Num() > 0)
		BoundaryVerts = InOverridedBoundaryVertices;
	else
		InSimpleComponent->GetVertexes(BoundaryVerts);
	float tempheight = 0.0f;
	EObjectType ObjType = InSimpleComponent->GetType();
	switch (ObjType)
	{
		case OT_Beam:
		case OT_AirFlue:
		case OT_Pillar:
		{
			FArmySimpleComponent *tempBeam = InSimpleComponent->AsassignObj<FArmySimpleComponent>();
			tempheight = tempBeam->GetHeight();	
			break;
		}
		case OT_PackPipe:
		{
			FArmyPackPipe* tempPipe = InSimpleComponent->AsassignObj<FArmyPackPipe>();
			tempheight = tempPipe->GetHeight();
			break;
		}
	}

	TArray<FVector2D> NewBoundarys;
	for (const FVector& TempIter : BoundaryVerts)
	{
		NewBoundarys.Emplace(FVector2D(TempIter));
	}
	TArray<FVector2D> CleanPoly;
	FArmyMath::CleanPolygon(NewBoundarys, CleanPoly, 0.05f);
	if (FArmyMath::IsClockWise(CleanPoly))
	{
		FArmyMath::ReverPointList(CleanPoly);
	}

	// 创建烟道，柱子，房梁的侧面
	int32 number = CleanPoly.Num();
	TArray<FVector> ExturdeVerts;
	ExturdeVerts.AddUninitialized(number);
	for (int32 i = 0; i < number; ++i)
	{
		ExturdeVerts[i] = FVector(CleanPoly[i], 0.0f);
	}
	TArray<FVector> NewOutLines = FArmyMath::ExturdePolygon(ExturdeVerts, FinishWallThick, false);
	int32 index = 0;
	FString ComponentGuidStr = InSimpleComponent->GetUniqueID().ToString();
	
	FString name = TEXT("NONE");
	if (ObjType == OT_Beam)
	{
		name = TEXT("Beam");
	}
	else if (ObjType == OT_Pillar)
	{
		name = TEXT("Pillar");
	}
	else if (ObjType == OT_AirFlue)
	{
		name = TEXT("AirFlue");
	}
	else if (ObjType == OT_PackPipe)
	{
		name = TEXT("PackPipe");
	}

	
	for (int32 i = 0; i < number; ++i)
	{
		const FVector2D& EdgeStartPos = CleanPoly[i%number];
		const FVector2D& EdgeEndPos = CleanPoly[(i + 1) % number];

		const FVector& extrudeStartPos = NewOutLines[i%number];
		const FVector& extrudeEndPos = NewOutLines[(i + 1) % number];
		// 判断边是否和房间上，在房间上的边不生成面片
		// 此处先略去

		if (true)
		{
			const FVector2D& dir0 = (EdgeEndPos - EdgeStartPos).GetSafeNormal();
			const FVector& FaceNormal = FVector(dir0, 0.0).RotateAngleAxis(-90, FVector::UpVector);
			FVector StartBottom, StartTop, EndTop, EndBottom;
			FVector NewStartBottom, NewStartTop, NewEndTop, NewEndBottom;
			if (ObjType == OT_Beam)
			{
				StartBottom = FVector(EdgeStartPos, WallHeight - tempheight);
				StartTop = FVector(EdgeStartPos, WallHeight);
				EndTop = FVector(EdgeEndPos, WallHeight);
				EndBottom = FVector(EdgeEndPos, WallHeight - tempheight);

				NewStartBottom = extrudeStartPos + (WallHeight - tempheight - FinishWallThick) * FVector::UpVector;
				NewStartTop = extrudeStartPos + FVector(0, 0, 1) * WallHeight;
				NewEndTop = extrudeEndPos + FVector(0, 0, 1)*WallHeight;
				NewEndBottom = extrudeEndPos + (WallHeight - tempheight - FinishWallThick) * FVector::UpVector;

			}
			else if (ObjType == OT_Pillar || ObjType == OT_AirFlue || ObjType == OT_PackPipe)
			{


				StartBottom = FVector(EdgeStartPos, 0);
				StartTop = FVector(EdgeStartPos, WallHeight);
				EndTop = FVector(EdgeEndPos, WallHeight);
				EndBottom = FVector(EdgeEndPos, 0);

				NewStartBottom = extrudeStartPos;
				NewStartTop = extrudeStartPos + FVector(0, 0, 1)*WallHeight;
				NewEndTop = extrudeEndPos + FVector(0, 0, 1)*WallHeight;
				NewEndBottom = extrudeEndPos;

				//@郭子阳
				// 判断边是否在房间的墙线上，在房间上的边不生成面片
				//获取当前房间
				TArray<TWeakPtr<FArmyObject>> AllInnerRooms;
				GetObjects(E_LayoutModel, OT_InternalRoom, AllInnerRooms);
				
				bool OverlapWithWallLine = false;
				for(TWeakPtr<FArmyObject> CurrentRoom: AllInnerRooms)
				{
					TArray<TSharedPtr<FArmyLine>> WallLines;
					CurrentRoom.Pin()->GetLines(WallLines, true);
					for (auto& Line : WallLines)
					{
						FVector V0 = Line->GetStart();
						FVector V1= Line->GetEnd();
						V0.Z = 0;
						V1.Z = 0;
					
					
						if (FMath::PointDistToSegment(StartBottom, V0, V1)<1 && FMath::PointDistToSegment(EndBottom, V0, V1)< 1)
						{
							OverlapWithWallLine = true;
						}
					
					}
				
				}
				//不生成与墙面重叠的面
				if (OverlapWithWallLine)
				{
					auto UniqueIdForRoomOrHardware=InSuffix.Len() ? FString::Printf(TEXT("%s-%s"), *ComponentGuidStr, *InSuffix) : ComponentGuidStr;
					auto UniqueId = InSuffix.Len() ? FString::Printf(TEXT("%s-%s-%sWallInfo%d"), *ComponentGuidStr, *InSuffix, *name, index) : FString::Printf(TEXT("%s-%sWallInfo%d"), *ComponentGuidStr, *name, index);
					TSharedPtr<FArmyRoomSpaceArea> surface = GetWallByRoomIdAndAttachWallLineId(UniqueIdForRoomOrHardware, UniqueId);
					FArmySceneData::Get()->Delete(surface, false);
					index++;
					continue;
				}

			}

			// FString IndexName = FString::Printf(TEXT("%d"), index);
			TArray<FVector> Vertices = { StartBottom,StartTop,EndTop,EndBottom };
			TArray<FVector> FinishVerts = { NewStartBottom,NewStartTop,NewEndTop,NewEndBottom };
			FBSPWallInfo OrignalVertInfo, FinishVertInfo;
			OrignalVertInfo.GenerateFromObjectType = FinishVertInfo.GenerateFromObjectType = ObjType;
			OrignalVertInfo.PolyVertices.Add(Vertices);
			FinishVertInfo.PolyVertices.Add(FinishVerts);
			OrignalVertInfo.Normal = FinishVertInfo.Normal = FaceNormal;
			OrignalVertInfo.UniqueIdForRoomOrHardware = FinishVertInfo.UniqueIdForRoomOrHardware = InSuffix.Len()? FString::Printf(TEXT("%s-%s"), *ComponentGuidStr, *InSuffix): ComponentGuidStr;
			OrignalVertInfo.UniqueId = FinishVertInfo.UniqueId = InSuffix.Len() ? FString::Printf(TEXT("%s-%s-%sWallInfo%d"), *ComponentGuidStr, *InSuffix, *name, index) : FString::Printf(TEXT("%s-%sWallInfo%d"),*ComponentGuidStr, *name, index);
			int flag = ObjType == OT_Beam ? 2 : 1;
			CaculateSurfaceInfoWhithWallInfo(FinishVertInfo, OrignalVertInfo, flag);
			index++;
		}
	}

	// 创建房梁的底面
	if (ObjType == OT_Beam)
	{
		TArray<FVector> bottomVerts;
		for (const FVector2D& TempIter : CleanPoly)
		{
			bottomVerts.Push(FVector(TempIter, WallHeight - tempheight));
		}
		TArray<FVector> FinishBottomVerts;
		for (const FVector& TempIter : NewOutLines)
		{
			FinishBottomVerts.Push(TempIter + (WallHeight - tempheight - FinishWallThick) * FVector::UpVector);
		}
		FBSPWallInfo BeamBottom, FinishBeamBottomInfo;
		BeamBottom.GenerateFromObjectType = FinishBeamBottomInfo.GenerateFromObjectType = ObjType;
		BeamBottom.PolyVertices.Push(bottomVerts);
		FinishBeamBottomInfo.PolyVertices.Push(FinishBottomVerts);
		BeamBottom.Normal = FinishBeamBottomInfo.Normal = -FVector::UpVector;
		BeamBottom.UniqueIdForRoomOrHardware = FinishBeamBottomInfo.UniqueIdForRoomOrHardware = FString::Printf(TEXT("%s-%s"), *ComponentGuidStr, *InSuffix);;
		BeamBottom.UniqueId = FinishBeamBottomInfo.UniqueId = FString::Printf(TEXT("%s-%s-BeamBottom"), *ComponentGuidStr, *InSuffix); // ComponentGuidStr + TEXT("BeamBottom");
		CaculateSurfaceInfoWhithWallInfo(FinishBeamBottomInfo, BeamBottom, 2);
	}
}

const TSharedRef<FArmySceneData>& FArmySceneData::Get()
{
	static const TSharedRef<FArmySceneData> Instance = MakeShareable(new FArmySceneData);
	return Instance;
}

const TSharedRef<FArmySceneData>& FArmySceneData::GetLayOutData()
{
	static const TSharedRef<FArmySceneData> Instance = MakeShareable(new FArmySceneData);
	return Instance;
}

void FArmySceneData::CopyModeData(EModelType InSrcData, EModelType InDesData, bool bCopyGUID)
{
	TArray<TWeakPtr<FArmyObject>> HomeModeObjects;

	if (InDesData == E_ModifyModel)
	{
		HomeModeObjects = FArmySceneData::Get()->GetObjects(E_ModifyModel);
	}
	else if (InDesData == E_LayoutModel)
	{
		HomeModeObjects = FArmySceneData::Get()->GetObjects(E_LayoutModel);
	}

	//@郭子阳
	auto SourceObjs=FArmySceneData::Get()->GetObjects(InSrcData);
	//不发生拷贝的ID
	TArray<FGuid> CantCopy; 

	for (auto ItObject : HomeModeObjects)
	{
		// @欧石楠 底图不走删除拷贝逻辑
		if (ItObject.IsValid() && ItObject.Pin()->GetType() == OT_ReferenceImage)
		{
			continue;
		}

		if (InDesData == E_ModifyModel)
		{
			if (ItObject.IsValid() &&
				(ItObject.Pin()->GetType() != OT_AddWall &&
					ItObject.Pin()->GetType() != OT_ModifyWall &&
					ItObject.Pin()->GetType() != OT_IndependentWall &&
					ItObject.Pin()->GetType() != OT_PackPipe &&
					ItObject.Pin()->GetType() != OT_NewPass &&
					ItObject.Pin()->GetType() != OT_SlidingDoor &&
					ItObject.Pin()->GetType() != OT_Door /*&&
					ItObject.Pin()->GetType() != OT_DoorHole*/)
				/*
				(ItObject.Pin()->GetType() == OT_OutRoom ||
					ItObject.Pin()->GetType() == OT_InternalRoom ||
					ItObject.Pin()->GetType() == OT_Window ||
					ItObject.Pin()->GetType() == OT_FloorWindow ||
					ItObject.Pin()->GetType() == OT_RectBayWindow ||
					ItObject.Pin()->GetType() == OT_TrapeBayWindow ||
					ItObject.Pin()->GetType() == OT_SecurityDoor ||
					ItObject.Pin()->GetType() == OT_Pillar ||
					ItObject.Pin()->GetType() == OT_Beam ||
					ItObject.Pin()->GetType() == OT_AirFlue)*/)
			{
				FArmySceneData::Get()->Delete(ItObject.Pin(), bCopyGUID ? true : false);
			}
		}
		else if (InDesData == E_LayoutModel)
		{
			//@郭子阳 不可以删除
			bool CantDelete =false;


			if (ItObject.IsValid())
			{
				//ItObject.Pin()->AsassignObj<FArmyFurniture>()
				auto furniture = ItObject.Pin()->AsassignObj<FArmyFurniture>();
				if (furniture)
				{
					if (!furniture->IsOrignalPoint())
					{
						//新增点位不删除 ,替换点位根据原始点位情况在下方删除
						CantDelete = true;
					}else if (furniture->IsOrignalPoint() && furniture->IsBPreforming())
					{
						bool Found = false;
						for (auto obj : SourceObjs)
						{
							if (obj.Pin()->ObjID != furniture->ObjID)
							{
								continue;
							}

							Found = true;
							//发生断点改造的原始点位也不删除
							CantDelete = true;
							CantCopy.Add(ItObject.Pin()->ObjID);
						

							//获取构件初始坐标
							FVector InLocation = furniture->LocalTransform.GetLocation();
							FRotator InRotator = FRotator(furniture->LocalTransform.GetRotation());
							FVector InScale = FVector::OneVector /*Fur->LocalTransform.GetScale3D()*/;

							//计算点位模型的下沿距离地面的高度，注意：在立面下离地高度非中心点距离地面的高度
							InLocation.Z = 0;
							InLocation.Z += furniture->GetFurniturePro()->GetAltitude();

							furniture->SetNativeLocation(InLocation);

							break;
						}

						if (!Found)
						{
							//删除替换点位
							auto OldSubstitute=furniture->Replace(nullptr);
							if (OldSubstitute.IsValid())
							{
								FArmySceneData::Get()->Delete(OldSubstitute.Pin(), bCopyGUID ? true : false);
							}
						}

					}
				}
			}

			if (ItObject.IsValid() && !CantDelete)
			{
				FArmySceneData::Get()->Delete(ItObject.Pin(), bCopyGUID ? true : false);
			}
		}
	}

	HomeModeObjects = FArmySceneData::Get()->GetObjects(InSrcData);

	for (auto ItObject : HomeModeObjects)
	{
		if (ItObject.Pin()->GetType() == OT_Pass || ItObject.Pin()->GetType() == OT_DoorHole)
		{
			TSharedPtr<FArmyPass> CurrentPass = StaticCastSharedPtr<FArmyPass>(ItObject.Pin());
			if (CurrentPass.IsValid() && CurrentPass->bModifyType)
			{
				FArmySceneData::Get()->PushModifyDataMapFromGUID(CurrentPass->GetUniqueID(), MT_Delete);
			}
		}

		//@郭子阳 一部分不拷贝
		if (CantCopy.Contains(ItObject.Pin()->ObjID))
		{
			continue;
		}

		TSharedPtr<FArmyObject> NewObject = ItObject.Pin()->CopySelf(bCopyGUID);

		// @欧石楠 不在RegisterClass表中的Class，不走下面的逻辑，底图也不走下面的逻辑
		if (NewObject.IsValid() && NewObject->GetType() != OT_ReferenceImage)
		{
			/**@欧石楠 如果是从原始户型往拆改模式拷贝数据，则对拷贝的数据加一个flag*/
			if (InDesData == E_ModifyModel)
			{
				NewObject->SetPropertyFlag(FArmyObject::FLAG_MODIFY, true);
			}
			else if (InDesData == E_LayoutModel)
			{
				NewObject->SetPropertyFlag(FArmyObject::FLAG_LAYOUT, true);
				NewObject->SetPropertyFlag(FArmyObject::FLAG_MODIFY, false);
			}
			FArmySceneData::Get()->Add(NewObject, XRArgument(1).ArgUint32(InDesData));
		}
	}

	CreateSpaceNameToRoomRelated(InDesData);

	if (InDesData == E_LayoutModel)
	{
		FArmySceneData::Get()->MergeRooms();
		//FArmySceneData::Get()->SplitSimpleComponentsAndRooms();
		//FArmySceneData::Get()->SplitPackPipeAndRooms();
		FArmySceneData::Get()->SplitRooms();

		TArray<FObjectWeakPtr> HomeModeRooms;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, HomeModeRooms);
		for (auto It : HomeModeRooms)
		{
			TSharedPtr<FArmyRoom> NewRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (NewRoom.IsValid())
			{
				NewRoom->bPreModified = false;
				
				
			}
		}
		//@郭子阳 房间SpaceID修改引起施工项变化
		TArray<FObjectWeakPtr> LayoutModeRooms;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, LayoutModeRooms);
		for (auto It : LayoutModeRooms)
		{
			TSharedPtr<FArmyRoom> NewRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (NewRoom.IsValid())
			{
				FArmySceneData::Get()->OnRoomAdded.ExecuteIfBound(NewRoom);
				NewRoom->OnSpaceIDChanged();
			}
		}


		// @欧石楠 因为新建墙和拆除墙会重新分割空间，需要在空间分割后为其打上PropertyFlag
		TArray<FObjectWeakPtr> NewRooms;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, NewRooms);
		for (auto It : NewRooms)
		{
			TSharedPtr<FArmyRoom> NewRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (NewRoom.IsValid())
			{
				NewRoom->SetPropertyFlag(FArmyObject::FLAG_LAYOUT, true);
			}
		}
	}

	//拷贝后重新计算Hardware关联的墙线,并自动更新适配门窗宽度
	TArray<TWeakPtr<FArmyObject>> HarewareList;
	FArmySceneData::Get()->GetHardWareObjects(HarewareList, InDesData);
	for (auto It : HarewareList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		if (Hardware.IsValid())
		{
			if (Hardware->CaptureDoubleLine(Hardware->GetPos(), InDesData))
			{
				Hardware->SetPos(Hardware->AutoPos);
				Hardware->SetWidth(Hardware->AutoThickness);
				//更新拆补门洞信息
				if (Hardware->GetType() == OT_Door)
				{
					TSharedPtr<FArmySingleDoor> TempHWDoor = StaticCastSharedPtr<FArmySingleDoor>(Hardware);
					TempHWDoor->UpdateDoorHole();
				}
				else if (Hardware->GetType() == OT_SlidingDoor)
				{
					TSharedPtr<FArmySlidingDoor> TempHWSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(Hardware);
					TempHWSlidingDoor->UpdateDoorHole();
				}
			}
		}

	}

	TArray<TWeakPtr<FArmyObject>> ModifyWallList;
	FArmySceneData::Get()->GetObjects(InDesData, OT_ModifyWall, ModifyWallList);
	for (auto It : ModifyWallList)
	{
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
		if (ModifyWall.IsValid())
		{
			ModifyWall->ResetCalPolygonVertices();
		}
	}
}

TSharedPtr<XRObjectModel> FArmySceneData::GetModel(EModelType InModelType)
{
	return ModelMap.FindRef(InModelType);;
}

void FArmySceneData::Add(FObjectPtr Object, XRArgument InArg, FObjectPtr InParent, bool bTransaction/* = false*/)
{
	// 记录Args
	Object->Args = InArg;

	if (InParent.IsValid())
	{
		InParent->AddChild(Object);
	}
	else
	{
		TSharedPtr<XRObjectModel> Model = ModelMap.FindRef(InArg._ArgUint32);
		if (Model.IsValid())
		{
			Model->Add(Object);
		}
		else
		{
			Model = MakeShareable(new XRObjectModel);
			ModelMap.Add(InArg._ArgUint32, Model);
			Model->Add(Object);
		}
	}
	ObjectArrayMap.FindOrAdd(InArg._ArgUint32).AddUnique(Object.ToSharedRef());


	XRArgument ChangeArg = InArg;
	PreOperationDelegate.Broadcast(ChangeArg.ArgInt32(1), Object, bTransaction);

	if (!bTransaction)
	{
		Object->Create();
	}
	PostOperationDelegate.Broadcast(ChangeArg.ArgInt32(1), Object, bTransaction);

	/**@欧石楠 存储下hardware*/
	if (Object->GetType() == OT_Door ||
		Object->GetType() == OT_SlidingDoor ||
		Object->GetType() == OT_SecurityDoor ||
		Object->GetType() == OT_Pass ||
		Object->GetType() == OT_NewPass ||
		Object->GetType() == OT_DoorHole ||
		Object->GetType() == OT_Window ||
		Object->GetType() == OT_FloorWindow ||
		Object->GetType() == OT_RectBayWindow ||
		Object->GetType() == OT_TrapeBayWindow ||
		Object->GetType() == OT_Punch)
	{
		TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(Object);
		if (TempHW.IsValid())
		{
			if (InArg._ArgUint32 == E_HomeModel)
			{
				AllHardwares_Home.Add(TempHW);
			}
			else if (InArg._ArgUint32 == E_ModifyModel)
			{
				AllHardwares_Modify.Add(TempHW);
			}
			else if (InArg._ArgUint32 == E_LayoutModel)
			{
				AllHardwares_Layout.Add(TempHW);
			}
		}
	}
}

void FArmySceneData::Delete(FObjectPtr Object, bool bTransaction/* = false*/)
{
	if (!Object.IsValid())
		return;

	if (!bTransaction)
	{
		Object->Delete();
	}

	XRArgument Param(0);
	for (auto& ModelIt : ModelMap)
	{
		if (ModelIt.Value->GetAllObjects().Contains(Object))
		{
			Param.ArgUint32(ModelIt.Key);
			ModelIt.Value->Delete(Object);
			break;
		}
	}
	for (auto& ArrayIt : ObjectArrayMap)
	{
		if (ArrayIt.Value.Contains(Object.ToSharedRef()))
		{
			Param.ArgUint32(ArrayIt.Key);
			ArrayIt.Value.Remove(Object.ToSharedRef());
			break;
		}
	}
	/**@欧石楠 删除hardware*/
	if (Object->GetType() == OT_Door ||
		Object->GetType() == OT_SlidingDoor ||
		Object->GetType() == OT_SecurityDoor ||
		Object->GetType() == OT_Pass ||
		Object->GetType() == OT_NewPass ||
		Object->GetType() == OT_DoorHole ||
		Object->GetType() == OT_Window ||
		Object->GetType() == OT_FloorWindow ||
		Object->GetType() == OT_RectBayWindow ||
		Object->GetType() == OT_TrapeBayWindow ||
		Object->GetType() == OT_Punch)
	{
		TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(Object);
		if (TempHW.IsValid())
		{
			AllHardwares_Home.Remove(TempHW);
			AllHardwares_Modify.Remove(TempHW);
			AllHardwares_Layout.Remove(TempHW);
		}
	}

	PreOperationDelegate.Broadcast(Param, Object, bTransaction);
	PostOperationDelegate.Broadcast(Param, Object, bTransaction);
	Object->RemoveFromParents();
	Object->Destroy();
    Object = nullptr;
}

void FArmySceneData::AddToGlobal(const TArray<FObjectWeakPtr>& Objects)
{
	GlobalDataArray.Append(Objects);
}

void FArmySceneData::EmptyGlobal()
{
	GlobalDataArray.Empty();
}
void FArmySceneData::Init(UWorld* InWorld)
{
	check(InWorld);
	World = InWorld;

	GGI->DesignEditor->OnCreateHomeModelDelegate.BindRaw(this, &FArmySceneData::GenerateDataTo3D);
	GGI->DesignEditor->OnCopyHomeDataDelegate.BindRaw(this, &FArmySceneData::ChangeBaseToModifyModelDelegate);

	//@郭子阳 初始化施工项管理器
	XRConstructionManager::Get()->Init();

}

const TArray<FObjectWeakPtr>& FArmySceneData::GetObjects(EModelType InModelType)
{
	return ObjectArrayMap.FindOrAdd(InModelType);
}
void FArmySceneData::GetObjects(EModelType InModelType, EObjectType InObjectType, TArray<FObjectWeakPtr>& OutObjects)
{
	for (auto ObjIt : GetObjects(InModelType))
	{
		if (ObjIt.Pin()->GetType() == InObjectType)
		{
			OutObjects.Add(ObjIt);
		}
	}
}

TWeakPtr<FArmyObject> FArmySceneData::GetObjectByGuid(EModelType InModelType, const FGuid& guid)
{
	for (TWeakPtr<FArmyObject> ObjIt : GetObjects(InModelType))
	{
		if (ObjIt.Pin()->GetUniqueID() == guid)
		{
			return ObjIt;
		}
	}
	return nullptr;
}
TWeakPtr<FArmyObject> FArmySceneData::GetObjectByGuidAlone(const FGuid& guid)
{
	for (auto ObjIt : ObjectArrayMap)
	{
		for (auto Obj : ObjIt.Value)
		{
			if (Obj.Pin()->GetUniqueID() == guid)
			{
				return Obj;
			}
		}
	}
	return NULL;
}

FObjectWeakPtr FArmySceneData::GetObjectByName(EModelType InModelType, const FString& Name)
{
	FGuid Guid;
	FGuid::Parse(Name.Right(32), Guid);

	return GetObjectByGuid(InModelType, Guid);
}

void FArmySceneData::GetAllObjects(TArray<TWeakPtr<FArmyObject>>& OutArray) const
{
	for (auto ObjArrayIt : ObjectArrayMap)
	{
		OutArray.Append(ObjArrayIt.Value);
	}
}

TArray<FObjectWeakPtr> FArmySceneData::GetObjectBySpaceName(EModelType InModelType, const FString& InSpaceName, bool InIgnoreOrder/* = false*/, bool InIgnoreNoBestowName/* = true*/)
{
	TArray<FObjectWeakPtr> Room;
	for (auto ObjIt : GetObjects(InModelType))
	{
		if (ObjIt.Pin()->GetType() == OT_InternalRoom)
		{
			TSharedPtr<FArmyRoom> RoomTemp = StaticCastSharedPtr<FArmyRoom>(ObjIt.Pin());
			if (RoomTemp.IsValid() && RoomTemp->GetSpaceNameLabel().IsValid()/* && !RoomTemp->GetSpaceName().Equals(TEXT("未命名"))*/)
			{
				FString SpaceName = RoomTemp->GetSpaceName();
				if (InIgnoreNoBestowName && SpaceName.Equals(TEXT("未命名")))
				{
					continue;
				}

				if (InIgnoreOrder)
				{
					if (SpaceName.Equals(InSpaceName))
					{
						Room.Add(ObjIt);
					}
				}
				else
				{
					//根据实际情况，一个户型房间数不可能大于10个，所以房间名称后缀默认取最后一位为房间编号
					FString NameTemp = SpaceName.Right(1).IsNumeric() ? SpaceName.Left(SpaceName.Len() - 1) : SpaceName;
					FString CheckNameTemp = InSpaceName.Right(1).IsNumeric() ? InSpaceName.Left(InSpaceName.Len() - 1) : InSpaceName;

					if (NameTemp.Equals(CheckNameTemp))
					{
						Room.Add(ObjIt);
					}
				}
			}
		}
	}
	return Room;
}

int32 FArmySceneData::GetMaxOrderBySpaceName(EModelType InModelType, const FString& InSpaceName)
{
	TArray<FObjectWeakPtr> RoomArr = GetObjectBySpaceName(InModelType, InSpaceName);
	//TMap<int32, FObjectWeakPtr> RoomMap = GetSortDataFromTArrayToTMap(RoomArr);
	return RoomArr.Num();
}

bool FArmySceneData::CalReduceRoomOrder(TArray<FObjectWeakPtr> & InRoomArr)
{
	if (InRoomArr.Num() < 1)
		return false;
	TMap<int32, FObjectWeakPtr > RoomMap = GetSortDataFromTArrayToTMap(InRoomArr);

	//更新房间序号
	int32 MinIndex = 2;
	TMap<int32, FObjectWeakPtr>::TIterator RoomMapIt(RoomMap);
	TSharedPtr<FArmyRoom> RoomTemp = StaticCastSharedPtr<FArmyRoom>(RoomMapIt->Value.Pin());
	FString SpaceName = RoomTemp->GetSpaceName();
	if (SpaceName.Right(1).IsNumeric())
	{
		RoomTemp->SetSpaceName(SpaceName.Left(SpaceName.Len() - 1));
	}
	if (RoomMap.Num() > 1)
	{
		++RoomMapIt;
		for (; RoomMapIt; ++RoomMapIt)
		{
			if (RoomMapIt->Key != 0)
			{
				TSharedPtr<FArmyRoom> TheRoom = StaticCastSharedPtr<FArmyRoom>(RoomMapIt->Value.Pin());
				FString NameTemp = TheRoom->GetSpaceName();
				FString NameNoIndex = NameTemp.Left(NameTemp.Len() - 1);
				NameNoIndex.AppendInt(MinIndex);
                TheRoom->SetSpaceName(NameNoIndex);
				MinIndex++;
			}
		}
	}

	return true;
}

FBox FArmySceneData::GetBounds()
{
	FBox outBox(ForceInitToZero);
	for (auto& ObjArrayIt : ObjectArrayMap)
	{
		for (auto ObjIt : ObjArrayIt.Value)
		{
			outBox += ObjIt.Pin()->GetBounds();
		}
	}
	return outBox;
}

void FArmySceneData::Clear(EModelType InModelType)
{
	TSharedPtr<XRObjectModel> Model = GetModel(InModelType);
	if (Model.IsValid())
	{
        TArray<FObjectPtr> DeleteObjects;
        for (auto It : Model->GetAllObjects())
        {
            DeleteObjects.Push(It);
        }

        Clear(DeleteObjects);
	}
}

void FArmySceneData::Clear(const TArray<FObjectPtr>& InObjects)
{
    if (InObjects.Num() > 0)
    {
        for (auto It : InObjects)
        {
            Delete(It);
        }

        //需要在空间发生变化时需要立面模式删除半透明蒙版
        if (TranslucentRoofActor && TranslucentRoofActor->IsValidLowLevel())
        {
            TranslucentRoofActor->Destroy();
            TranslucentRoofActor = nullptr;
        }
        if (TranslucentRoofActor_SuspendedCelling && TranslucentRoofActor_SuspendedCelling->IsValidLowLevel())
        {
            TranslucentRoofActor_SuspendedCelling->Destroy();
            TranslucentRoofActor_SuspendedCelling = nullptr;
        }
    }
}

void FArmySceneData::ClearModifyData()
{
    TSharedPtr<XRObjectModel> Model = GetModel(E_ModifyModel);
    if (Model.IsValid())
    {
        TArray<FObjectPtr> DeleteObjects;
        for (auto It : Model->GetAllObjects())
        {
            if (It->GetType() == OT_AddWall ||
                It->GetType() == OT_ModifyWall ||
                It->GetType() == OT_Door ||
                It->GetType() == OT_SlidingDoor ||
                It->GetType() == OT_IndependentWall ||
                It->GetType() == OT_PackPipe ||
                It->GetType() == OT_NewPass)
            {
                DeleteObjects.Push(It);
            }
        }

        Clear(DeleteObjects);
    }
}

void FArmySceneData::HideModelGeometry()
{
	//if (BSPManager->IsValidLowLevel())
	//{
	//	BSPManager->HideModelGeometry();
	//}
}

void FArmySceneData::SetHomeOpacity(float Opacity)
{
	Opacity = FMath::Clamp<float>(Opacity, 0.f, 1.f);

	TArray<FObjectWeakPtr> RoomSpaceAreas;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, RoomSpaceAreas);
	for (FObjectWeakPtr It : RoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
		if (RoomSpaceArea.IsValid())
		{
			AXRWallActor* WallActor = RoomSpaceArea->GetOriginalSurfaceActor();
			if (RoomSpaceArea->SurfaceType == 0 || RoomSpaceArea->SurfaceType == 2) // 地 & 顶
			{
				WallActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial(Opacity));
			}
			else if (RoomSpaceArea->SurfaceType == 1) // 墙
			{
				WallActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial(Opacity));
			}
		}
	}
	if (FArmySceneData::Get()->TotalSolidWallActor && FArmySceneData::Get()->TotalOutRoomActor)
	{
		FArmySceneData::Get()->TotalSolidWallActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial(Opacity));
		FArmySceneData::Get()->TotalOutRoomActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial(Opacity));
	}
}

void FArmySceneData::ShowHardModeCeilingOpacity()
{
	if (TranslucentRoofActor_SuspendedCelling)
		TranslucentRoofActor_SuspendedCelling->ResetMeshTriangles();
	else
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = "NOLIST-CEILINGOPACITY-SuspendedCelling";
		TranslucentRoofActor_SuspendedCelling = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity, SpawnParam);
		TranslucentRoofActor_SuspendedCelling->Tags.Add(TEXT("Immovable"));
		TranslucentRoofActor_SuspendedCelling->Tags.Add(TEXT("CanNotDelete"));
		TranslucentRoofActor_SuspendedCelling->Tags.Add(TEXT("TranslucentRoof"));
		TranslucentRoofActor_SuspendedCelling->bIsSelectable = false;
		UMaterialInstanceDynamic* MID_TranslucentRoofMaterial_SuspendedCelling = FArmyEngineModule::Get().GetEngineResource()->GetTranslucentRoofMaterial(0.7);
		MID_TranslucentRoofMaterial_SuspendedCelling->AddToRoot();
		TranslucentRoofActor_SuspendedCelling->SetMaterial(MID_TranslucentRoofMaterial_SuspendedCelling);
	}

	if (TranslucentRoofActor)
		TranslucentRoofActor->ResetMeshTriangles();
	else
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = "NOLIST-CEILINGOPACITY";
		TranslucentRoofActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity, SpawnParam);
		TranslucentRoofActor->Tags.Add(TEXT("Immovable"));
		TranslucentRoofActor->Tags.Add(TEXT("CanNotDelete"));
		TranslucentRoofActor->Tags.Add(TEXT("TranslucentRoof"));
		TranslucentRoofActor->bIsSelectable = false;
		UMaterialInstanceDynamic* MID_TranslucentRoofMaterial = FArmyEngineModule::Get().GetEngineResource()->GetTranslucentRoofMaterial(0.5);
		MID_TranslucentRoofMaterial->AddToRoot();
		TranslucentRoofActor->SetMaterial(MID_TranslucentRoofMaterial);
	}

	//获取RoomSpace区域
	TArray<TWeakPtr<FArmyObject>> RoomSpaceAreaListsObj;
	GetObjects(E_HardModel, OT_RoomSpaceArea, RoomSpaceAreaListsObj);
	for (TWeakPtr<FArmyObject > iter : RoomSpaceAreaListsObj)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->SurfaceType == 2 && tempRoom->GenerateFromObjectType == OT_InternalRoom)
		{
			CaculateTranslucentRoofActor(tempRoom);
		}
	}
	//获取绘制的其他区域
	TArray<TWeakPtr<FArmyObject>> AreaListsObj;
	GetObjects(E_HardModel, OT_RectArea, AreaListsObj);
	GetObjects(E_HardModel, OT_CircleArea, AreaListsObj);
	GetObjects(E_HardModel, OT_FreePolygonArea, AreaListsObj);
	GetObjects(E_HardModel, OT_PolygonArea, AreaListsObj);
	for (TWeakPtr<FArmyObject > iter : AreaListsObj)
	{
		TSharedPtr<FArmyBaseArea> tempRoom = StaticCastSharedPtr<FArmyBaseArea>(iter.Pin());
		if (tempRoom->SurfaceType == 2)
		{
			CaculateTranslucentRoofActor(tempRoom);
		}
	}
	TranslucentRoofActor->UpdateAllVetexBufferIndexBuffer();
	TranslucentRoofActor_SuspendedCelling->UpdateAllVetexBufferIndexBuffer();
}

void FArmySceneData::CaculateTranslucentRoofActor(TSharedPtr<FArmyBaseArea> tempRoom)
{
	if (tempRoom->GetExtrusionHeight() == 0)
	{
		TArray<struct FDynamicMeshVertex> BackGroundSurface = tempRoom->GetBackGroundSurface();
		for (int32 i = 0; i < BackGroundSurface.Num(); i++)
		{
			BackGroundSurface[i].Position.Z -= 50;
		}
		TranslucentRoofActor->AddVerts(BackGroundSurface);
	}
	else
	{
		TArray<struct FDynamicMeshVertex> BackGroundSurface = tempRoom->GetBackGroundSurface();
		for (int32 i = 0; i < BackGroundSurface.Num(); i++)
		{
			BackGroundSurface[i].Position.Z -= 10;
		}
		TranslucentRoofActor_SuspendedCelling->AddVerts(BackGroundSurface);
	}
}

TMap<int32, TSharedPtr<FJsonObject> > FArmySceneData::HyConstructionDataMap = TMap<int32, TSharedPtr<FJsonObject> >();

TSharedPtr<FJsonObject> FArmySceneData::GetHyConstructionData(int32 ConstructionKey)
{
	if (HyConstructionDataMap.Num() > 0)
	{
		auto Value = HyConstructionDataMap.Find(ConstructionKey);
		if (Value)
		{
			return *Value;
		}
	}
	return nullptr;
}

void FArmySceneData::AddHyConstructionData(int32 ConstructionKey, TSharedPtr<FJsonObject> ConstructionData)
{
	HyConstructionDataMap.Add(ConstructionKey, ConstructionData);
}

int32 FArmySceneData::GetConstructionDataListID(FArmyFurniture * Furniture)
{
	if (Furniture->IsOrignalPoint())
	{
		//原始点位用ComponentType*(-1)
		return  Furniture->ComponentType*(-1);
	}
	else {
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(Furniture->GetRelevanceActor()->GetSynID());
		int32  GoodsID = ActorItemInfo->ID;
		return GoodsID;
	}
}

TArray<TSharedPtr<FArmyRoomSpaceArea>> FArmySceneData::ForceSelectRoomAreas(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, TArray<float>& Dists)
{
	TArray<TWeakPtr<FArmyObject>> SelecteListsObj;
	GetObjects(E_HardModel, OT_RoomSpaceArea, SelecteListsObj);
	TArray<TSharedPtr<FArmyRoomSpaceArea>> SelecteLists;
	TArray<TempRoomInstersection> tempIntersection;
	for (TWeakPtr<FArmyObject > iter : SelecteListsObj)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		float dist;
		if (tempRoom->IsSelectEditPlane(Pos, InViewportClient, dist))
		{
			tempIntersection.Push(TempRoomInstersection(tempRoom, dist));
		}
	}
	tempIntersection.Sort();
	Dists.Empty();
	for (int i = 0; i < tempIntersection.Num(); i++)
	{
		SelecteLists.Push(StaticCastSharedPtr<FArmyRoomSpaceArea>(tempIntersection[i].roomSpace.Pin()));
		Dists.Push(tempIntersection[i].dist);
	}
	return SelecteLists;

}

void FArmySceneData::ForceOnRoomAreas(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, TArray<TSharedPtr<class FArmyRoomSpaceArea>>& OnSelectes)
{
	OnSelectes.Empty();
	TArray<TWeakPtr<FArmyObject>> SelecteListsObj;
	GetObjects(E_HardModel, OT_RoomSpaceArea, SelecteListsObj);
	for (TWeakPtr<FArmyObject > iter : SelecteListsObj)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->IsPointOneSurface(Pos))
		{
			OnSelectes.Add(tempRoom);
		}
	}
}

TArray<TSharedPtr<class FArmyRoomSpaceArea>> FArmySceneData::GetRoomAttachedSurfacesWidthRoomID(const FString& roomId)
{
	TArray<TWeakPtr<FArmyObject>> SelecteListsObj;
	TArray<TSharedPtr<FArmyRoomSpaceArea>> ResultSurfaces;
	GetObjects(E_HardModel, OT_RoomSpaceArea, SelecteListsObj);
	for (TWeakPtr<FArmyObject > iter : SelecteListsObj)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->AttachRoomID == roomId)
		{
			ResultSurfaces.Add(tempRoom);
		}
	}
	return ResultSurfaces;
}

TArray<TSharedPtr<class FArmyRoomSpaceArea>> FArmySceneData::GetRoomAttachedComponentSurfaceWithRoomId(const FString& roomId)
{
	TArray<TWeakPtr<FArmyObject>> SelecteListsObj;
	TArray<TSharedPtr<FArmyRoomSpaceArea>> ResultSurfaces;
	GetObjects(E_HardModel, OT_RoomSpaceArea, SelecteListsObj);
	TArray<TWeakPtr<FArmyObject>> AllInnerRooms;
	GetObjects(E_LayoutModel, OT_InternalRoom, AllInnerRooms);

	TWeakPtr<FArmyObject>* CurrentRoom;
	CurrentRoom = AllInnerRooms.FindByPredicate([roomId](const TWeakPtr<FArmyObject>& Data)->bool { return Data.Pin()->GetUniqueID().ToString() == roomId; });
	TArray<FString> AttahComponentIDs;
	if ((*CurrentRoom).IsValid())
	{
		TSharedPtr<FArmyRoom> tempRoom = StaticCastSharedPtr<FArmyRoom>((*CurrentRoom).Pin());
		TArray<TWeakPtr<FArmyObject>> allInnerComponets;
		tempRoom->GetObjectsRelevance(allInnerComponets);
		TArray<FVector> roomOutLines = tempRoom->GetWorldPoints(true);
		TArray<TWeakPtr<FArmyObject>> TotalAddWalls;
		GetObjects(E_ModifyModel, OT_AddWall, TotalAddWalls);
		for (TWeakPtr<FArmyObject> iter : TotalAddWalls)
		{
			FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
			if (tempAddWall &&tempAddWall->GetIsHalfWall())
			{
				TArray<FVector> clippHalfWalls;
				tempAddWall->GetVertexes(clippHalfWalls);
				if (FArmyMath::ArePolysOverlap(roomOutLines, clippHalfWalls))
				{
					allInnerComponets.Emplace(iter);
				}
			}
		}
		for (TWeakPtr<FArmyObject> iter : allInnerComponets)
		{
			AttahComponentIDs.Push(iter.Pin()->GetUniqueID().ToString());
		}
	}
	if (AttahComponentIDs.Num() > 0)
	{
		for (TWeakPtr<FArmyObject> iter : SelecteListsObj)
		{
			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
			for (const FString & ID : AttahComponentIDs)
			{
				
				if (tempRoom->AttachRoomID.Contains(ID))
				{
					ResultSurfaces.Push(tempRoom);
					break;
				}
			}
		}
	}


	return ResultSurfaces;
}

void FArmySceneData::GetBuckleConstrucionItem(const FString& roomId, TArray<struct FArmyGoods>& results)
{
	TArray<FString> RelatedHardWareId;
	TArray<TWeakPtr<FArmyObject>> TotalInnearRoom;
	GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRoom);

	for (TWeakPtr<FArmyObject> iter : TotalInnearRoom)
	{
		if (iter.Pin()->GetUniqueID().ToString() == roomId)
		{
			FArmyRoom* currentRoom = iter.Pin()->AsassignObj<FArmyRoom>();
			if (currentRoom)
			{
				TArray<TSharedPtr<FArmyHardware>> relatedObjects;
				currentRoom->GetHardwareListRelevance(relatedObjects);
				for (TWeakPtr<FArmyObject> iter0 : relatedObjects)
				{
					RelatedHardWareId.Emplace(iter0.Pin()->GetUniqueID().ToString());
				}
			}
			break;
		}
	}
	TArray<TWeakPtr<FArmyObject>> TotalRoomSpaceArea;
	GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomSpaceArea);
	TSharedPtr<FArmyRoomSpaceArea> CurrentRoomFloorArea;
	TArray<TSharedPtr<FArmyRoomSpaceArea>> CurrentRoomRelatedBridgeStoneAreas;
	FString tempBspId = TEXT("FloorSurface") + roomId;
	for (TWeakPtr<FArmyObject> iter : TotalRoomSpaceArea)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->AttachRoomID == roomId && tempRoom->AttachBspID == tempBspId)
			CurrentRoomFloorArea = tempRoom;
		if (RelatedHardWareId.Find(tempRoom->AttachRoomID) != INDEX_NONE && tempRoom->SurfaceType == 0)
		{
			if (tempRoom->GenerateFromObjectType == OT_Pass || tempRoom->GenerateFromObjectType == OT_Door || tempRoom->GenerateFromObjectType == OT_SlidingDoor
				|| tempRoom->GenerateFromObjectType == OT_SecurityDoor || tempRoom->GenerateFromObjectType == OT_NewPass || tempRoom->GenerateFromObjectType == OT_DoorHole)
			{
				CurrentRoomRelatedBridgeStoneAreas.Push(tempRoom);
			}
		}
	}
	for (TSharedPtr < FArmyRoomSpaceArea> iter : CurrentRoomRelatedBridgeStoneAreas)
	{
		iter->GetBuckleConstructionItems(results, CurrentRoomFloorArea);
	}
}

void FArmySceneData::ChangeAllConstructionItemInRoom(TSharedPtr<FArmyRoom> Room)
{

}

TArray<TSharedPtr<class FArmyRoomSpaceArea>> FArmySceneData::GetCurrentRoofAtttachWallRoomSpace(const FString& roomId)
{
	TArray<TSharedPtr<FArmyRoomSpaceArea>> results;
	TArray<TWeakPtr<FArmyObject>> totalRoomSpace;
	GetObjects(E_HardModel, OT_RoomSpaceArea, totalRoomSpace);
	TArray<FString> RelateRoomOrHardWareId;
	RelateRoomOrHardWareId.Emplace(roomId);
	TArray<TWeakPtr<FArmyObject>> TotalInnearRooms;
	GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRooms);
	for (TWeakPtr<FArmyObject> iter : TotalInnearRooms)
	{
		if (iter.Pin()->GetUniqueID().ToString() == roomId)
		{
			FArmyRoom* currentRoom = iter.Pin()->AsassignObj<FArmyRoom>();
			if (currentRoom)
			{
				TArray<TWeakPtr<FArmyObject>> relatedObjects;
				currentRoom->GetObjectsRelevance(relatedObjects);
				for (TWeakPtr<FArmyObject> iter0 : relatedObjects)
				{
					RelateRoomOrHardWareId.Emplace(iter0.Pin()->GetUniqueID().ToString());
				}
			}
			break;

		}
	}
	for (TWeakPtr<FArmyObject> iter : totalRoomSpace)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->SurfaceType == 1)
		{
			if (RelateRoomOrHardWareId.Find(tempRoom->AttachRoomID) != INDEX_NONE)
				results.Push(tempRoom);
		}
	}

	return results;
}



bool FArmySceneData::GetInnearRoomByRoomID(const FString& RoomId, TSharedPtr<FArmyRoom>& Innearroom)
{
	TArray<TWeakPtr<FArmyObject>> SelectListRoomObjs;
	GetObjects(E_LayoutModel, OT_InternalRoom, SelectListRoomObjs);
	for (TWeakPtr<FArmyObject> itr : SelectListRoomObjs)
	{
		TSharedPtr<FArmyRoom> tempRoom = StaticCastSharedPtr<FArmyRoom>(itr.Pin());
		if (tempRoom->GetUniqueID().ToString() == RoomId)
		{
			Innearroom = tempRoom;
			return true;
		}
	}
	return false;
}



bool FArmySceneData::GetRoomHoleInfoById(const FString& RoomId, TArray<FVector2D>& OutVertices)
{
	TSharedPtr<FArmyRoom> result;
	GetInnearRoomByRoomID(RoomId, result);
	if (result.IsValid())
	{
		const TArray<TSharedPtr<FArmyWallLine>>& allInnerWalls = result->GetWallLines();
		for (TSharedPtr<FArmyWallLine> iter : allInnerWalls)
		{
			TArray<TWeakPtr<FArmyObject>> AllAttachHardWare;
			iter->GetAppendObjects(AllAttachHardWare);
			const FVector& WallStartPos = iter->GetCoreLine().Get()->GetStart();
			const FVector& WallEndPos = iter->GetCoreLine().Get()->GetEnd();
			for (TWeakPtr<FArmyObject> iter0 : AllAttachHardWare)
			{
				if (iter0.Pin()->GetType() == OT_Pass
					|| iter0.Pin()->GetType() == OT_Door
					|| iter0.Pin()->GetType() == OT_FloorWindow
					|| iter0.Pin()->GetType() == OT_SecurityDoor
					|| iter0.Pin()->GetType() == OT_SlidingDoor
					|| iter0.Pin()->GetType() == OT_Punch
					|| iter0.Pin()->GetType() == OT_NewPass
					)
				{
					TSharedPtr<FArmyHardware> tempHardWare = StaticCastSharedPtr<FArmyHardware>(iter0.Pin());
					FVector startPos = tempHardWare->GetStartPos();
					FVector endPos = tempHardWare->GetEndPos();
					bool addHolesPos = true;
					if (tempHardWare->GetType() == OT_Pass)
					{
						TSharedPtr<FArmyPass> pass = StaticCastSharedPtr<FArmyPass>(iter0.Pin());
						if (pass->GetIfGeneratePassModel())
						{
							FVector dir = (endPos - startPos).GetSafeNormal();
							// 踢脚线哑口包边外扩
							endPos = endPos + dir * 7.0f;
							startPos = startPos - dir *7.0f;
						}
						if (pass->GetIfFillPass())
							addHolesPos = false;
					}
					if (tempHardWare->GetType() == OT_NewPass)
					{
						TSharedPtr<FArmyNewPass> newPass = StaticCastSharedPtr<FArmyNewPass>(iter0.Pin());
						if (newPass->GetIfGeneratePassModel())
						{
							FVector dir = (endPos - startPos).GetSafeNormal();
							// 踢脚线哑口包边外扩
							endPos = endPos + dir * 7.0f;
							startPos = startPos - dir *7.0f;
						}
					}
					if (addHolesPos)
					{
						const FVector& projectStartPos = FArmyMath::GetProjectionPoint(startPos, WallStartPos, WallEndPos);
						const FVector& projectEndPos = FArmyMath::GetProjectionPoint(endPos, WallStartPos, WallEndPos);
						OutVertices.Push(FVector2D(projectStartPos));
						OutVertices.Push(FVector2D(projectEndPos));
					}

				}
			}
		}
		return true;
	}
	return false;
}

TArray<TSharedPtr<class FArmyBaseArea>> FArmySceneData::PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient)
{
	TArray<TSharedPtr<FArmyBaseArea>> SelecteLists;
	TArray<TempRoomInstersection> tempIntersection;
	TArray<TWeakPtr<FArmyObject>> TotalObjects;
	for (int i = OT_TextureEditorBegin; i < OT_TextureEditorEnd; ++i)
	{
		TArray<TWeakPtr<FArmyObject>> SelecteListsObj;
		FArmySceneData::Get()->GetObjects(E_HardModel, (EObjectType)i, SelecteListsObj);
		TotalObjects.Append(SelecteListsObj);
	}

	for (TWeakPtr<FArmyObject > iter : TotalObjects)
	{
		TSharedPtr<FArmyBaseArea> temp = StaticCastSharedPtr<FArmyBaseArea>(iter.Pin());
		TWeakPtr<FArmyBaseArea> tempRoomSpace = temp;
		float dist;
		if (iter.Pin()->GetType() == OT_RoomSpaceArea)
		{
			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());

			if (tempRoom->IsSelectEditPlane(pos, InViewportClient, dist, true))
			{
				tempIntersection.Push(TempRoomInstersection(tempRoom, dist));
			}
		}
		else
		{
			TSharedPtr<FArmyBaseArea> area = tempRoomSpace.Pin()->ForceSelected(pos, InViewportClient, dist);
			if (area.IsValid())
			{
				tempIntersection.Push(TempRoomInstersection(area, dist));
			}
		}


	}
	tempIntersection.Sort();
	for (int i = 0; i < tempIntersection.Num(); i++)
	{
		SelecteLists.Push(tempIntersection[i].roomSpace.Pin());
	}
	return SelecteLists;
}



void FArmySceneData::Set3DRoomVisible(bool BVisible)
{
	if (TotalOutRoomActor)
		TotalOutRoomActor->SetActorHiddenInGame(!BVisible);
	if (TotalSolidWallActor)
		TotalSolidWallActor->SetActorHiddenInGame(!BVisible);
}


void FArmySceneData::SetHardwareModeActorVisible(bool BVisible)
{
	if (TotalSolidWallActor)
		TotalSolidWallActor->SetActorHiddenInGame(!BVisible);
	if (TotalOutRoomActor)
		TotalOutRoomActor->SetActorHiddenInGame(!BVisible);

	TArray<TWeakPtr<FArmyObject>> TotalRoomSpaces;
	GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomSpaces);
	for (TWeakPtr<FArmyObject> iter : TotalRoomSpaces)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		tempRoom->SetOrignalFinishWallVisible(BVisible);
	}
}

void FArmySceneData::Generate3DSurfaceActors()
{
	if (!LoadDataFromJason)
	{
		ConvertRoomInfoToWallInfo();
	}
	LoadDataFromJason = false;
}

#define BottomOrTopWallThick 20.0f
void FArmySceneData::ConvertRoomInfoToWallInfo()
{
	// 删除不必要的面片
	DeleteSurfaceWhithId();

	// 所有内房间
	TArray<TWeakPtr<FArmyObject>> AllInnerRoom;
	TArray<TWeakPtr<FArmyObject>> AllOuterRoom;
	GetObjects(E_LayoutModel, OT_InternalRoom, AllInnerRoom);
	GetObjects(E_LayoutModel, OT_OutRoom, AllOuterRoom);
	// 所有开阳台
	TArray<TWeakPtr<FArmyObject>> AllPunches;
	GetObjects(E_HomeModel, OT_Punch, AllPunches);
	TArray<TWeakPtr<FArmyObject>> AllHalfWalls;
	TArray<TWeakPtr<FArmyObject>> AllAddWalls;
	GetObjects(E_ModifyModel, OT_AddWall, AllAddWalls);
	for (TWeakPtr<FArmyObject>iter : AllAddWalls)
	{
		FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
		if (tempAddWall->GetIsHalfWall())
		{
			AllHalfWalls.Emplace(iter);
		}
	}

	TArray<TArray<FVector>> innerRoomHole;
	TArray<TArray<FVector>> TotalInnearRoomVerts;
	// @曾援
	// 存储每个房间的Guid和轮廓点信息
	TArray< TPair<FString, TArray<TArray<FVector>>> > FinishInnerRoomsRoofInfos;
	FinishInnerRoomsRoofInfos.SetNum(AllInnerRoom.Num());
	int32 RoomIndex = 0;
	for (TWeakPtr<FArmyObject> iter : AllInnerRoom)
	{
		// 计算每个房间
		TSharedPtr<FArmyRoom> innerRoom = StaticCastSharedPtr<FArmyRoom>(iter.Pin());
		FinishInnerRoomsRoofInfos[RoomIndex].Key = innerRoom->GetUniqueID().ToString();
		TArray<FVector> FinishRoomVerts; //房间完成面顶、地面顶点数据
		UpdateInnearWallInfo(innerRoom, FinishRoomVerts);
		// 计算房间内的烟道、柱子和房梁面片数据
		CalculateInnerRoomSimpleComponent(innerRoom);
		// 计算房间的地和顶
		const TArray<FVector>& OriginalFloorRoomVerts = innerRoom->GetWorldPoints(true);
		TotalInnearRoomVerts.Push(OriginalFloorRoomVerts);
		TArray<FVector> OriginalRoofRoomVerts;
		for (const FVector& TempIter : OriginalFloorRoomVerts)
		{
			OriginalRoofRoomVerts.Push(FVector(0, 0, WallHeight) + TempIter);
		}
		int number = FinishRoomVerts.Num();
		TArray<FVector> FinishFloorRoomVerts, FinishRoofRoomVerts;
		FinishFloorRoomVerts.AddUninitialized(number);
		FinishRoofRoomVerts.AddUninitialized(number);
		for (int i = 0; i < number; ++i)
		{
			FinishFloorRoomVerts[i] = FinishRoomVerts[i] + FVector(0, 0, 1) * FinishWallThick;
			FinishRoofRoomVerts[i] = FinishRoomVerts[i] + FVector(0, 0, 1)  * (WallHeight - FinishWallThick);
		}

		FBSPWallInfo OriginalFloorInfo, FinishFloorInfo, OriginalRoofRoomInfo, FinishRoofRoomInfo;
		// 原始房间地面，和完成面
		OriginalFloorInfo.Normal = FinishFloorInfo.Normal = FVector(0, 0, 1);
		OriginalFloorInfo.UniqueIdForRoomOrHardware = FinishFloorInfo.UniqueIdForRoomOrHardware = innerRoom->GetUniqueID().ToString();
		OriginalFloorInfo.PolyVertices.Add(OriginalFloorRoomVerts);
		FinishFloorInfo.PolyVertices.Add(FinishFloorRoomVerts);
		OriginalFloorInfo.UniqueId = FinishFloorInfo.UniqueId = TEXT("FloorSurface") + innerRoom->GetUniqueID().ToString();;
		OriginalFloorInfo.GenerateFromObjectType = FinishFloorInfo.GenerateFromObjectType = OT_InternalRoom;

		// 原始房间顶面和完成面顶面
		OriginalRoofRoomInfo.Normal = FinishRoofRoomInfo.Normal = FVector(0, 0, -1);
		OriginalRoofRoomInfo.PolyVertices.Add(OriginalRoofRoomVerts);
		FinishRoofRoomInfo.PolyVertices.Add(FinishRoofRoomVerts);
		OriginalRoofRoomInfo.UniqueIdForRoomOrHardware = FinishRoofRoomInfo.UniqueIdForRoomOrHardware = innerRoom->GetUniqueID().ToString();
		OriginalRoofRoomInfo.UniqueId = FinishRoofRoomInfo.UniqueId = TEXT("RoofSurface") + innerRoom->GetUniqueID().ToString();;
		OriginalRoofRoomInfo.GenerateFromObjectType = FinishRoofRoomInfo.GenerateFromObjectType = OT_InternalRoom;
		FinishInnerRoomsRoofInfos[RoomIndex].Value.Push(FinishRoofRoomVerts);

		TArray<TWeakPtr<FArmyObject>> AllPillarBeams;
		innerRoom->GetObjectsRelevance(AllPillarBeams);
		innerRoom->GetObjectsRelevanceByObjectVertices(AllPillarBeams);
		for (TWeakPtr<FArmyObject> iterCom : AllPillarBeams)
		{
			TArray<FVector> outHoleVetices;
			iterCom.Pin()->GetVertexes(outHoleVetices);
			TArray<FVector> result;
			if (Translate::ArePolysOverlap(OriginalFloorRoomVerts, outHoleVetices, result))
			{
				if (iterCom.Pin()->GetType() != OT_Beam)
				{
					if (iterCom.Pin()->GetType() == OT_PackPipe)
					{
						TArray<FVector> combineVerts;
						Translate::CaculatePackPipeOutLineVerts(OriginalFloorRoomVerts, outHoleVetices, combineVerts);
						OriginalFloorInfo.PolyVertices.Push(combineVerts);
						FinishFloorInfo.PolyVertices.Push(combineVerts);
						OriginalRoofRoomInfo.PolyVertices.Push(combineVerts);
						FinishRoofRoomInfo.PolyVertices.Push(combineVerts);
						FinishInnerRoomsRoofInfos[RoomIndex].Value.Push(combineVerts);
					}
					else if (iterCom.Pin()->GetType() == OT_IndependentWall)
					{
						OriginalFloorInfo.PolyVertices.Push(result);
						FinishFloorInfo.PolyVertices.Push(result);
						FArmyIndependentWall* tempIndependentWall = iterCom.Pin()->AsassignObj<FArmyIndependentWall>();
						if (tempIndependentWall && !tempIndependentWall->GetIsHalfWall())
						{
							OriginalRoofRoomInfo.PolyVertices.Push(result);
							FinishRoofRoomInfo.PolyVertices.Push(result);
							FinishInnerRoomsRoofInfos[RoomIndex].Value.Push(result);
						}
					}
					else
					{

						OriginalFloorInfo.PolyVertices.Push(result);
						FinishFloorInfo.PolyVertices.Push(result);
						OriginalRoofRoomInfo.PolyVertices.Push(result);
						FinishRoofRoomInfo.PolyVertices.Push(result);
						FinishInnerRoomsRoofInfos[RoomIndex].Value.Push(result);
					}
				}
				else if (iterCom.Pin()->GetType() == OT_Beam)
				{
					OriginalRoofRoomInfo.PolyVertices.Push(result);
					FinishRoofRoomInfo.PolyVertices.Push(result);
				}
			}
		}

		for (TWeakPtr<FArmyObject> iterHalfWall : AllHalfWalls)
		{
			TArray<FVector> outHoleVetices;
			iterHalfWall.Pin()->GetVertexes(outHoleVetices);
			TArray<FVector> result;
			if (Translate::ArePolysOverlap(OriginalFloorRoomVerts, outHoleVetices, result))
			{
				OriginalFloorInfo.PolyVertices.Push(result);
				FinishFloorInfo.PolyVertices.Push(result);
			}
		}
		for (TWeakPtr<FArmyObject> TempIter : AllPunches)
		{
			TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(TempIter.Pin());
			if ((tempPunch->GetHeight() + tempPunch->GetHeightToFloor()) > WallHeight)
			{
				const TArray<FVector>& clippBox = tempPunch->GetClipingBox();
				TArray<FVector> result;
				if (Translate::ArePolysOverlap(clippBox, OriginalRoofRoomVerts, result))
				{
					OriginalRoofRoomInfo.PolyVertices.Push(result);
					FinishRoofRoomInfo.PolyVertices.Push(result);
					FinishInnerRoomsRoofInfos[RoomIndex].Value.Push(result);
				}
			}
		}


		CaculateSurfaceInfoWhithWallInfo(FinishFloorInfo, OriginalFloorInfo, 0);

		CaculateSurfaceInfoWhithWallInfo(FinishRoofRoomInfo, OriginalRoofRoomInfo, 2);

		// FinishInnerRoomsRoofInfos.Emplace(TPairInitializer< FString, TArray<TArray<FVector>> >(innerRoom->GetUniqueID().ToString(), FinishRoofRoomInfo.PolyVertices));
		++RoomIndex;
	}
	// 计算房顶墙体面片,房地墙体面片
	if (AllOuterRoom.Num() == 1)
	{
		TSharedPtr<FArmyRoom> outRoom = StaticCastSharedPtr<FArmyRoom>(AllOuterRoom[0].Pin());
		// 合并整个外墙体、墙体顶和底成为一个Actor，减小DrawCall提升渲染效率
		UpdateOutWallInfo(outRoom);
		// 更新外墙体数据；
		const TArray<FVector>& TotalHouseVerts = outRoom->GetWorldPoints(true);
		// 计算整个户型的墙体数据，主要用于三维视图中顶视图和平面时候时候
		CaculateSolidWall(TotalHouseVerts, TotalInnearRoomVerts);
	}
	// 计算门洞窗洞侧面片数据
	CaculateWindowHoleSurface();
	//  计算开阳台补上侧面数
	CaculatePunchSurface();
	// 处理摆放时不受房间空间限制的对象的面片数据
	CalculateOuterRoomSimpleComponent(FinishInnerRoomsRoofInfos);

}

void FArmySceneData::DeleteSurfaceWhithId()
{
	auto GetDeleteSpaces = [this](const TArray<FString>& deleteObjectId, TArray < TSharedPtr<FArmyRoomSpaceArea>>& outResults)
	{
		TArray<TWeakPtr<FArmyObject>> TotalRoomSpacesArea;
		GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomSpacesArea);
		for (const FString& iter : deleteObjectId)
		{
			for (TWeakPtr<FArmyObject> iter0 : TotalRoomSpacesArea)
			{
				TSharedPtr<FArmyRoomSpaceArea> tempRoomSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter0.Pin());
				if (tempRoomSpace->AttachRoomID == iter)
				{
					outResults.Push(tempRoomSpace);
				}
			}

		}
		TArray<FString> TotalGenerateSurfaceObjectId;


		for (TWeakPtr<FArmyObject> ObjIt : GetObjects(E_LayoutModel))
		{
			if (ObjIt.Pin()->GetType() == OT_Window
				|| ObjIt.Pin()->GetType() == OT_FloorWindow
				|| ObjIt.Pin()->GetType() == OT_Pass
				|| ObjIt.Pin()->GetType() == OT_Door
				|| ObjIt.Pin()->GetType() == OT_SlidingDoor
				|| ObjIt.Pin()->GetType() == OT_SecurityDoor
				|| ObjIt.Pin()->GetType() == OT_RectBayWindow
				|| ObjIt.Pin()->GetType() == OT_TrapeBayWindow
				|| ObjIt.Pin()->GetType() == OT_InternalRoom
				|| ObjIt.Pin()->GetType() == OT_Punch
				|| ObjIt.Pin()->GetType() == OT_Beam
				|| ObjIt.Pin()->GetType() == OT_Pillar
				|| ObjIt.Pin()->GetType() == OT_AirFlue
				|| ObjIt.Pin()->GetType() == OT_IndependentWall)

			{
				TotalGenerateSurfaceObjectId.Push(ObjIt.Pin()->GetUniqueID().ToString());
			}
		}
		TArray<TWeakPtr<FArmyObject>> TotalAddWalls;
		GetObjects(E_ModifyModel, OT_AddWall, TotalAddWalls);
		for (TWeakPtr<FArmyObject> iter : TotalAddWalls)
		{
			FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
			if (tempAddWall && tempAddWall->GetIsHalfWall())
			{
				TotalGenerateSurfaceObjectId.Emplace(tempAddWall->GetUniqueID().ToString());
			}
		}
		for (TWeakPtr<FArmyObject> iter0 : TotalRoomSpacesArea)
		{
			TSharedPtr<FArmyRoomSpaceArea> tempRoomSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter0.Pin());
			if (TotalGenerateSurfaceObjectId.Find(tempRoomSpace->AttachRoomID) == INDEX_NONE)
			{
				outResults.Push(tempRoomSpace);
			}
		}
	};
	TArray<FString> DeleteObjectsId;
	for (auto& iter : ModifyDataMap)
	{
		DeleteObjectsId.Push(iter.Key);
	}
	TArray<TSharedPtr<FArmyRoomSpaceArea>> UsedDeletedRoomspace;
	TArray<TWeakPtr<FArmyObject>> allPass;
	GetObjects(E_LayoutModel, OT_Pass, allPass);
	for (TWeakPtr<FArmyObject> iter : allPass)
	{
		FArmyPass* tempPass = iter.Pin()->AsassignObj<FArmyPass>();
		if (tempPass->GetIfFillPass())
			DeleteObjectsId.Push(tempPass->GetUniqueID().ToString());
	}
	GetDeleteSpaces(DeleteObjectsId, UsedDeletedRoomspace);
	for (TSharedPtr<FArmyRoomSpaceArea> iter : UsedDeletedRoomspace)
	{
		TArray<TSharedPtr<FArmyBaseArea>> allAttachAreas = iter->GetEditAreas();
		for (TSharedPtr<FArmyBaseArea> iter0 : allAttachAreas)
		{
			Delete(iter0);
		}
		Delete(iter);
	}
	ModifyDataMap.Empty();
}



void FArmySceneData::UpdateInnearWallInfo(TSharedPtr<FArmyRoom> inRoom, TArray<FVector>& FinishRoomVertices)
{
	TArray<TSharedPtr<FArmyWallLine>> allRoomInnearWalls = inRoom->GetWallLines();
	if (inRoom->GetType() == OT_InternalRoom)
	{
		// 删掉房间里面没有出现在户型列表中对应墙体
		DeleteAttachInnerRoomUnUsedWalls(inRoom);
		TArray<TSharedPtr<FArmyPunch>> RoomPunchs;
		GetInnerRoomRelatedPunch(inRoom, RoomPunchs);
		struct TempWallInfo
		{
			TempWallInfo() = default;
			TempWallInfo(const FVector&  InStart, const FVector& InEnd, TSharedPtr<FArmyWallLine> InId)
			{
				StartPos = InStart;
				EndPos = InEnd;
				AttachWallline = InId;
			}
			bool operator==(const TempWallInfo& other)const
			{
				return StartPos == other.StartPos&&EndPos == other.EndPos && AttachWallline == other.AttachWallline;
			}
			bool Intersect(const TempWallInfo& other, FVector& IntersectPoint)
			{
				FVector2D resut = FVector2D::ZeroVector;
				if (FArmyMath::Line2DIntersection(FVector2D(StartPos), FVector2D(EndPos), FVector2D(other.StartPos), FVector2D(other.EndPos), resut))
				{
					IntersectPoint = FVector(resut, 0.0);
					return true;
				}
				return false;
			}
			void Swap()
			{
				FVector Temp = StartPos;
				StartPos = EndPos;
				EndPos = Temp;
			}
			FVector StartPos;
			FVector EndPos;
			TSharedPtr<FArmyWallLine> AttachWallline;
		};
		int InnerWallNum = allRoomInnearWalls.Num();
		if (InnerWallNum < 3)
			return;
		TArray<TempWallInfo> TempWallInfos;

		for (int i = 0; i < InnerWallNum; ++i)
		{
			TempWallInfos.Add(TempWallInfo(allRoomInnearWalls[i]->GetCoreLine().Get()->GetStart(), allRoomInnearWalls[i]->GetCoreLine().Get()->GetEnd(), allRoomInnearWalls[i]));
		}
		TArray<TempWallInfo> OrderWallLists;
		if (TempWallInfos.Num() > 0)
		{
			OrderWallLists.Add(TempWallInfos[0]);
			TempWallInfos.RemoveAt(0);

			while (TempWallInfos.Num() > 0)
			{
				int number = TempWallInfos.Num();
				for (int i = 0; i < number; ++i)
				{
					const TempWallInfo& tempInfo = OrderWallLists.Last();
					// if (tempInfo.StartPos.Equals(TempWallInfos[i].StartPos, 0.01f) || tempInfo.StartPos.Equals(TempWallInfos[i].EndPos, 0.01f)
					// 	|| tempInfo.EndPos.Equals(TempWallInfos[i].StartPos, 0.01f) || tempInfo.EndPos.Equals(TempWallInfos[i].EndPos, 0.01f))
					// {
					// 	OrderWallLists.Push(TempWallInfos[i]);
					// 	TempWallInfos.RemoveAt(i);
					// 	break;
					// }
					if (tempInfo.EndPos.Equals(TempWallInfos[i].StartPos, 0.01f))
					{
						OrderWallLists.Push(TempWallInfos[i]);
						TempWallInfos.RemoveAt(i);
						break;
					}
					else if (tempInfo.EndPos.Equals(TempWallInfos[i].EndPos, 0.01f))
					{
						TempWallInfos[i].Swap();
						OrderWallLists.Push(TempWallInfos[i]);
						TempWallInfos.RemoveAt(i);
						break;
					}
				}
			}
		}
		TempWallInfo PreWall, CurrentWall, NextWall;
		for (int i = 0; i < InnerWallNum; i++)
		{

			if (i == 0)
			{
				PreWall = OrderWallLists.Last();
				CurrentWall = OrderWallLists[i];
				NextWall = OrderWallLists[i + 1];
			}
			else if (i == InnerWallNum - 1)
			{
				PreWall = OrderWallLists[i - 1];
				CurrentWall = OrderWallLists[i];
				NextWall = OrderWallLists[0];
			}
			else
			{
				PreWall = OrderWallLists[i - 1];
				CurrentWall = OrderWallLists[i];
				NextWall = OrderWallLists[i + 1];
			}
			CurrentWall.StartPos = CurrentWall.StartPos + CurrentWall.AttachWallline->GetNormal()*FinishWallThick;
			CurrentWall.EndPos = CurrentWall.EndPos + CurrentWall.AttachWallline->GetNormal()*FinishWallThick;
			NextWall.StartPos = NextWall.StartPos + NextWall.AttachWallline->GetNormal()*FinishWallThick;
			NextWall.EndPos = NextWall.EndPos + NextWall.AttachWallline->GetNormal()*FinishWallThick;
			PreWall.StartPos = PreWall.StartPos + PreWall.AttachWallline->GetNormal()*FinishWallThick;
			PreWall.EndPos = PreWall.EndPos + PreWall.AttachWallline->GetNormal()*FinishWallThick;
			FBSPWallInfo OrginalWallInfo, FinishWallInfo;
			//OrginalWallInfo.InnerRoomId = inRoom->GetUniqueID();
			//.InnerRoomId = inRoom->GetUniqueID();
			OrginalWallInfo.UniqueIdForRoomOrHardware = FinishWallInfo.UniqueIdForRoomOrHardware = inRoom->GetUniqueID().ToString();
			OrginalWallInfo.UniqueId = FinishWallInfo.UniqueId = CurrentWall.AttachWallline->GetUniqueID().ToString();
			OrginalWallInfo.Normal = FinishWallInfo.Normal = -CurrentWall.AttachWallline->GetNormal();

			TSharedPtr<FArmyLine> lineInfo = CurrentWall.AttachWallline->GetCoreLine();
			const FVector& OrignalWallStart = lineInfo->GetStart();
			const FVector& OrignalWallEnd = lineInfo->GetEnd();

			TArray<FVector> OrignalWallVertices = { OrignalWallStart ,OrignalWallStart + FVector(0,0,1) * WallHeight,OrignalWallEnd + FVector(0,0,1) * WallHeight,OrignalWallEnd };
			OrginalWallInfo.PolyVertices.Push(OrignalWallVertices);
			FVector finishStart, finishEnd;
			if (!CurrentWall.Intersect(NextWall, finishEnd))
				finishEnd = CurrentWall.EndPos;
			if (!CurrentWall.Intersect(PreWall, finishStart))
				finishStart = CurrentWall.StartPos;
			if (i == 0)
			{
				FinishRoomVertices.Push(finishStart);
				FinishRoomVertices.Push(finishEnd);
			}
			else if (i < InnerWallNum - 1)
			{
				FinishRoomVertices.Push(finishEnd);
			}

			TArray<FVector> FinishWallVerts = { finishStart ,finishStart + FVector(0,0,1) * WallHeight,finishEnd + FVector(0,0,1) * WallHeight,finishEnd };
			FinishWallInfo.PolyVertices.Push(FinishWallVerts);

			CalculateWallAttachHardwareInfo(CurrentWall.AttachWallline, OrginalWallInfo);
			CalculateWallAttachHardwareInfo(CurrentWall.AttachWallline, FinishWallInfo, false);

			CalculateRoomPunchHoles(RoomPunchs, CurrentWall.AttachWallline, OrginalWallInfo);
			CalculateRoomPunchHoles(RoomPunchs, CurrentWall.AttachWallline, FinishWallInfo);

			CaculateSurfaceInfoWhithWallInfo(FinishWallInfo, OrginalWallInfo, 1);
		}
	}
}

void FArmySceneData::DeleteAttachInnerRoomUnUsedWalls(TSharedPtr<FArmyRoom> InnerRoom)
{
	const TArray<TSharedPtr<FArmyWallLine>>& allRoomInnearWalls = InnerRoom->GetWallLines();
	TArray<TWeakPtr<FArmyObject>> TotalRoomSpaceAreas;
	GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomSpaceAreas);
	const FString& AttachRoomId = InnerRoom->GetUniqueID().ToString();
	TArray<TSharedPtr<FArmyRoomSpaceArea>> AttachRoomWallAreas;
	for (TWeakPtr<FArmyObject> iter : TotalRoomSpaceAreas)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (tempRoom->AttachRoomID == AttachRoomId &&
			tempRoom->SurfaceType == 1 &&
			tempRoom->GenerateFromObjectType == OT_WallLine)
		{
			AttachRoomWallAreas.Push(tempRoom);
		}
	}

	TArray<FString> AttachRoomWallIds;
	for (TSharedPtr<FArmyWallLine> iter : allRoomInnearWalls)
	{
		AttachRoomWallIds.Push(iter->GetUniqueID().ToString());
	}
	TArray<TSharedPtr<FArmyRoomSpaceArea>> DeleteWallAreas;
	for (TSharedPtr<FArmyRoomSpaceArea> iter : AttachRoomWallAreas)
	{
		const FString& AttachWallId = iter->AttachBspID;
		if (AttachRoomWallIds.Find(AttachWallId) == INDEX_NONE)
		{
			DeleteWallAreas.Push(iter);
		}
	}
	for (TSharedPtr<FArmyRoomSpaceArea> iter : DeleteWallAreas)
	{
		TArray<TSharedPtr<FArmyBaseArea>> allAttachAreas = iter->GetEditAreas();
		for (TSharedPtr<FArmyBaseArea> iter0 : allAttachAreas)
		{
			Delete(iter0);
		}
		Delete(iter);
	}

}

void FArmySceneData::CalculateWallAttachHardwareInfo(TSharedPtr<FArmyWallLine> wall, FBSPWallInfo& wallInfo, bool originalFace)
{
	TArray<TWeakPtr<FArmyObject>> allAttachWindowOrDoors;
	wall->GetAppendObjects(allAttachWindowOrDoors);
	// @zengy Guid的缓存
	TArray<FGuid> CachedObjGuidArr;
	for (TWeakPtr<FArmyObject> holeIter : allAttachWindowOrDoors)
	{
		TSharedPtr<FArmyHardware> tempHardWare = StaticCastSharedPtr<FArmyHardware>(holeIter.Pin());
		if (tempHardWare.IsValid())
		{
			const FGuid &ObjGuid = tempHardWare->GetUniqueID();
			// @zengy 如果某个Guid对应的对象已经添加，就跳过不再处理
			if (CachedObjGuidArr.Contains(ObjGuid))
				continue;
			if (holeIter.Pin()->GetType() == OT_Window)
			{
				TSharedPtr<FArmyWindow> window = StaticCastSharedPtr<FArmyWindow>(holeIter.Pin());
				float heightToFloor = window->GetHeightToFloor();
				const FVector& Position = FVector(window->GetPos().X, window->GetPos().Y, 0.0f);
				const FVector& direction = window->GetHorizontalDirecton();
				float windowLength = window->GetLength();
				float WindowHeight = window->GetHeight();
				if (originalFace)
				{
					TArray<FVector> clippBox = {
						(Position + windowLength / 2 * direction + FVector(0,0,1) * heightToFloor),
						(Position + windowLength / 2 * direction + FVector(0,0,1) * (heightToFloor + WindowHeight)),
						(Position - windowLength / 2 * direction + FVector(0,0,1) * (heightToFloor + WindowHeight)),
						(Position - windowLength / 2 * direction + FVector(0,0,1) * heightToFloor)
					};
					wallInfo.PolyVertices.Push(clippBox);
				}
				else
				{
					TArray<FVector> clippbox = { FVector(Position + (0.5f * windowLength - FinishWallThick) * direction) + FVector(0,0,1)*(FinishWallThick + heightToFloor),
						FVector(Position + (0.5f *windowLength - FinishWallThick) * direction) + FVector(0,0,1)*(heightToFloor + WindowHeight - FinishWallThick),
						FVector(Position - (0.5 * windowLength - FinishWallThick) *direction) + FVector(0,0,1)* (heightToFloor + WindowHeight - FinishWallThick),
						FVector(Position - (0.5f*windowLength - FinishWallThick) * direction) + FVector(0,0,1) * (FinishWallThick + heightToFloor)
					};
					wallInfo.PolyVertices.Push(clippbox);
				}


			}
			else if (holeIter.Pin()->GetType() == OT_RectBayWindow || holeIter.Pin()->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> window = StaticCastSharedPtr<FArmyRectBayWindow>(holeIter.Pin());
				float heightToFloor = window->GetHeightToFloor();
				const FVector& Position = FVector(window->GetPos().X, window->GetPos().Y, 0.0f);
				const FVector& direction = window->GetHorizontalDirecton();
				float windowLength = window->GetLength();
				float WindowHeight = window->GetHeight();

				if (originalFace)
				{
					TArray<FVector> clippBox = {
					   (Position + windowLength / 2 * direction + FVector(0,0,1) * heightToFloor),
					   (Position + windowLength / 2 * direction + FVector(0,0,1) * (heightToFloor + WindowHeight)),
					   (Position - windowLength / 2 * direction + FVector(0,0,1) * (heightToFloor + WindowHeight)),
					   (Position - windowLength / 2 * direction + FVector(0,0,1) * heightToFloor)
					};
					wallInfo.PolyVertices.Push(clippBox);
				}
				else
				{
					TArray<FVector> clippbox = { FVector(Position + (0.5f * windowLength - FinishWallThick) * direction) + FVector(0,0,1)*(FinishWallThick + heightToFloor),
						FVector(Position + (0.5f *windowLength - FinishWallThick) * direction) + FVector(0,0,1)*(heightToFloor + WindowHeight - FinishWallThick),
						FVector(Position - (0.5 * windowLength - FinishWallThick) *direction) + FVector(0,0,1)* (heightToFloor + WindowHeight - FinishWallThick),
						FVector(Position - (0.5f*windowLength - FinishWallThick) * direction) + FVector(0,0,1) * (FinishWallThick + heightToFloor)
					};
					wallInfo.PolyVertices.Push(clippbox);
				}



			}
			else if (holeIter.Pin()->GetType() == OT_FloorWindow
				|| holeIter.Pin()->GetType() == OT_Pass
				|| holeIter.Pin()->GetType() == OT_SlidingDoor
				|| holeIter.Pin()->GetType() == OT_Door
				|| holeIter.Pin()->GetType() == OT_SecurityDoor
				|| holeIter.Pin()->GetType() == OT_NewPass)
			{
				bool addPass = true;
				if (holeIter.Pin()->GetType() == OT_Pass)
				{
					TSharedPtr<FArmyPass> tempPass = StaticCastSharedPtr<FArmyPass>(holeIter.Pin());
					if (tempPass->GetIfFillPass())
						addPass = false;
				}
				if (addPass)
				{
					const TArray<FVector>& clippbox = CalculateHardWareClipBox(tempHardWare, originalFace);
					wallInfo.PolyVertices.Push(clippbox);
				}

			}
			CachedObjGuidArr.Emplace(ObjGuid);
			wallInfo.AttachWindowOrDoorName.Push(tempHardWare->GetAttachModelName());
		}

	}
}

void FArmySceneData::GetInnerRoomRelatedPunch(TSharedPtr<FArmyRoom>Inroom, TArray<TSharedPtr<FArmyPunch>>& RelatedPunchs)
{
	TArray<TWeakPtr<FArmyObject>> RoomPuchs;
	GetObjects(E_HomeModel, OT_Punch, RoomPuchs);
	const TArray<FVector>& InnerRoomVertices = Inroom->GetWorldPoints(true);
	for (TWeakPtr<FArmyObject> iter : RoomPuchs)
	{
		TSharedPtr<FArmyPunch> punch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());
		TArray<FVector> result;
		if (Translate::ArePolysOverlap(InnerRoomVertices, punch->GetClipingBox(), result))
		{
			RelatedPunchs.Push(punch);
		}
	}
}

void FArmySceneData::CalculateRoomPunchHoles(TArray<TSharedPtr<FArmyPunch>> RelatedPunchs, const TSharedPtr<FArmyWallLine> InWallLine, FBSPWallInfo& InWallInfo, bool beColinear)
{
	for (TSharedPtr<FArmyPunch> iter : RelatedPunchs)
	{
		const FVector& wallStart = InWallLine->GetCoreLine().Get()->GetStart();
		const FVector& wallEnd = InWallLine->GetCoreLine().Get()->GetEnd();

		TArray<FVector> bounding = iter->GetClipingBox();
		FArmyMath::CleanPolygon(bounding);
		if (FArmyMath::IsClockWise(bounding))
			FArmyMath::ReversePointList(bounding);
		// @zengy 将阳台轮廓向外挤出0.5mm，避免相交测试时出现精度误差
		bounding = FArmyMath::ExturdePolygon(bounding, 0.05f, false);
		TArray<FVector2D> newBoundings;
		for (FVector TempIter : bounding)
			newBoundings.Push(FVector2D(TempIter));
		
		// TODO: @曾援 重写一个函数，在给梁柱在墙上挖洞时使用 IntersectSegmentWithConvexPolygon
		TArray<FVector> results;
		bool IntersectionResult = true;
		if (beColinear)
		{
			// TArray<TPair<int32, FVector>> TempResults;
			IntersectionResult = FArmyMath::IntersectSegmentWithConvexPolygon(FVector2D(wallStart), FVector2D(wallEnd), newBoundings, results);
			// for (const auto &TempResult : TempResults)
				// results.Emplace(TempResult.Value);
		}
		else
			IntersectionResult = Translate::IntersectSegmentWithConvexPolygon(FVector2D(wallStart), FVector2D(wallEnd), newBoundings, results);

		if (IntersectionResult)
		{
			if (results.Num() == 2)
			{
				float tempheight = iter->GetHeight();
				if ((iter->GetHeightToFloor() + iter->GetHeight() > WallHeight) && !beColinear)
					tempheight = WallHeight - iter->GetHeightToFloor();
				else if ((iter->GetHeightToFloor() + iter->GetHeight() > WallHeight) && beColinear)
					tempheight = WallHeight + BottomOrTopWallThick - iter->GetHeightToFloor();

				FVector tempStart = results[0] + FVector(0, 0, 1) * iter->GetHeightToFloor();
				FVector tempStarUp = tempStart + FVector(0, 0, 1) * tempheight;
				FVector tempEnd = results[1] + FVector(0, 0, 1)*iter->GetHeightToFloor();
				FVector tempEndUp = tempEnd + FVector(0, 0, 1)*tempheight;
				TArray<FVector> clippVerts = { tempStart,tempStarUp,tempEndUp,tempEnd };

				InWallInfo.PolyVertices.Add(clippVerts);
			}
		}
	}
}

void FArmySceneData::CaculateSurfaceInfoWhithWallInfo(const FBSPWallInfo& FinishSurfaceInfo, const FBSPWallInfo& originalSurface, uint32 supportGoodType)
{
	TArray<FVector> FinishSurfaceVertices;
	TArray<TArray<FVector>> FinshSurfaceHoles;
	uint32 SurfaceType = 0;
	FVector finshPlaneCenterPos, finishPlaneXDir, FinshPlaneYDir;
	TArray<FVector> OrginalSufaceVertices;
	TArray<TArray<FVector>> OrginalHoleSurfaceVertices;
	ConvertBspWallInfoToSurfaceHole(FinishSurfaceInfo, finishPlaneXDir, FinshPlaneYDir, finshPlaneCenterPos, SurfaceType, FinishSurfaceVertices, FinshSurfaceHoles);

	FVector orginalX, orginalY, orginalCenter;
	ConvertBspWallInfoToSurfaceHole(originalSurface, orginalX, orginalY, orginalCenter, SurfaceType, OrginalSufaceVertices, OrginalHoleSurfaceVertices);

	//float outAreaSum = FArmyMath::CalcPolyArea(OrginalSufaceVertices);
	//float sumHoles = 0;
	//for (const TArray<FVector>& iter : OrginalHoleSurfaceVertices)
	//{
	//	sumHoles += FArmyMath::CalcPolyArea(iter);
	//}
	//if (outAreaSum <= sumHoles + 0.2f) 
	//	return;
	TSharedPtr<FArmyRoomSpaceArea> surface = GetWallByRoomIdAndAttachWallLineId(FinishSurfaceInfo.UniqueIdForRoomOrHardware, FinishSurfaceInfo.UniqueId);
	if (FinishSurfaceInfo.GenerateFromObjectType == OT_WallLine && SurfaceType == 1)
	{
		FBox box(FinishSurfaceVertices);
		float length = (box.Max - box.Min).X;
		TArray<TArray<FVector>> projectResults;
		UpdateInnearWallAttachSimpleComponentInfo(finshPlaneCenterPos, finishPlaneXDir, FinshPlaneYDir, length, projectResults);
		FinshSurfaceHoles.Append(projectResults);
	}
	if (surface.IsValid())
	{
		surface->SetPlaneInfo(finishPlaneXDir, FinshPlaneYDir, finshPlaneCenterPos);
		TArray<TSharedPtr<FArmyBaseArea>> allAttachArea = surface->GetEditAreas();
		for (TSharedPtr<FArmyBaseArea> iter : allAttachArea)
		{
			const TArray<FVector>& editBoudary = iter->GetOutArea()->Vertices;
			bool areaIntersectWhithHole = false;
			for (const TArray<FVector>& iter0 : FinshSurfaceHoles)
			{
				if (FArmyMath::ArePolysOverlap(iter0, editBoudary))
					areaIntersectWhithHole = true;
			}
			if (areaIntersectWhithHole)
			{
				surface->RemoveArea(iter);
				Delete(iter);
			}
		}
	//	surface->InnerRoomID = FinishSurfaceInfo.InnerRoomId;
		surface->SetVerticesAndHoles(FinishSurfaceVertices, FinshSurfaceHoles);
		surface->GenerateOrignalActor(OrginalSufaceVertices, OrginalHoleSurfaceVertices, orginalX, orginalY, orginalCenter);
		surface->GetStyle()->SetPlaneInfo(finshPlaneCenterPos, finishPlaneXDir, FinshPlaneYDir);
		uint32 tempStatus = surface->GetRoomStatus();
		surface->SetStyle(surface->GetStyle());
		surface->RoomDataStatus = tempStatus;
		surface->CurrentAttachWindowName = FinishSurfaceInfo.AttachWindowOrDoorName;
		surface->GenerateFromObjectType = FinishSurfaceInfo.GenerateFromObjectType;
	}
	else
	{
		surface = MakeShareable(new FArmyRoomSpaceArea());
		surface->SurportPlaceArea = supportGoodType;
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_Pass ||
			FinishSurfaceInfo.GenerateFromObjectType == OT_Door ||
			FinishSurfaceInfo.GenerateFromObjectType == OT_SecurityDoor ||
			FinishSurfaceInfo.GenerateFromObjectType == OT_SlidingDoor ||
			FinishSurfaceInfo.GenerateFromObjectType == OT_NewPass)
		{
			if (SurfaceType == 2 || SurfaceType == 1)
				surface->SurportPlaceArea = 1;
			else if (SurfaceType == 0)
				surface->SurportPlaceArea = 0;
		}
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_Beam)
		{
			surface->SurportPlaceArea = 2;
		}
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_Punch &&SurfaceType == 0)
		{
			surface->SurportPlaceArea = 0;
		}
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_WallLine || FinishSurfaceInfo.GenerateFromObjectType == OT_InternalRoom)
		{
			surface->SurportPlaceArea = SurfaceType;
		}
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_Window || FinishSurfaceInfo.GenerateFromObjectType == OT_Window
			|| FinishSurfaceInfo.GenerateFromObjectType == OT_FloorWindow || FinishSurfaceInfo.GenerateFromObjectType == OT_ArcWindow
			|| FinishSurfaceInfo.GenerateFromObjectType == OT_CornerBayWindow || FinishSurfaceInfo.GenerateFromObjectType == OT_RectBayWindow
			|| FinishSurfaceInfo.GenerateFromObjectType == OT_TrapeBayWindow)
		{
			surface->SurportPlaceArea = 1;
		}
		if (FinishSurfaceInfo.GenerateFromObjectType == OT_FloorWindow && SurfaceType == 0)
		{
			surface->SurportPlaceArea = SurfaceType;
		}
//		surface->InnerRoomID = FinishSurfaceInfo.InnerRoomId;
		surface->AttachRoomID = FinishSurfaceInfo.UniqueIdForRoomOrHardware;
		surface->AttachBspID = FinishSurfaceInfo.UniqueId;
		surface->SetPlaneInfo(finishPlaneXDir, FinshPlaneYDir, finshPlaneCenterPos);
		surface->SurfaceType = SurfaceType;
		surface->SetVerticesAndHoles(FinishSurfaceVertices, FinshSurfaceHoles);
		surface->GenerateOrignalActor(OrginalSufaceVertices, OrginalHoleSurfaceVertices, orginalX, orginalY, orginalCenter);
		surface->ResetSurfaceStyle(true);
		surface->CurrentAttachWindowName = FinishSurfaceInfo.AttachWindowOrDoorName;
		surface->GenerateFromObjectType = FinishSurfaceInfo.GenerateFromObjectType;

		XRArgument arg;
		arg._ArgUint32 = E_HardModel;
		FArmySceneData::Get()->Add(surface, arg);
	}

}

void FArmySceneData::UpdateInnearWallAttachSimpleComponentInfo(const FVector& InPlanceCenter, const FVector& InPlaneXDir, const FVector& InPlaneYDir, float wallLength, TArray<TArray<FVector>>& results)
{
	TArray<TWeakPtr<FArmyObject>> TotalFurnitures;
	GetObjects(E_LayoutModel, OT_Beam, TotalFurnitures);
	

	GetObjects(E_LayoutModel, OT_Pillar, TotalFurnitures);
	GetObjects(E_LayoutModel, OT_AirFlue, TotalFurnitures);
	GetObjects(E_LayoutModel, OT_PackPipe, TotalFurnitures);

	TArray<TWeakPtr<FArmyObject>> AllHalfWalls;
	TArray<TWeakPtr<FArmyObject>> AllAddWalls;
	GetObjects(E_LayoutModel, OT_AddWall, AllAddWalls);
	for (TWeakPtr<FArmyObject>iter : AllAddWalls)
	{
		FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
		if (tempAddWall->GetIsHalfWall())
		{
			AllHalfWalls.Emplace(iter);
		}
	}
	TotalFurnitures.Append(AllHalfWalls);
	FBox box;
	FVector2D WallWorldStartPos, WallWorldEndPos;
	WallWorldStartPos = FVector2D(InPlanceCenter + wallLength *InPlaneXDir / 2);
	WallWorldEndPos = FVector2D(InPlanceCenter - wallLength *  InPlaneXDir / 2);

	//	TArray<FArmyRoomSpaceArea::SurfaceHoleInfo> SurfaceAttachHolesInfo;
	for (TWeakPtr<FArmyObject> iter : TotalFurnitures)
	{
		TArray<FVector> vertices;
		float height = WallHeight;
		float OffsetGroundHeight = 0;
		if (iter.Pin()->GetType() == OT_Beam || iter.Pin()->GetType() == OT_Pillar || iter.Pin()->GetType() == OT_AirFlue)
		{
			TSharedPtr<FArmySimpleComponent> pillar = StaticCastSharedPtr<FArmySimpleComponent>(iter.Pin());
			pillar->GetVertexes(vertices);
			if (iter.Pin()->GetType() == OT_Beam)
			{
				OffsetGroundHeight = WallHeight - pillar->GetHeight();
				height = pillar->GetHeight();
			}
			else
			{
				OffsetGroundHeight = 0.0f;
				height = WallHeight;
			}
		}
		else if (iter.Pin()->GetType() == OT_PackPipe)
		{
			FArmyPackPipe* tempPackPipe = iter.Pin()->AsassignObj<FArmyPackPipe>();
			if (tempPackPipe)
			{
				tempPackPipe->GetFillOutsideVertices(vertices);
				OffsetGroundHeight = 0.0f;
				height = WallHeight;
			}
		}
		else if (iter.Pin()->GetType() == OT_AddWall)
		{
			FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
			if (tempAddWall && tempAddWall->GetIsHalfWall())
			{
				OffsetGroundHeight = 0.0f;
				height = tempAddWall->GetHeight();
				tempAddWall->GetVertexes(vertices);
			}
		}
		FArmyMath::CleanPolygon(vertices);
		// @zengy 向外挤出半毫米，避免在进行相交运算时由于精度的误差导致错误
		vertices = FArmyMath::ExturdePolygon(vertices, 0.05f, false);
		TArray<FVector2D> pillarOutLines;
		for (FVector& TempIter : vertices)
		{
			pillarOutLines.Push(FVector2D(TempIter));
		}
		FVector2D outStart, outEnd;
		TArray<FVector> outVertices;
		if (FArmyMath::IntersectSegmentWithConvexPolygon(WallWorldStartPos, WallWorldEndPos, pillarOutLines, outVertices))
		{
			if (outVertices.Num() >= 2)
			{
				outStart = FVector2D(outVertices[0]);
				outEnd = FVector2D(outVertices[1]);
				FArmyRoomSpaceArea::SurfaceHoleInfo tempInfo;
				tempInfo.surfaceHoleType = iter.Pin()->GetType();
				const FVector& bottomStart = FVector(outStart, OffsetGroundHeight);
				const FVector& botoomEnd = FVector(outEnd, OffsetGroundHeight);
				const FVector& topStart = bottomStart + FVector(0, 0, 1) * height;
				const FVector& topEnd = botoomEnd + FVector(0, 0, 1) * height;
				TArray<FVector> tempInnearHoles = { bottomStart, topStart,topEnd,botoomEnd };

				for (int j = 0; j < tempInnearHoles.Num(); ++j)
				{
					float x = FVector::DotProduct((tempInnearHoles[j] - InPlanceCenter), InPlaneXDir);
					float y = FVector::DotProduct((tempInnearHoles[j] - InPlanceCenter), InPlaneYDir);
					tempInnearHoles[j] = FVector(x, y, 0.0f);
				}
				results.Emplace(tempInnearHoles);
			}

		}
	}

}

void FArmySceneData::ConvertBspWallInfoToSurfaceHole(const FBSPWallInfo& InWallInfo, FVector& InXDir, FVector& InYDir, FVector& InCenter, uint32& OutSurfaceType, TArray<FVector>& orginalVertices, TArray<TArray<FVector>>& AreaHoles)
{

	if (InWallInfo.Normal.Equals(FVector(0, 0, 1), 0.0001f))
	{
		FBox box(InWallInfo.PolyVertices[0]);
		InCenter = box.GetCenter();
		InXDir = FVector(1, 0, 0);
		InYDir = FVector(0, 1, 0);
		OutSurfaceType = 0;
	}
	else if (InWallInfo.Normal.Equals(FVector(0, 0, -1), 0.0001f))
	{
		FBox box(InWallInfo.PolyVertices[0]);
		InCenter = box.GetCenter();
		InYDir = FVector(0, 1, 0);
		InXDir = FVector(-1, 0, 0);
		OutSurfaceType = 2;
	}
	else
	{
		FBox box(InWallInfo.PolyVertices[0]);
		InCenter = box.GetCenter();
		InYDir = FVector(0, 0, -1);
		InXDir = (InWallInfo.Normal ^ InYDir).GetSafeNormal();
		OutSurfaceType = 1;
	}
	int vertListNumber = InWallInfo.PolyVertices.Num();
	for (int index = 0; index < vertListNumber; ++index)
	{
		int number = InWallInfo.PolyVertices[index].Num();
		TArray<FVector> tempVertices;
		for (int j = 0; j < number; ++j)
		{
			float x = FVector::DotProduct((InWallInfo.PolyVertices[index][j] - InCenter), InXDir);
			float y = FVector::DotProduct((InWallInfo.PolyVertices[index][j] - InCenter), InYDir);
			tempVertices.Push(FVector(x, y, 0.0f));
		}
		if (index == 0)
		{
			orginalVertices = tempVertices;
		}
		else
		{
			AreaHoles.Push(tempVertices);
		}
	}
}

TSharedPtr<FArmyRoomSpaceArea> FArmySceneData::GetWallByRoomIdAndAttachWallLineId(const FString& AttachRoomId, const FString& AttachWallLineId)
{
	TArray<TWeakPtr<FArmyObject>> AllWalls;
	GetObjects(E_HardModel, OT_RoomSpaceArea, AllWalls);
	for (TWeakPtr<FArmyObject> iter : AllWalls)
	{
		TSharedPtr<FArmyRoomSpaceArea> wall = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (wall->AttachBspID == AttachWallLineId&&wall->AttachRoomID == AttachRoomId)
		{
			return wall;
		}
	}
	return NULL;
}

void FArmySceneData::UpdateOutWallInfo(TSharedPtr<FArmyRoom> OuterRoom)
{
	TArray<FBSPWallInfo> OutWallInfos;

	TArray<TSharedPtr<FArmyPunch>> RoomPunchs;

	TArray<TWeakPtr<FArmyObject>> TotalRoomPuchs;
	GetObjects(E_HomeModel, OT_Punch, TotalRoomPuchs);
	for (TWeakPtr<FArmyObject> iter : TotalRoomPuchs)
	{
		TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());
		RoomPunchs.Push(tempPunch);
	}
	TArray <TSharedPtr<FArmyWallLine>> allRoomInnearWalls = OuterRoom->GetWallLines();
	for (TSharedPtr<FArmyWallLine> iter : allRoomInnearWalls)
	{
		// 计算每个房间的内墙线
		// 获取每个房间内墙线上所有的门和窗户
		FBSPWallInfo wallInfo;
		wallInfo.UniqueIdForRoomOrHardware = OuterRoom->GetUniqueID().ToString();
		wallInfo.UniqueId = iter->GetUniqueID().ToString();
		wallInfo.GenerateFromObjectType = OT_OutRoom;
		TSharedPtr<FArmyLine> lineInfo = iter->GetCoreLine();
		const FVector& wallStart = lineInfo->GetStart();
		const FVector& wallEnd = lineInfo->GetEnd();
		float tempWallHeight = WallHeight + BottomOrTopWallThick;
		float tempOffset = BottomOrTopWallThick;
		TArray<FVector> wallVertices = { wallStart + FVector(0,0,-tempOffset) ,wallStart + FVector(0,0,1) * tempWallHeight,wallEnd + FVector(0,0,1) * tempWallHeight,wallEnd + FVector(0,0,-tempOffset) };
		wallInfo.PolyVertices.Push(wallVertices);
		FVector normal = iter->GetNormal();
		wallInfo.Normal = normal;


		CalculateWallAttachHardwareInfo(iter, wallInfo);
		CalculateRoomPunchHoles(RoomPunchs, iter, wallInfo, true);
		OutWallInfos.Push(wallInfo);
	}

	TArray<FVector>	BottomOutRoomVerties = OuterRoom->GetWorldPoints(true);
	TArray<FVector> TopBottomOutRoomVertices;
	for (FVector& iter : BottomOutRoomVerties)
	{
		iter = iter - FVector(0, 0, BottomOrTopWallThick);
	}
	for (const FVector& iter : BottomOutRoomVerties)
	{
		TopBottomOutRoomVertices.Push(iter + FVector(0, 0, WallHeight + BottomOrTopWallThick * 2.0f));
	}
	FBSPWallInfo topWallInfo, BottomWallInfo;
	// 计算房顶外墙体
	topWallInfo.PolyVertices.Add(TopBottomOutRoomVertices);
	TArray<TWeakPtr<FArmyObject>> AllPunches;
	GetObjects(E_HomeModel, OT_Punch, AllPunches);
	for (TWeakPtr<FArmyObject> iter : AllPunches)
	{
		TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());
		if ((tempPunch->GetHeight() + tempPunch->GetHeightToFloor()) > WallHeight)
		{
			TArray<FVector> outverts = tempPunch->GetClipingBox();
			for (FVector& TempIter : outverts)
			{
                TempIter = FVector(TempIter.X, TempIter.Y, WallHeight + BottomOrTopWallThick * 2.0f);
			}
			topWallInfo.PolyVertices.Push(outverts);
		}
	}
	topWallInfo.Normal = FVector(0, 0, 1);
	BottomWallInfo.PolyVertices.Add(BottomOutRoomVerties);
	BottomWallInfo.Normal = FVector(0, 0, -1);
	OutWallInfos.Push(topWallInfo);
	OutWallInfos.Push(BottomWallInfo);
	//@打扮家 XRLightmass 临时 不生成外部墙体
	if (TotalOutRoomActor == NULL)
	{
		FActorSpawnParameters ActorParam;
		ActorParam.Name = FName(TEXT("NOLIST-OUTERWALL"));

		TotalOutRoomActor = GVC->GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FTransform::Identity, ActorParam);
		TotalOutRoomActor->BoardMesh->bBuildStaticLighting = true;
		//@马云龙 只需要极小的光照图分辨率即可
		TotalOutRoomActor->BoardMesh->SetLightmapResolution(32);
		//@马云龙 双面烘焙，否则太阳光会穿透外墙，照射到室内
		TotalOutRoomActor->BoardMesh->LightmassSettings.bUseTwoSidedLighting = true;
		TotalOutRoomActor->bIsSelectable = false;
		TotalOutRoomActor->Tags.Add(TEXT("Immovable"));
		// zengy 外墙添加一个Tag用于分辨
		TotalOutRoomActor->Tags.Add(TEXT("OuterWall"));
	}
	TArray<FDynamicMeshVertex> TotalOutRoomVerts;
	for (const FBSPWallInfo& tempInfo : OutWallInfos)
	{
		TArray<FVector> orignVertices;
		TArray<TArray<FVector>> AreaHoles;
		uint32 SurfaceType = 0;
		FVector planeCenterPos, planeXDir, planeYDir;

		ConvertBspWallInfoToSurfaceHole(tempInfo, planeXDir, planeYDir, planeCenterPos, SurfaceType, orignVertices, AreaHoles);
		TArray<FVector2D> Outresults;
		TArray<TArray<FVector>> ClippResults;
		Translate::ClippPolygonHoleArea(orignVertices, AreaHoles, ClippResults);
		float InArea = 0.0f;
		for (TArray<FVector> iter : ClippResults)
		{
			InArea += FArmyMath::CalcPolyArea(iter);
		}
		float outArea = FArmyMath::CalcPolyArea(orignVertices);
		if ((outArea - InArea) > 0.2f)
		{
			Translate::TriangleAreaWithHole(orignVertices, ClippResults, Outresults);
			FBox box(orignVertices);
			int TraianglePoints = Outresults.Num();
			FArmyMath::ReverPointList(Outresults);
			FVector Normal = FVector::CrossProduct(planeXDir, planeYDir);
			for (int index = 0; index < TraianglePoints; ++index)
			{
				float VCoord, UCoord;
				UCoord = (Outresults[index].X - box.Min.X) * 10.0f / 1024;
				VCoord = (Outresults[index].Y - box.Min.Y) *10.0f / 1024;
				TotalOutRoomVerts.Push(FDynamicMeshVertex(planeXDir * Outresults[index].X + planeYDir * Outresults[index].Y + planeCenterPos, FVector(0, 1, 0), Normal, FVector2D(UCoord, VCoord), FColor::White));
			}
		}

	}
	//@打扮家 XRLightmass 临时 不生成外部墙体
	// 统一更新外墙体
	TotalOutRoomActor->ResetMeshTriangles();
	TotalOutRoomActor->AddVerts(TotalOutRoomVerts);
	TotalOutRoomActor->UpdateAllVetexBufferIndexBuffer();
	TotalOutRoomActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
}

void FArmySceneData::CalculateOuterRoomSimpleComponent(const TArray< TPair<FString, TArray<TArray<FVector>>> > &InFinishInnerRoomsRoofInfos)
{
	TArray<TArray<CSG::FCSGPolygon>> RoomCSGPolygonGroups;
	for (const auto &FinishInnerRoomInfo : InFinishInnerRoomsRoofInfos)
	{
		const TArray<TArray<FVector>> &FinishInnerRoomVertArr = FinishInnerRoomInfo.Value;
		if (FinishInnerRoomVertArr.Num() == 0)
			continue;

		// 房间轮廓减去柱子、风道等形成的洞
		Clipper clipper;
		Paths ClipperSubjects;
		ClipperSubjects.resize(1);
		for (const auto &FinishInnerRoomVert : FinishInnerRoomVertArr[0])
		{
			cInt x = FMath::RoundToInt(FinishInnerRoomVert.X * 100.0f);
			cInt y = FMath::RoundToInt(FinishInnerRoomVert.Y * 100.0f);
			ClipperSubjects[0].emplace_back(x, y);
		}
		Paths ClipperClips;
		ClipperClips.resize(FinishInnerRoomVertArr.Num() - 1);
		for (int32 i = 1; i < FinishInnerRoomVertArr.Num(); ++i)
		{
			for (const auto &FinishInnerRoomVert : FinishInnerRoomVertArr[i])
			{
				cInt x = FMath::RoundToInt(FinishInnerRoomVert.X * 100.0f);
				cInt y = FMath::RoundToInt(FinishInnerRoomVert.Y * 100.0f);
				ClipperClips[i - 1].emplace_back(x, y);
			}
		}
		clipper.AddPaths(ClipperSubjects, ptSubject, true);
		clipper.AddPaths(ClipperClips, ptClip, true);
		Paths ClipperSolution;
		clipper.Execute(ctDifference, ClipperSolution, pftEvenOdd, pftEvenOdd);
		TArray<FVector> FinishInnerRoomVerts;
		if (ClipperSolution.size() > 0)
		{
			TArray<FVector> ResultVerts;
			const Path &p = ClipperSolution[0];
			for (const auto &IntPt : p)
				FinishInnerRoomVerts.Emplace(IntPt.X * 0.01f, IntPt.Y * 0.01f, 0.0f);
		}
		if (FinishInnerRoomVerts.Num() == 0)
			continue;

		// 逆时针的房间要转换为顺时针
		TArray<FVector2D> TempFinishInnerRoomVerts2D;
		for (const auto &Vert : FinishInnerRoomVerts)
			TempFinishInnerRoomVerts2D.Emplace(Vert);
		if (!FArmyMath::IsClockWise(TempFinishInnerRoomVerts2D))
			FArmyMath::ReverPointList(TempFinishInnerRoomVerts2D);
		for (int32 i = 0; i < TempFinishInnerRoomVerts2D.Num(); ++i)
			FinishInnerRoomVerts[i] = FVector(TempFinishInnerRoomVerts2D[i], 0.0f);

		// 构建房间侧面
		int32 n = FinishInnerRoomVerts.Num();
		TArray<CSG::FCSGPolygon> RoomCSGPolygons;
		for (int32 i = 0; i < n; ++i)
		{
			TArray<CSG::FCSGVertex> RoomSideVertices;
			RoomSideVertices.Emplace(FinishInnerRoomVerts[i % n]);
			RoomSideVertices.Emplace(FinishInnerRoomVerts[(i + 1) % n]);
			RoomSideVertices.Emplace(FinishInnerRoomVerts[(i + 1) % n] + FVector::UpVector * WallHeight);
			RoomSideVertices.Emplace(FinishInnerRoomVerts[i % n] + FVector::UpVector * WallHeight);
			RoomCSGPolygons.Emplace(RoomSideVertices);
		}

		// 房间地面
		TArray<CSG::FCSGVertex> RoomFloorVertices;
		for (int32 i = n - 1; i >= 0; --i)
			RoomFloorVertices.Emplace(FinishInnerRoomVerts[i]);
		RoomCSGPolygons.Emplace(RoomFloorVertices);

		// 房间顶面
		TArray<CSG::FCSGVertex> RoomRoofVertices;
		for (int32 i = 0; i < n; ++i)
			RoomRoofVertices.Emplace(FinishInnerRoomVerts[i]);
		RoomCSGPolygons.Emplace(RoomRoofVertices);

		RoomCSGPolygonGroups.Emplace(RoomCSGPolygons);
	}

	// 构建梁CSGNode
	TArray<TWeakPtr<FArmyObject>> ObjectList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Beam, ObjectList);
	for (const auto & Object : ObjectList)
	{
		const TSharedPtr<FArmyObject> &SimpleComponent = Object.Pin();

		TArray<FVector> BoundaryVerts;
		SimpleComponent->GetVertexes(BoundaryVerts);
		float tempheight = 0.0f;
		EObjectType ObjType = SimpleComponent->GetType();
		switch (ObjType)
		{
			case OT_Beam:
			case OT_AirFlue:
			case OT_Pillar:
			{
				FArmySimpleComponent *tempBeam = SimpleComponent->AsassignObj<FArmySimpleComponent>();
				tempheight = tempBeam->GetHeight();	
				break;
			}
			case OT_PackPipe:
			{
				FArmyPackPipe* tempPipe = SimpleComponent->AsassignObj<FArmyPackPipe>();
				tempheight = tempPipe->GetHeight();
				break;
			}
		}

		TArray<FVector2D> NewBoundarys;
		for (const FVector& TempIter : BoundaryVerts)
			NewBoundarys.Emplace(FVector2D(TempIter));

		TArray<FVector2D> CleanPoly;
		FArmyMath::CleanPolygon(NewBoundarys, CleanPoly);
		if (!FArmyMath::IsClockWise(CleanPoly))
			FArmyMath::ReverPointList(CleanPoly);

		int32 n = CleanPoly.Num();
		TArray<FVector> ExturdeVerts;
		ExturdeVerts.AddUninitialized(n);
		for (int32 i = 0; i < n; ++i)
			ExturdeVerts[i] = FVector(CleanPoly[i], 0.0f);

		float BeamBottomHeight = WallHeight - tempheight;

		// 构建梁侧面的CSGPolygon
		TArray<CSG::FCSGPolygon> BeamCSGPolygons;
		for (int32 i = 0; i < n; ++i)
		{
			const FVector& extrudeStartPos = ExturdeVerts[i % n];
			const FVector& extrudeEndPos = ExturdeVerts[(i + 1) % n];

			FVector NewStartBottom, NewStartTop, NewEndTop, NewEndBottom;
			NewStartBottom = extrudeStartPos + FVector::UpVector * BeamBottomHeight;
			NewStartTop = extrudeStartPos + FVector::UpVector * WallHeight;
			NewEndTop = extrudeEndPos + FVector::UpVector * WallHeight;
			NewEndBottom = extrudeEndPos + FVector::UpVector * BeamBottomHeight;

			TArray<CSG::FCSGVertex> BeamSideVertices;
			BeamSideVertices.Emplace(NewStartBottom);
			BeamSideVertices.Emplace(NewStartTop);
			BeamSideVertices.Emplace(NewEndTop);
			BeamSideVertices.Emplace(NewEndBottom);
			BeamCSGPolygons.Emplace(BeamSideVertices);
		}

		// 构建梁底面的CSGPolygon
		TArray<CSG::FCSGVertex> BeamBottomVertices;
		for (int32 i = n - 1; i >= 0; --i)
			BeamBottomVertices.Emplace(ExturdeVerts[i] + FVector::UpVector * BeamBottomHeight);
		BeamCSGPolygons.Emplace(BeamBottomVertices);

		// 构建梁顶面的CSGPolygon
		TArray<CSG::FCSGVertex> BeamTopVertices;
		for (int32 i = 0; i < n; ++i)
			BeamTopVertices.Emplace(ExturdeVerts[i] + FVector::UpVector * WallHeight);
		BeamCSGPolygons.Emplace(BeamTopVertices);

		
		for (int32 i = 0; i < RoomCSGPolygonGroups.Num(); ++i)
		{
			CSG::FCSGNode *RoomCSGNode = CSG::MakeCSGNode(RoomCSGPolygonGroups[i]);
			CSG::FCSGNode *BeamCSGNode = CSG::MakeCSGNode(BeamCSGPolygons);
			CSG::Intersect(RoomCSGNode, BeamCSGNode);
			TArray<CSG::FCSGPolygon> Polygons = RoomCSGNode->GetFlattenedPolygons();
			CSG::DestroyCSGNode(RoomCSGNode);
			CSG::DestroyCSGNode(BeamCSGNode);

			// 将梁的底面进行合并
			// 当梁横跨柱子或墙的时候，经过布尔运算后梁会被切分为多个部分
			Clipper clipper;
			Paths ClipperSubjects;
			int32 nStart = 0;
			for (; nStart < Polygons.Num(); ++nStart)
			{
				FVector PlaneNormal(Polygons[nStart].Plane.X, Polygons[nStart].Plane.Y, Polygons[nStart].Plane.Z);
				if (PlaneNormal.Equals(-FVector::UpVector) && FMath::IsNearlyEqual(FMath::Abs(Polygons[nStart].Plane.W), BeamBottomHeight, 0.1f))
				{
					Path Subject;
					for (int j = 0; j < Polygons[nStart].Vertices.Num(); ++j)
					{
						cInt x = FMath::RoundToInt(Polygons[nStart].Vertices[j].P.X * 100.0f);
						cInt y = FMath::RoundToInt(Polygons[nStart].Vertices[j].P.Y * 100.0f);
						Subject.push_back(IntPoint(x, y));
					}
					ClipperSubjects.emplace_back(Subject);
					break;
				}
			}
			if (ClipperSubjects.size() == 0)
				continue;
			
			Paths ClipperClips;
			for (++nStart; nStart < Polygons.Num(); ++nStart)
			{
				FVector PlaneNormal(Polygons[nStart].Plane.X, Polygons[nStart].Plane.Y, Polygons[nStart].Plane.Z);
				if (PlaneNormal.Equals(-FVector::UpVector) && FMath::IsNearlyEqual(FMath::Abs(Polygons[nStart].Plane.W), BeamBottomHeight, 0.1f))
				{
					Path Clip;
					for (int j = 0; j < Polygons[nStart].Vertices.Num(); ++j)
					{
						cInt x = FMath::RoundToInt(Polygons[nStart].Vertices[j].P.X * 100.0f);
						cInt y = FMath::RoundToInt(Polygons[nStart].Vertices[j].P.Y * 100.0f);
						Clip.push_back(IntPoint(x, y));
					}
					ClipperClips.emplace_back(Clip);
				}
			}
			clipper.AddPaths(ClipperSubjects, ptSubject, true);
			Paths ClipperSolution;
			if (ClipperClips.size() > 0)
			{
				clipper.AddPaths(ClipperClips, ptClip, true);
				clipper.Execute(ctUnion, ClipperSolution, pftEvenOdd, pftEvenOdd);
			}
			else
				ClipperSolution = ClipperSubjects;
			if (ClipperSolution.size() > 0)
			{
				int32 SolutionIndex = 0;
				for (const auto &p : ClipperSolution)
				{
					TArray<FVector> ResultVerts;
					for (const auto &IntPt : p)
						ResultVerts.Emplace(IntPt.X * 0.01f, IntPt.Y * 0.01f, 0.0f);
					CalculateSimpleComponent(Object.Pin(), FString::Printf(TEXT("%s-%d"), *(InFinishInnerRoomsRoofInfos[i].Key), SolutionIndex++), ResultVerts);
				}
			}
		}
	}
}

void FArmySceneData::CalculateInnerRoomSimpleComponent(TSharedPtr<FArmyRoom> InRoom)
{
	TArray<TWeakPtr<FArmyObject>> Results;
	InRoom->GetObjectsRelevance(Results);

	TArray<FVector> RoomPoints = InRoom-> GetWorldPoints(true);

	for (TWeakPtr<FArmyObject> iter : Results)
	{
		//@郭子阳
		//过滤掉中心点在房间之外的柱子 风道
		if (iter.Pin()->GetType() == OT_Pillar)
		{
			TSharedPtr<FArmyPillar> Component = StaticCastSharedPtr<FArmyPillar>(iter.Pin());
			if (!FArmyMath::IsPointInOrOnPolygon2D(Component->GetPos(), RoomPoints))
			{
				continue;
			}
		}
		else if (iter.Pin()->GetType() == OT_AirFlue)
		{
			TSharedPtr<FArmyAirFlue> Component = StaticCastSharedPtr<FArmyAirFlue>(iter.Pin());
			if (!FArmyMath::IsPointInOrOnPolygon2D(Component->GetPos(), RoomPoints))
			{
				continue;
			}
		}

		CalculateSimpleComponent(iter.Pin());
	}
}

void FArmySceneData::CaculateWindowHoleSurface()
{
	TArray<TWeakPtr<FArmyObject>> TotalRectBayOrTrapeBayWindow;
	GetObjects(E_LayoutModel, OT_RectBayWindow, TotalRectBayOrTrapeBayWindow);
	GetObjects(E_LayoutModel, OT_TrapeBayWindow, TotalRectBayOrTrapeBayWindow);
	TArray<FString> TotalBayWindowIds;
	for (TWeakPtr<FArmyObject> BayItr : TotalRectBayOrTrapeBayWindow)
	{
		TSharedPtr<FArmyRectBayWindow> TempBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(BayItr.Pin());
		TotalBayWindowIds.Push(TempBayWindow->GetUniqueID().ToString());
	}
	TArray<TWeakPtr<FArmyObject>> TotalRoomAreas;
	GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomAreas);

	TArray<TSharedPtr<FArmyRoomSpaceArea>> BayWindowAreas;
    if (TotalBayWindowIds.Num() > 0)
    {
        for (TWeakPtr<FArmyObject> iter : TotalRoomAreas)
        {
            TSharedPtr<FArmyRoomSpaceArea> TempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
            if (TotalBayWindowIds.Find(TempRoom->AttachRoomID) != INDEX_NONE)
            {
                BayWindowAreas.Push(TempRoom);
            }
        }
    }

	TArray<TWeakPtr<FArmyObject>> AllWindowHoleObjects;
	for (auto ObjIt : GetObjects(E_LayoutModel))
	{
		if (ObjIt.Pin()->GetType() == OT_Window
			|| ObjIt.Pin()->GetType() == OT_FloorWindow
			|| ObjIt.Pin()->GetType() == OT_Pass
			|| ObjIt.Pin()->GetType() == OT_NewPass
			|| ObjIt.Pin()->GetType() == OT_Door
			|| ObjIt.Pin()->GetType() == OT_SlidingDoor
			|| ObjIt.Pin()->GetType() == OT_SecurityDoor
			|| ObjIt.Pin()->GetType() == OT_RectBayWindow
			|| ObjIt.Pin()->GetType() == OT_TrapeBayWindow
			|| ObjIt.Pin()->GetType() == OT_IndependentWall
			)
		{

			AllWindowHoleObjects.Add(ObjIt);
		}
	}
	TArray<TWeakPtr<FArmyObject>> TotalAddWalls;
	GetObjects(E_ModifyModel, OT_AddWall, TotalAddWalls);
	for (TWeakPtr<FArmyObject> iter : TotalAddWalls)
	{
		FArmyAddWall* halfAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
		if (halfAddWall && halfAddWall->GetIsHalfWall())
		{
			AllWindowHoleObjects.Add(iter);
		}
	}

	// 进行飘窗左右侧墙面和当前飘窗类型的同步，如果当前飘窗类型不包含特定侧的墙面，需要将该墙面删除
	auto SyncRectBayWindowBoard = [this](const TSharedPtr<FArmyRectBayWindow> &InRectBayWindow, const TArray<TSharedPtr<FArmyRoomSpaceArea>> &InAllSpaceAreas) {
		FArmyRectBayWindow::BayWindowType Type = InRectBayWindow->GetWindowType();
		FString GuidStr = InRectBayWindow->GetUniqueID().ToString();
		switch (Type)
		{
			case FArmyRectBayWindow::AntiBoard:
			{
				FString LeftBoardGuidStr = GuidStr + TEXT("LeftBoard");
				FString RightBoardGuidStr = GuidStr + TEXT("RightBoard");
				const TSharedPtr<FArmyRoomSpaceArea> *LeftBoardPtr = InAllSpaceAreas.FindByPredicate([&LeftBoardGuidStr](const TSharedPtr<FArmyRoomSpaceArea> &Data) -> bool {
					return Data->AttachBspID == LeftBoardGuidStr;
				});
				if (LeftBoardPtr)
					Delete(*LeftBoardPtr);
				const TSharedPtr<FArmyRoomSpaceArea> *RightBoardPtr = InAllSpaceAreas.FindByPredicate([&RightBoardGuidStr](const TSharedPtr<FArmyRoomSpaceArea> &Data) -> bool {
					return Data->AttachBspID == RightBoardGuidStr;
				});
				if (RightBoardPtr)
					Delete(*RightBoardPtr);
				break;
			}
			case FArmyRectBayWindow::LeftBoard:
			{
				FString RightBoardGuidStr = GuidStr + TEXT("RightBoard");
				const TSharedPtr<FArmyRoomSpaceArea> *RightBoardPtr = InAllSpaceAreas.FindByPredicate([&RightBoardGuidStr](const TSharedPtr<FArmyRoomSpaceArea> &Data) -> bool {
					return Data->AttachBspID == RightBoardGuidStr;
				});
				if (RightBoardPtr)
					Delete(*RightBoardPtr);
				break;
			}
			case FArmyRectBayWindow::RightBoard:
			{
				FString LeftBoardGuidStr = GuidStr + TEXT("LeftBoard");
				const TSharedPtr<FArmyRoomSpaceArea> *LeftBoardPtr = InAllSpaceAreas.FindByPredicate([&LeftBoardGuidStr](const TSharedPtr<FArmyRoomSpaceArea> &Data) -> bool {
					return Data->AttachBspID == LeftBoardGuidStr;
				});
				if (LeftBoardPtr)
					Delete(*LeftBoardPtr);
				break;
			}
		}
	};
	for (TWeakPtr<FArmyObject> ObjItr : AllWindowHoleObjects)
	{
		if (ObjItr.Pin()->GetType() == OT_FloorWindow
			|| ObjItr.Pin()->GetType() == OT_Pass
			|| ObjItr.Pin()->GetType() == OT_Door
			|| ObjItr.Pin()->GetType() == OT_SlidingDoor
			|| ObjItr.Pin()->GetType() == OT_SecurityDoor
			|| ObjItr.Pin()->GetType() == OT_RectBayWindow
			|| ObjItr.Pin()->GetType() == OT_TrapeBayWindow
			|| ObjItr.Pin()->GetType() == OT_Window
			|| ObjItr.Pin()->GetType() == OT_NewPass
			)
		{
			TSharedPtr<FArmyHardware> tempHardWare = StaticCastSharedPtr<FArmyHardware>(ObjItr.Pin());
			TArray<FBSPWallInfo> OutResults;
			if (ObjItr.Pin()->GetType() == OT_RectBayWindow || ObjItr.Pin()->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> rectBay = StaticCastSharedPtr<FArmyRectBayWindow>(ObjItr.Pin());
				// 飘窗侧面同步
				SyncRectBayWindowBoard(rectBay, BayWindowAreas);
				rectBay->GetWindowWallVertices(OutResults);
			}

			tempHardWare->GetAttachHoleWallInfo(OutResults);
			int number = OutResults.Num();
			for (int i = 0; i < number; i += 2)
			{
				CaculateSurfaceInfoWhithWallInfo(OutResults[i + 1], OutResults[i], 1);
			}


		}
		else if (ObjItr.Pin()->GetType() == OT_IndependentWall || ObjItr.Pin()->GetType() == OT_AddWall)
		{
			FArmyAddWall* tempIndependentWall = ObjItr.Pin()->AsassignObj<FArmyAddWall>();
			TArray<FBSPWallInfo> OutResults;
			if (tempIndependentWall)
			{
				tempIndependentWall->GetAttachHoleWallInfo(OutResults);
				int number = OutResults.Num();
				for (int i = 0; i < number; i += 2)
				{
					CaculateSurfaceInfoWhithWallInfo(OutResults[i + 1], OutResults[i], 1);
				}
			}
		}

	}

}

void FArmySceneData::GetHardwareRelateWalllines(TWeakPtr<FArmyObject> InHardWare, TArray<TSharedPtr<class FArmyWallLine>>& AttachWalllines)
{
	TArray<TWeakPtr<FArmyObject>> TotalInnearRooms;
	TArray<TWeakPtr<FArmyObject>> TotalOutRooms;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRooms);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, TotalOutRooms);
	for (TWeakPtr<FArmyObject> iterInner : TotalInnearRooms)
	{
		TSharedPtr<FArmyRoom> tempInnerRoom = StaticCastSharedPtr<FArmyRoom>(iterInner.Pin());
		TArray<TSharedPtr<FArmyWallLine>> RoomWallLines = tempInnerRoom->GetWallLines();
		for (TSharedPtr<FArmyWallLine> wallIter : RoomWallLines)
		{
			TArray<TWeakPtr<FArmyObject>> attachHards;
			wallIter->GetAppendObjects(attachHards);
			if (attachHards.Find(InHardWare) != INDEX_NONE)
			{
				AttachWalllines.Push(wallIter);
				break;
			}
		}
	}
	for (TWeakPtr<FArmyObject> iterOut : TotalOutRooms)
	{
		TSharedPtr<FArmyRoom> tempOutRoom = StaticCastSharedPtr<FArmyRoom>(iterOut.Pin());
		TArray<TSharedPtr<FArmyWallLine>> OutRoomLines = tempOutRoom->GetWallLines();
		for (TSharedPtr<FArmyWallLine> iter : OutRoomLines)
		{
			TArray<TWeakPtr<FArmyObject>> atthards;
			iter->GetAppendObjects(atthards);
			if (atthards.Find(InHardWare) != INDEX_NONE)
			{
				AttachWalllines.Push(iter);
				break;
			}
		}
	}
}

void FArmySceneData::CaculatePunchSurface()
{
	TArray<TWeakPtr<FArmyObject>> TotalPunches;
	GetObjects(E_HomeModel, OT_Punch, TotalPunches);
	TArray<TWeakPtr<FArmyObject>> TotalInnearRoom;
	TArray<TWeakPtr<FArmyObject>> TotalOutRoom;
	GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRoom);
	GetObjects(E_LayoutModel, OT_OutRoom, TotalOutRoom);
	if (TotalOutRoom.Num() != 1)
		return;
	TArray<TWeakPtr<FArmyObject>> TotalRoomSpaces;
	GetObjects(E_HardModel, OT_RoomSpaceArea, TotalRoomSpaces);
	TArray<FString> TotalPunchIDS;
	for (TWeakPtr<FArmyObject> iter : TotalPunches)
	{
		TotalPunchIDS.Push(iter.Pin()->GetUniqueID().ToString());
	}
	for (TWeakPtr<FArmyObject> iter : TotalRoomSpaces)
	{
		FArmyRoomSpaceArea* temp = iter.Pin()->AsassignObj<FArmyRoomSpaceArea>();
		if (TotalPunchIDS.Find(temp->AttachRoomID) != INDEX_NONE)
		{
			Delete(iter.Pin());
		}
	}
	TWeakPtr<FArmyObject> outroom = TotalOutRoom[0];
	FArmyRoom* tempOutRoom = outroom.Pin()->AsassignObj<FArmyRoom>();
	TArray<FVector> OutRoomVerts = tempOutRoom->GetWorldPoints(true);
	for (TWeakPtr<FArmyObject> iter : TotalPunches)
	{
		TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());

		TArray<FVector> PunchOutVertices = tempPunch->GetClipingBox();
		TArray<TArray<FVector>> PunchIntersectRooms;

		for (TWeakPtr<FArmyObject> iter0 : TotalInnearRoom)
		{
			FArmyRoom* tempInnearRoom = iter0.Pin()->AsassignObj<FArmyRoom>();
			TArray<FVector> roomVerts = tempInnearRoom->GetWorldPoints(true);
			TArray<FVector> result;
			if (Translate::ArePolysOverlap(PunchOutVertices, roomVerts, result))
			{
				FArmyMath::ReversePointList(roomVerts);
				PunchIntersectRooms.Add(roomVerts);
			}
		}

		FArmyMath::ReversePointList(PunchOutVertices);
		BspPolygon2d* punchPolygon = new BspPolygon2d(PunchOutVertices);
		BspPolygon2d* outPolygon = new BspPolygon2d(OutRoomVerts, PunchIntersectRooms);

		BspPolygon2d result;
		BspPolygon2d neg = ~(*punchPolygon);
		outPolygon->GetInsideEdgesFrom(neg, result);
		int numberOfEdges = result.GetNumEdges();
		float doubleSideHeight = tempPunch->GetHeight();
		if (tempPunch->GetHeight() + tempPunch->GetHeightToFloor() > WallHeight)
		{
			doubleSideHeight = WallHeight + BottomOrTopWallThick - tempPunch->GetHeightToFloor();
		}
		for (int i = 0; i < numberOfEdges; i++)
		{
			Edge2d edge;
			result.GetEdge(i, edge);

			FVector2D intersectStart, intersectEnd;
			result.GetVertex(edge.I0, intersectStart);
			result.GetVertex(edge.I1, intersectEnd);

			FVector tempStart = FVector(intersectStart, 0.0) + FVector(0, 0, 1) * tempPunch->GetHeightToFloor();
			FVector tempStarUp = FVector(intersectStart, 0.0) + FVector(0, 0, 1) * (doubleSideHeight + tempPunch->GetHeightToFloor());
			FVector tempEnd = FVector(intersectEnd, 0.0) + FVector(0, 0, 1)*tempPunch->GetHeightToFloor();
			FVector tempEndUp = FVector(intersectEnd, 0.0) + FVector(0, 0, 1)*(doubleSideHeight + tempPunch->GetHeightToFloor());
			TArray<FVector> clippVerts = { tempStart,tempStarUp,tempEndUp,tempEnd };
			FVector dir = (tempEnd - tempStart).GetSafeNormal();
			FBSPWallInfo sideWallInfo, sideWallFinishInfo;
			sideWallInfo.GenerateFromObjectType = sideWallFinishInfo.GenerateFromObjectType = OT_Punch;
			sideWallInfo.UniqueIdForRoomOrHardware = sideWallFinishInfo.UniqueIdForRoomOrHardware = iter.Pin()->GetUniqueID().ToString();
			sideWallInfo.Normal = sideWallFinishInfo.Normal = dir.RotateAngleAxis(90, FVector(0, 0, 1));
			FString IndexName = FString::Printf(TEXT("%d"), i);

			sideWallInfo.UniqueId = sideWallFinishInfo.UniqueId = iter.Pin()->GetUniqueID().ToString() + TEXT("Punch") + IndexName;
			sideWallInfo.PolyVertices.Push(clippVerts);
			TArray<FVector> clippVertsFinish;
			clippVertsFinish.AddUninitialized(4);
			for (int j = 0; j < 4; j++)
			{
				clippVertsFinish[j] = clippVerts[j] - sideWallInfo.Normal * FinishWallThick;
			}
			sideWallFinishInfo.PolyVertices.Emplace(clippVertsFinish);
			CaculateSurfaceInfoWhithWallInfo(sideWallFinishInfo, sideWallInfo, 1);
		}
		delete punchPolygon;
		delete outPolygon;
		TArray<TArray<FVector>> ClippAreas;
		ClippAreas.Push(OutRoomVerts);
		ClippAreas.Append(PunchIntersectRooms);
		TArray<TArray<FVector>> IntersectionArea;
		Translate::PolygonIntersect(PunchOutVertices, ClippAreas, IntersectionArea);
		// 计算开阳台下侧面面片
		int bottomNumber = IntersectionArea.Num();
		for (int i = 0; i < bottomNumber; i++)
		{
			FBSPWallInfo bottomPunch, BottomPuchFinish;
			bottomPunch.GenerateFromObjectType = BottomPuchFinish.GenerateFromObjectType = OT_Punch;
			bottomPunch.UniqueIdForRoomOrHardware = BottomPuchFinish.UniqueIdForRoomOrHardware = iter.Pin()->GetUniqueID().ToString();
			bottomPunch.Normal = BottomPuchFinish.Normal = FVector(0, 0, 1);
			FString IndexName = FString::Printf(TEXT("%d"), i);
			bottomPunch.UniqueId = BottomPuchFinish.UniqueId = iter.Pin()->GetUniqueID().ToString() + TEXT("BottomPunch") + IndexName;
			TArray<FVector> BottomVerts, BottomVertsFinish;
			for (const FVector& vertItr : IntersectionArea[i])
			{
				BottomVerts.Push(vertItr + FVector(0, 0, 1) * tempPunch->GetHeightToFloor());
				BottomVertsFinish.Push(vertItr + FVector(0, 0, 1) * tempPunch->GetHeightToFloor() + FVector(0, 0, FinishWallThick));
			}
			bottomPunch.PolyVertices.Emplace(BottomVerts);
			BottomPuchFinish.PolyVertices.Emplace(BottomVertsFinish);
			CaculateSurfaceInfoWhithWallInfo(BottomPuchFinish, bottomPunch, 1);

		}
		// 计算上台面
		if (tempPunch->GetHeightToFloor() + tempPunch->GetHeight() < WallHeight + 0.1f)
		{
			for (int i = 0; i < bottomNumber; i++)
			{
				FBSPWallInfo TopPunch, TopPuchFinish;
				TopPunch.GenerateFromObjectType = TopPuchFinish.GenerateFromObjectType = OT_Punch;
				TopPunch.UniqueIdForRoomOrHardware = TopPuchFinish.UniqueIdForRoomOrHardware = iter.Pin()->GetUniqueID().ToString();
				TopPunch.Normal = TopPuchFinish.Normal = FVector(0, 0, -1);
				FString IndexName = FString::Printf(TEXT("%d"), i);
				TopPunch.UniqueId = TopPuchFinish.UniqueId = iter.Pin()->GetUniqueID().ToString() + TEXT("TopPunch") + IndexName;
				TArray<FVector> BottomVerts, BottomVertsFinish;
				for (const FVector& vertItr : IntersectionArea[i])
				{
					BottomVerts.Push(vertItr + FVector(0, 0, 1) * (tempPunch->GetHeightToFloor() + tempPunch->GetHeight()));
					BottomVertsFinish.Push(vertItr + FVector(0, 0, 1)* (tempPunch->GetHeightToFloor() + tempPunch->GetHeight()) + FVector(0, 0, -FinishWallThick));
				}
				TopPunch.PolyVertices.Push(BottomVerts);
				TopPuchFinish.PolyVertices.Push(BottomVertsFinish);
				CaculateSurfaceInfoWhithWallInfo(TopPuchFinish, TopPunch, 1);

			}
		}
		else
		{
			CaculatePunchTopSurface(tempPunch, OutRoomVerts, PunchIntersectRooms);
		}
	}
}

void FArmySceneData::CaculateSolidWall(const TArray<FVector>& HouseVerts, const TArray<TArray<FVector>>& TotalInneroomVerts)
{
	if (TotalSolidWallActor == NULL)
	{
		FActorSpawnParameters ActorParam;
		ActorParam.Name = FName(TEXT("NOLIST-WALLCOVER"));

		TotalSolidWallActor = GVC->GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FTransform::Identity, ActorParam);
		TotalSolidWallActor->Tags.Add(TEXT("Immovable"));

		TotalSolidWallActor->bIsSelectable = false;

	}
	TArray<FVector2D> Outresults;
	Translate::TriangleAreaWithHole(HouseVerts, TotalInneroomVerts, Outresults);
	FBox box(HouseVerts);
	int TraianglePoints = Outresults.Num();
	FArmyMath::ReverPointList(Outresults);
	FVector Normal = FVector(0, 0, 1);
	TArray<FDynamicMeshVertex> TotalOutRoomVerts;
	for (int index = 0; index < TraianglePoints; ++index)
	{
		float VCoord, UCoord;
		UCoord = (Outresults[index].X - box.Min.X) * 10.0f / 1024;
		VCoord = (Outresults[index].Y - box.Min.Y) *10.0f / 1024;
		TotalOutRoomVerts.Push(FDynamicMeshVertex(FVector(Outresults[index], -5.0f), FVector(0, 1, 0), Normal, FVector2D(UCoord, VCoord), FColor::White));
	}
	TotalSolidWallActor->ResetMeshTriangles();
	TotalSolidWallActor->AddVerts(TotalOutRoomVerts);
	TotalSolidWallActor->UpdateAllVetexBufferIndexBuffer();
	TotalSolidWallActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
}

void FArmySceneData::CaculatePunchTopSurface(TSharedPtr<FArmyPunch> punch, const TArray<FVector>& OutRoomVerts, const TArray<TArray<FVector>>& IntersectInnerooms)
{
	const TArray<FVector>& PunchVerts = punch->GetClipingBox();
	TArray<TArray<FVector>> tempReverse;
	for (TArray<FVector> iter : IntersectInnerooms)
	{
		FArmyMath::ReversePointList(iter);
		tempReverse.Push(iter);
	}
	BspPolygon2d* punchPolygon = new BspPolygon2d(PunchVerts);
	BspPolygon2d* outPolygon = new BspPolygon2d(tempReverse);
	BspPolygon2d result, coSame, coDiff;
	outPolygon->GetInsideOrCoSameCoDiff(*punchPolygon, result, coSame, coDiff);
	auto CaculatePuchTop = [this](const BspPolygon2d& bsp, TSharedPtr<FArmyPunch> punch, bool reverse, int inIndex = 0)
	{
		int numberOfEdges = bsp.GetNumEdges();
		for (int i = 0; i < numberOfEdges; i++)
		{
			Edge2d edge;
			bsp.GetEdge(i, edge);

			FVector2D intersectStart, intersectEnd;
			if (reverse)
			{
				bsp.GetVertex(edge.I1, intersectStart);
				bsp.GetVertex(edge.I0, intersectEnd);
			}
			else
			{
				bsp.GetVertex(edge.I0, intersectStart);
				bsp.GetVertex(edge.I1, intersectEnd);
			}
			FVector tempStart = FVector(intersectStart, 0.0) + FVector(0, 0, 1) * WallHeight;
			FVector tempStarUp = tempStart + FVector(0, 0, 1) *BottomOrTopWallThick;
			FVector tempEnd = FVector(intersectEnd, 0.0) + FVector(0, 0, 1)*WallHeight;
			FVector tempEndUp = tempEnd + FVector(0, 0, 1)*BottomOrTopWallThick;
			TArray<FVector> clippVerts = { tempStart,tempStarUp,tempEndUp,tempEnd };
			FVector dir = (tempEnd - tempStart).GetSafeNormal();
			FBSPWallInfo sideWallInfo, sideWallFinishInfo;
			sideWallInfo.GenerateFromObjectType = sideWallFinishInfo.GenerateFromObjectType = OT_Punch;
			sideWallInfo.UniqueIdForRoomOrHardware = sideWallFinishInfo.UniqueIdForRoomOrHardware = punch->GetUniqueID().ToString();
			sideWallInfo.Normal = sideWallFinishInfo.Normal = dir.RotateAngleAxis(90, FVector(0, 0, 1));
			FString IndexName = FString::Printf(TEXT("%d"), i + inIndex);

			sideWallInfo.UniqueId = sideWallFinishInfo.UniqueId = punch->GetUniqueID().ToString() + TEXT("TopPunchSurface") + IndexName;
			sideWallInfo.PolyVertices.Push(clippVerts);
			TArray<FVector> clippFinishVerts;
			clippFinishVerts.AddUninitialized(4);
			for (int j = 0; j < 4; j++)
			{
				clippFinishVerts[j] = clippVerts[j] - sideWallInfo.Normal * FinishWallThick;
			}
			sideWallFinishInfo.PolyVertices.Emplace(clippFinishVerts);
			CaculateSurfaceInfoWhithWallInfo(sideWallFinishInfo, sideWallInfo, 1);
		}
	};
	int inIndex = 0;
	CaculatePuchTop(result, punch, false, inIndex);
	inIndex += result.GetNumEdges() * 2;
	CaculatePuchTop(coSame, punch, false, inIndex);
	inIndex += coSame.GetNumEdges() * 2;
	CaculatePuchTop(coDiff, punch, true, inIndex);
	delete punchPolygon;
	delete outPolygon;
}

TArray<FVector> FArmySceneData::CalculateHardWareClipBox(TSharedPtr<FArmyHardware> InHardWare, bool orignalFace)
{
	const FVector& pos = FVector(InHardWare->GetPos().X, InHardWare->GetPos().Y, 0.0f);
	const FVector& direction = InHardWare->GetHorizontalDirecton();
	float passLength = InHardWare->GetLength();
	float height = InHardWare->GetHeight();
	if (orignalFace)
	{
		TArray<FVector> clipperBox = { FVector(pos + 0.5 * passLength * direction),
			FVector(pos + 0.5 * passLength * direction + FVector(0,0,1)* height),
			FVector(pos - 0.5 * passLength * direction + FVector(0,0,1)* height),
			FVector(pos - 0.5f * passLength * direction) };
		return clipperBox;

	}
	else
	{
		TArray<FVector> clippbox = { FVector(pos + (0.5f * passLength - FinishWallThick) * direction) + FVector(0,0,1)*FinishWallThick,
									  FVector(pos + (0.5f *passLength - FinishWallThick) * direction) + FVector(0,0,1)*(height - FinishWallThick),
									 FVector(pos - (0.5 * passLength - FinishWallThick) *direction) + FVector(0,0,1)* (height - FinishWallThick),
									FVector(pos - (0.5f*passLength - FinishWallThick) * direction) + FVector(0,0,1) * FinishWallThick
		};

		return clippbox;
	}

}

void FArmySceneData::CollectConstructionQuantity(FArmyConstructionQuantity& OutCQ)
{
	// 收集空间数据
	CollectSpaceData(OutCQ.SpaceData);

	// 收集拆改建数据
	CollectModifyWallData(OutCQ.SpaceData);

	// 收集水电数据
	CollectHydropowerData(OutCQ.SpaceData);

	// 收集门窗工程数据
	CollectHardwareData(OutCQ.SpaceData);

	// 收集木作数据
	CollectWHCData(OutCQ.CustomCupboard, 2);
	CollectWHCData(OutCQ.CustomWardrobe, 1);
	CollectWHCData(OutCQ.CustomOtherCabinet, 0);
}

void FArmySceneData::CollectSpaceData(TArray<FArmySpaceData>& OutSpaceData)
{
	// 遍历所有房间
	TArray<FObjectWeakPtr> RoomList;
	GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);

	//int32 VRSpaceID = 0;
	for (FObjectWeakPtr It : RoomList)
	{
		// 获取房间面积
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		float RoomArea, RoomPerimeter;
		Room->GetRoomAreaAndPerimeter(RoomArea, RoomPerimeter);


		FArmySpaceData SpaceData;
		SpaceData.SpaceID = Room->GetSpaceId();
		SpaceData.SpaceName = Room->GetSpaceName();
		SpaceData.SpaceType = 1;
		SpaceData.Param.Rs = RoomArea;
		SpaceData.Param.Rc = RoomPerimeter;
		SpaceData.Param.Rh = FArmySceneData::WallHeight / 100.f;
		TArray<TSharedPtr<FArmyRoomSpaceArea>> totalRoomAreas = FArmySceneData::Get()->GetRoomAttachedSurfacesWidthRoomID(Room->GetUniqueID().ToString());

		auto ComponentsSurfaces = FArmySceneData::Get()->GetRoomAttachedComponentSurfaceWithRoomId(Room->GetUniqueID().ToString());
		for (auto& ComponentsSurface : ComponentsSurfaces)
		{
			totalRoomAreas.AddUnique(ComponentsSurface);
		}

		
		//获取梁上的面
		TArray<TWeakPtr<FArmyObject>> AllsurfaceObjs;
		TArray<TSharedPtr<FArmyRoomSpaceArea>> ResultSurfaces;
		GetObjects(E_HardModel, OT_RoomSpaceArea, AllsurfaceObjs);
		for (auto& SurfaceObj : AllsurfaceObjs)
		{
			TSharedPtr<FArmyRoomSpaceArea> Surface = StaticCastSharedPtr<FArmyRoomSpaceArea>(SurfaceObj.Pin());
			if(Surface->GenerateFromObjectType!= OT_Beam)
			{
				continue;
			}
			
			if (Room->IsPointInRoom(Surface->GetPlaneCenter()))
			{
				totalRoomAreas.Add(Surface);
			}
		}
		
		float area = 0.0f;

		for (TSharedPtr<FArmyRoomSpaceArea> iter : totalRoomAreas)
		{
			/* 该编辑面上的施工区域*/
			iter->GetConstructionAreas(SpaceData.ArtificialData);
			if (iter->SurfaceType == 1)
			{
				area += iter->GetAreaDimension();
			}
		}
		
		//扣条
		GetBuckleConstrucionItem(Room->GetUniqueID().ToString(), SpaceData.ArtificialData);

		// 收集房间中的商品
		Room->CollectItemsInRoom(SpaceData.ArtificialData);

		SpaceData.Param.Ws += area;
		OutSpaceData.Add(SpaceData);
	}
}


void FArmySceneData::CollectModifyWallData(TArray<FArmySpaceData>& OutModifyWallData)
{
	FArmySpaceData ModifyWallData;
	ModifyWallData.SpaceID = 21;
	ModifyWallData.SpaceType = 4;

	// 新建墙
	TArray<FObjectWeakPtr> AddWallList;
	GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
	GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
	/** @欧石楠 获取全部拆改后的门洞、开垭口和门构件*/
	TArray<TWeakPtr<FArmyHardware>> AllHardwareList;
	GetAllHardWare(AllHardwareList, E_LayoutModel);	
	for (auto It : AddWallList)
	{
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
		if (AddWall.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 1; // 新建墙
			Goods.wallMaterial = AddWall->GetSelectedMaterialID();

			float AddWallArea = 0.f;
			float AddWallPerimeter = 0.f;
			AddWall->GetAreaAndPerimeter(AddWallArea, AddWallPerimeter);

			float TotalSubArea = 0;
			for (auto HardIt : AllHardwareList)
			{
				if (HardIt.IsValid())
				{
					TArray<FVector> TempPolyPoints;
					TempPolyPoints = AddWall->Polygon->Vertices;
					if (FArmyMath::IsPointInPolygon2D(HardIt.Pin()->GetPos(), TempPolyPoints))
					{
						TotalSubArea += HardIt.Pin()->GetHeight() * HardIt.Pin()->GetLength() * 0.0001f;
					}
				}
			}

			FArmyConstructionParameter Param;
			Param.S = AddWallArea - TotalSubArea;
			Param.C = AddWallPerimeter;
			Param.L = (AddWallPerimeter * 100 / 2.f - AddWall->GetWidth()) / 100.f;
			Param.H = AddWall->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(AddWall->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);
		}
	}

	// 拆除墙
	TArray<FObjectWeakPtr> ModifiedWallList;
	GetObjects(E_ModifyModel, OT_ModifyWall, ModifiedWallList);
	/** @欧石楠 这里获取拆改中的控件*/
	GetAllHardWare(AllHardwareList, E_ModifyModel);
	for (auto It : ModifiedWallList)
	{
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
		if (ModifyWall.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 2; // 新建墙
			Goods.wallMaterial = ModifyWall->GetSelectedMaterialID();

			float ModifyWallArea = 0.f;
			float ModifyWallPerimeter = 0.f;
			ModifyWall->GetAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

			float TotalSubArea = 0;
			for (auto HardIt : AllHardwareList)
			{
				if (HardIt.IsValid())
				{
					TArray<FVector> TempPolyPoints;
					TempPolyPoints = ModifyWall->Polygon->Vertices;
					if (FArmyMath::IsPointInPolygon2D(HardIt.Pin()->GetPos(), TempPolyPoints))
					{
						TotalSubArea += HardIt.Pin()->GetHeight() * HardIt.Pin()->GetLength() * 0.0001f;
					}
				}
			}

			FArmyConstructionParameter Param;
			Param.S = ModifyWallArea - TotalSubArea;
			Param.C = ModifyWallPerimeter;
			Param.L = (ModifyWallPerimeter * 100 / 2.f - ModifyWall->GetWidth()) / 100.f;
			Param.H = ModifyWall->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(ModifyWall->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);
		}
	}

	// 原始门洞
	TArray<FObjectWeakPtr> PassList;
	GetObjects(E_ModifyModel, OT_Pass, PassList);
	for (auto It : PassList)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
		if (Pass.IsValid())
		{
			if (Pass->GetIfFillPass()) // 是否填门洞
			{
				FArmyArea AreaData;

				FArmyGoods Goods;
				Goods.Type = 4;
				Goods.DemolitionOrRebuilding = 6; // 填门洞
				Goods.wallMaterial = Pass->GetSelectedMaterialID();

				float FillWallArea = 0.f;
				float FillWallPerimeter = 0.f;
				Pass->GetFillAreaAndPerimeter(FillWallArea, FillWallPerimeter);

				FArmyConstructionParameter Param;
				Param.S = FillWallArea;
				Param.C = FillWallPerimeter;
				Param.H = Pass->GetHeight() / 100.f;
				Goods.Param = Param;

				// 施工项
				CollectQuotaData(Pass->ConstructionItemData->CheckedId, Goods);

				AreaData.ArtificialData.Add(Goods);
				ModifyWallData.AreaData.Add(AreaData);
			}
			else
			{
				FArmyArea AreaData;

				FArmyGoods Goods;
				Goods.Type = 4;
				Goods.DemolitionOrRebuilding = 3; // 原始门洞

				float WallArea = 0.f;
				float Perimeter = 0.f;
				Pass->GetFillAreaAndPerimeter(WallArea, Perimeter);
				Pass->GetOriginPassArea(WallArea);
				FArmyConstructionParameter Param;
				Param.S = WallArea;
				Param.C = Perimeter;
				Param.H = Pass->GetHeight() / 100.f;
				Goods.Param = Param;

				// 施工项
				CollectQuotaData(Pass->ConstructionItemData->CheckedId, Goods);

				AreaData.ArtificialData.Add(Goods);
				ModifyWallData.AreaData.Add(AreaData);

				if (Pass->bPassHasAdd)
				{
					FArmyArea TempAreaData;

					FArmyGoods TempGoods;
					TempGoods.Type = 4;
					TempGoods.DemolitionOrRebuilding = 5; // 补门洞

					float AddWallArea = 0.f;
					float AddWallPerimeter = 0.f;
					Pass->GetAddWallAreaAndPerimeter(AddWallArea, AddWallPerimeter);

					FArmyConstructionParameter TempParam;
					TempParam.S = AddWallArea;
					TempParam.C = AddWallPerimeter;
					TempParam.L = Pass->GetAddLength() / 100.f;
					TempParam.H = Pass->bPassHasVerticalAdd ? Pass->GetOriginHeight() / 100.f : Pass->GetHeight() / 100.f;
					TempGoods.Param = TempParam;

					// 施工项
					CollectQuotaData(Pass->ConstructionItemData->CheckedId, TempGoods);

					TempAreaData.ArtificialData.Add(TempGoods);
					ModifyWallData.AreaData.Add(TempAreaData);
				}

				if (Pass->bPassHasModify)
				{
					FArmyArea TempAreaData;

					FArmyGoods TempGoods;
					TempGoods.Type = 4;
					TempGoods.DemolitionOrRebuilding = 4; // 拆门洞

					float ModifyWallArea = 0.f;
					float ModifyWallPerimeter = 0.f;
					Pass->GetModifyWallAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

					FArmyConstructionParameter TempParam;
					TempParam.S = ModifyWallArea;
					TempParam.C = ModifyWallPerimeter;
					TempParam.L = Pass->GetModifyLength() / 100.f;
					TempParam.H = Pass->bPassHasVerticalModify ? Pass->GetOriginHeight() / 100.f : Pass->GetHeight() / 100.f;
					TempGoods.Param = TempParam;

					// 施工项
					CollectQuotaData(Pass->ConstructionItemData->CheckedId, TempGoods);

					TempAreaData.ArtificialData.Add(TempGoods);
					ModifyWallData.AreaData.Add(TempAreaData);
				}
				
				if (Pass->bPassHasVerticalAdd)
				{
					FArmyArea TempAreaData;

					FArmyGoods TempGoods;
					TempGoods.Type = 4;
					TempGoods.DemolitionOrRebuilding = 9;//垂直新建
					
					FArmyConstructionParameter TempParam;
					float TempHeight = FMath::Abs(Pass->GetHeight() - Pass->GetOriginHeight()) / 100.f;
					float TempLength = Pass->GetLength() / 100.f;
					TempParam.S = TempHeight * TempLength;
					TempParam.L = TempLength;
					TempParam.H = TempHeight;
					TempGoods.Param = TempParam;

					// 施工项
					CollectQuotaData(Pass->ConstructionItemData->CheckedId, TempGoods);

					TempAreaData.ArtificialData.Add(TempGoods);
					ModifyWallData.AreaData.Add(TempAreaData);
				}
				else if (Pass->bPassHasVerticalModify)
				{
					FArmyArea TempAreaData;

					FArmyGoods TempGoods;
					TempGoods.Type = 4;
					TempGoods.DemolitionOrRebuilding = 10;//垂直拆除					

					FArmyConstructionParameter TempParam;
					float TempHeight = FMath::Abs(Pass->GetHeight() - Pass->GetOriginHeight()) / 100.f;
					float TempLength = Pass->GetLength() / 100.f;
					if (Pass->bPassHasAdd && !Pass->bPassHasModify)
					{
						TempLength = Pass->GetOriginLength() / 100.f;
					}
					else if(Pass->bPassHasAdd && Pass->bPassHasModify)
					{						
						TempLength = (Pass->GetLength() + Pass->GetHorizantalAddLength()) / 100.f;
					}
					TempParam.S = TempHeight * TempLength;
					TempParam.L = TempLength;
					TempParam.H = TempHeight;
					TempGoods.Param = TempParam;

					// 施工项
					CollectQuotaData(Pass->ConstructionItemData->CheckedId, TempGoods);

					TempAreaData.ArtificialData.Add(TempGoods);
					ModifyWallData.AreaData.Add(TempAreaData);
				}
			}

		}
	}

	// 门洞
	TArray<FObjectWeakPtr> DoorHoleList;
	GetObjects(E_ModifyModel, OT_DoorHole, DoorHoleList);
	for (auto It : DoorHoleList)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
		if (Pass.IsValid())
		{
			if (Pass->bHasAdd)
			{
				FArmyArea AreaData;

				FArmyGoods Goods;
				Goods.Type = 4;
				Goods.DemolitionOrRebuilding = 5; // 补门洞

				float AddWallArea = 0.f;
				float AddWallPerimeter = 0.f;
				Pass->GetAddWallAreaAndPerimeter(AddWallArea, AddWallPerimeter);

				FArmyConstructionParameter Param;
				Param.S = AddWallArea;
				Param.C = AddWallPerimeter;
				Param.L = Pass->GetAddLength() / 100.f;
				Param.H = Pass->GetHeight() / 100.f;
				Goods.Param = Param;

				// 施工项
				CollectQuotaData(Pass->ConstructionItemData->CheckedId, Goods);

				AreaData.ArtificialData.Add(Goods);
				ModifyWallData.AreaData.Add(AreaData);
			}

			if (Pass->bHasModify)
			{
				FArmyArea AreaData;

				FArmyGoods Goods;
				Goods.Type = 4;
				Goods.DemolitionOrRebuilding = 4; // 拆门洞

				float ModifyWallArea = 0.f;
				float ModifyWallPerimeter = 0.f;
				Pass->GetModifyWallAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

				FArmyConstructionParameter Param;
				Param.S = ModifyWallArea;
				Param.C = ModifyWallPerimeter;
				Param.L = Pass->GetModifyLength() / 100.f;
				Param.H = Pass->GetHeight() / 100.f;
				Goods.Param = Param;

				// 施工项
				CollectQuotaData(Pass->ConstructionItemData->CheckedId, Goods);

				AreaData.ArtificialData.Add(Goods);
				ModifyWallData.AreaData.Add(AreaData);
			}
		}
	}

	// 开垭口
	TArray<FObjectWeakPtr> NewPassList;
	GetObjects(E_ModifyModel, OT_NewPass, NewPassList);
	for (auto It : NewPassList)
	{
		TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(It.Pin());
		if (NewPass.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 3; // 原始门洞

			float WallArea = 0.f;
			NewPass->GetOriginPassArea(WallArea);

			FArmyConstructionParameter Param;
			Param.S = WallArea;
			Param.H = NewPass->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(NewPass->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);

			if (NewPass->InWallType == 0)
			{
				FArmyArea TempAreaData;

				FArmyGoods TempGoods;
				TempGoods.Type = 4;
				TempGoods.DemolitionOrRebuilding = 7; // 开垭口
				TempGoods.wallMaterial = NewPass->GetSelectedMaterialID();

				float ModifyWallArea = 0.f;
				float ModifyWallPerimeter = 0.f;
				NewPass->GetModifyWallAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

				FArmyConstructionParameter TempParam;
				TempParam.S = ModifyWallArea;
				TempParam.C = ModifyWallPerimeter;
				TempParam.H = NewPass->GetHeight() / 100.f;
				TempGoods.Param = TempParam;

				// 施工项
				CollectQuotaData(NewPass->ConstructionItemData->CheckedId, TempGoods);

				TempAreaData.ArtificialData.Add(TempGoods);
				ModifyWallData.AreaData.Add(TempAreaData);
			}
		}
	}

	// 室内门
	TArray<FObjectWeakPtr> DoorList;
	GetObjects(E_ModifyModel, OT_Door, DoorList);
	for (auto It : DoorList)
	{
		TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(It.Pin());
		if (Door.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 3; // 原始门洞

			float WallArea = 0.f;
			Door->GetOriginPassArea(WallArea);

			FArmyConstructionParameter Param;
			Param.S = WallArea;
			Param.H = Door->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(Door->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);

			if (Door->InWallType == 0)
			{
				FArmyArea TempAreaData;

				FArmyGoods TempGoods;
				TempGoods.Type = 4;
				TempGoods.DemolitionOrRebuilding = 7; // 开垭口
				TempGoods.wallMaterial = Door->GetSelectedMaterialID();

				float ModifyWallArea = 0.f;
				float ModifyWallPerimeter = 0.f;
				Door->GetModifyWallAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

				FArmyConstructionParameter TempParam;
				TempParam.S = ModifyWallArea;
				TempParam.C = ModifyWallPerimeter;
				TempParam.H = Door->GetHeight() / 100.f;
				TempGoods.Param = TempParam;

				// 施工项
				CollectQuotaData(Door->ConstructionItemData->CheckedId, TempGoods);

				TempAreaData.ArtificialData.Add(TempGoods);
				ModifyWallData.AreaData.Add(TempAreaData);
			}
		}
	}

	// 推拉门
	TArray<FObjectWeakPtr> SlidingDoorList;
	GetObjects(E_ModifyModel, OT_SlidingDoor, SlidingDoorList);
	for (auto It : SlidingDoorList)
	{
		TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(It.Pin());
		if (Door.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 3; // 原始门洞

			float WallArea = 0.f;
			Door->GetOriginPassArea(WallArea);

			FArmyConstructionParameter Param;
			Param.S = WallArea;
			Param.H = Door->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(Door->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);

			if (Door->InWallType == 0)
			{
				FArmyArea TempAreaData;

				FArmyGoods TempGoods;
				TempGoods.Type = 4;
				TempGoods.DemolitionOrRebuilding = 7; // 开垭口
				TempGoods.wallMaterial = Door->GetSelectedMaterialID();

				float ModifyWallArea = 0.f;
				float ModifyWallPerimeter = 0.f;
				Door->GetModifyWallAreaAndPerimeter(ModifyWallArea, ModifyWallPerimeter);

				FArmyConstructionParameter TempParam;
				TempParam.S = ModifyWallArea;
				TempParam.C = ModifyWallPerimeter;
				TempParam.H = Door->GetHeight() / 100.f;
				TempGoods.Param = TempParam;

				// 施工项
				CollectQuotaData(Door->ConstructionItemData->CheckedId, TempGoods);

				TempAreaData.ArtificialData.Add(TempGoods);
				ModifyWallData.AreaData.Add(TempAreaData);
			}
		}
	}

	//包立管
	TArray<FObjectWeakPtr> PackPipeList;
	GetObjects(E_ModifyModel, OT_PackPipe, PackPipeList);
	for (auto It : PackPipeList)
	{
		TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(It.Pin());
		if (PackPipe.IsValid())
		{
			FArmyArea AreaData;

			FArmyGoods Goods;
			Goods.Type = 4;
			Goods.DemolitionOrRebuilding = 8; // 包立管

			float WallArea = 0.f;
			float WallPerimeter = 0.f;
			PackPipe->GetAreaAndPerimeter(WallArea, WallPerimeter);

			FArmyConstructionParameter Param;
			Param.S = WallArea;
			Param.C = WallPerimeter;
			Param.H = PackPipe->GetHeight() / 100.f;
			Goods.Param = Param;

			// 施工项
			CollectQuotaData(PackPipe->ConstructionItemData->CheckedId, Goods);

			AreaData.ArtificialData.Add(Goods);
			ModifyWallData.AreaData.Add(AreaData);
		}
	}

	OutModifyWallData.Add(ModifyWallData);
}

void FArmySceneData::CollectHydropowerData(TArray<FArmySpaceData>& OutHydropowerData)
{
	//@郭子阳
	//导出清单伪代码
	//for (水电对象 : 所有水电对象)
	//{
	//	if (水电对象有构件)
	//	{
	//		if (水电对象是原始点位)
	//		{
	//			只导出施工项
	//				return;
	//		}

	//		导出商品及施工项
	//			return;
	//	}
	//	else if (水电对象是管子)
	//	{
	//		导出商品及施工项
	//			return;
	//	}else if（水电对象是接头）
	//	{ 导出商品及施工项
	//		return;
	//	}
	//}


	int32 SpaceDataIndex = OutHydropowerData.Add(FArmySpaceData());
	FArmySpaceData& SpaceData = OutHydropowerData[SpaceDataIndex];
	SpaceData.SpaceID = 22;
	SpaceData.SpaceType = 2;
	SpaceData.SpaceName = TEXT("水电");

	TArray<FObjectWeakPtr> RoomArray;
	GetObjects(E_LayoutModel, OT_InternalRoom, RoomArray);

	TArray<FObjectWeakPtr> ObjArray = GetObjects(E_HydropowerModel);
	GetObjects(E_LayoutModel, OT_ComponentBase, ObjArray);
	GetObjects(E_LayoutModel, OT_Component_None, ObjArray);
	for (auto ObjPtr : ObjArray)
	{
		FArmyFurniture* FurnitureObj = ObjPtr.Pin()->AsassignObj<FArmyFurniture>();

		if (FurnitureObj &&FurnitureObj->CanOutPutCalcList())
		{
			AActor* ObjActor = FurnitureObj->GetRelevanceActor();
			if (ObjActor
				&& (ObjActor->Tags.Contains("HydropowerActor") || ObjActor->Tags.Contains(XRActorTag::OriginalPoint)))
			{

				//纯移位施工项
				bool OffsetConstruction = FurnitureObj->IsOrignalPoint();
				if (OffsetConstruction)
				{
	
					TSharedPtr<FArmyConstructionItemInterface> ConstructionData  = XRConstructionManager::Get()->GetSavedCheckedData(FurnitureObj->GetUniqueID(), FurnitureObj->GetConstructionParameter());
					if (!ConstructionData.IsValid())
					{
						//构件取消勾选了所有施工项
						continue;
					}
					else if (ConstructionData->CheckedId.Num() <= 0)
					{
						continue;
					}

					FString SpaceName1;
					for (auto R : RoomArray)
					{
						TSharedPtr<FArmyRoom> RPtr = StaticCastSharedPtr<FArmyRoom>(R.Pin());
						if (RPtr->IsPointInRoom(FurnitureObj->GetBasePos()))
						{
							SpaceName1 == RPtr->GetSpaceName();
						}
					}

					auto GoodsPtr = SpaceData.ArtificialData.FindByPredicate([&](const FArmyGoods& goods) {
						return goods.Type == 4
							&& goods.SpaceMsg.SpaceName1 == SpaceName1;
					});

					int32 Goodsindex;
					if (!GoodsPtr)
					{
						Goodsindex = SpaceData.ArtificialData.Add(FArmyGoods());
						SpaceData.ArtificialData[Goodsindex].Type = 4;
						SpaceData.ArtificialData[Goodsindex].SpaceMsg.SpaceName1 == SpaceName1;
					}
					else
					{
						Goodsindex = SpaceData.ArtificialData.Find(*GoodsPtr);
					}
					FArmyGoods& Goods = SpaceData.ArtificialData[Goodsindex];
					//合并施工项数量
					for (int32 i = 0; i < Goods.QuotaData.Num(); i++)
					{
						Goods.QuotaData[i].Dosage += 1;
					}

					//TArray<FArmyQuota> Quotas;

					for (auto& CheckID : ConstructionData->CheckedId)
					{
						FArmyQuota Quota;
						Quota.QuotaId = CheckID.Key; //施工项ID

						for (auto& Property : CheckID.Value)
						{
							Quota.QuotaProperties.Add(FArmyPropertyValue(Property.Key, Property.Value));
						}
						//Quotas.Add(Quota);

						int32 QuotaIndex;
						if (Goods.QuotaData.Find(Quota, QuotaIndex))
						{
							Goods.QuotaData[QuotaIndex].Dosage += 1;
						}
						else
						{
							Goods.QuotaData.Add(Quota);
						}
					}
					continue;
				}

				//非纯移位施工项
				FArmyGoods goods;
				goods.Type = 1;
				goods.BreakpointReform = FurnitureObj->bReform || FurnitureObj->IsBPreforming();

				//2.获取商品信息
				UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
				TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(FurnitureObj->RelevanceActor->GetSynID());
				goods.GoodsId = ActorItemInfo->ID;

				//1.获取施工项
				TSharedPtr<FArmyConstructionItemInterface> ConstructionData = nullptr;

				ConstructionData =XRConstructionManager::Get()->GetSavedCheckedData(FurnitureObj->GetUniqueID(), FurnitureObj->GetConstructionParameter());

				//TArray<FArmyQuota> Quotas;
				if (ConstructionData.IsValid())
				{
					for (auto& CheckID : ConstructionData->CheckedId)
					{
						FArmyQuota Quota;
						Quota.QuotaId = CheckID.Key; //施工项ID

						for (auto& Property : CheckID.Value)
						{
							Quota.QuotaProperties.Add(FArmyPropertyValue(Property.Key, Property.Value));
						}
						goods.QuotaData.Add(Quota);
						//Quotas
					}
				}

				
				//3.填充房间信息
				TArray<FObjectWeakPtr> RoomList;
				GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
				for (FObjectWeakPtr It : RoomList)
				{

					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
					if (Room->IsPointInRoom(FurnitureObj->GetBasePos()))
					{
						goods.SpaceMsg.SpaceName1 = Room->GetSpaceName();
						break;
					}
				}

				//4.匹配商品
				int32 Goodsindex;
				if (!SpaceData.ArtificialData.Find(goods, Goodsindex))
				{
					Goodsindex = SpaceData.ArtificialData.Add(goods);
				}
				FArmyGoods& Goods = SpaceData.ArtificialData[Goodsindex];

				//合并施工项数量
				for (int32 i = 0; i < Goods.QuotaData.Num(); i++)
				{
					Goods.QuotaData[i].Dosage += 1;
				}

				Goods.Param.D += 1;
			}
		}
		else if (FArmyPipeline* PipeObj = ObjPtr.Pin()->AsassignObj<FArmyPipeline>())
		{
			if (PipeObj->GetLenght() <= 0 || PipeObj->ProductID == -1)
			{
				continue;
			}

			AActor* ObjActor = PipeObj->GetRelevanceActor();
			if (!ObjActor || ObjActor->Tags.Contains(XRActorTag::OriginalPoint))
			{
				continue;
			}
			FArmyGoods goods;
			goods.Type = 1;
			TSharedPtr<FArmyConstructionItemInterface> ConstructionData = XRConstructionManager::Get()->GetSavedCheckedData(PipeObj->GetUniqueID(), EConstructionType::NormalGoods);
			goods.GoodsId = PipeObj->ProductID;

			if (ConstructionData.IsValid() && ConstructionData->CheckedId.Num() > 0)
			{
				for (auto& CheckID : ConstructionData->CheckedId)
				{
					FArmyQuota Quota;
					Quota.QuotaId = CheckID.Key; //施工项ID
					for (auto& Property : CheckID.Value)
					{
						Quota.QuotaProperties.Add(FArmyPropertyValue(Property.Key, Property.Value));
					}
					goods.QuotaData.Add(Quota);
				}
			}

			//3.匹配商品
			int32 Goodsindex;
			if (!SpaceData.ArtificialData.Find(goods, Goodsindex))
			{
				Goodsindex = SpaceData.ArtificialData.Add(goods);
			}
			FArmyGoods& Goods = SpaceData.ArtificialData[Goodsindex];

			//合并施工项数量
			for (int32 i = 0; i < Goods.QuotaData.Num(); i++)
			{
				Goods.QuotaData[i].Dosage += 1;
			}

			double Length = FMath::CeilToInt(PipeObj->GetLenght());
			Goods.Param.D += Length / 100;
			Goods.Param.L += Length / 100;
			Goods.Param.C += Length / 100;
		}
		else if (FArmyPipePoint* PointObj = ObjPtr.Pin()->AsassignObj<FArmyPipePoint>())
		{
			AActor* ObjActor = PointObj->GetRelevanceActor();
			if (!ObjActor || ObjActor->Tags.Contains(XRActorTag::OriginalPoint))
			{
				continue;
			}

			FArmyGoods goods;
			goods.Type = 1;
			goods.GoodsId = PointObj->ProductID;
			if (goods.GoodsId == -1)
			{
				//没有商品,不输出
				continue;
			}
			TSharedPtr<FArmyConstructionItemInterface> ConstructionData  = XRConstructionManager::Get()->GetSavedCheckedData(PointObj->GetUniqueID(), EConstructionType::NormalGoods);

			if (ConstructionData.IsValid() && ConstructionData->CheckedId.Num() >= 0)
			{
				for (auto& CheckID : ConstructionData->CheckedId)
				{
					FArmyQuota Quota;
					Quota.QuotaId = CheckID.Key; //施工项ID

					for (auto& Property : CheckID.Value)
					{
						Quota.QuotaProperties.Add(FArmyPropertyValue(Property.Key, Property.Value));
					}
					goods.QuotaData.Add(Quota);
				}
			}

			//3.匹配商品
			int32 Goodsindex;
			if (!SpaceData.ArtificialData.Find(goods, Goodsindex))
			{
				Goodsindex = SpaceData.ArtificialData.Add(goods);
			}
			FArmyGoods& Goods = SpaceData.ArtificialData[Goodsindex];

			//合并施工项数量
			for (int32 i = 0; i < Goods.QuotaData.Num(); i++)
			{
				Goods.QuotaData[i].Dosage += 1;
			}
			Goods.Param.D += 1;

		}
	}
}

void FArmySceneData::CollectHardwareData(TArray<FArmySpaceData>& OutHardwareData)
{
	FArmySpaceData HardwareData;
	HardwareData.SpaceID = 25;
	HardwareData.SpaceType = 3;

	// 室内外门
	TArray<FObjectWeakPtr> DoorList;
	GetObjects(E_LayoutModel, OT_Door, DoorList);
    GetObjects(E_LayoutModel, OT_SecurityDoor, DoorList);
	for (FObjectWeakPtr It : DoorList)
	{
		TSharedPtr<FArmySingleDoor> SingleDoor = StaticCastSharedPtr<FArmySingleDoor>(It.Pin());
		if (SingleDoor.IsValid() && SingleDoor->GetDoorSaleID() >= 0)
		{
			SingleDoor->ConstructionData(HardwareData.ArtificialData);

			/*FArmyGoods Goods;
			Goods.Type = 1;
			Goods.GoodsId = SingleDoor->GetDoorSaleID();

			FArmyConstructionParameter Param;
			Param.D = 1;
			Goods.Param = Param;

			FArmySpaceMsg SpaceMsg;
			if (SingleDoor->FirstRelatedRoom.IsValid() && SingleDoor->FirstRelatedRoom->GetSpaceNameLabel().IsValid())
			{
				SpaceMsg.SpaceName1 = SingleDoor->FirstRelatedRoom->GetSpaceName();
			}
			if (SingleDoor->SecondRelatedRoom.IsValid() && SingleDoor->SecondRelatedRoom->GetSpaceNameLabel().IsValid())
			{
				SpaceMsg.SpaceName2 = SingleDoor->SecondRelatedRoom->GetSpaceName();
			}
			Goods.SpaceMsg = SpaceMsg;

			FArmyDoorMsg DoorMsg;
			DoorMsg.DoorWidth = SingleDoor->GetLength() * 10.f;
			DoorMsg.DoorHeight = SingleDoor->GetHeight() * 10.f;
			DoorMsg.WallThickness = SingleDoor->GetWidth() * 10.f;
			DoorMsg.SwitchingDirection = SingleDoor->GetOpenDirectionID();
			Goods.DoorMsg = DoorMsg;

			HardwareData.ArtificialData.Add(Goods);*/
		}
	}

	// 推拉门
	TArray<FObjectWeakPtr> SlidingDoorList;
	GetObjects(E_LayoutModel, OT_SlidingDoor, SlidingDoorList);
	for (FObjectWeakPtr It : SlidingDoorList)
	{
		TSharedPtr<FArmySlidingDoor> SlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(It.Pin());
		if (SlidingDoor.IsValid() && SlidingDoor->GetDoorSaleID() >= 0)
		{
			SlidingDoor->ConstructionData(HardwareData.ArtificialData);

		/*	FArmyGoods Goods;
			Goods.Type = 1;
			Goods.GoodsId = SlidingDoor->GetDoorSaleID();

			FArmyConstructionParameter Param;
			Param.D = 1;
			Goods.Param = Param;

			FArmySpaceMsg SpaceMsg;
			if (SlidingDoor->FirstRelatedRoom.IsValid() && SlidingDoor->FirstRelatedRoom->GetSpaceNameLabel().IsValid())
			{
				SpaceMsg.SpaceName1 = SlidingDoor->FirstRelatedRoom->GetSpaceName();
			}
			if (SlidingDoor->SecondRelatedRoom.IsValid() && SlidingDoor->SecondRelatedRoom->GetSpaceNameLabel().IsValid())
			{
				SpaceMsg.SpaceName2 = SlidingDoor->SecondRelatedRoom->GetSpaceName();
			}
			Goods.SpaceMsg = SpaceMsg;

			FArmyDoorMsg DoorMsg;
			DoorMsg.DoorWidth = SlidingDoor->GetLength() * 10.f;
			DoorMsg.DoorHeight = SlidingDoor->GetHeight() * 10.f;
			DoorMsg.WallThickness = SlidingDoor->GetWidth() * 10.f;
			DoorMsg.SwitchingDirection = SlidingDoor->GetOpenDirectionID();
			Goods.DoorMsg = DoorMsg;

			HardwareData.ArtificialData.Add(Goods);*/
		}
	}

	// 垭口和开垭口
	TArray<FObjectWeakPtr> PassList;
	GetObjects(E_LayoutModel, OT_Pass, PassList);
	GetObjects(E_LayoutModel, OT_NewPass, PassList);
	for (FObjectWeakPtr It : PassList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		if (Hardware.IsValid())
		{
			FArmyGoods Goods;
			Goods.Type = 1;
			if (It.Pin()->GetType() == OT_Pass)
			{
				TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());

				Pass->ConstructionData(HardwareData.ArtificialData);
			}
			else if (It.Pin()->GetType() == OT_NewPass)
			{
				TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(It.Pin());

				NewPass->ConstructionData(HardwareData.ArtificialData);
			}
		}
	}

	//过门石、哑口侧面等
	TArray<FObjectWeakPtr> BridgeStoneList;
	GetObjects(E_HardModel, OT_RoomSpaceArea, BridgeStoneList);
	//TArray<FArmyArea> TempXRArea;
	for (auto It : BridgeStoneList)
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
		if (RoomSpaceArea->GenerateFromObjectType == OT_Pass ||
			RoomSpaceArea->GenerateFromObjectType == OT_DoorHole ||
			RoomSpaceArea->GenerateFromObjectType == OT_NewPass ||
			RoomSpaceArea->GenerateFromObjectType == OT_Door ||
			RoomSpaceArea->GenerateFromObjectType == OT_SecurityDoor ||
			RoomSpaceArea->GenerateFromObjectType == OT_SlidingDoor)
		{
			RoomSpaceArea->GetConstructionAreas(HardwareData.ArtificialData);
		}
	}

	//@郭子阳
	//收集窗户施工项
	TArray<TWeakPtr<FArmyObject>> AllRoomArea;
	GetObjects(E_HardModel, OT_RoomSpaceArea, AllRoomArea);

	TArray<FObjectWeakPtr> LayOutObjects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, LayOutObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, LayOutObjects);
	for (auto It : LayOutObjects)
	{
		TSharedPtr<FArmyWindow> WindowObj = StaticCastSharedPtr<FArmyWindow>(It.Pin());
		WindowObj->ConstructionData(HardwareData.ArtificialData);

	}
	LayOutObjects.Empty();
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, LayOutObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, LayOutObjects);
	for (auto It : LayOutObjects)
	{
		TSharedPtr<FArmyRectBayWindow> WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(It.Pin());
		WindowObj->ConstructionData(HardwareData.ArtificialData);
	}

	OutHardwareData.Add(HardwareData);
}

void FArmySceneData::CollectWHCData(FArmyWHCModeListData& OutWHCModeData, int32 InType)
{
	GetWHCModeListData.ExecuteIfBound(OutWHCModeData, InType);
}

void FArmySceneData::CollectQuotaData(TMap<int32, TMap<int32, int32> > CheckedId, FArmyGoods& OutGoods)
{
	for (auto It : CheckedId)
	{
		FArmyQuota QuotaData;
		QuotaData.QuotaId = It.Key;
		QuotaData.Dosage = 1;

		for (auto PropertyIt : It.Value)
		{
			FArmyPropertyValue QuotaProperties;
			QuotaProperties.propertyId = PropertyIt.Key;
			QuotaProperties.valueId = PropertyIt.Value;
			QuotaData.QuotaProperties.Add(QuotaProperties);
		}

		OutGoods.QuotaData.Add(QuotaData);
	}
}

void FArmySceneData::SetFacsimile(TSharedPtr<FArmyReferenceImage> InFacsimile)
{
    TWeakPtr<FArmyReferenceImage> Facsimile = GetFacsimile();
    if (Facsimile.IsValid())
    {
        Delete(Facsimile.Pin());
    }

    if (InFacsimile.IsValid())
    {
        XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("底图")))).ArgUint32(E_HomeModel).ArgString(InFacsimile->GetName());
        Add(InFacsimile, Arg);
    }
}

void FArmySceneData::ChangeConstructionHelpersSpaceID(TSharedPtr<FArmyRoom> ChangedRoom)
{
	for (TActorIterator<AActor> ActorItr(GEngine->GameViewport->GetWorld()); ActorItr; ++ActorItr)
	{
		auto IConstructionActor = Cast<AArmyExtrusionActor>(ActorItr.operator->());
		if (IConstructionActor)
		{
			auto ConstructionHelper = dynamic_cast<IArmyConstructionHelper*>(IConstructionActor);
			ConstructionHelper->OnRoomSpaceIDChanged(ChangedRoom->GetSpaceId());
		}
	}
	auto Objs = GetObjects(EModelType::E_LayoutModel);
	auto HardModObjs= GetObjects(EModelType::E_HardModel);
	Objs.Append(HardModObjs);
	for (auto & Obj : Objs)
	{
		auto IConstruction = dynamic_cast<IArmyConstructionHelper*>(Obj.Pin().Get());
		if (IConstruction)
		{
			IConstruction->OnRoomSpaceIDChanged(ChangedRoom->GetSpaceId());
		}
	}
}


void FArmySceneData::GetHardWareObjects(TArray<FObjectWeakPtr>& OutObjects, EModelType InModelType)
{
	for (auto ObjIt : GetObjects(InModelType))
	{
		if (ObjIt.Pin()->GetType() == OT_Window
			|| ObjIt.Pin()->GetType() == OT_FloorWindow
			|| ObjIt.Pin()->GetType() == OT_Pass
			|| ObjIt.Pin()->GetType() == OT_NewPass
			|| ObjIt.Pin()->GetType() == OT_DoorHole
			|| ObjIt.Pin()->GetType() == OT_Door
			|| ObjIt.Pin()->GetType() == OT_SlidingDoor
			|| ObjIt.Pin()->GetType() == OT_SecurityDoor
			|| ObjIt.Pin()->GetType() == OT_RectBayWindow
			|| ObjIt.Pin()->GetType() == OT_TrapeBayWindow
			|| ObjIt.Pin()->GetType() == OT_Punch)

		{
			OutObjects.Add(ObjIt);
		}
	}
}

TSharedPtr<class FArmyRoom> FArmySceneData::GetRoomFromLine(TSharedPtr<class FArmyLine> InLine)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			Room->GetLines(RoomLines);
			for (auto line : RoomLines)
			{
				if (line == InLine)
				{
					return Room;
				}
			}
		}
	}

	return nullptr;
}


TSharedPtr<class FArmyRoom> FArmySceneData::GetRoomFromPoint(TSharedPtr<class FArmyEditPoint> InPoint)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			TArray<TSharedPtr<FArmyEditPoint>> RoomPoints = Room->GetPoints();
			for (auto Point : RoomPoints)
			{
				if (Point == InPoint)
				{
					return Room;
				}
			}
		}
	}

	return nullptr;
}

void FArmySceneData::SetHasOutWall(bool bHas)
{
	bHasOutWall = bHas;
	//@欧石楠 下面设置所有的线条在有外墙的条件下不显示操作点
	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	TSharedPtr<FArmyRoom> Room;
	if (bHasOutWall)
	{
		for (auto It : RoomList)
		{
			Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (Room.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> RoomLines;
				Room->GetLines(RoomLines);
				for (auto line : RoomLines)
				{
					if (line.IsValid())
					{
						line->SetShowEditPoint(false);
					}
				}
			}
		}
	}
	else
	{
		for (auto It : RoomList)
		{
			Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (Room.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> RoomLines;
				Room->GetLines(RoomLines);
				for (auto line : RoomLines)
				{
					if (line.IsValid())
					{
						line->SetShowEditPoint(true);
					}
				}
			}
		}
	}
}

void FArmySceneData::SplitSimpleComponentsAndRooms()
{
	TArray<TWeakPtr<FArmyObject>> ComponentList;//柱子风道
	GetObjects(E_LayoutModel, OT_Pillar, ComponentList);
	GetObjects(E_LayoutModel, OT_AirFlue, ComponentList);	

	if (ComponentList.Num() == 0)
		return;

	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		/*TSharedPtr<FArmyRoom> HomeMode_Room = StaticCastSharedPtr<FArmyRoom>(GetObjectByGuid(E_HomeModel, CurrentRoom->GetUniqueID()).Pin());*/
		if (CurrentRoom.IsValid()/* && HomeMode_Room.IsValid() && HomeMode_Room->bPreModified*/)
		{
			Clipper Cli;
			TArray<FVector> RoomPoints = CurrentRoom->GetWorldPoints(true);
			Paths ClipperPath;
			Path RoomPath;
			for (int i = 0; i < RoomPoints.Num(); i++)
				RoomPath.push_back(IntPoint(RoomPoints[i].X * 10, RoomPoints[i].Y * 10));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptSubject, true);
			
			//把柱子风道加入布尔运算
			for (auto ItComponent : ComponentList)
			{
				TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(ItComponent.Pin());
				if (CurrentComponent->IsAttachedToRoom())
				{
					TArray<FVector> ComponentPoints;
					CurrentComponent->Get4Vertexes(ComponentPoints);
					FVector CenterPoint = FBox(ComponentPoints).GetCenter();
					if (FArmyMath::IsPointInOrOnPolygon2D(CenterPoint, RoomPoints))
					{
						ClipperPath.clear();
						RoomPath.clear();
						for (int i = 0; i < ComponentPoints.Num(); i++)
							RoomPath.push_back(IntPoint(ComponentPoints[i].X * 10, ComponentPoints[i].Y * 10));
						ClipperPath.push_back(RoomPath);
						Cli.AddPaths(ClipperPath, ptClip, true);
					}
				}				
			}			

			Paths ResultPoints;
			if (Cli.Execute(ctDifference, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				if (ResultPoints.size() < 1)
					continue;

				FString RoomSpaceName = CurrentRoom->GetSpaceName();
				FGuid PreRommID = CurrentRoom->GetUniqueID();
				CurrentRoom->DeleteSpaceNameObj(true);//@ 临时处理
				Delete(CurrentRoom, false);
				for (int i = 0; i < ResultPoints.size(); i++)
				{

					//@欧石楠 在这里进行判断是否布尔运算结果出来的点跟柱子风道或者包立管的点完全一致，一致则说明不需要生成房间
					bool bFindNotNeedToCreate = false;
					for (auto ItComponent : ComponentList)
					{
						TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(ItComponent.Pin());
						if (CurrentComponent->IsAttachedToRoom())
						{
							TArray<FVector> ComponentPoints;
							CurrentComponent->Get4Vertexes(ComponentPoints);
							int TempCount = 0;
							for (int j = 0; j < ResultPoints[i].size(); j++)
							{
								for (auto ItPoint : ComponentPoints)
								{
									IntPoint TempPoint(ItPoint.X * 10, ItPoint.Y * 10);
									if (ResultPoints[i][j] == TempPoint)
									{
										++TempCount;
										break;
									}
								}
							}
							if (TempCount > ComponentPoints.Num() / 2 && TempCount > ResultPoints[i].size() / 2)
							{
								bFindNotNeedToCreate = true;
								break;
							}
						}						
					}					
					
					if (bFindNotNeedToCreate && ResultPoints.size() != 1)
					{
						continue;
					}


					TSharedPtr<FArmyRoom> NewRoom = MakeShareable(new FArmyRoom);
					NewRoom->SetUniqueID(PreRommID);
					NewRoom->SetSpaceName(RoomSpaceName);
					FString TempRoomName = RoomSpaceName;
					TempRoomName = TempRoomName.Right(1).IsNumeric() ? RoomSpaceName.Left(RoomSpaceName.Len() - 1) : RoomSpaceName;
					NewRoom->SetSpaceId(FArmyUser::Get().SpaceTypeList->FindByValue(TempRoomName).Get()->Key);

					SCOPE_TRANSACTION(TEXT("添加空间"));
					Add(NewRoom, XRArgument(1).ArgUint32(E_LayoutModel));

					TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
					TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
					for (int j = 0; j < ResultPoints[i].size(); j++)
					{
						FVector CurrentPoint(ResultPoints[i][j].X / 10.f, ResultPoints[i][j].Y / 10.f, 0);
						TSharedPtr<FArmyEditPoint> NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
						if (j > 0)
						{
							TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
							NewRoom->AddLine(WallLine);
							PrePoint->AddReferenceLine(WallLine->GetCoreLine());
							NewPoint->AddReferenceLine(WallLine->GetCoreLine());
						}
						else
							FirstPoint = NewPoint;
						PrePoint = NewPoint;
						if (j == ResultPoints[i].size() - 1)
						{
							TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
							NewPoint->AddReferenceLine(LastLine->GetCoreLine());
							FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
							NewRoom->AddLine(LastLine);
						}

					}
					GenerateNewRoomSpaceNameLabel(NewRoom, E_LayoutModel);
				}
			}

		}
	}
}

void FArmySceneData::SplitPackPipeAndRooms()
{	
	TArray<TWeakPtr<FArmyObject>> PackPipeList;//包立管
	GetObjects(E_LayoutModel, OT_PackPipe, PackPipeList);

	if (PackPipeList.Num() == 0)
		return;

	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		/*TSharedPtr<FArmyRoom> HomeMode_Room = StaticCastSharedPtr<FArmyRoom>(GetObjectByGuid(E_HomeModel, CurrentRoom->GetUniqueID()).Pin());*/
		if (CurrentRoom.IsValid()/* && HomeMode_Room.IsValid() && HomeMode_Room->bPreModified*/)
		{
			Clipper Cli;
			TArray<FVector> RoomPoints = CurrentRoom->GetWorldPoints(true);
			Paths ClipperPath;
			Path RoomPath;
			for (int i = 0; i < RoomPoints.Num(); i++)
				RoomPath.push_back(IntPoint(RoomPoints[i].X * 10, RoomPoints[i].Y * 10));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptSubject, true);			

			//把包立管加入布尔运算
			for (auto ItPackPipe : PackPipeList)
			{
				TSharedPtr<FArmyPackPipe> CurrentPackPipe = StaticCastSharedPtr<FArmyPackPipe>(ItPackPipe.Pin());

				TArray<FVector> PackPipePoints;
				CurrentPackPipe->GetVertexes(PackPipePoints);				
				ClipperPath.clear();
				RoomPath.clear();
				for (int i = 0; i < PackPipePoints.Num(); i++)
					RoomPath.push_back(IntPoint(PackPipePoints[i].X * 10, PackPipePoints[i].Y * 10));
				ClipperPath.push_back(RoomPath);
				Cli.AddPaths(ClipperPath, ptClip, true);
			}

			Paths ResultPoints;
			if (Cli.Execute(ctDifference, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				if (ResultPoints.size() < 1)
					continue;

				FString RoomSpaceName = CurrentRoom->GetSpaceName();
				FGuid PreRommID = CurrentRoom->GetUniqueID();
				CurrentRoom->DeleteSpaceNameObj(true);//@ 临时处理
				Delete(CurrentRoom, false);
				for (int i = 0; i < ResultPoints.size(); i++)
				{

					//@欧石楠 在这里进行判断是否布尔运算结果出来的房间是否在包立管内部，在则不生成
					bool bFindNotNeedToCreate = false;
					TArray<FVector> TempRoomPoints;
					for (int j = 0; j < ResultPoints[i].size(); j++)
					{
						FVector CurrentPoint(ResultPoints[i][j].X / 10.f, ResultPoints[i][j].Y / 10.f, 0);
						TempRoomPoints.Add(CurrentPoint);
					}
					for (auto ItPackPipe : PackPipeList)
					{
						TSharedPtr<FArmyPackPipe> CurrentPackPipe = StaticCastSharedPtr<FArmyPackPipe>(ItPackPipe.Pin());
						TArray<FVector> PackPipePoints;
						CurrentPackPipe->GetVertexes(PackPipePoints);
						FVector CenterPoint = FBox(PackPipePoints).GetCenter();						
						if (FArmyMath::IsPointInPolygon2D(CenterPoint, TempRoomPoints))
						{
							bFindNotNeedToCreate = true;
							break;
						}
						else
						{
							TArray<TWeakPtr<FArmyObject>> ComponentList;//柱子风道
							GetObjects(E_LayoutModel, OT_Pillar, ComponentList);
							GetObjects(E_LayoutModel, OT_AirFlue, ComponentList);

							if (ComponentList.Num() == 0)
							{
								continue;
							}								
							for (auto ItComponent : ComponentList)
							{
								TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(ItComponent.Pin());
								TArray<FVector> ComponentPoints;
								CurrentComponent->Get4Vertexes(ComponentPoints);								
								int TempCount = 0;
								for (int j = 0; j < ResultPoints[i].size(); j++)
								{
									for (auto ItPoint : ComponentPoints)
									{
										IntPoint TempPoint(ItPoint.X * 10, ItPoint.Y * 10);
										if (ResultPoints[i][j] == TempPoint)
										{
											++TempCount;
											break;
										}
									}
								}
								if (FArmyMath::IsPointInPolygon2D(CenterPoint, ComponentPoints) && TempCount == 4)
								{
									bFindNotNeedToCreate = true;
									break;
								}
							}
						}
					}
					if (bFindNotNeedToCreate && ResultPoints.size() != 1)
					{
						continue;
					}


					TSharedPtr<FArmyRoom> NewRoom = MakeShareable(new FArmyRoom);
					NewRoom->SetUniqueID(PreRommID);
					NewRoom->SetSpaceName(RoomSpaceName);
					FString TempRoomName = RoomSpaceName;
					TempRoomName = TempRoomName.Right(1).IsNumeric() ? RoomSpaceName.Left(RoomSpaceName.Len() - 1) : RoomSpaceName;
					NewRoom->SetSpaceId(FArmyUser::Get().SpaceTypeList->FindByValue(TempRoomName).Get()->Key);

					SCOPE_TRANSACTION(TEXT("添加空间"));
					Add(NewRoom, XRArgument(1).ArgUint32(E_LayoutModel));

					TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
					TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
					for (int j = 0; j < ResultPoints[i].size(); j++)
					{
						FVector CurrentPoint(ResultPoints[i][j].X / 10.f, ResultPoints[i][j].Y / 10.f, 0);
						TSharedPtr<FArmyEditPoint> NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
						if (j > 0)
						{
							TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
							NewRoom->AddLine(WallLine);
							PrePoint->AddReferenceLine(WallLine->GetCoreLine());
							NewPoint->AddReferenceLine(WallLine->GetCoreLine());
						}
						else
							FirstPoint = NewPoint;
						PrePoint = NewPoint;
						if (j == ResultPoints[i].size() - 1)
						{
							TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
							NewPoint->AddReferenceLine(LastLine->GetCoreLine());
							FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
							NewRoom->AddLine(LastLine);
						}

					}

					OnRoomAdded.ExecuteIfBound(NewRoom);

					GenerateNewRoomSpaceNameLabel(NewRoom, E_LayoutModel);
				}
			}

		}
	}
}

void FArmySceneData::SplitRooms()
{
	TArray<TWeakPtr<FArmyObject>> AddWallList;
	GetObjects(E_LayoutModel, OT_AddWall, AddWallList);
	if (AddWallList.Num() == 0)
		return;	

	//TArray<TWeakPtr<FArmyObject>> ComponentList;//柱子风道
	//GetObjects(E_LayoutModel, OT_Pillar, ComponentList);
	//GetObjects(E_LayoutModel, OT_AirFlue, ComponentList);

	TArray<TWeakPtr<FArmyObject>> RoomList;
	GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		//TSharedPtr<FArmyRoom> HomeMode_Room = StaticCastSharedPtr<FArmyRoom>(GetObjectByGuid(E_HomeModel, CurrentRoom->GetUniqueID()).Pin());
		if (CurrentRoom.IsValid()/* && HomeMode_Room.IsValid() && HomeMode_Room->bPreModified*/)
		{
			Clipper Cli;
			TArray<FVector> RoomPoints = CurrentRoom->GetWorldPoints(true);
			Paths ClipperPath;
			Path RoomPath;
			for (int i = 0; i < RoomPoints.Num(); i++)
				RoomPath.push_back(IntPoint(RoomPoints[i].X * 10, RoomPoints[i].Y * 10));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptSubject, true);

			bool bInRoom = false;
			bool bSplitRoom = false;
			TArray<TWeakPtr<FArmyObject>> DeleteAddWallList;
			for (auto ItAddWall : AddWallList)
			{
				TSharedPtr<FArmyAddWall> CurrentAddWall = StaticCastSharedPtr<FArmyAddWall>(ItAddWall.Pin());
				if (CurrentAddWall->GetIsHalfWall())
					continue;

				TArray<FVector> AddWallPoints;
				CurrentAddWall->GetVertexes(AddWallPoints);
				FVector CenterPoint = FBox(AddWallPoints).GetCenter();
				if (FArmyMath::IsPointInOrOnPolygon2D(CenterPoint, RoomPoints))
				{
					//找出对应的拆改中模式的AddWall，判断是否被修改过
					TWeakPtr<FArmyObject> ModifyModeObj = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, CurrentAddWall->GetUniqueID());
					if (ModifyModeObj.Pin().IsValid())
					{
						TSharedPtr<FArmyAddWall> ModifyModeAddWall = StaticCastSharedPtr<FArmyAddWall>(ModifyModeObj.Pin());
						if (ModifyModeAddWall.IsValid() && ModifyModeAddWall->bModified)
						{
							ModifyModeAddWall->bModified = false;
							bSplitRoom = true;
						}
					}

					ClipperPath.clear();
					RoomPath.clear();
					for (int i = 0; i < AddWallPoints.Num(); i++)
						RoomPath.push_back(IntPoint(AddWallPoints[i].X * 10, AddWallPoints[i].Y * 10));
					ClipperPath.push_back(RoomPath);
					Cli.AddPaths(ClipperPath, ptClip, true);

					DeleteAddWallList.Push(ItAddWall);
					bInRoom = true;
				}
			}			
			if (!bInRoom/* || !bSplitRoom*/)
				continue;			

			//把柱子风道加入布尔运算
			//此处只把不贴墙的柱子风道加入，贴墙的在前面已并入房间
			/*for (auto ItComponent : ComponentList)
			{
				TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(ItComponent.Pin());
				if (!CurrentComponent->IsAttachedToRoom())
				{
					TArray<FVector> ComponentPoints;
					CurrentComponent->Get4Vertexes(ComponentPoints);
					FVector CenterPoint = FBox(ComponentPoints).GetCenter();
					if (FArmyMath::IsPointInOrOnPolygon2D(CenterPoint, RoomPoints))
					{
						ClipperPath.clear();
						RoomPath.clear();
						for (int i = 0; i < ComponentPoints.Num(); i++)
							RoomPath.push_back(IntPoint(ComponentPoints[i].X * 10, ComponentPoints[i].Y * 10));
						ClipperPath.push_back(RoomPath);
						Cli.AddPaths(ClipperPath, ptClip, true);
					}
				}
			}*/

			for (auto ItDeleteWall : DeleteAddWallList)
			{
				AddWallList.Remove(ItDeleteWall);
				Delete(ItDeleteWall.Pin());
			}

			Paths ResultPoints;
			if (Cli.Execute(ctDifference, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				FString RoomSpaceName = CurrentRoom->GetSpaceName();
				if (ResultPoints.size() == 1)//空间没有被拆分，只是修改，单独处理
				{
					SCOPE_TRANSACTION(TEXT("修改空间"));

					TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
					TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
					for (int j = 0; j < ResultPoints[0].size(); j++)
					{
						FVector CurrentPoint(ResultPoints[0][j].X / 10.f, ResultPoints[0][j].Y / 10.f, 0);
						TSharedPtr<FArmyEditPoint> NewPoint = CurrentRoom->GetEditPoint(CurrentPoint);
						if (!NewPoint.IsValid())
							NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
						if (j > 0)
						{
							TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
							if (!CurrentRoom->IsHasLine(WallLine))
							{
								CurrentRoom->AddLine(WallLine);
								PrePoint->AddReferenceLine(WallLine->GetCoreLine());
								NewPoint->AddReferenceLine(WallLine->GetCoreLine());
							}
						}
						else
							FirstPoint = NewPoint;
						PrePoint = NewPoint;
						if (j == ResultPoints[0].size() - 1)
						{
							TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
							if (!CurrentRoom->IsHasLine(LastLine))
							{
								CurrentRoom->AddLine(LastLine);
								PrePoint->AddReferenceLine(LastLine->GetCoreLine());
								FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
							}
						}
					}

					//找出引用次数大等于3的线段，删除
					for (auto ItLine : CurrentRoom->GetWallLines())
					{
						if (ItLine->GetCoreLine()->GetStartPointer()->GetReferenceNum() >= 3
							&& ItLine->GetCoreLine()->GetEndPointer()->GetReferenceNum() >= 3)
						{
							ItLine->GetCoreLine()->GetStartPointer()->MinusReferenceLine(ItLine->GetCoreLine());
							ItLine->GetCoreLine()->GetEndPointer()->MinusReferenceLine(ItLine->GetCoreLine());
							CurrentRoom->RemoveLine(ItLine->GetCoreLine());
						}
					}
				}
				else
				{
					CurrentRoom->DeleteSpaceNameObj(false);//@ 临时处理
					Delete(CurrentRoom, !bSplitRoom);

					for (int i = 0; i < ResultPoints.size(); i++)
					{						

						//@欧石楠 在这里进行判断是否布尔运算结果出来的点跟柱子风道或者包立管的点完全一致，一致则说明不需要生成房间
						/*bool bFindNotNeedToCreate = false;
						for (auto ItComponent : ComponentList)
						{
							TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(ItComponent.Pin());
							if (CurrentComponent->IsAttachedToRoom())
							{
								continue;
							}
							TArray<FVector> ComponentPoints;
							CurrentComponent->Get4Vertexes(ComponentPoints);
							int TempCount = 0;
							for (int j = 0; j < ResultPoints[i].size(); j++)
							{
								for (auto ItPoint : ComponentPoints)
								{
									IntPoint TempPoint(ItPoint.X * 10, ItPoint.Y * 10);
									if (ResultPoints[i][j] == TempPoint)
									{
										++TempCount;
										break;
									}
								}
							}
							if (TempCount > ComponentPoints.Num() / 2 && TempCount > ResultPoints[i].size() / 2)
							{
								bFindNotNeedToCreate = true;
								break;
							}
						}
						if (bFindNotNeedToCreate)
						{
							continue;
						}*/

						TSharedPtr<FArmyRoom> NewRoom = MakeShareable(new FArmyRoom);

						//修改空间名称，使其增加编号
						NewRoom->ModifySpaceName(RoomSpaceName);
						FString TempRoomName = RoomSpaceName;
						TempRoomName = TempRoomName.Right(1).IsNumeric() ? RoomSpaceName.Left(RoomSpaceName.Len() - 1) : RoomSpaceName;
						NewRoom->SetSpaceId(FArmyUser::Get().SpaceTypeList->FindByValue(TempRoomName).Get()->Key);

						SCOPE_TRANSACTION(TEXT("添加空间"));
						Add(NewRoom, XRArgument(1).ArgUint32(E_LayoutModel));

						TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
						TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
						for (int j = 0; j < ResultPoints[i].size(); j++)
						{
							FVector CurrentPoint(ResultPoints[i][j].X / 10.f, ResultPoints[i][j].Y / 10.f, 0);
							TSharedPtr<FArmyEditPoint> NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
							if (j > 0)
							{
								TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
								NewRoom->AddLine(WallLine);
								PrePoint->AddReferenceLine(WallLine->GetCoreLine());
								NewPoint->AddReferenceLine(WallLine->GetCoreLine());
							}
							else
								FirstPoint = NewPoint;
							PrePoint = NewPoint;
							if (j == ResultPoints[i].size() - 1)
							{
								TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
								NewPoint->AddReferenceLine(LastLine->GetCoreLine());
								FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
								NewRoom->AddLine(LastLine);
							}

						}

						OnRoomAdded.ExecuteIfBound(NewRoom);

						GenerateNewRoomSpaceNameLabel(NewRoom, E_LayoutModel);
					}
				}
			}

		}
	}
}

void FArmySceneData::MergeRooms()
{
	TArray<TWeakPtr<FArmyObject>> ModifyWallList;
	GetObjects(E_LayoutModel, OT_ModifyWall, ModifyWallList);
	if (ModifyWallList.Num() == 0)
		return;

	TArray<TWeakPtr<FArmyObject>> AddWallList;
	GetObjects(E_LayoutModel, OT_AddWall, AddWallList);

	TArray<TWeakPtr<FArmyObject>> DeleteModifyWallList;
	for (auto ItModifyWall : ModifyWallList)
	{
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(ItModifyWall.Pin());
		TArray< TSharedPtr<FArmyLine> > ModifyLines;
		ModifyWall->GetLines(ModifyLines);

		Clipper Cli;
		TArray<FVector> ModifyPoints;
		ModifyWall->GetVertexes(ModifyPoints);
		Paths ClipperPath;
		Path ModifyPath;
		for (int i = 0; i < ModifyPoints.Num(); i++)
			ModifyPath.push_back(IntPoint(ModifyPoints[i].X * 1000, ModifyPoints[i].Y * 1000));
		ClipperPath.push_back(ModifyPath);
		Cli.AddPaths(ClipperPath, ptSubject, true);

		TArray<TSharedPtr<FArmyRoom>> CalRoomList;
		TArray<TWeakPtr<FArmyObject>> RoomList;
		GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
		for (auto It : RoomList)
		{
			TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			TSharedPtr<FArmyRoom> HomeMode_Room = StaticCastSharedPtr<FArmyRoom>(GetObjectByGuid(E_HomeModel, CurrentRoom->GetUniqueID()).Pin());
			if (CurrentRoom.IsValid()/* && HomeMode_Room.IsValid() && HomeMode_Room->bPreModified*/)
			{
				TArray< TSharedPtr<FArmyLine> > RoomLines;
				CurrentRoom->GetLines(RoomLines);
				for (auto ItRoomLine : RoomLines)
				{
					bool bFind = false;
					for (auto ItModifyLine : ModifyLines)
					{
						if (FArmyMath::IsLinesCollineationAndIntersection(ItRoomLine->GetStart(), ItRoomLine->GetEnd(),
							ItModifyLine->GetStart(), ItModifyLine->GetEnd(), false))
						{
							CalRoomList.Push(CurrentRoom);
							bFind = true;
							break;
						}
					}
					if (bFind)
						break;
				}
			}
		}

		for (auto ItCalRoom : CalRoomList)
		{
			TArray<FVector> RoomPoints = ItCalRoom->GetWorldPoints(true);
			Path RoomPath;
			for (int i = 0; i < RoomPoints.Num(); i++)
				RoomPath.push_back(IntPoint(RoomPoints[i].X * 1000, RoomPoints[i].Y * 1000));
			ClipperPath.clear();
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptClip, true);
		}


		Paths ResultPoints;
		if (Cli.Execute(ctUnion, ResultPoints, pftEvenOdd, pftEvenOdd))
		{
			for (auto ItCalRoom : CalRoomList)
			{
				Delete(ItCalRoom);
				OnRoomRemoved.ExecuteIfBound(ItCalRoom);
			}
			for (int i = 0; i < ResultPoints.size(); i++)
			{
				TSharedPtr<FArmyRoom> NewRoom = MakeShareable(new FArmyRoom);
				SCOPE_TRANSACTION(TEXT("添加空间"));
				Add(NewRoom, XRArgument(1).ArgUint32(E_LayoutModel));

				TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
				TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
				for (int j = 0; j < ResultPoints[i].size(); j++)
				{
					FVector CurrentPoint(ResultPoints[i][j].X / 1000.f, ResultPoints[i][j].Y / 1000.f, 0);
					TSharedPtr<FArmyEditPoint> NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
					if (j > 0)
					{
						TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
						NewRoom->AddLine(WallLine);
						PrePoint->AddReferenceLine(WallLine->GetCoreLine());
						NewPoint->AddReferenceLine(WallLine->GetCoreLine());
					}
					else
						FirstPoint = NewPoint;
					PrePoint = NewPoint;
					if (j == ResultPoints[i].size() - 1)
					{
						TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
						NewPoint->AddReferenceLine(LastLine->GetCoreLine());
						FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
						NewRoom->AddLine(LastLine);
					}

				}
				OnRoomAdded.ExecuteIfBound(NewRoom);

				GenerateNewRoomSpaceNameLabel(NewRoom, E_LayoutModel);
			}
		}

		DeleteModifyWallList.Push(ItModifyWall);
		/*ModifyWallList.Remove(ItModifyWall);
		Delete(ItModifyWall.Pin());
		if (ModifyWallList.Num() == 0)
			break;*/
	}

	TArray<TWeakPtr<FArmyObject>> HardwareList;
	GetHardWareObjects(HardwareList, E_LayoutModel);
	for (auto ItModifyWall : DeleteModifyWallList)
	{
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(ItModifyWall.Pin());
		if (ModifyWall.IsValid())
		{
			TArray<TWeakPtr<FArmyObject>> DeleteHardwareList;
			for (auto ItHardware : HardwareList)
			{
				TSharedPtr<FArmyHardware> CurrentObj = StaticCastSharedPtr<FArmyHardware>(ItHardware.Pin());
				if (CurrentObj.IsValid())
				{
					/** @欧石楠 此段注释取消了--当原始墙体上有构件，将此段墙体拆掉，然后新建墙，构件会留在新建墙上的功能逻辑*/
					/*bool isInAddWall = false;
					for (auto ItAddWall : AddWallList)
					{
						TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(ItAddWall.Pin());
						if (AddWall->IsPointInObj(CurrentObj->GetStartPos())
							&& AddWall->IsPointInObj(CurrentObj->GetEndPos()))
						{
							isInAddWall = true;
							break;
						}
					}*/
					if (/*!isInAddWall && */ModifyWall->IsPointInObj(CurrentObj->GetPos()))
					{
						DeleteHardwareList.Push(CurrentObj);
					}
				}
			}
			for (auto ItHardware : DeleteHardwareList)
			{
				HardwareList.Remove(ItHardware.Pin());
				Delete(ItHardware.Pin());
			}
		}

		ModifyWallList.Remove(ItModifyWall);
		Delete(ItModifyWall.Pin());
	}
}

void FArmySceneData::GetAllHardWare(TArray<TWeakPtr<FArmyHardware>>& OutHardwares, EModelType InModelType)
{
	if (InModelType == E_HomeModel)
	{
		OutHardwares = AllHardwares_Home;
	}
	else if (InModelType == E_ModifyModel)
	{
		OutHardwares = AllHardwares_Modify;
	}
	else if (InModelType == E_LayoutModel)
	{
		OutHardwares = AllHardwares_Layout;
	}
}

void FArmySceneData::SerializeToJsonDifferentMode(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter, EModelType InModelType /*= E_HomeModel*/)
{
	/**@欧石楠 存储墙高*/
	JsonWriter->WriteValue("wallHeight", WallHeight);

    // @欧石楠 保存墙体颜色
    JsonWriter->WriteValue("wallLinesColor", WallLinesColor.ToString());

	FString ModeName;
	if (InModelType == E_HomeModel)
	{
		ModeName = TEXT("homeMode");

		//防止原始模式保存后直接关闭程序
		TArray<TWeakPtr<FArmyObject>> CurrentOutRoom;
		GetObjects(E_ModifyModel, OT_OutRoom, CurrentOutRoom);
		if (CurrentOutRoom.Num() == 0 || bUpdateHomeModeData)
		{
			CopyModeData(E_HomeModel, E_ModifyModel);
			CopyModeData(E_ModifyModel, E_LayoutModel, true);
		}
	}
	else if (InModelType == E_ModifyModel)
	{
		ModeName = TEXT("modifyMode");

		//防止拆改中模式保存后直接关闭程序
		TArray<TWeakPtr<FArmyObject>> CurrentOutRoom;
		GetObjects(E_LayoutModel, OT_OutRoom, CurrentOutRoom);
		if (CurrentOutRoom.Num() == 0 || bUpdateModifyModeData)
		{
			CopyModeData(E_ModifyModel, E_LayoutModel, true);
		}
	}
	else if (InModelType == E_LayoutModel)
	{
		ModeName = TEXT("layoutMode");
	}

	// 写入开始
	JsonWriter->WriteObjectStart(ModeName);

    // @欧石楠 底图
    TWeakPtr<FArmyReferenceImage> Facsimile = GetFacsimile();
    if (Facsimile.IsValid())
    {
        JsonWriter->WriteObjectStart("facsimile");
        Facsimile.Pin()->SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }

	//标准窗
	TArray<FObjectWeakPtr> Windows;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_Window, Windows);

	JsonWriter->WriteArrayStart(TEXT("windowList"));
	for (auto It : Windows)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//落地窗
	TArray<FObjectWeakPtr> FloorWindows;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_FloorWindow, FloorWindows);

	JsonWriter->WriteArrayStart(TEXT("floorWindowList"));
	for (auto It : FloorWindows)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//飘窗
	TArray<FObjectWeakPtr> BayFloorWindows;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_RectBayWindow, BayFloorWindows);

	JsonWriter->WriteArrayStart(TEXT("bayFloorWindowList"));
	for (auto It : BayFloorWindows)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//梯形飘窗
	TArray<FObjectWeakPtr> TrapeBayWindows;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_TrapeBayWindow, TrapeBayWindows);

	JsonWriter->WriteArrayStart(TEXT("TrapeBayWindowList"));
	for (auto It : TrapeBayWindows)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//普通门
	TArray<FObjectWeakPtr> SingleDoors;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_Door, SingleDoors);

	JsonWriter->WriteArrayStart(TEXT("singleDoorList"));
	for (auto It : SingleDoors)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//推拉门
	TArray<FObjectWeakPtr> SlidingDoors;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_SlidingDoor, SlidingDoors);

	JsonWriter->WriteArrayStart(TEXT("slidingDoorList"));
	for (auto It : SlidingDoors)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//防盗门
	TArray<FObjectWeakPtr> SecurityDoors;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_SecurityDoor, SecurityDoors);

	JsonWriter->WriteArrayStart(TEXT("securityDoorList"));
	for (auto It : SecurityDoors)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//垭口
	TArray<FObjectWeakPtr> Passes;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_Pass, Passes);
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_DoorHole, Passes);

	JsonWriter->WriteArrayStart(TEXT("passList"));
	for (auto It : Passes)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//欧石楠 开垭口
	TArray<FObjectWeakPtr> NewPasses;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_NewPass, NewPasses);

	JsonWriter->WriteArrayStart(TEXT("newPassList"));
	for (auto It : NewPasses)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//开洞
	TArray<FObjectWeakPtr> Punches;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_Punch, Punches);

	JsonWriter->WriteArrayStart(TEXT("PunchList"));
	for (auto It : Punches)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//空间
	TArray<FObjectWeakPtr> Rooms;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_InternalRoom, Rooms);
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_OutRoom, Rooms);

	JsonWriter->WriteArrayStart(TEXT("roomList"));
	for (auto It : Rooms)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//--------------------------构件-------------------------------------
	TArray<TWeakPtr<FArmyObject>> Components;
	FArmySceneData::Get()->GetObjects(InModelType, OT_ComponentBase, Components);
	FArmySceneData::Get()->GetObjects(InModelType, OT_Drain_Point, Components);
	FArmySceneData::Get()->GetObjects(InModelType, OT_Gas_MainPipe, Components);
	JsonWriter->WriteArrayStart(TEXT("componentList"));
	for (auto It : Components)
	{
		if(!It.IsValid())
			continue;
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(It.Pin());
		if (Furniture->GetBreakPointType() == NewPoint)
		{
			continue;
		}

		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//--------------------------simple component-------------------------------------
	TArray<TWeakPtr<FArmyObject>> Beams;
	FArmySceneData::Get()->GetObjects(InModelType, OT_Beam, Beams);
	JsonWriter->WriteArrayStart(TEXT("beamList"));
	for (auto It : Beams)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	TArray<TWeakPtr<FArmyObject>> Pillars;
	FArmySceneData::Get()->GetObjects(InModelType, OT_Pillar, Pillars);
	JsonWriter->WriteArrayStart(TEXT("pillarList"));
	for (auto It : Pillars)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	TArray<TWeakPtr<FArmyObject>> AirFlues;
	FArmySceneData::Get()->GetObjects(InModelType, OT_AirFlue, AirFlues);
	JsonWriter->WriteArrayStart(TEXT("airFlueList"));
	for (auto It : AirFlues)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	/** @欧石楠 空调孔*/
	TArray<TWeakPtr<FArmyObject>> AirLouvers;
	FArmySceneData::Get()->GetObjects(InModelType, OT_AirLouver, AirLouvers);
	JsonWriter->WriteArrayStart(TEXT("AirLouversList"));
	for (auto It : AirLouvers)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	if (InModelType == E_ModifyModel)
	{
		//-----------------------拆改墙---------------------------------------
		TArray<FObjectWeakPtr> ModifyWalls;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, EObjectType::OT_ModifyWall, ModifyWalls);

		JsonWriter->WriteArrayStart(TEXT("modifyWallList"));
		for (auto It : ModifyWalls)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		TArray<FObjectWeakPtr> AddWalls;
		FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, EObjectType::OT_AddWall, AddWalls);

		JsonWriter->WriteArrayStart(TEXT("addWallList"));
		for (auto It : AddWalls)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		/**@欧石楠 独立墙存档*/
		TArray<FObjectWeakPtr> IndependentWalls;
		FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, EObjectType::OT_IndependentWall, IndependentWalls);

		JsonWriter->WriteArrayStart(TEXT("independentWallList"));
		for (auto It : IndependentWalls)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();		
	}
	if (InModelType == E_ModifyModel || InModelType == E_LayoutModel)
	{
		/**@欧石楠 包立管存档*/
		TArray<FObjectWeakPtr> PackPipes;
		FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, EObjectType::OT_PackPipe, PackPipes);

		JsonWriter->WriteArrayStart(TEXT("packPipeList"));
		for (auto It : PackPipes)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}	

	//-----------------------承重墙---------------------------------------
	TArray<FObjectWeakPtr> BearingWalls;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_BearingWall, BearingWalls);

	JsonWriter->WriteArrayStart(TEXT("bearingwalllist"));
	for (auto It : BearingWalls)
	{
		JsonWriter->WriteObjectStart();
		It.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//-----------------------空间名称---------------------------------------
	TArray<FObjectWeakPtr> TextLabels;
	FArmySceneData::Get()->GetObjects(InModelType, EObjectType::OT_TextLabel, TextLabels);

	JsonWriter->WriteArrayStart(TEXT("spacenamelist"));
	for (auto &It : TextLabels)
	{
		TSharedPtr<FArmyTextLabel> TLabel = StaticCastSharedPtr<FArmyTextLabel>(It.Pin());
		if (TLabel.IsValid() && TLabel->GetLabelType() == FArmyTextLabel::LT_SpaceName)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 区域相关数据序列化操作
	if (InModelType == E_LayoutModel)
	{
		//-----------------------分割线---------------------------------------
		TArray<FObjectWeakPtr> SplitLineArry;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, EObjectType::OT_SplitLine, SplitLineArry);
		JsonWriter->WriteArrayStart(TEXT("splitLineList"));
		for (auto It : SplitLineArry)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		//-----------------------区域---------------------------------------
		TArray<FObjectWeakPtr> RegionArry;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, EObjectType::OT_Region, RegionArry);
		JsonWriter->WriteArrayStart(TEXT("regionList"));
		for (auto It : RegionArry)
		{
			JsonWriter->WriteObjectStart();
			It.Pin()->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}

	//-----------------------模型Actor施工项---------------------------------------
	/* 放在这里是为了方便存取数据*/
	if (InModelType == E_HomeModel)
	{

		/*@郭子阳 保存施工项*/
		XRConstructionManager::Get()->SerializeToJson(JsonWriter);
		//保存完毕


		//TArray<AActor*> ActorList;
		//TArray<AArmyExtrusionActor*> ExtrusionActorList;
		///** @梁晓菲 遍历场景中所有的有效保存Actor*/
		//for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		//{
		//	AActor* TempActor = *ActorItr;
		//	if (TempActor->Tags.Contains("HydropowerActor") || TempActor->Tags.Contains("HydropowerPipeActor") || TempActor->Tags.Contains(XRActorTag::OriginalPoint))
		//	{
		//		continue;
		//	}

		//	EActorType ActorType = FArmyResourceModule::Get().GetResourceManager()->GetActorType(*ActorItr);
		//	if (ActorType == EActorType::Moveable || ActorType == EActorType::Blueprint)
		//	{
		//		ActorList.Add(TempActor);
		//	}

		//	AArmyExtrusionActor * ExtrusionActor = Cast<AArmyExtrusionActor>(*ActorItr);
		//	if (ExtrusionActor)
		//	{
		//		ExtrusionActorList.Add(ExtrusionActor);
		//	}
		//}
		///** @梁晓菲 先清空，再使用新数据保存，程序运行中Map数据改变不会影响，只关注保存时新赋的数据*/
		//ActorVectorConstructionItemData.Empty();
		//if (ActorUniqueIDConstructionItemDataMap.Num() > 0)
		//{
		//	for (auto ActorIt : ActorList)
		//	{
		//		if (ActorUniqueIDConstructionItemDataMap.Contains(ActorIt->GetUniqueID()))
		//		{
		//			TSharedPtr<FArmyConstructionItemInterface> tempItem = ActorUniqueIDConstructionItemDataMap[ActorIt->GetUniqueID()];
		//			if (tempItem.IsValid())
		//			{
		//				/** @梁晓菲 放置后没有被点击过，需要把默认勾选项设置进勾选数据中*/
		//				UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		//				TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(ActorIt->GetSynID());
		//				if (!tempItem->bHasSetted)
		//				{
		//					TSharedPtr<SArmyMulitCategory> ConstructionDetail = MakeShareable(new SArmyMulitCategory);
		//					ConstructionDetail->Init(GetJsonDataByItemID(ActorItemInfo->ID));
		//					tempItem->SetConstructionItemCheckedId(ConstructionDetail->GetCheckedData());
		//					tempItem->bHasSetted = true;
		//				}
		//				ActorVectorConstructionItemData.Add(VectorAndItemID(ActorIt->GetActorLocation(), ActorItemInfo->ID), tempItem);
		//			}
		//		}
		//	}
		//}
		//JsonWriter->WriteArrayStart(TEXT("ActorVectorConstructionItemData"));
		//for (auto It : ActorVectorConstructionItemData)
		//{
		//	JsonWriter->WriteObjectStart();
		//	It.Key.SerializeToJson(JsonWriter);
		//	It.Value->SerializeToJson(JsonWriter);
		//	JsonWriter->WriteObjectEnd();
		//}
		//JsonWriter->WriteArrayEnd();

		//if (ExtrusionConstructionItemDataMap.Num() > 0)
		//{
		//	for (auto ActorIt : ExtrusionActorList)
		//	{
		//		if (ExtrusionConstructionItemDataMap.Contains(ActorIt->UniqueCodeExtrusion))
		//		{
		//			TSharedPtr<FArmyConstructionItemInterface> tempItem = ExtrusionConstructionItemDataMap[ActorIt->UniqueCodeExtrusion];
		//			if (tempItem.IsValid())
		//			{
		//				/** @梁晓菲 放置后没有被点击过，需要把默认勾选项设置进勾选数据中*/
		//				if (!tempItem->bHasSetted)
		//				{
		//					TSharedPtr<SArmyMulitCategory> ConstructionDetail = MakeShareable(new SArmyMulitCategory);
		//					ConstructionDetail->Init(GetJsonDataByItemID(ActorIt->GetSynID()));
		//					tempItem->SetConstructionItemCheckedId(ConstructionDetail->GetCheckedData());
		//					tempItem->bHasSetted = true;
		//				}
		//			}
		//		}
		//	}
		//}
		//JsonWriter->WriteArrayStart(TEXT("ExtrusionConstructionItemData"));
		//for (auto It : ExtrusionConstructionItemDataMap)
		//{
		//	JsonWriter->WriteObjectStart();
		//	JsonWriter->WriteValue("ExtrusionCode", It.Key);
		//	It.Value->SerializeToJson(JsonWriter);
		//	JsonWriter->WriteObjectEnd();
		//}
		//JsonWriter->WriteArrayEnd();
	}

	// 写入结束
	JsonWriter->WriteObjectEnd();
}

void FArmySceneData::DeserializationDifferentMode(TSharedPtr<FJsonObject> Data, EModelType InModelType /*= E_HomeModel*/)
{
	/**@欧石楠 存储墙高*/
	float TempHeight = Data->GetNumberField("wallHeight");
	if (TempHeight != 0)
	{
		WallHeight = TempHeight;
	}

    // @欧石楠 加载墙体颜色
    FString WallLinesColorStr;
    if (Data->TryGetStringField("wallLinesColor", WallLinesColorStr))
    {
        WallLinesColor.InitFromString(WallLinesColorStr);
        SetWallLinesColor(WallLinesColor);
    }

	FString ModeName;
	if (InModelType == E_HomeModel)
	{
		ModeName = TEXT("homeMode");
	}
	else if (InModelType == E_ModifyModel)
	{
		ModeName = TEXT("modifyMode");
	}
	else if (InModelType == E_LayoutModel)
	{
		IgnoredModelIndexes.Reset();
		ModeName = TEXT("layoutMode");
	}

	TSharedPtr<FJsonObject> HomeModeData = Data->GetObjectField(ModeName);

    // @欧石楠 加载底图
    TSharedPtr<FJsonObject> FacsimileData = HomeModeData->GetObjectField("facsimile");
    if (FacsimileData.IsValid())
    {
        TSharedPtr<FArmyReferenceImage> TempFacsimile = MakeShareable(new FArmyReferenceImage);
        TempFacsimile->Deserialization(FacsimileData);

        if (TempFacsimile->GetData().Num() > 0 && InModelType == E_HomeModel)
        {
            SetFacsimile(TempFacsimile);
        }
    }

	const TArray<TSharedPtr<FJsonValue>> Rooms = HomeModeData->GetArrayField(TEXT("roomList"));
	for (int i = 0; i < Rooms.Num(); i++)
	{
		TSharedPtr<FArmyRoom> Room = MakeShareable(new FArmyRoom());
		Room->Deserialization(Rooms[i]->AsObject());
		if (Room->GetType() == OT_OutRoom)
		{
			bHasOutWall = true;
		}

		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Room, arg);
	}

	if (InModelType == E_ModifyModel)
	{
		const TArray<TSharedPtr<FJsonValue>> ModifyWalls = HomeModeData->GetArrayField(TEXT("modifyWallList"));
		for (auto& It : ModifyWalls)
		{
			TSharedPtr<FArmyModifyWall> ModifyWall = MakeShareable(new FArmyModifyWall());
			ModifyWall->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(ModifyWall, arg);
		}

		const TArray<TSharedPtr<FJsonValue>> AddWalls = HomeModeData->GetArrayField(TEXT("addWallList"));
		for (auto& It : AddWalls)
		{
			TSharedPtr<FArmyAddWall> AddWall = MakeShareable(new FArmyAddWall());
			AddWall->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(AddWall, arg);
		}

		const TArray<TSharedPtr<FJsonValue>> IndependentWalls = HomeModeData->GetArrayField(TEXT("independentWallList"));
		for (auto& It : IndependentWalls)
		{
			TSharedPtr<FArmyIndependentWall> IndependentWall = MakeShareable(new FArmyIndependentWall());
			IndependentWall->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(IndependentWall, arg);
		}
	}
	if (InModelType == E_ModifyModel || InModelType == E_LayoutModel)
	{
		/**@欧石楠 读档包立管*/
		const TArray<TSharedPtr<FJsonValue>> PackPipes = HomeModeData->GetArrayField(TEXT("packPipeList"));
		for (auto& It : PackPipes)
		{
			TSharedPtr<FArmyPackPipe> PackPipe = MakeShareable(new FArmyPackPipe());
			PackPipe->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(PackPipe, arg);
		}
	}

	const TArray<TSharedPtr<FJsonValue>> Windows = HomeModeData->GetArrayField(TEXT("windowList"));
	for (auto& It : Windows)
	{
		TSharedPtr<FArmyWindow> Window = MakeShareable(new FArmyWindow());
		Window->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Window, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> FloorWindows = HomeModeData->GetArrayField(TEXT("floorWindowList"));
	for (auto& It : FloorWindows)
	{
		TSharedPtr<FArmyFloorWindow> Window = MakeShareable(new FArmyFloorWindow());
		Window->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Window, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> RectBayWindows = HomeModeData->GetArrayField(TEXT("bayFloorWindowList"));
	for (auto& It : RectBayWindows)
	{
		TSharedPtr<FArmyRectBayWindow> Window = MakeShareable(new FArmyRectBayWindow());
		Window->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Window, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> TrapeBayWindows = HomeModeData->GetArrayField(TEXT("TrapeBayWindowList"));
	for (auto& It : TrapeBayWindows)
	{
		TSharedPtr<FArmyTrapeBayWindow> Window = MakeShareable(new FArmyTrapeBayWindow());
		Window->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Window, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> Passes = HomeModeData->GetArrayField(TEXT("passList"));
	for (auto& It : Passes)
	{
		TSharedPtr<FArmyPass> Pass = MakeShareable(new FArmyPass());
		Pass->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Pass, arg);
	}

	/**@欧石楠 读档开垭口*/
	const TArray<TSharedPtr<FJsonValue>> NewPasses = HomeModeData->GetArrayField(TEXT("newPassList"));
	for (auto& It : NewPasses)
	{
		TSharedPtr<FArmyNewPass> Pass = MakeShareable(new FArmyNewPass());
		Pass->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Pass, arg);
	}	

	const TArray<TSharedPtr<FJsonValue>> Doors = HomeModeData->GetArrayField(TEXT("singleDoorList"));
	for (auto& It : Doors)
	{
		TSharedPtr<FArmySingleDoor> Door = MakeShareable(new FArmySingleDoor());
		Door->Deserialization(It->AsObject());
		IgnoredModelIndexes.AddUnique(Door->GetDoorSaleID());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Door, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> SecurityDoors = HomeModeData->GetArrayField(TEXT("securityDoorList"));
	for (auto& It : SecurityDoors)
	{
		TSharedPtr<FArmySecurityDoor> Door = MakeShareable(new FArmySecurityDoor());
		Door->Deserialization(It->AsObject());
		IgnoredModelIndexes.AddUnique(Door->GetDoorSaleID());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Door, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> SlidingDoors = HomeModeData->GetArrayField(TEXT("slidingDoorList"));
	for (auto& It : SlidingDoors)
	{
		TSharedPtr<FArmySlidingDoor> Door = MakeShareable(new FArmySlidingDoor());
		Door->Deserialization(It->AsObject());
		IgnoredModelIndexes.AddUnique(Door->GetDoorSaleID());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Door, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> Punches = HomeModeData->GetArrayField(TEXT("PunchList"));
	for (auto& It : Punches)
	{
		TSharedPtr<FArmyPunch> Punch = MakeShareable(new FArmyPunch());
		Punch->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Punch, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> Components = HomeModeData->GetArrayField(TEXT("componentList"));
	for (auto& It : Components)
	{
		TSharedPtr<FArmyFurniture> Component = MakeShareable(new FArmyFurniture());
		Component->Deserialization(It->AsObject());
	

		//常远 2019/06/06 特殊处理，兼容地漏模型
		if (Component->ComponentType == EC_Basin)
		{
			FString MaterialParameter = Component->GetFurniturePro()->GetOptimizeParam();
			if (MaterialParameter.Contains("SN:0-V7_Base_Plastic_MT"))
			{
				FString ParamPath = Component->GetFurniturePro()->GetModelPath();
				ParamPath = FPaths::SetExtension(ParamPath, TEXT(".json"));
				if (FPaths::FileExists(ParamPath))
				{
					Component->GetFurniturePro()->SetOptimizeParam(ParamPath);
				}
			}
		}

		//@郭子阳 V1.3 将原始户型和拆改中都设置为原始点位,这是为了向下兼容
		if ((InModelType == E_HomeModel) || (InModelType == E_ModifyModel))
		{
			Component->SetBreakPointType(EBreakPointType::NativePoint);
		}


		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Component, arg);
		if (Component.IsValid())
		{
			FArmyAutoCad::Get()->AddComponent(Component->ComponentType, Component);
		}
	}

	//测试梁
	const TArray<TSharedPtr<FJsonValue>> Beams = HomeModeData->GetArrayField(TEXT("beamList"));
	for (auto& It : Beams)
	{
		TSharedPtr<FArmyBeam> Component = MakeShareable(new FArmyBeam());
		Component->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Component, arg);
	}

	//测试柱
	const TArray<TSharedPtr<FJsonValue>> Pillars = HomeModeData->GetArrayField(TEXT("pillarList"));
	for (auto& It : Pillars)
	{
		TSharedPtr<FArmyPillar> Component = MakeShareable(new FArmyPillar());
		Component->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Component, arg);
	}

	//测试风道
	const TArray<TSharedPtr<FJsonValue>> AirFlues = HomeModeData->GetArrayField(TEXT("airFlueList"));
	for (auto& It : AirFlues)
	{
		TSharedPtr<FArmyAirFlue> Component = MakeShareable(new FArmyAirFlue());
		Component->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Component, arg);
	}

	/** @欧石楠 空调孔*/
	const TArray<TSharedPtr<FJsonValue>> AirLouvers = HomeModeData->GetArrayField(TEXT("AirLouversList"));
	for (auto& It : AirLouvers)
	{
		TSharedPtr<FArmyAirLouver> Component = MakeShareable(new FArmyAirLouver());
		Component->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(Component, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> BearingWalls = HomeModeData->GetArrayField(TEXT("bearingwalllist"));
	for (auto& It : BearingWalls)
	{
		TSharedPtr<FArmyBearingWall> BearingWall = MakeShareable(new FArmyBearingWall());
		BearingWall->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(BearingWall, arg);
	}

	const TArray<TSharedPtr<FJsonValue>> SpaceNames = HomeModeData->GetArrayField(TEXT("spacenamelist"));
	for (auto& It : SpaceNames)
	{
		TSharedPtr<FArmyTextLabel> SpaceName = MakeShareable(new FArmyTextLabel());
		SpaceName->Deserialization(It->AsObject());
		XRArgument arg;
		arg._ArgUint32 = InModelType;
		FArmySceneData::Get()->Add(SpaceName, arg);

        UE_LOG(LogHomeData, Log, TEXT("SpaceName : %s"), *SpaceName->GetLabelContent().ToString());
	}

	CreateSpaceNameToRoomRelated(InModelType);

	// 区域相关初始化
	if (InModelType == E_LayoutModel)
	{
		// 分割线初始化
		const TArray<TSharedPtr<FJsonValue>> SplitLineArray = HomeModeData->GetArrayField(TEXT("splitLineList"));
		for (auto& It : SplitLineArray)
		{
			TSharedPtr<FArmySplitLine> SplitLine = MakeShareable(new FArmySplitLine());
			SplitLine->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(SplitLine, arg);
		}

		// 区域初始化
		const TArray<TSharedPtr<FJsonValue>> RegionArray = HomeModeData->GetArrayField(TEXT("regionList"));
		for (auto& It : RegionArray)
		{
			TSharedPtr<FArmyRegion> Region = MakeShareable(new FArmyRegion());
			Region->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = InModelType;
			FArmySceneData::Get()->Add(Region, arg);
		}
	}

	if (InModelType == E_HomeModel)
	{

		/*@郭子阳 读取施工项*/
		XRConstructionManager::Get()->Deserialization(HomeModeData);
		// 读取完毕

		//反序列化旧版施工项
		const TArray<TSharedPtr<FJsonValue> > ConstructionArray = HomeModeData->GetArrayField("ActorVectorConstructionItemData");
		for (auto& It : ConstructionArray)
		{
			TSharedPtr<FJsonObject> ConJObject = It->AsObject();
			VectorAndItemID TempVectorAndItemID = VectorAndItemID(FVector::ZeroVector, -1);
			TempVectorAndItemID.Deserialization(ConJObject);
			TSharedPtr<FArmyConstructionItemInterface> tempConInter = MakeShareable(new FArmyConstructionItemInterface);
			tempConInter->Deserialization(ConJObject);
			ActorVectorConstructionItemData.Add(TempVectorAndItemID, tempConInter);
		}

		const TArray<TSharedPtr<FJsonValue> > ExtrusionConstructionArray = HomeModeData->GetArrayField("ExtrusionConstructionItemData");
		for (auto& It : ExtrusionConstructionArray)
		{
			TSharedPtr<FJsonObject> ConJObject = It->AsObject();
			FString temp;
			ConJObject->TryGetStringField("ExtrusionCode", temp);
			TSharedPtr<FArmyConstructionItemInterface> tempConInter = MakeShareable(new FArmyConstructionItemInterface);
			tempConInter->Deserialization(ConJObject);
			ExtrusionConstructionItemDataMap.Add(temp, tempConInter);
		}
	}
}

void FArmySceneData::ChangeBaseToModifyModelDelegate()
{
	if (bUpdateHomeModeData)
	{
		if (FArmySceneData::Get()->bIsDisplayDismantle)//拆改中
		{
			CopyModeData(E_HomeModel, E_ModifyModel);
		}
		else                                        //拆改后
		{
			CopyModeData(E_HomeModel, E_ModifyModel);
			CopyModeData(E_ModifyModel, E_LayoutModel, true);
			bUpdateModifyModeData = false;
			bUpdateLayoutModeData = true;
		}

		bUpdateHomeModeData = false;
	}
}

void FArmySceneData::GenerateHardwareModel()
{
	/////////////////////////////////生成门窗/////////////////////////////////////////
	TArray<TWeakPtr<FArmyObject>> WindowList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CornerBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, WindowList);
	for (TWeakPtr<FArmyObject> iter : WindowList)
	{
		iter.Pin()->Generate(GGI->GetWorld());
	}
}

void FArmySceneData::GeneratePointPositionModel()
{
	TArray<TWeakPtr<FArmyObject>> FurnitureList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, FurnitureList);
	for (auto & F : FurnitureList)
	{
		TSharedPtr<FArmyFurniture> Fur = StaticCastSharedPtr<FArmyFurniture>(F.Pin());
		AActor * FurActor = Fur->GetRelevanceActor();
		AActor * PipeActor = nullptr;
		bool bIsExist = false;
		TArray<FObjectPtr> Objs = Fur->GetChildren();
		TSharedPtr<FArmyPipeline> PipeLine;
		for (auto& ObjectPtr : Objs)
		{
			if (ObjectPtr->GetType() >= OT_Preload_Begin && ObjectPtr->GetType() <= OT_Preload_End)
			{
				PipeLine = StaticCastSharedPtr<FArmyPipeline>(ObjectPtr);
				PipeActor = PipeLine->GetRelevanceActor();
			}
		}
		if (FurActor->IsValidLowLevel() || PipeActor->IsValidLowLevel())
		{
			for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
			{
				if (Fur->GetFurniturePro()->GetbIsPakModel() && FurActor == *ActorItr)
				{
					Fur->ModifyRelatedActorTransform(FurActor);
					bIsExist = true;
					break;
				}
				else if (PipeActor == *ActorItr)
				{
					Fur->ModifyRelatedFurnitureActorTransform();
					bIsExist = true;
					break;
				}
			}
		}

		if (!bIsExist)
		{
			if (Fur.IsValid() && Fur->GetFurniturePro().IsValid())
			{

				if (Fur->GetFurniturePro()->GetbIsPakModel())
				{
					if (!FPaths::FileExists(Fur->GetFurniturePro()->GetModelPath()))
						continue;

					FVector InLocation = Fur->LocalTransform.GetLocation();
					FRotator InRotator = FRotator(Fur->LocalTransform.GetRotation());
					FVector InScale = FVector::OneVector /*Fur->LocalTransform.GetScale3D()*/;

					//计算点位模型的下沿距离地面的高度，注意：在立面下离地高度非中心点距离地面的高度
					InLocation.Z = 0;
					InLocation.Z += Fur->GetFurniturePro()->GetAltitude();

					AActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), Fur->GetFurniturePro()->FurContentItem, InLocation, InRotator, InScale);
					if (!NewActor)
						continue;
					NewActor->Tags.Add(XRActorTag::IgnoreCollision);
					Fur->ModifyRelatedActorTransform(NewActor);

					NewActor->SetFolderPath(FArmyActorPath::GetOriginalPointPath());
					NewActor->SetActorLabel(Fur->GetName());
					NewActor->Tags.Add(TEXT("HydropowerActor"));
					
					PointPosGenerateDelegate.ExecuteIfBound(Fur, NewActor);
				}
				else
				{
					PointPosGenerateDelegate.ExecuteIfBound(Fur, nullptr);
				}

				Fur->SetRelatedActorSelected(false);
				//Fur->SetBreakPointType(EBreakPointType::NativePoint);
			}
		}
	}
}

TSharedPtr<FArmyRoom> FArmySceneData::GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType /*= E_HomeModel*/)
{
	TArray<TWeakPtr<FArmyObject>> InternalRoomList;
	TArray<TWeakPtr<FArmyObject>> OutRoomList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_InternalRoom, InternalRoomList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_OutRoom, OutRoomList);

	TSharedPtr<FArmyRoom> Room;
	for (auto It : InternalRoomList)
	{
		Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
			{
				if (Line.IsValid() && Line == CheckedLine)
				{
					return Room;
				}
			}
		}
	}

	for (auto It : OutRoomList)
	{
		Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
			{
				if (Line.IsValid() && Line == CheckedLine)
				{
					return Room;
				}
			}
		}
	}

	return nullptr;
}

bool FArmySceneData::CaptureDoubleLine(const FVector& InPos, EModelType InModelType, FDoubleWallCaptureInfo& OutInfo)
{
    TArray<WallLineInfo> Lines;
    TArray<TWeakPtr<FArmyObject>> RoomList;
    GetObjects(InModelType, OT_InternalRoom, RoomList);
    GetObjects(InModelType, OT_OutRoom, RoomList);
    for (auto It : RoomList)
    {
        TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
        TArray<TSharedPtr<FArmyLine>> RoomLines;
        if (Room.IsValid())
        {
            Room->GetLines(RoomLines);
            for (auto Line : RoomLines)
            {
                if (Line.IsValid())
                {
                    Lines.AddUnique(WallLineInfo(Line, Room));
                }
            }
        }
    }

    TArray< TWeakPtr<FArmyObject> > AddWallList;
    GetObjects(InModelType, OT_AddWall, AddWallList);
    GetObjects(InModelType, OT_IndependentWall, AddWallList);
    for (auto It : AddWallList)
    {
        TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
        TArray<TSharedPtr<FArmyLine>> AddWallLines;
        if (AddWall.IsValid())
        {
            AddWall->GetLines(AddWallLines);
            for (auto Line : AddWallLines)
            {
                if (Line.IsValid())
                {
                    Lines.AddUnique(WallLineInfo(Line, AddWall));
                }
            }
        }
    }

    return CaptureDoubleLine(Lines, InPos, OutInfo);
}

bool FArmySceneData::CaptureDoubleLine(TArray<WallLineInfo>& InLines, const FVector& InPos, FDoubleWallCaptureInfo& OutInfo)
{
	TSharedPtr<FArmyLine> FirstLine;
	TSharedPtr<FArmyLine> SecondLine;
	FObjectPtr FirstOwnerObj;
	FObjectPtr SecondOwnerObj;

	// 捕捉墙体的最大距离
	const float MaxDistance = FArmySceneData::MaxWallCaptureThickness;
	float MinDistance = MaxDistance;

	// 捕捉第一面墙
	FVector FirstSnapPos = InPos;
	for (auto & LInfo : InLines)
	{
		TWeakPtr<FArmyLine> Object = LInfo.Line;
        if (!Object.IsValid())
        {
            continue;
        }

		TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());
		FVector P0 = Wall->GetStart();
		FVector P1 = Wall->GetEnd();

		FVector Projection = InPos;
		if (FArmyMath::GetLineSegmentProjectionPos(P0, P1, Projection))
		{
			float Distance = FVector::Distance(InPos, Projection);
			if (Distance > 0.f && Distance < MaxDistance && Distance < MinDistance)
			{
				FirstSnapPos = Projection;
				FirstLine = Wall;
				MinDistance = Distance;
				FirstOwnerObj = LInfo.Owner;
			}
		}
	}

	// 捕捉第二面墙
	FVector SecondSnapPos = InPos;
	if (FirstLine.IsValid())
	{
		float TempMinDistance = MaxDistance;
		for (auto & LInfo : InLines)
		{
			TWeakPtr<FArmyLine> Object = LInfo.Line;
            if (!Object.IsValid())
            {
                continue;
            }

			TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());

			bool IsSameline = (FirstLine->GetStart() == Wall->GetStart() && FirstLine->GetEnd() == Wall->GetEnd()) ? true : false;
			bool bIsParallel = FArmyMath::AreLinesParallel(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd());
			if (bIsParallel && !IsSameline)
			{
				FVector Projection = InPos;
				if (FArmyMath::GetLineSegmentProjectionPos(Wall->GetStart(), Wall->GetEnd(), Projection))
				{
					float Distance = FVector::Distance(InPos, Projection);
					if (Distance >= 0.0f && Distance <= MaxDistance && Distance < TempMinDistance)
					{
						SecondSnapPos = Projection;
						SecondLine = Wall;
						TempMinDistance = Distance;
						SecondOwnerObj = LInfo.Owner;
					}
				}
			}
		}
	}

	if (FirstLine.IsValid() && SecondLine.IsValid() && FirstOwnerObj.IsValid() && SecondOwnerObj.IsValid())
	{
        // @欧石楠 只有当新的门窗放置位置不在空间内，才允许摆放
        FVector NewHardwarePos = FArmyMath::GetLineCenter(FirstSnapPos, SecondSnapPos);

        if (FirstOwnerObj->GetType() == OT_InternalRoom && FirstOwnerObj->IsSelected(NewHardwarePos, GVC))
        {
            return false;
        }
        if (SecondOwnerObj->GetType() == OT_InternalRoom && SecondOwnerObj->IsSelected(NewHardwarePos, GVC))
        {
            return false;
        }

		OutInfo.FirstLine = FirstLine;
		OutInfo.SecondLine = SecondLine;
		OutInfo.FirstOwner = FirstOwnerObj;
		OutInfo.SecondOwner = SecondOwnerObj;

		// 保存两面墙中长度较短的那一面，用作计算门到两边墙面的距离
		const float FirstWallLength = FVector::Distance(FirstLine->GetStart(), FirstLine->GetEnd());
		const float SecondWallLength = FVector::Distance(SecondLine->GetStart(), SecondLine->GetEnd());
		OutInfo.MinDistanceWall = FirstWallLength <= SecondWallLength ? FirstLine : SecondLine;

		OutInfo.Pos = (FirstSnapPos + SecondSnapPos) / 2.0f;
		OutInfo.Thickness = FVector::Distance(FirstSnapPos, SecondSnapPos);

		if (FVector::Distance(InPos, FirstSnapPos) > FVector::Distance(InPos, SecondSnapPos))
		{
			OutInfo.Direction = FArmyMath::GetLineDirection(FirstSnapPos, SecondSnapPos);
		}
		else
		{
			OutInfo.Direction = FArmyMath::GetLineDirection(SecondSnapPos, FirstSnapPos);
		}

		return true;
	}

	return false;
}

void FArmySceneData::ForceCaptureDoubleLine(TSharedPtr<FArmyHardware> InHardware, EModelType InModelType/* = E_HomeModel*/)
{
	TArray<WallLineInfo> Lines;
	TArray< TWeakPtr<FArmyObject> > RoomList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_OutRoom, RoomList);

	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray< TSharedPtr<FArmyLine> > RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto ItLine : RoomLines)
				if (ItLine.IsValid())
					Lines.AddUnique(WallLineInfo(ItLine, Room));
		}
	}

	TArray< TWeakPtr<FArmyObject> > AddWallList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_AddWall, AddWallList);
	for (auto It : AddWallList)
	{
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> AddWallLines;
		if (AddWall.IsValid())
		{
			AddWall->GetLines(AddWallLines);
			for (auto Line : AddWallLines)
				if (Line.IsValid())
					Lines.AddUnique(WallLineInfo(Line, AddWall));
		}
	}

	FVector WorldPos = InHardware->GetPos();
	WorldPos.Z = 0;

	FDoubleWallCaptureInfo CaptureWallInfo;
	if (CaptureDoubleLine(Lines, WorldPos, CaptureWallInfo))
	{
		if (InHardware->LinkFirstLine != CaptureWallInfo.FirstLine)
		{
			InHardware->LinkFirstLine = CaptureWallInfo.FirstLine;
			InHardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.FirstLine, InModelType);
			InHardware->InWallType = CaptureWallInfo.FirstOwner->GetType() == OT_AddWall ? 1 : 0;
		}
		if (InHardware->LinkSecondLine != CaptureWallInfo.SecondLine)
		{
			InHardware->LinkSecondLine = CaptureWallInfo.SecondLine;
			InHardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.SecondLine, InModelType);
			InHardware->InWallType = CaptureWallInfo.FirstOwner->GetType() == OT_AddWall ? 1 : 0;
		}

		FVector Direction;
		if (CaptureWallInfo.FirstLine.IsValid())
		{
			Direction = CaptureWallInfo.FirstLine->GetStart() - CaptureWallInfo.FirstLine->GetEnd();
		}
		else
		{
			Direction = FVector(1, 0, 0);
		}
		Direction = Direction.GetSafeNormal();
		Direction = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
		InHardware->SetPos(CaptureWallInfo.Pos);
		InHardware->SetDirection(Direction);
		InHardware->SetWidth(CaptureWallInfo.Thickness);
	}
}

void FArmySceneData::ForceUpdateHardwaresCapturing(TSharedPtr<FArmyRoom> InRoom)
{
	if (InRoom.IsValid())
	{
		TArray< TSharedPtr<FArmyHardware> > Hardwares;
		InRoom->GetHardwareListRelevance(Hardwares);

		for (auto It : Hardwares)
		{
			ForceCaptureDoubleLine(It);
			It->Update();
			if (It->GetType() == OT_SecurityDoor)
			{
				TSharedPtr<FArmySecurityDoor> SecurityDoor = StaticCastSharedPtr<FArmySecurityDoor>(It);
				SecurityDoor->UpdateDoorHole();
			}
		}
	}
}

void FArmySceneData::GetRelatedRoomSpaceAreas(TSharedPtr<FArmyRoomSpaceArea> SourceRoomSpaceArea, TArray< TSharedPtr<FArmyRoomSpaceArea> >& OutRoomSpaceAreas)
{
    TSharedPtr<FArmyRoom> AttachRoom;
    if (GetInnearRoomByRoomID(SourceRoomSpaceArea->AttachRoomID, AttachRoom))
    {
        // @欧石楠 获取与该面邻接的两个面
        TArray< TSharedPtr<FArmyWallLine> > RelatedWallLines;
        AttachRoom->GetRelatedWallLines(SourceRoomSpaceArea->GetAttachLine(), RelatedWallLines);

        TArray< TSharedPtr<FArmyRoomSpaceArea> > RelatedRoomSpaceAreas;
        for (auto WallLine : RelatedWallLines)
        {
            TSharedPtr<FArmyRoomSpaceArea> RelatedRoomSpaceArea =
                FArmySceneData::Get()->GetWallByRoomIdAndAttachWallLineId(SourceRoomSpaceArea->AttachRoomID, WallLine->GetUniqueID().ToString());
            OutRoomSpaceAreas.Add(RelatedRoomSpaceArea);
        }
    }
}

TWeakPtr<class FArmyReferenceImage> FArmySceneData::GetFacsimile()
{
    TArray<FObjectWeakPtr> ReferenceImageArray;
    GetObjects(E_HomeModel, OT_ReferenceImage, ReferenceImageArray);

    return ReferenceImageArray.Num() > 0 ? StaticCastSharedPtr<FArmyReferenceImage>(ReferenceImageArray[0].Pin()) : nullptr;
}

const int32 FArmySceneData::GetFacsimileTransparency()
{
    return GetFacsimile().IsValid() ? GetFacsimile().Pin()->GetTransparency() : 60;
}

void FArmySceneData::SetFacsimileTransparency(int32 InImageTransparency)
{
    TWeakPtr<FArmyReferenceImage> Facsimile = GetFacsimile();
    if (Facsimile.IsValid())
    {
        Facsimile.Pin()->SetTransparency(FMath::Clamp<int32>(InImageTransparency, 0, 100));
    }
}

void FArmySceneData::SetWallLinesColor(const FLinearColor InColor)
{
	WallLinesColor = InColor;

	TArray<FObjectWeakPtr> Rooms;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, Rooms);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, Rooms);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, Rooms);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_OutRoom, Rooms);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, Rooms);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, Rooms);

	for (auto It : Rooms)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			Room->SetWallLinesColor(InColor);
		}
	}
}

void FArmySceneData::GenerateDataTo3D()
{
	if (bUpdateHomeModeData/* && !bUpdateModifyModeData*/)
	{
		CopyModeData(E_HomeModel, E_ModifyModel);
		CopyModeData(E_ModifyModel, E_LayoutModel, true);

		bUpdateHomeModeData = false;
		bUpdateModifyModeData = false;
		bRebuild3DModel = true;
	}
	else if (!bUpdateHomeModeData && bUpdateModifyModeData)
	{
		CopyModeData(E_ModifyModel, E_LayoutModel, true);
		bUpdateModifyModeData = false;
		bRebuild3DModel = true;
	}
	else if (!bUpdateHomeModeData && !bUpdateModifyModeData && bUpdateLayoutModeData)
	{
		bUpdateLayoutModeData = false;
		bRebuild3DModel = true;
	}
	else if (!bUpdateHomeModeData && !bUpdateModifyModeData && !bUpdateLayoutModeData)
	{
		bRebuild3DModel = false;
	}

	if (bRebuild3DModel)
	{
		GenerateHardwareModel();
		Generate3DSurfaceActors();
		GeneratePointPositionModel();

		bRebuild3DModel = false;
		ChangedHomeDataDelegate.Broadcast();
	}
}

void FArmySceneData::ClearHardModeData()
{
	//加载新方案前清空当前方案数据

	//先手动删除面
	TArray<TWeakPtr<FArmyObject>> SelectedRoomLists;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, SelectedRoomLists);
	for (TWeakPtr<FArmyObject > iter : SelectedRoomLists)
	{
		TSharedPtr<FArmyRoomSpaceArea> temp = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		//@郭子阳
		//递归地删除绘制区域
		std::function<void(TSharedPtr<FArmyBaseArea>& Area)>
			DeleteSubAreas = [&DeleteSubAreas](TSharedPtr<FArmyBaseArea> & Area) {
			for (auto & SubArea : Area->GetEditAreas())
			{
				DeleteSubAreas(SubArea);
			}
			//Area->RoomEditAreas.Empty();

			FArmySceneData::Get()->Delete(Area);
		};

		for (auto& SubArea : temp->GetEditAreas())
		{
			DeleteSubAreas(SubArea);
		}
		temp->RoomEditAreas.Empty();

		FArmySceneData::Get()->Delete(iter.Pin(), true);
	}


	const	TArray<TWeakPtr<FArmyObject>>& TotalHardModeData = GetObjects(E_HardModel);
	while (TotalHardModeData.Num() > 0)
	{
		FArmySceneData::Get()->Delete(TotalHardModeData.Last().Pin());
	}
	if (TotalSolidWallActor)
	{
		TotalSolidWallActor->Destroy();
		TotalSolidWallActor = NULL;
	}
	if (TotalOutRoomActor)
	{
		TotalOutRoomActor->Destroy();
		TotalOutRoomActor = NULL;
	}

	OutterWallLightMapID = FGuid();
}


void FArmySceneData::Refresh()
{
	for (auto& ObjArrayIt : ObjectArrayMap)
	{
		for (auto ObjIt : ObjArrayIt.Value)
		{
			ObjIt.Pin()->Refresh();
		}
	}
}
void FArmySceneData::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto& ObjArrayIt : ObjectArrayMap)
	{
		for (auto ObjIt : ObjArrayIt.Value)
		{
			ObjIt.Pin()->Draw(PDI, View);
		}
	}
}

void FArmySceneData::DrawGlobalObject(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto iter : GlobalDataArray)
	{
		iter.Pin()->Draw(PDI, View);
	}
}
void FArmySceneData::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	for (auto& ObjArrayIt : ObjectArrayMap)
	{
		for (auto ObjIt : ObjArrayIt.Value)
		{
			ObjIt.Pin()->DrawHUD(InViewPortClient, Viewport, View, Canvas);
		}
	}
}

void FArmySceneData::Generate(EObjectType ObjectType)
{
	for (auto ObjIt : ObjectArrayMap.FindOrAdd(ObjectType))
	{
		ObjIt.Pin()->Generate(World);
	}
}

void FArmySceneData::RegenerateAll()
{
	// 重新生成前先清空Brushes
	//BSPManager->EmptyBrushes();

	for (auto& ObjArrayIt : ObjectArrayMap)
	{
		for (auto ObjIt : ObjArrayIt.Value)
		{
			ObjIt.Pin()->Generate(World);
		}
	}

	//BSPManager->RebuildAlteredBSP();
}
void FArmySceneData::SetObjectVisible(bool InVisible, EModelType InModelType, EObjectType InObjectType)
{
	if (InModelType == E_NoneModel)
	{
		for (auto & ModelIt : ObjectArrayMap)
		{
			for (auto ObjIt : ModelIt.Value)
			{
				if (InObjectType == OT_None)
				{
					ObjIt.Pin()->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
				}
				else if (ObjIt.Pin()->GetType() == InObjectType)
				{
					ObjIt.Pin()->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
				}
			}
		}
	}
	else
	{
		TArray<FObjectWeakPtr>* ObjList = ObjectArrayMap.Find(InModelType);
		if (ObjList)
		{
			for (auto ObjIt : *ObjList)
			{
				if (InObjectType == OT_None)
				{
					ObjIt.Pin()->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
				}
				else if (ObjIt.Pin()->GetType() == InObjectType)
				{
					ObjIt.Pin()->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
				}
			}
		}
	}
}

void FArmySceneData::AddItemIDConstructionData(int32 InItemID, TSharedPtr<FJsonObject> InJsonData)
{
	if (!ItemIDJsonDataMap.Contains(InItemID))
	{
		ItemIDJsonDataMap.Emplace(InItemID, InJsonData);
	}
	else
	{
		ItemIDJsonDataMap[InItemID] = InJsonData;
	}
}

TSharedPtr<FJsonObject> FArmySceneData::GetJsonDataByItemID(int32 InItemID)
{
	if (ItemIDJsonDataMap.Contains(InItemID))
	{
		return ItemIDJsonDataMap[InItemID];
	}
	return nullptr;
}

void FArmySceneData::AddActorConstructionItemData(int32 InActorID, TSharedPtr<FArmyConstructionItemInterface> InConstructionItemData)
{
	if (!ActorUniqueIDConstructionItemDataMap.Contains(InActorID))
	{
		ActorUniqueIDConstructionItemDataMap.Emplace(InActorID, InConstructionItemData);
	}
	else
	{
		ActorUniqueIDConstructionItemDataMap[InActorID] = InConstructionItemData;
	}
}

void FArmySceneData::DeleteActorConstructionItemData(int32 InActorID)
{
	if (ActorUniqueIDConstructionItemDataMap.Contains(InActorID))
	{
		ActorUniqueIDConstructionItemDataMap.Remove(InActorID);
	}
}

TSharedPtr<FArmyConstructionItemInterface> FArmySceneData::GetConstructionItemDataByActorUniqueID(int32 InActorID)
{
	if (ActorUniqueIDConstructionItemDataMap.Contains(InActorID))
	{
		return ActorUniqueIDConstructionItemDataMap[InActorID];
	}
	return nullptr;
}

TSharedPtr<FArmyConstructionItemInterface> FArmySceneData::GetConstructionItemDataByActorVector(FVector AvtorVector, int32 InItemID)
{
	if (ActorVectorConstructionItemData.Contains(VectorAndItemID(AvtorVector, InItemID)))
	{
		return ActorVectorConstructionItemData[VectorAndItemID(AvtorVector, InItemID)];
	}
	return nullptr;
}

void FArmySceneData::AddExtrusionConstructionData(FString ExturesionUniqueCode, TSharedPtr<FArmyConstructionItemInterface> InConstructionItemData)
{
	if (!ExtrusionConstructionItemDataMap.Contains(ExturesionUniqueCode))
	{
		ExtrusionConstructionItemDataMap.Emplace(ExturesionUniqueCode, InConstructionItemData);
	}
	else
	{
		ExtrusionConstructionItemDataMap[ExturesionUniqueCode] = InConstructionItemData;
	}
}

TSharedPtr<FArmyConstructionItemInterface> FArmySceneData::GetConstructionItemDataByExtrusionUniqueCode(FString ExturesionUniqueCode)
{
	if (ExtrusionConstructionItemDataMap.Contains(ExturesionUniqueCode))
	{
		return ExtrusionConstructionItemDataMap[ExturesionUniqueCode];
	}
	return nullptr;
}

void FArmySceneData::DeleteExtrusionConstructionItemData(FString ExturesionUniqueCode)
{
	if (ExtrusionConstructionItemDataMap.Contains(ExturesionUniqueCode))
	{
		ExtrusionConstructionItemDataMap.Remove(ExturesionUniqueCode);
	}
}