#include "ArmyAutoDesignPipeLine.h"
#include "ArmyCommonTypes.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyHydropowerPowerSystem.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyFurniture.h"
#include "ArmyMath.h"
#include "ArmyCommonTypes.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyPipePoint.h"
#include "ArmyHardware.h"
#include "ArmyViewportClient.h"
#include "SArmyHydropowerAutoDesignError.h"
#include "ArmyGameInstance.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "FArmyAutoDesignPipeLine"

#define AIR_CONDITIONING_SOCKET_FIRST 1000
#define POWER_SOCKET_FIRST 2000

FArmyAutoDesignPipeLine::FArmyAutoDesignPipeLine()
{
	ErrorMap.Add(EAutoDesignErrorType::EAD_ERROR_SPACENAME,FAutoDesignErrorInfo(TEXT("请命名房间"),TEXT("HydropowerMode.namespace"),TEXT("请在【原始户型】或【拆改户型】下为所有房间命名"),TEXT("HydropowerMode.namespaceInfo")));
	ErrorMap.Add(EAutoDesignErrorType::EAD_ERROR_ELE_STRONG,FAutoDesignErrorInfo(TEXT("请添加强电箱"),TEXT("HydropowerMode.eleh"),TEXT("请在左侧商品列表添加强电箱"),TEXT("HydropowerMode.elehInfo")));
	ErrorMap.Add(EAutoDesignErrorType::EAD_ERROR_ELE_SOCKET_STRONG,FAutoDesignErrorInfo(TEXT("请添加强电插座"),TEXT("HydropowerMode.socket"),TEXT("请在左侧商品列表添加强电插座"),TEXT("HydropowerMode.socketInfo")));//1
	ErrorMap.Add(EAutoDesignErrorType::EAD_ERROR_POWERSYSTEM,FAutoDesignErrorInfo(TEXT("请添加回路"),TEXT("HydropowerMode.Loop"),TEXT("请在【配电箱系统设置】中添加回路"),TEXT("HydropowerMode.LoopInfo")));
	SAssignNew(ResultInfoView,SArmyHydropowerAutoDesignFinish)
	.MaxListHeight(320)
	.OnClicked_Lambda([&]()
	{
		GGI->Window->DismissModalDialog();
		return FReply::Handled();
	})
	.OptionsSource(&ResultList);
}

void FArmyAutoDesignPipeLine::AutoDesign(const TSharedPtr<FHydropowerPowerSytem> InLoopInfo)
{
	if (AutoDesignPreJudge(InLoopInfo))
	{
		AutoDesignStart();
		GGI->GetWorld()->GetTimerManager().SetTimer(DesignBeginTimeHandle,FTimerDelegate::CreateRaw(this,&FArmyAutoDesignPipeLine::OnAutoDesign_Delayed,InLoopInfo),0.3f,false);
		//AutoDesignEnd();
	}
	AutoDesignAdjust();
}
TSharedPtr<FArmyAutoDesignPipeLine::FATPass> FArmyAutoDesignPipeLine::AutoDesignGetOrCreatePass(FArmyAutoDesignPipeLine::FATPass& InPass)
{
	for (auto PassPtr : AllPassArray)
	{
		if (*PassPtr == InPass)
		{
			return PassPtr;
		}
	}
	TSharedPtr<FATPass> NewPassPtr = MakeShareable(new FATPass(InPass));
	AllPassArray.Add(NewPassPtr);
	return NewPassPtr;
}

void FArmyAutoDesignPipeLine::SetUIState(EAutoDesignState::Type InState)
{
	GGI->Window->DismissModalDialog();
	CurrentState = InState;
	switch (CurrentState)
	{
	case EAutoDesignState::AutoDesigning:
	{
		TAttribute<FText> SyncProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([&]()
		{
			return FText::FromString(TEXT("智能布线中..."));
		}));
		GGI->Window->ShowThrobber(SyncProgressTextAttr);
	}
	break;
	case EAutoDesignState::AutoDesignSuccessed:
	{
		//
		GGI->Window->ShowMessage(MT_Success,TEXT("布置成功!"));
	}
	break;
	case EAutoDesignState::AutoDesignFailed:
	{
		//GGI->Window->DismissModalDialog();
		GGI->Window->PresentModalDialog(
			TEXT("布置完成"),
			ResultInfoView->AsShared()
		);
	}
	break;
	default:
		break;
	}
//
	
}

void FArmyAutoDesignPipeLine::AutoDesignStart()
{
	SetUIState(EAutoDesignState::AutoDesigning);
	ResultList.Reset();
}

void FArmyAutoDesignPipeLine::AutoDesignEnd()
{
	GGI->GetWorld()->GetTimerManager().SetTimer(DesignTimeHandle, FTimerDelegate::CreateLambda([&]()
	{
		bool bSuccess = true;
		for (auto& Item : ResultList)
		{
			if (Item->bSuccessed == false)
			{
				bSuccess = false;
				break;
			}
		}
		if (bSuccess)
		{
			SetUIState(EAutoDesignState::AutoDesignSuccessed);
		}
		else
		{
			SetUIState(EAutoDesignState::AutoDesignFailed);
		}
	}), 0.3f, false);
}
void FArmyAutoDesignPipeLine::OnAutoDesignPre()
{
}
void FArmyAutoDesignPipeLine::OnAutoDesign_Delayed(const TSharedPtr<FHydropowerPowerSytem> InLoopInfo)
{
	OnAutoDesignPre();

	int32 LoopNum = InLoopInfo->LoopArray.Num();
	Step1Weight = 3;
	Step2Weight = 1;
	Step4Weight = 4;
	Step5Weight = 4*2;
	Step6Weight = 4*2;
	Step7Weight = 3*3;
	CountWeight = Step1Weight+Step2Weight+Step4Weight * LoopNum+Step5Weight * LoopNum+Step6Weight * LoopNum+Step7Weight * LoopNum;
	SyncProgress = 0;
	AutoDesignUpdateRoom();
	SyncProgress += 100*Step1Weight/CountWeight;

	//自动设计水管

	//获取回路信息
	//遍历所有回路
	//自动设计插座回路
	//自动设计照明回路
	//自动设计冰箱回路
	//自动设计空调回路
	//自动设计动力回路
	//自动设计弱电回路
	//所有线路避让、排布调整（优化）
	//100 / (3 + loop num * 3 * (4 + 2 * 4 + 2* 4 + 3* 3))
	AutoDesignWaterPipeLine();// step 2
	SyncProgress += 100*Step2Weight/CountWeight;

	for (auto & LoopInfo:InLoopInfo->LoopArray) //step 3 : all loop room number
	{
		switch (LoopInfo->LoopType)
		{
		case FArmyHydropowerDataManager::PSP_WK:
		{
			AutoDesignSocketPipeLine(LoopInfo, 1);
			break;
		}
		case FArmyHydropowerDataManager::PSP_WP:
		{
			AutoDesignSocketPipeLine(LoopInfo, 1);
			break;
		}
		case FArmyHydropowerDataManager::PSP_WX:
		case FArmyHydropowerDataManager::PSP_WB:
			AutoDesignSocketPipeLine(LoopInfo);
			break;
		case FArmyHydropowerDataManager::PSP_WL:
			AutoDesignLightingPipeLine(LoopInfo);
			break;
		case 101://弱电以后对接
			AutoDesignELVPipeLine(LoopInfo);
			break;
		default:
			break;
		}
	}
	AutoDesignGeneratePipeLines();

	SyncProgress = 100;
	OnAutoDesignPost();
}
void FArmyAutoDesignPipeLine::OnAutoDesignPost()
{
	AutoDesignEnd();
}

void FArmyAutoDesignPipeLine::Empty()
{
	AllRoomArray.Empty();
	AllPassArray.Empty();
	PipeLoopArray.Empty();
	PassRoomMap.Empty();
	PassPlaneMap.Empty();
	PointPassMap.Empty();
	OriginalPoint.Reset();
	OriginalPlane.Reset();
	OriginalRoom.Reset();
}

void FArmyAutoDesignPipeLine::AutoDesignUpdateRoom()
{
	AllRoomArray.Empty();
	TArray<FObjectWeakPtr> InnerRoomArray, ComponentArray;
	TArray<FATPipePointInfo> ElectricObjArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel,OT_InternalRoom, InnerRoomArray);

	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_ComponentBase, ComponentArray);
	FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_ComponentBase, ComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel,OT_ComponentBase,ComponentArray);

	TArray<TWeakPtr<FArmyHardware>> DetailObjects;
	FArmySceneData::Get()->GetAllHardWare(DetailObjects,E_LayoutModel);

	//int32 AirConditionerCount = 0,PowerCount = 0;
	for (auto CObj : ComponentArray)
	{
		
		if (CObj.Pin()->GetPropertyFlag(FArmyObject::PropertyType::FLAG_STOPPED))
		{
			continue;
		}
		TSharedPtr<FArmyFurniture> CPtr = StaticCastSharedPtr<FArmyFurniture>(CObj.Pin());
		FVector BasePos = CPtr->GetBasePos();
		BasePos.Z = CPtr->GetAltitude();

		switch (CPtr->BelongClass)
		{
		case 8://Class_Switch
		{
			int32 PointIndex = ElectricObjArray.Add(FATPipePointInfo(BasePos, FVector(0, 0, 0)));
			ElectricObjArray[PointIndex].PointType = 5;
		}
		break;
		case 9://Class_Socket
		{
			int32 PointIndex = ElectricObjArray.Add(FATPipePointInfo(BasePos, FVector(0, 0, 0)));
			switch (CPtr->GetSocketType())
			{
			case ESF_Normal: //普通
				ElectricObjArray[PointIndex].PointType = 1;
				break;
			case ESF_AirConditioner: //空调
				ElectricObjArray[PointIndex].PointType = 2;/* + AIR_CONDITIONING_SOCKET_FIRST + ++AirConditionerCount;*/
				break;
			case ESF_Refrigerator: //冰箱
				ElectricObjArray[PointIndex].PointType = 3;
				break;
			case ESF_Power: //动力:
				ElectricObjArray[PointIndex].PointType = 4;/* + POWER_SOCKET_FIRST + ++PowerCount;*/
				break;
			default:
				ElectricObjArray[PointIndex].PointType = 1;
				break;
			}
		}
		break;
		case 10://Class_ElectricBoxH
		{
			int32 PointIndex = ElectricObjArray.Add(FATPipePointInfo(BasePos, FVector(0, 0, 0)));
			ElectricObjArray[PointIndex].PointType = 7;
		}
		break;
		case 11://Class_ElectricBoxL
		{
			int32 PointIndex = ElectricObjArray.Add(FATPipePointInfo(BasePos, FVector(0, 0, 0)));
			ElectricObjArray[PointIndex].PointType = 8;
		}
		break;
		case 12://Class_Light
		{
			int32 PointIndex = ElectricObjArray.Add(FATPipePointInfo(BasePos, FVector(0, 0, 0)));
			ElectricObjArray[PointIndex].PointType = 6;
		}
		break;
		default:
			break;
		}
	}
	for (auto R : InnerRoomArray)
	{
		TSharedPtr<FArmyRoom>RoomPtr = StaticCastSharedPtr<FArmyRoom>(R.Pin());
		TSharedPtr<FATRoom> ATRoomPtr = MakeShareable(new FATRoom(R.Pin()->GetUniqueID()));
		int32 Index = AllRoomArray.AddUnique(ATRoomPtr);
		ATRoomPtr->RoomName = RoomPtr->GetSpaceName();
		if (RoomPtr->GetSpaceName().Contains(TEXT("厨房")))
		{
			ATRoomPtr->RoomType = 1;
		}
		else if (RoomPtr->GetSpaceName().Contains(TEXT("卫生间")))
		{
			ATRoomPtr->RoomType = 2;
		}
		else if (RoomPtr->GetSpaceName().Contains(TEXT("阳台")))
		{
			ATRoomPtr->RoomType = 3;
		}

		TArray< TSharedPtr<FArmyLine> > WallLines;
		RoomPtr->GetLines(WallLines);
		for (auto L : WallLines)
		{
			FVector StartP = L->GetStart();
			FVector EndP = L->GetEnd();
			StartP.Z = 0;
			EndP.Z = 0;
			FVector LDir = EndP - StartP;
			FVector Normal(-LDir.Y, LDir.X, 0);
			LDir.Normalize();
			Normal.Normalize();
			
			if (!RoomPtr->IsPointInRoom((StartP + EndP) / 2 + Normal))
			{
				Normal *= -1;
			}
			TSharedPtr<FATRoomPlane> ATPlanePtr = MakeShareable(new FATRoomPlane(1, StartP, EndP, Normal));
			ATRoomPtr->PlaneArray.AddUnique(ATPlanePtr);
			////////////////////////////////////////////////////////////
			for (auto Obj : DetailObjects)
			{
				EObjectType ObjType = Obj.Pin()->GetType();
				if (ObjType == OT_Pass ||
					ObjType == OT_DoorHole ||
					ObjType == OT_NewPass ||
					(ObjType == OT_Door && Obj.Pin()->AsassignObj<FArmyHardware>() && Obj.Pin()->AsassignObj<FArmyHardware>()->InWallType == 1))
				{
					FVector PStart = Obj.Pin()->GetStartPos();
					FVector PEnd = Obj.Pin()->GetEndPos();

					float W = Obj.Pin()->GetWidth();
					FVector HardWareDir = (PEnd - PStart).GetSafeNormal();
					FVector WDir(-HardWareDir.Y, HardWareDir.X, 0);

					float StartDis1 = FMath::PointDistToSegment(PStart + WDir * W / 2, StartP, EndP);
					float EndDis1 = FMath::PointDistToSegment(PEnd + WDir * W / 2, StartP, EndP);

					float StartDis2 = FMath::PointDistToSegment(PStart - WDir * W / 2, StartP, EndP);
					float EndDis2 = FMath::PointDistToSegment(PEnd - WDir * W / 2, StartP, EndP);

					if ((StartDis1 < 0.5 && EndDis1 < 0.5) || (StartDis2 < 0.5 && EndDis2 < 0.5))
					{
						FATPass Temp(PStart, PEnd);
						TSharedPtr<FATPass> ATPassPtr = AutoDesignGetOrCreatePass(Temp);
						ATPlanePtr->PassArray.AddUnique(ATPassPtr);
						ATPlanePtr->PipePointArray.AddUnique(ATPassPtr->DoorCenterPoint);
						PointPassMap.Add(ATPassPtr->DoorCenterPoint, ATPassPtr);
						PassRoomMap.AddUnique(ATPassPtr, ATRoomPtr);
						PassPlaneMap.AddUnique(ATPassPtr, ATPlanePtr);
					}
				}
			}

			float Angle = FQuat::FindBetweenNormals(LDir, Normal).GetAngle();
			for (auto C : ElectricObjArray)
			{	
				if (FMath::PointDistToSegment(FVector(C.InnerPos.X, C.InnerPos.Y,0), StartP, EndP) < 0.1)
				{
					TSharedPtr<FATPipePointInfo> ATPointPtr = MakeShareable(new FATPipePointInfo(C.InnerPos, Normal));
					ATPlanePtr->PipePointArray.AddUnique(ATPointPtr);
					ATPointPtr->PointType = C.PointType;

					if (Angle < 0)
					{
						ATPointPtr->LinkPointMap.Add(1, TPair<FVector, bool>(C.InnerPos + LDir * 2.5,false));
						ATPointPtr->LinkPointMap.Add(2, TPair<FVector, bool>(C.InnerPos - LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(3, TPair<FVector, bool>(C.InnerPos + LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(4, TPair<FVector, bool>(C.InnerPos - LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(5, TPair<FVector, bool>(C.InnerPos, false));
						ATPointPtr->LinkPointMap.Add(6, TPair<FVector, bool>(C.InnerPos, false));
					}
					else
					{
						ATPointPtr->LinkPointMap.Add(1, TPair<FVector, bool>(C.InnerPos - LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(2, TPair<FVector, bool>(C.InnerPos + LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(3, TPair<FVector, bool>(C.InnerPos - LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(4, TPair<FVector, bool>(C.InnerPos + LDir * 2.5, false));
						ATPointPtr->LinkPointMap.Add(5, TPair<FVector, bool>(C.InnerPos, false));
						ATPointPtr->LinkPointMap.Add(6, TPair<FVector, bool>(C.InnerPos, false));
					}
					
					if (C.PointType == 7)//强电箱
					{
						OriginalPoint = ATPointPtr;
						OriginalPlane = ATPlanePtr;
						OriginalRoom = ATRoomPtr;
						OriginalPoint->Index = 0;
						OriginalPlane->Index = 0;
						OriginalRoom->Index = 0;

						//强电箱上的连接点
						OriginalIndexRangeDir = (EndP - StartP).GetSafeNormal();
						for (int32 i = -4,j = 1;i < 5;++i,++j)
						{
							FVector OP = OriginalPoint->InnerPos + OriginalIndexRangeDir * i * 2.5;
							OriginalPointMap.Add(j, TPair<FVector, bool>(OP, false));
						}
					}
				}
			}
		}
	}
}
int32 FArmyAutoDesignPipeLine::AutoDesignGetPassType(const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATPass> InPass,const TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>>& RouteMap, TSharedPtr<FATRoom>& OutRoom)
{
	TArray<TSharedPtr<FATRoom>> LinkRooms;
	PassRoomMap.MultiFind(InPass, LinkRooms);
	for (auto R : LinkRooms)
	{
		if (R != InRoom && RouteMap.Contains(R))
		{
			for (auto & RouteInfo : RouteMap[R])
			{
				if (RouteInfo.RouteRoom == InRoom)
				{
					OutRoom = R;
					return 1;
				}
			}
		}
	}
	bool RouteInRoom = false;

	for (auto RObj : RouteMap)
	{
		int32 RouteNum = 0;
		for (auto& RInfo : RObj.Value)
		{
			for (auto R : LinkRooms)
			{
				if (RInfo.RouteRoom == R)
				{
					RouteNum++;
				}
			}

			if (RouteNum > 1)
			{
				OutRoom = RObj.Key;
				return 2;
			}
		}
	}

	return -1;
}
void FArmyAutoDesignPipeLine::AutoDesignPlaneRange(TArray<TSharedPtr<FATRoomPlane>>& InPlaneArray,TSharedPtr<FATRoomPlane> RoomStartPlane ,TSharedPtr<FATRoomPlane>& PrePlane, TSharedPtr<FATRoomPlane>& PassPlane, int32& MaxIndex)
{
	TArray<TSharedPtr<FATRoomPlane>> LostPlaneArray;
	for (auto Plane : InPlaneArray)
	{
		if (Plane == RoomStartPlane)
		{
			continue;
		}
		bool SS = Plane->StartPos == PrePlane->StartPos;
		bool SE = Plane->StartPos == PrePlane->EndPos;
		bool ES = Plane->EndPos == PrePlane->StartPos;
		bool EE = Plane->EndPos == PrePlane->EndPos;
		if (SS || SE || ES || EE)
		{
			Plane->Index = ++MaxIndex;
			PrePlane = Plane;

			FVector Dir = Plane->EndPos - Plane->StartPos;
			if (ES || EE)
			{
				Dir *= -1;
			}
			Dir.Normalize();
			Plane->RangeDirection = Dir;
		}
		else
		{
			LostPlaneArray.AddUnique(Plane);
		}
	}
	if (LostPlaneArray.Num() > 0)
	{
		AutoDesignPlaneRange(LostPlaneArray, RoomStartPlane, PrePlane, PassPlane, MaxIndex);
	}
}
bool FArmyAutoDesignPipeLine::AutoDesignRouteRoom(TArray<TSharedPtr<FATRoom>>& InRoomArray, TSharedPtr<FATRoom> InRoom, TSharedPtr<FATRoomPlane> InPlane,TSharedPtr<FATPass> InPass, TArray<TSharedPtr<FATPass>>& InRoutePassArray, TArray<RoomRouteInfo>& InRouteArray)
{
	for (auto R : InRoomArray)
	{
		if (InRoom == R) continue;

		for (auto PlanePtr : R->PlaneArray)
		{
			for (auto PassPtr : PlanePtr->PassArray)
			{
				if (*PassPtr == *InPass)
				{
					InRouteArray.Insert(RoomRouteInfo(R, PlanePtr, InPass),0);
					if (R == OriginalRoom)
					{
						return true;
					}
					else
					{
						bool ExistRoute = false;
						TArray<RoomRouteInfo> MinRouteArray;
						for (auto PlanePtr : R->PlaneArray)
						{
							for (auto PassPtr : PlanePtr->PassArray)
							{
								if (PassPtr != InPass && !InRoutePassArray.Contains(PassPtr))
								{
									InRoutePassArray.AddUnique(PassPtr);
									TArray<RoomRouteInfo> TempRouteArray;
									if (AutoDesignRouteRoom(InRoomArray, R, PlanePtr, PassPtr, InRoutePassArray, TempRouteArray))
									{
										//TempRouteArray.AddUnique(RoomRouteInfo(R, PlanePtr, PassPtr));
										if (ExistRoute)
										{
											if (MinRouteArray.Num() > TempRouteArray.Num())
											{
												MinRouteArray = TempRouteArray;
											}
										}
										else
										{
											MinRouteArray = TempRouteArray;
											ExistRoute = true;
										}
									}
								}
							}
						}
						//for (auto Pass : PlanePtr->PassArray)
						//{
						//	if (Pass != InPass)
						//	{
						//		TArray<RoomRouteInfo> TempRouteArray;
						//		if (AutoDesignRouteRoom(InRoomArray, R, PlanePtr, Pass, TempRouteArray))
						//		{
						//			InRouteArray.AddUnique(RoomRouteInfo(R, PlanePtr, Pass));
						//			if (ExistRoute)
						//			{
						//				if (MinRouteArray.Num() > TempRouteArray.Num())
						//				{
						//					MinRouteArray = TempRouteArray;
						//				}
						//			}
						//			else
						//			{
						//				MinRouteArray = TempRouteArray;
						//				ExistRoute = true;
						//			}
						//		}
						//	}
						//}
						if (ExistRoute)
						{
							//InRouteArray.Append(MinRouteArray);
							for (int32 i = MinRouteArray.Num() - 1;i >= 0 ;--i)
							{
								InRouteArray.Insert(MinRouteArray[i],0);
							}
							return true;
						}
					}
					break;
				}
			}
		}
	}
	return false;
}
void FArmyAutoDesignPipeLine::AutoDesignRange(TArray<TSharedPtr<FATRoom>>& InRoomArray, TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>>& InRouteMap)
{
	//从配电箱所在墙面开始，沿墙体走向，关联所有房间，将房间排序，同时将墙体排序（考虑柱子和烟道、房梁）
	//排序墙体，定义墙体编号
	//插座按墙体顺序并且按距离顺序排序


	//查找从出发房间到所有房间的联通关系（通过门洞关联）
	//TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>> RouteMap;
	InRouteMap.Empty();
	for (auto R : InRoomArray)
	{
		if (R == OriginalRoom) continue;

		TArray<RoomRouteInfo>& CurrentRouteArray = InRouteMap.Add(R, TArray<RoomRouteInfo>());

		bool RouteExist = false;
		TArray<RoomRouteInfo> MinRouteArray;
		for (auto PlanePtr : R->PlaneArray)
		{
			for (auto Pass : PlanePtr->PassArray)
			{
				TArray<RoomRouteInfo> TempRouteArray;
				TArray<TSharedPtr<FATPass>> CurrentRoutePassArray{ Pass };
				if (AutoDesignRouteRoom(AllRoomArray, R, PlanePtr, Pass, CurrentRoutePassArray, TempRouteArray))
				{
					TempRouteArray.AddUnique(RoomRouteInfo(R, PlanePtr, Pass));
					if (RouteExist)
					{
						if (MinRouteArray.Num() > TempRouteArray.Num())
						{
							MinRouteArray = TempRouteArray;
						}
					}
					else
					{
						MinRouteArray = TempRouteArray;
						RouteExist = true;
					}
				}
			}
		}
		if (RouteExist)
		{
			CurrentRouteArray.Append(MinRouteArray);
		}
		else
		{
			InRouteMap.Remove(R);
		}
	}

	//根据房间的路由关系排序房间
	struct FRouteSort
	{
		FRouteSort(FVector InStart) :StartPos(InStart) {};
		FORCEINLINE bool operator()(const TArray<RoomRouteInfo>& A, const TArray<RoomRouteInfo>& B) const
		{
			if (A.Num() > 0 && B.Num() > 0)
			{
				if (A[0].RoutePass == B[0].RoutePass)
				{
					return A.Num() < B.Num();
				}
				else
				{
					FVector AOffset = StartPos - (A[0].RoutePass->StartPos + A[0].RoutePass->EndPos) / 2;
					FVector BOffset = StartPos - (B[0].RoutePass->StartPos + B[0].RoutePass->EndPos) / 2;
					return AOffset.Size() < BOffset.Size();
				}
			}
			else
			{
				return A.Num() == 0;
			}
		}
		FVector StartPos = FVector(ForceInitToZero);
	};

	InRouteMap.ValueSort(FRouteSort(OriginalPoint->InnerPos));

	TSharedPtr<FATRoomPlane> RoutePassPlane = InRouteMap.Num() == 0 ? NULL : InRouteMap.CreateIterator().Value()[0].RoutePlane;

	//排序墙面
	int32 MaxIndex = OriginalPlane->Index;
	TSharedPtr<FATRoomPlane> PrePlane = OriginalPlane;
	//TArray<TSharedPtr<FATRoomPlane>> LostPlaneArray;
	for (auto PlanePtr : OriginalRoom->PlaneArray)
	{
		if (PlanePtr == OriginalPlane)
		{
			continue;
		}
		if (RoutePassPlane == PlanePtr)
		{
			PrePlane = OriginalPlane;
		}
		bool SS = PlanePtr->StartPos == PrePlane->StartPos;
		bool SE = PlanePtr->StartPos == PrePlane->EndPos;
		bool ES = PlanePtr->EndPos == PrePlane->StartPos;
		bool EE = PlanePtr->EndPos == PrePlane->EndPos;
		if (SS || SE || ES || EE)
		{
			PlanePtr->Index = ++MaxIndex;
			PrePlane = PlanePtr;

			FVector Dir = PlanePtr->EndPos - PlanePtr->StartPos;
			if (ES || EE)
			{
				Dir *= -1;
			}
			Dir.Normalize();
			PlanePtr->RangeDirection = Dir;
			if (MaxIndex == OriginalPlane->Index + 1)
			{
				Dir = OriginalPlane->EndPos - OriginalPlane->StartPos;
				if (SS || ES)
				{
					Dir *= -1;
				}
				Dir.Normalize();
				OriginalPlane->RangeDirection = Dir;
			}
		}
		//else
		//{
		//	LostPlaneArray.AddUnique(PlanePtr);
		//}
	}
	//AutoDesignPlaneRange(LostPlaneArray, OriginalPlane, PrePlane, RoutePassPlane, MaxIndex);
	//if (RoutePassPlane != OriginalPlane)
	//{
	//	RoutePassPlane->Index = ++MaxIndex;
	//}


	//for (auto RInfo : InRouteMap)
	//{
	//	MaxIndex = 0;
	//	TSharedPtr<FATRoomPlane> PassPlane = RInfo.Value.Last().RoutePlane;
	//	TSharedPtr<FATRoomPlane> PrePlane = PassPlane;
	//	PassPlane->Index = MaxIndex;
	//	TArray<TSharedPtr<FATRoomPlane>> LostPlaneArray;
	//	for (auto PlanePtr : RInfo.Key->PlaneArray)
	//	{
	//		bool SS = PlanePtr->StartPos == PrePlane->StartPos;
	//		bool SE = PlanePtr->StartPos == PrePlane->EndPos;
	//		bool ES = PlanePtr->EndPos == PrePlane->StartPos;
	//		bool EE = PlanePtr->EndPos == PrePlane->EndPos;
	//		if (SS || SE || ES || EE)
	//		{
	//			if (PassPlane == PlanePtr)
	//			{
	//				PrePlane = PassPlane;
	//			}
	//			else
	//			{
	//				PlanePtr->Index = ++MaxIndex;
	//				PrePlane = PlanePtr;

	//				FVector Dir = PlanePtr->EndPos - PlanePtr->StartPos;
	//				if (ES || EE)
	//				{
	//					Dir *= -1;
	//				}
	//				Dir.Normalize();
	//				PlanePtr->RangeDirection = Dir;

	//				if (MaxIndex == 1)
	//				{
	//					Dir = PassPlane->EndPos - PassPlane->StartPos;
	//					if (SS || ES)
	//					{
	//						Dir *= -1;
	//					}
	//					Dir.Normalize();
	//					PassPlane->RangeDirection = Dir;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			LostPlaneArray.AddUnique(PlanePtr);
	//		}
	//	}
	//	AutoDesignPlaneRange(LostPlaneArray, PassPlane,PrePlane,PassPlane,MaxIndex);
	//}
}
void FArmyAutoDesignPipeLine::AutoDesignRouteAllPoint(const TArray<TSharedPtr<FATRoom>>& InRelateRoomArray, const TSharedPtr<FArmyAutoDesignPipeLine::FATRoom> InCurrentRoom, const TSharedPtr<FArmyAutoDesignPipeLine::FATRoomPlane> InStartPlane, const TSharedPtr<FArmyAutoDesignPipeLine::FATPipePointInfo> InStartPoint, TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>>& InRouteMap, TArray<TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>>& RoutePointArray, TArray<TSharedPtr<FATRoomPlane>>& OutSortPlaneArray, int32& InCount,int32 InPointType,int32 InLimitCount,bool InStart)
{
	TSharedPtr<FATPipePointInfo> PrePoint = InStartPoint;
	TSharedPtr<FATRoomPlane> CurrentPlane = InStartPlane;
	TSharedPtr<FATRoom> CurrentRoom = InCurrentRoom;

	CurrentPlane->PipePointArray.Sort([&](const TSharedPtr<FATPipePointInfo>& A, const TSharedPtr<FATPipePointInfo>& B)
	{
		return (A->InnerPos - PrePoint->InnerPos).Size2D() < (B->InnerPos - PrePoint->InnerPos).Size2D();
	});//提前对当前面上的点做排序 按离当前面的起始点的距离

	bool ExistLinkPlane = false;

	for (auto PointPtr : CurrentPlane->PipePointArray)
	{
		if (PointPtr == OriginalPoint)
		{
			if (!InStart)
			{
				return;//递归到起始点，需要跳出
			}
			continue;
		}
		if (PointPtr->PointType != -1 && PointPtr->PointType != InPointType)
		{
			continue;//非普通插座排除
		}
		if (PointPtr->PointType != -1 && !InRelateRoomArray.Contains(CurrentRoom))
		{
			continue;//没有在关联的房间内
		}
		if (PointPtr->Index >= 0)
		{
			continue;//已经被占用了
		}
		if (InLimitCount > 0 && InCount >= InLimitCount)
		{
			return;//回路连接插座个数超出限制个数则不再连接
		}

		FVector PointDir(PointPtr->InnerPos - PrePoint->InnerPos);
		PointDir.Z = 0;
		PointDir.Normalize();
		if (PointDir.Size() > 0 && (PointDir + CurrentPlane->RangeDirection).Size() > 1)//同向
		{
			if (PointPtr->PointType == -1)
			{
				TSharedPtr<FATPass> Pass = PointPassMap.FindRef(PointPtr);
				if (Pass.IsValid())
				{
					//如果存在 说明现在是需要出门
					bool InPass = false;
					for (auto & V : RoutePointArray)
					{
						if (V.Key == PointPtr)
						{
							InPass = true;
							break;
						}
					}
					if (InPass)
					{
						TArray<TSharedPtr<FATRoom>> LinkRooms;
						PassRoomMap.MultiFind(Pass, LinkRooms);
						for (auto R : LinkRooms)
						{
							if (R != CurrentRoom)
							{
								CurrentRoom = R;
								
								for (auto TempPlanePtr : CurrentRoom->PlaneArray)
								{
									if (TempPlanePtr->PassArray.Contains(Pass))
									{
										RoutePointArray.AddUnique(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(PointPtr, CurrentPlane));
										CurrentPlane = TempPlanePtr;
										PrePoint = PointPtr;
										ExistLinkPlane = true;
										break;
									}
								}
								break;
							}
						}
					}
					else
					{
						TSharedPtr<FATRoom> NextTargetRoom;
						int32 PassType = AutoDesignGetPassType(CurrentRoom, Pass, InRouteMap, NextTargetRoom);
						if (PassType == 1)//直连门
						{
							RoutePointArray.AddUnique(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(PointPtr, CurrentPlane));
							RoomRouteInfo& NextRoomRoute = InRouteMap[NextTargetRoom].Last();
							NextRoomRoute.RoutePlane->RangeDirection = CurrentPlane->RangeDirection * -1;//因为同门的两房间 按墙体的连续排序 下一个房间的墙面方向应该是当前空间当前面的反方向
																										 //next room 递归NextRoomRoute.RouteRoom 、NextRoomRoute.RoutePlane 、PointPtr
							CurrentRoom = NextRoomRoute.RouteRoom;
							CurrentPlane = NextRoomRoute.RoutePlane;
							PrePoint = PointPtr;
							ExistLinkPlane = true;
							break;
						}
						else if (PassType == 2)//路由门
						{
							RoutePointArray.AddUnique(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(PointPtr, CurrentPlane));

							//TSharedPtr<FATPass> PrePass = Pass;
							TSharedPtr<FATRoomPlane> RoutePrePlane = InStartPlane;
							bool RouteStart = false;
							for (auto& Route : InRouteMap[NextTargetRoom])
							{
								if (RouteStart)
								{
									//RoutePointArray.AddUnique(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(Route.RoutePass->DoorCenterPoint, Route.RoutePlane));
									TArray<TSharedPtr<FATRoomPlane>> LinkRoomPlanes;
									PassPlaneMap.MultiFind(Pass, LinkRoomPlanes);
									for (auto RPlane : LinkRoomPlanes)
									{
										if (RPlane != RoutePrePlane)
										{
											RPlane->RangeDirection = CurrentPlane->RangeDirection * -1;//因为同门的两房间 按墙体的连续排序 下一个房间的墙面方向应该是当前空间当前面的反方向
																														 //next room 递归NextRoomRoute.RouteRoom 、NextRoomRoute.RoutePlane 、PointPtr
											NextTargetRoom = Route.RouteRoom;
											RoutePrePlane = RPlane;
											ExistLinkPlane = true;
											break;
										}
									}
									if (ExistLinkPlane) break;
									//bool DoorDirReverse = false;
									//FVector DoorDir(PrePass->EndPos - PrePass->StartPos);
									//DoorDir.Z = 0;
									//DoorDir.Normalize();
									//if ((DoorDir - RoutePrePlane->RangeDirection).Size() > 1)//方向相反
									//{
									//	DoorDirReverse = true;
									//}
									//FVector InterPos;
									//if (FMath::SegmentIntersection2D(PrePass->StartPos, Route.RoutePass->StartPos, PrePass->EndPos, Route.RoutePass->EndPos, InterPos))
									//{
									//	//next end star
									//	FVector NextRoomStartPlaneDir(Route.RoutePass->EndPos - Route.RoutePass->StartPos);
									//	NextRoomStartPlaneDir.Z = 0;
									//	NextRoomStartPlaneDir.Normalize();
									//	if (DoorDirReverse)
									//	{
									//		NextRoomStartPlaneDir *= -1;
									//	}
									//	NextTargetRoom = Route.RouteRoom;
									//	RoutePrePlane = Route.RoutePlane;
									//	RoutePrePlane->RangeDirection = NextRoomStartPlaneDir;
									//	PrePass = Route.RoutePass;
									//	break;
									//}
									//else
									//{
									//	FVector NextRoomStartPlaneDir(Route.RoutePass->StartPos - Route.RoutePass->EndPos);
									//	NextRoomStartPlaneDir.Z = 0;
									//	NextRoomStartPlaneDir.Normalize();
									//	if (DoorDirReverse)
									//	{
									//		NextRoomStartPlaneDir *= -1;
									//	}
									//	NextTargetRoom = Route.RouteRoom;
									//	RoutePrePlane = Route.RoutePlane;
									//	RoutePrePlane->RangeDirection = NextRoomStartPlaneDir;
									//	PrePass = Route.RoutePass;
									//	break;
									//}
								}
								else if (Route.RouteRoom == CurrentRoom)
								{
									RouteStart = true;
									continue;
								}
							}
							//next room 递归NextTargetRoom 、RoutePrePlane 、RoutePointArray.Last()
							CurrentRoom = NextTargetRoom;
							CurrentPlane = RoutePrePlane;
							PrePoint = RoutePointArray.Last().Key;
							ExistLinkPlane = true;
							break;
						}
						else
						{
							continue;
						}
					}
				}
			}
			else
			{
				if (RoutePointArray.Contains(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(PointPtr, CurrentPlane)))
				{
					return;
				}
				else
				{
					PointPtr->Index = ++InCount;
					RoutePointArray.AddUnique(TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>(PointPtr, CurrentPlane));
				}
			}
		}
	}

	if (InStartPlane == CurrentPlane)
	{
		for (auto PlanePtr : InCurrentRoom->PlaneArray)
		{
			if (PlanePtr == CurrentPlane)
			{
				continue;
			}

			bool SS = PlanePtr->StartPos == CurrentPlane->StartPos;
			bool SE = PlanePtr->StartPos == CurrentPlane->EndPos;
			bool ES = PlanePtr->EndPos == CurrentPlane->StartPos;
			bool EE = PlanePtr->EndPos == CurrentPlane->EndPos;
			if (SS || SE || ES || EE)
			{
				//if (PlanePtr->Index == 0)
				//{
				//	continue;
				//}
				FVector PlaneDir(CurrentPlane->EndPos - CurrentPlane->StartPos);
				PlaneDir.Z = 0;
				PlaneDir.Normalize();

				if ((PlaneDir + CurrentPlane->RangeDirection).Size() > 1)//同向
				{
					if (SE)
					{
						FVector NewPlaneDir(PlanePtr->EndPos - PlanePtr->StartPos);
						NewPlaneDir.Z = 0;
						NewPlaneDir.Normalize();
						PlanePtr->RangeDirection = NewPlaneDir;
						PlanePtr->Index = CurrentPlane->Index + 1;
						CurrentPlane = PlanePtr;
						PrePoint = MakeShareable(new FATPipePointInfo(PlanePtr->StartPos, PlanePtr->Normal));
						ExistLinkPlane = true;
						break;
					}
					else if (EE)
					{
						FVector NewPlaneDir(PlanePtr->StartPos - PlanePtr->EndPos);
						NewPlaneDir.Z = 0;
						NewPlaneDir.Normalize();
						PlanePtr->RangeDirection = NewPlaneDir;
						PlanePtr->Index = CurrentPlane->Index + 1;
						CurrentPlane = PlanePtr;
						PrePoint = MakeShareable(new FATPipePointInfo(PlanePtr->EndPos, PlanePtr->Normal));
						ExistLinkPlane = true;
						break;
					}
				}
				else
				{
					if (SS)
					{
						FVector NewPlaneDir(PlanePtr->EndPos - PlanePtr->StartPos);
						NewPlaneDir.Z = 0;
						NewPlaneDir.Normalize();
						PlanePtr->RangeDirection = NewPlaneDir;
						PlanePtr->Index = CurrentPlane->Index + 1;
						CurrentPlane = PlanePtr;
						PrePoint = MakeShareable(new FATPipePointInfo(PlanePtr->StartPos, PlanePtr->Normal));
						ExistLinkPlane = true;
						break;
					}
					else if (ES)
					{
						FVector NewPlaneDir(PlanePtr->StartPos - PlanePtr->EndPos);
						NewPlaneDir.Z = 0;
						NewPlaneDir.Normalize();
						PlanePtr->RangeDirection = NewPlaneDir;
						PlanePtr->Index = CurrentPlane->Index + 1;
						CurrentPlane = PlanePtr;
						PrePoint = MakeShareable(new FATPipePointInfo(PlanePtr->EndPos, PlanePtr->Normal));
						ExistLinkPlane = true;
						break;
					}
				}
			}
		}
	}
	if (ExistLinkPlane)
	{
		OutSortPlaneArray.Add(CurrentPlane);
		AutoDesignRouteAllPoint(InRelateRoomArray,CurrentRoom, CurrentPlane, PrePoint, InRouteMap, RoutePointArray,OutSortPlaneArray, InCount, InPointType,InLimitCount);
	}
}


bool FArmyAutoDesignPipeLine::AutoDesignPreJudge(const TSharedPtr<FHydropowerPowerSytem> InLoopInfo)
{
	TArray<FObjectWeakPtr> InnerRoomArray,ComponentArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel,OT_InternalRoom,InnerRoomArray);

	FArmySceneData::Get()->GetObjects(E_HydropowerModel,OT_ComponentBase,ComponentArray);
	FArmySceneData::Get()->GetObjects(E_ConstructionModel,OT_ComponentBase,ComponentArray);
	FArmySceneData::Get()->GetObjects(E_LayoutModel,OT_ComponentBase,ComponentArray);

	EAutoDesignErrorType errorType = EAD_NONE;

	for (auto R:InnerRoomArray)
	{
		TSharedPtr<FArmyRoom>RoomPtr = StaticCastSharedPtr<FArmyRoom>(R.Pin());

		if (RoomPtr->GetSpaceName().Contains(TEXT(""))||RoomPtr->GetSpaceName().Contains(TEXT("未命名")))
		{
			errorType = EAutoDesignErrorType::EAD_ERROR_SPACENAME;
			break;
		}
	}
	bool bHasEleBoxH = false;
	bool bHasSocket = false;
	for (auto CObj:ComponentArray)
	{
		TSharedPtr<FArmyFurniture> CPtr = StaticCastSharedPtr<FArmyFurniture>(CObj.Pin());
		if (CPtr.IsValid())
		{
			if (CPtr->IsStrongElectirc()&&CPtr->IsSocket())
			{
				bHasSocket = true;
			}
			if (CPtr->IsStrongElectirc()&&CPtr->IsEleBox())
			{
				bHasEleBoxH = true;
			}
		}
	}
	if (!bHasEleBoxH&&errorType==EAutoDesignErrorType::EAD_NONE)
	{
		errorType = EAutoDesignErrorType::EAD_ERROR_ELE_STRONG;
	}

	if (!bHasSocket&&errorType==EAutoDesignErrorType::EAD_NONE)
	{
		errorType = EAutoDesignErrorType::EAD_ERROR_ELE_SOCKET_STRONG;
	}

	if (InLoopInfo->LoopArray.Num() == 0&&errorType==EAutoDesignErrorType::EAD_NONE)
	{
		errorType = EAutoDesignErrorType::EAD_ERROR_POWERSYSTEM;
	}



	if (errorType == EAutoDesignErrorType::EAD_NONE) 
	{

		return true;
	}
	FAutoDesignErrorInfo ErrorInfo = ErrorMap[errorType];

	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.Padding(0,86,20,0)
		
		[
		SNew(SArmyHydropowerAutoDesignError)
		.Title(FText::FromString(ErrorInfo.ErrorTitle))
		.TitleIcon(FName(*ErrorInfo.ErrorICon))
		.Content(FText::FromString(ErrorInfo.ErrorContent))
		.ContentIcon(FName(*ErrorInfo.ErrorImage))
		.Visibility((EVisibility::Visible))
		];
	return false;
}
void FArmyAutoDesignPipeLine::AutoDesignInterpolationToPlane(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InPreRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const FVector& InPreNormal, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InRoomPlane,const float InHeight)
{
	if (InPreRoomPlane == InRoomPlane)
	{
		OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, InPreRoomPlane->Normal));

		if (InHeight < 0.05)//接近地面直接到顶
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, FArmySceneData::WallHeight), InPreRoomPlane->Normal));
		}
		else
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, InHeight), InPreRoomPlane->Normal));
		}
	}
	else
	{
		AutoDesignRouteNextPlane(OutLInfo, RoutePlaneArray, InPreRoom, InPreRoomPlane, InPrePos, InRoomPlane);
	}
}
void FArmyAutoDesignPipeLine::AutoDesignInterpolation(FATPipeLineInfo& OutLInfo,const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InPreRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos,const FVector& InPreNormal, const TSharedPtr<FATRoom> InRoom,const TSharedPtr<FATRoomPlane> InRoomPlane,const TSharedPtr<FATPipePointInfo> InPointInfo)
{
	bool CeilingRoom = InPreRoom->RoomType == 1 || InPreRoom->RoomType == 2 || InPreRoom->RoomType == 3;
	float BaseHeight = CeilingRoom ? FArmySceneData::WallHeight : 0;
	int32 LIndex = CeilingRoom ? 3 : 1;
	int32 RIndex = CeilingRoom ? 4 : 2;

	FVector PreNormal = InPreNormal == FVector(0, 0, 0) ? InPreRoomPlane->Normal : InPreNormal;
	if (InPointInfo->PointType != -1)//插座
	{
		if (InPreRoomPlane == InRoomPlane)
		{
			if ((FVector2D(InPrePos) - FVector2D(InPointInfo->InnerPos)).Size() < 30)//cm
			{
				if (FMath::Abs(InPointInfo->InnerPos.Z - InPrePos.Z) < 5)
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
					OutLInfo.PipePointArray.AddUnique(*InPointInfo);
				}
				else
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, InPointInfo->InnerPos.Z), InPointInfo->Normal));
					OutLInfo.PipePointArray.AddUnique(*InPointInfo);
				}
			}
			else
			{
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
				if (InPrePos.Z != BaseHeight)
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, BaseHeight), PreNormal));
					if (!CeilingRoom)
					{
						OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, BaseHeight) + PreNormal * 5, PreNormal));
					}
				}

				FVector ObjLinkPoint = InPointInfo->InnerPos;

				TPair<FVector, bool>* BLP = InPointInfo->LinkPointMap.Find(LIndex);
				TPair<FVector, bool>* BRP = InPointInfo->LinkPointMap.Find(RIndex);
				if (BLP && BRP)
				{
					if (!BLP->Value && !BRP->Value)
					{
						if ((BLP->Key - InPrePos).Size() < (BRP->Key - InPrePos).Size())
						{
							ObjLinkPoint = BLP->Key;
							BLP->Value = true;
						}
						else
						{
							ObjLinkPoint = BRP->Key;
							BRP->Value = true;
						}
					}
					else if (BLP->Value && !BRP->Value)
					{
						ObjLinkPoint = BRP->Key;
						BRP->Value = true;
					}
					else if (!BLP->Value && BRP->Value)
					{
						ObjLinkPoint = BLP->Key;
						BLP->Value = true;
					}
				}

				if (!CeilingRoom)
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(ObjLinkPoint.X, ObjLinkPoint.Y, BaseHeight) + InPointInfo->Normal * 5, InPointInfo->Normal));
				}
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(ObjLinkPoint.X, ObjLinkPoint.Y, BaseHeight), InPointInfo->Normal));
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InPointInfo->Normal));
			}
		}
		else
		{
			FVector ObjLinkPoint = InPointInfo->InnerPos;

			TPair<FVector, bool>* BLP = InPointInfo->LinkPointMap.Find(LIndex);
			TPair<FVector, bool>* BRP = InPointInfo->LinkPointMap.Find(RIndex);
			if (BLP && BRP)
			{
				if (!BLP->Value && !BRP->Value)
				{
					if (((BRP->Key - BLP->Key).GetSafeNormal() + InRoomPlane->RangeDirection).Size() > 1)
					{
						ObjLinkPoint = BLP->Key;
						BLP->Value = true;
					}
					else
					{
						ObjLinkPoint = BRP->Key;
						BRP->Value = true;
					}
				}
				else if (BLP->Value && !BRP->Value)
				{
					ObjLinkPoint = BRP->Key;
					BRP->Value = true;
				}
				else if (!BLP->Value && BRP->Value)
				{
					ObjLinkPoint = BLP->Key;
					BLP->Value = true;
				}
			}

			
			if (!CeilingRoom)
			{
				FVector CurrentProjection(ObjLinkPoint.X, ObjLinkPoint.Y, BaseHeight);
				FVector CurrentProjectionTemp = CurrentProjection + InRoomPlane->Normal * 5;
				AutoDesignRouteNextPoint(OutLInfo, RoutePlaneArray, InPreRoom, InPreRoomPlane, InPrePos, InRoomPlane, CurrentProjectionTemp);
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(CurrentProjection, InRoomPlane->Normal));
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
			}
			else
			{
				FVector CurrentProjection(ObjLinkPoint.X, ObjLinkPoint.Y, BaseHeight);
				AutoDesignRouteNextPoint(OutLInfo, RoutePlaneArray, InPreRoom, InPreRoomPlane, InPrePos, InRoomPlane, CurrentProjection);
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
			}
		}
	}
	else
	{
		FVector CurrentPos = InPointInfo->InnerPos;
		TSharedPtr<FATPass> Pass = PointPassMap.FindRef(InPointInfo);
		if (Pass.IsValid())
		{
			CurrentPos = Pass->GetLinkPoint(InRoomPlane->RangeDirection);
		}
		if (InPreRoomPlane == InRoomPlane)
		{
			FVector NearPos = FMath::ClosestPointOnInfiniteLine(InRoomPlane->StartPos, InRoomPlane->EndPos, CurrentPos);
			if ((FVector2D(InPrePos) - FVector2D(NearPos)).Size() < 30)//cm
			{
				if (FMath::Abs(CurrentPos.Z - InPrePos.Z) < 5)
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NearPos, InRoomPlane->Normal));
				}
				else
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, CurrentPos.Z), InRoomPlane->Normal));
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NearPos, InRoomPlane->Normal));
				}
			}
			else
			{
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, PreNormal));
				if (InPrePos.Z != 0)OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, BaseHeight), PreNormal));
				if (!CeilingRoom)
				{
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(InPrePos.X, InPrePos.Y, BaseHeight) + PreNormal * 5, PreNormal));
					OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NearPos + InRoomPlane->Normal * 5, InRoomPlane->Normal));
				}
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NearPos, InRoomPlane->Normal));
			}

			FVector NextStart = CurrentPos + CurrentPos - NearPos;
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NextStart - InRoomPlane->Normal * 5, -InRoomPlane->Normal));
		}
		else
		{
			FVector NearPos = FMath::ClosestPointOnInfiniteLine(InRoomPlane->StartPos, InRoomPlane->EndPos, CurrentPos);
			NearPos.Z = BaseHeight;

			AutoDesignRouteNextPoint(OutLInfo, RoutePlaneArray, InPreRoom, InPreRoomPlane, InPrePos, InRoomPlane, NearPos);
			FVector NextStart = CurrentPos + CurrentPos - NearPos;
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NextStart - InRoomPlane->Normal * 5, -InRoomPlane->Normal));
		}
	}
}
bool FArmyAutoDesignPipeLine::AutoDesignLinkToPlane(const TSharedPtr<FATRoom> InRoom, const FVector& InPrePoint, const FVector& InPreNormal, const TSharedPtr<FATRoomPlane> InTargetPlane, FATPipeLineInfo& OutLInfo)
{
	FVector InterPos = FMath::ClosestPointOnInfiniteLine(InTargetPlane->StartPos,InTargetPlane->EndPos,InPrePoint);
	float PlaneLen = (InTargetPlane->StartPos - InTargetPlane->EndPos).Size();

	FVector Dir = (InterPos - InPrePoint).GetSafeNormal();
	bool ExistPoint = true;
	for (int32 i = 0; i < InRoom->PlaneArray.Num(); ++i)
	{
		FVector TempInterPos;
		const TSharedPtr<FATRoomPlane> TempPlane = InRoom->PlaneArray[i];
		if (FMath::SegmentIntersection2D(InPrePoint + Dir + InPreNormal, InterPos + InPreNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
		{
			ExistPoint = false;
			break;
		}
		if (FMath::SegmentIntersection2D((InTargetPlane->StartPos + InTargetPlane->EndPos) / 2 + InTargetPlane->Normal, InterPos + InTargetPlane->Normal + InPreNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
		{
			ExistPoint = false;
			break;
		}
	}
	if (ExistPoint)
	{
		OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePoint, InPreNormal));
		float StartDis = (InterPos - InTargetPlane->StartPos).Size();
		float EndDis = (InterPos - InTargetPlane->EndPos).Size();
		if (StartDis <= PlaneLen && EndDis <= PlaneLen)
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InterPos, InTargetPlane->Normal));
		}
		else
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InterPos, InTargetPlane->Normal));
			if (StartDis < EndDis)
			{
				FVector LastPos = InTargetPlane->StartPos + (InTargetPlane->EndPos - InTargetPlane->StartPos).GetSafeNormal() * 2.5;
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(LastPos, InTargetPlane->Normal));
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(LastPos.X, LastPos.Y,FArmySceneData::WallHeight), InTargetPlane->Normal));
			}
			else
			{
				FVector LastPos = InTargetPlane->EndPos + (InTargetPlane->StartPos - InTargetPlane->EndPos).GetSafeNormal() * 2.5;
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(LastPos, InTargetPlane->Normal));
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(FVector(LastPos.X, LastPos.Y, FArmySceneData::WallHeight), InTargetPlane->Normal));
			}
		}
		return true;
	}
	return false;
}
bool FArmyAutoDesignPipeLine::AutoDesignLinkTwoPoint(const TSharedPtr<FATRoom> InRoom,const FVector& InPrePoint,const FVector& InPreNormal, const FVector& InNextPoint, const FVector& InNextNormal, FATPipeLineInfo& OutLInfo)
{
	bool CeilingRoom = InRoom->RoomType == 1 || InRoom->RoomType == 2 || InRoom->RoomType == 3;
	float BaseHeight = CeilingRoom ? FArmySceneData::WallHeight : 0;

	FVector InterProjectionPos;
	if (FArmyMath::CalculateLinesIntersection(InPrePoint, InPrePoint + InPreNormal * 100000, InNextPoint, InNextPoint + InNextNormal * 100000, InterProjectionPos))
	{
		bool ExistPoint = true;
		for (int32 i = 0; i < InRoom->PlaneArray.Num(); ++i)
		{
			FVector TempInterPos;
			const TSharedPtr<FATRoomPlane> TempPlane = InRoom->PlaneArray[i];
			if (FMath::SegmentIntersection2D(InPrePoint + InPreNormal, InterProjectionPos, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
			{
				ExistPoint = false;
				break;
			}
			else if (FMath::SegmentIntersection2D(InNextPoint + InNextNormal, InterProjectionPos, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
			{
				ExistPoint = false;
				break;
			}
		}
		if (ExistPoint)
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePoint, InPreNormal));
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InterProjectionPos, InPreNormal));
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InNextPoint, InNextNormal));
			return true;
		}
	}
	else
	{
		bool ExistPoint = true, ExistPoint2 = true, ExistPoint3 = true;

		InterProjectionPos = FMath::ClosestPointOnInfiniteLine(InNextPoint, InNextPoint + InNextNormal * 100000, InPrePoint);
		for (int32 i = 0; i < InRoom->PlaneArray.Num(); ++i)
		{
			FVector TempInterPos;
			const TSharedPtr<FATRoomPlane> TempPlane = InRoom->PlaneArray[i];
			if (FMath::SegmentIntersection2D(InterProjectionPos + InPreNormal, InPrePoint + InPreNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
			{
				ExistPoint = false;
				break;
			}
			else if (FMath::SegmentIntersection2D(InterProjectionPos + InPreNormal, InNextPoint + InNextNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
			{
				ExistPoint = false;
				break;
			}
		}
		if (!ExistPoint)
		{
			InterProjectionPos = FMath::ClosestPointOnInfiniteLine(InPrePoint, InPrePoint + InPreNormal * 100000, InNextPoint);
			for (int32 i = 0; i < InRoom->PlaneArray.Num(); ++i)
			{
				FVector TempInterPos;
				const TSharedPtr<FATRoomPlane> TempPlane = InRoom->PlaneArray[i];
				if (FMath::SegmentIntersection2D(InterProjectionPos + InNextNormal, InNextPoint + InNextNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
				{
					ExistPoint2 = false;
					break;
				}
				else if(FMath::SegmentIntersection2D(InterProjectionPos + InNextNormal, InPrePoint + InPreNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
				{
					ExistPoint2 = false;
					break;
				}
			}

			if (!ExistPoint2)
			{
				FVector2D PreDir(-InPreNormal.Y, InPreNormal.X), NextDir2D(-InNextNormal.Y, InNextNormal.X), InterPos2D;
				ExistPoint3 = FArmyMath::Line2DIntersection(FVector2D(InPrePoint), FVector2D(InPrePoint) + PreDir, FVector2D(InNextPoint), FVector2D(InNextPoint) + NextDir2D, InterPos2D);
				if (ExistPoint3)
				{
					InterProjectionPos = FVector(InterPos2D, 0);
					for (int32 i = 0; i < InRoom->PlaneArray.Num(); ++i)
					{
						FVector TempInterPos;
						const TSharedPtr<FATRoomPlane> TempPlane = InRoom->PlaneArray[i];
						if (FMath::SegmentIntersection2D(InterProjectionPos + InPreNormal + InNextNormal, InPrePoint + InPreNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
						{
							ExistPoint3 = false;
							break;
						}
						else if (FMath::SegmentIntersection2D(InterProjectionPos + InNextNormal + InPreNormal, InNextPoint + InNextNormal, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
						{
							ExistPoint3 = false;
							break;
						}
					}
				}
			}
		}

		if (ExistPoint || ExistPoint2 || ExistPoint3)
		{
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePoint, InPreNormal));
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InterProjectionPos, InPreNormal));
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InNextPoint, InNextNormal));
			return true;
		}
	}
	return false;
}
void FArmyAutoDesignPipeLine::AutoDesignRouteNextPlane(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const TSharedPtr<FATRoomPlane> InRoomPlane)
{
	bool CeilingRoom = InRoom->RoomType == 1 || InRoom->RoomType == 2 || InRoom->RoomType == 3;
	float BaseHeight = CeilingRoom ? FArmySceneData::WallHeight : 0;

	FVector InPreNormal = InPreRoomPlane->Normal;

	OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, InPreNormal));

	FVector CurrentPoint(InPrePos.X, InPrePos.Y, BaseHeight);
	if (InPrePos.Z != BaseHeight)OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(CurrentPoint, InPreNormal));

	int32 StartIndex = RoutePlaneArray.FindLast(InPreRoomPlane);
	int32 EndIndex = RoutePlaneArray.FindLast(InRoomPlane);

	TSharedPtr<FATRoom> CurrentRoom = InRoom;

	if (AutoDesignLinkToPlane(CurrentRoom, CurrentPoint, InPreNormal, InRoomPlane,OutLInfo))
	{
		return;
	}

	///
	for (int32 i = StartIndex; i < EndIndex - 1; i++)
	{
		TSharedPtr<FATRoomPlane> CurrentPlane = RoutePlaneArray[i];
		TSharedPtr<FATRoomPlane> NextPlane = RoutePlaneArray[i + 1];

		FVector CurrentNormal = CurrentPlane->Normal;
		FVector CurrentDirection = CurrentPlane->RangeDirection;

		FVector NextPoint = NextPlane->StartPos;
		if (NextPoint == CurrentPlane->StartPos || NextPoint == CurrentPlane->EndPos)
		{
			NextPoint = NextPlane->EndPos;
		}
		FVector NextNormal = NextPlane->Normal;
		FVector NextDirection = NextPlane->RangeDirection;

		FVector NextClosestPoint = FMath::ClosestPointOnInfiniteLine(CurrentPoint, CurrentPoint + CurrentNormal, NextPoint);

		FVector Offset = NextClosestPoint - CurrentPoint;

		FVector TempNormal = Offset.GetSafeNormal();

		if ((CurrentNormal + TempNormal).Size() > 1)//同向
		{
			float NearestDis = Offset.Size();
			TSharedPtr<FATRoomPlane> InterPlane;
			FVector ClosestInterPos = NextClosestPoint;
			for (int32 i = 0; i < CurrentRoom->PlaneArray.Num(); ++i)
			{
				const TSharedPtr<FATRoomPlane> TempPlane = CurrentRoom->PlaneArray[i];
				FVector TempInterPos;
				if (FMath::SegmentIntersection2D(CurrentPoint + CurrentNormal, CurrentPoint + Offset, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
				{
					float TempDis = (TempInterPos - CurrentPoint).Size();
					if (TempDis < NearestDis)
					{
						NearestDis = TempDis;
						InterPlane = TempPlane;
						ClosestInterPos = TempInterPos;
					}
				}
			}

			if (InterPlane.IsValid())
			{
				int32 RouteIndex = RoutePlaneArray.Find(InterPlane);
				if (RouteIndex != INDEX_NONE && RouteIndex > StartIndex && RouteIndex <= EndIndex)
				{
					//可合并
					//NextPoint = ClosestInterPos;
					//NextPlane = InterPlane;
					//NextNormal = InterPlane->Normal;
					//NextDirection = InterPlane->RangeDirection;

					if (AutoDesignLinkToPlane(CurrentRoom, ClosestInterPos, InterPlane->Normal, InRoomPlane, OutLInfo))
					{
						return;
						//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
					}
					else
					{
						TSharedPtr<FATPipePointInfo> NewPoint = MakeShareable(new FATPipePointInfo(ClosestInterPos, InterPlane->Normal));
						OutLInfo.PipePointArray.AddUnique(*NewPoint);
						int32 InterIndex = RoutePlaneArray.Find(InterPlane);
						i = InterIndex - 1;
						if (InterIndex >= EndIndex)
						{
							return;
							//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
						}
					}
					CurrentPoint = ClosestInterPos;
				}
			}
			else
			{
				//可合并
				if (RoutePlaneArray.Num() > (i + 2))
				{
					//NextPoint = ClosestInterPos;
					TSharedPtr<FATRoomPlane> TempPlane = RoutePlaneArray[i + 2];
					//NextNormal = TempPlane->Normal;
					//NextDirection = TempPlane->RangeDirection;

					if (AutoDesignLinkToPlane(CurrentRoom, ClosestInterPos, TempPlane->Normal, InRoomPlane, OutLInfo))
					{
						return;
						//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
					}
					else
					{
						TSharedPtr<FATPipePointInfo> NewPoint = MakeShareable(new FATPipePointInfo(ClosestInterPos, TempPlane->Normal));
						OutLInfo.PipePointArray.AddUnique(*NewPoint);
						int32 InterIndex = RoutePlaneArray.Find(TempPlane);
						i = InterIndex - 1;
						if (InterIndex >= EndIndex)
						{
							return;
							//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
						}
					}
					CurrentPoint = ClosestInterPos;
				}
				else
				{
					if (AutoDesignLinkToPlane(CurrentRoom, ClosestInterPos, CurrentDirection, InRoomPlane, OutLInfo))
					{
						return;
					}
					CurrentPoint = ClosestInterPos;
				}
			}
		}
		else
		{
			//judge inter target
			//else
			if (NextPoint == NextPlane->StartPos)
			{
				NextPoint = NextPlane->EndPos;
			}
			else
			{
				NextPoint = NextPlane->StartPos;
			}

			if (AutoDesignLinkToPlane(CurrentRoom, NextPoint, NextPlane->Normal, InRoomPlane, OutLInfo))
			{
				//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
				return;
			}
			else
			{
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NextPoint, NextPlane->Normal));
				CurrentPoint = NextPoint;
			}
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignRouteNextPoint(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const TSharedPtr<FATRoomPlane> InRoomPlane, const FVector& InTargetPoint)
{
	bool CeilingRoom = InRoom->RoomType == 1 || InRoom->RoomType == 2 || InRoom->RoomType == 3;
	float BaseHeight = CeilingRoom ? FArmySceneData::WallHeight : 0;


	FVector InPreNormal = InPreRoomPlane->Normal;

	OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(InPrePos, InPreNormal));

	FVector CurrentPoint(InPrePos.X, InPrePos.Y, BaseHeight);
	if (InPrePos.Z != BaseHeight)
	{
		OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(CurrentPoint, InPreNormal));
		if (!CeilingRoom)
		{
			CurrentPoint += InPreNormal * 5;
			OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(CurrentPoint, InPreNormal));
		}
	}

	int32 StartIndex = RoutePlaneArray.FindLast(InPreRoomPlane);
	int32 EndIndex = RoutePlaneArray.FindLast(InRoomPlane);

	TSharedPtr<FATRoom> CurrentRoom = InRoom;

	if (AutoDesignLinkTwoPoint(CurrentRoom, CurrentPoint, InPreNormal, InTargetPoint, InRoomPlane->Normal, OutLInfo))
	{
		return;
	}

	///
	for (int32 i = StartIndex; i < EndIndex - 1; i++)
	{
		TSharedPtr<FATRoomPlane> CurrentPlane = RoutePlaneArray[i];
		TSharedPtr<FATRoomPlane> NextPlane = RoutePlaneArray[i + 1];

		FVector CurrentNormal = CurrentPlane->Normal;
		FVector CurrentDirection = CurrentPlane->RangeDirection;

		FVector NextPlaneStart(NextPlane->StartPos.X, NextPlane->StartPos.Y,BaseHeight);
		FVector NextPlaneEnd(NextPlane->EndPos.X, NextPlane->EndPos.Y, BaseHeight);

		FVector NextPoint = NextPlaneStart;
		if (NextPoint == CurrentPlane->StartPos || NextPoint == CurrentPlane->EndPos)
		{
			NextPoint = NextPlaneEnd;
		}
		FVector NextNormal = NextPlane->Normal;
		FVector NextDirection = NextPlane->RangeDirection;

		FVector NextClosestPoint = FMath::ClosestPointOnInfiniteLine(CurrentPoint, CurrentPoint + CurrentNormal, NextPoint);

		FVector Offset = NextClosestPoint - CurrentPoint;

		FVector TempNormal = Offset.GetSafeNormal();

		if ((CurrentNormal + TempNormal).Size() > 1)//同向
		{
			float NearestDis = Offset.Size();
			TSharedPtr<FATRoomPlane> InterPlane;
			FVector ClosestInterPos = NextClosestPoint;
			for (int32 j = 0; j < CurrentRoom->PlaneArray.Num(); ++j)
			{
				const TSharedPtr<FATRoomPlane> TempPlane = CurrentRoom->PlaneArray[j];
				FVector TempInterPos;
				if (FMath::SegmentIntersection2D(CurrentPoint + CurrentNormal, CurrentPoint + Offset, TempPlane->StartPos, TempPlane->EndPos, TempInterPos))
				{
					TempInterPos.Z = BaseHeight;
					float TempDis = (TempInterPos - CurrentPoint).Size();
					if (TempDis < NearestDis)
					{
						NearestDis = TempDis;
						InterPlane = TempPlane;
						ClosestInterPos = TempInterPos;
					}
				}
			}

			if (InterPlane.IsValid())
			{
				int32 RouteIndex = RoutePlaneArray.Find(InterPlane);
				if (RouteIndex != INDEX_NONE && RouteIndex > StartIndex && RouteIndex <= EndIndex)
				{
					//可合并
					//NextPoint = ClosestInterPos;
					//NextPlane = InterPlane;
					//NextNormal = InterPlane->Normal;
					//NextDirection = InterPlane->RangeDirection;

					if (AutoDesignLinkTwoPoint(CurrentRoom, ClosestInterPos, InterPlane->Normal, InTargetPoint, InRoomPlane->Normal, OutLInfo))
					{
						return;
						//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
					}
					else
					{
						TSharedPtr<FATPipePointInfo> NewPoint = MakeShareable(new FATPipePointInfo(ClosestInterPos, InterPlane->Normal));
						OutLInfo.PipePointArray.AddUnique(*NewPoint);
						int32 InterIndex = RoutePlaneArray.Find(InterPlane);
						i = InterIndex - 1;
						if (InterIndex >= EndIndex)
						{
							return;
							//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
						}
					}
					CurrentPoint = ClosestInterPos;
				}
			}
			else
			{
				//可合并
				if (RoutePlaneArray.Num() > (i + 2))
				{
					//NextPoint = ClosestInterPos;
					TSharedPtr<FATRoomPlane> TempPlane = RoutePlaneArray[i + 2];
					//NextNormal = TempPlane->Normal;
					//NextDirection = TempPlane->RangeDirection;

					if (AutoDesignLinkTwoPoint(CurrentRoom, ClosestInterPos, TempPlane->Normal, InTargetPoint, InRoomPlane->Normal, OutLInfo))
					{
						return;
						//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
					}
					else
					{
						TSharedPtr<FATPipePointInfo> NewPoint = MakeShareable(new FATPipePointInfo(ClosestInterPos, TempPlane->Normal));
						OutLInfo.PipePointArray.AddUnique(*NewPoint);
						int32 InterIndex = RoutePlaneArray.Find(TempPlane);
						i = InterIndex - 1;
						if (InterIndex >= EndIndex)
						{
							return;
							//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
						}
					}
					CurrentPoint = ClosestInterPos;
				}
				else
				{
					if (AutoDesignLinkTwoPoint(CurrentRoom, ClosestInterPos, CurrentDirection, InTargetPoint, InRoomPlane->Normal, OutLInfo))
					{
						return;
					}
					CurrentPoint = ClosestInterPos;
				}
			}
		}
		else
		{
			//judge inter target
			//else
			if (NextPoint == NextPlaneStart)
			{
				NextPoint = NextPlaneEnd;
			}
			else
			{
				NextPoint = NextPlaneStart;
			}

			if (AutoDesignLinkTwoPoint(CurrentRoom, NextPoint, NextPlane->Normal, InTargetPoint, InRoomPlane->Normal, OutLInfo))
			{
				//OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(ObjLinkPoint, InRoomPlane->Normal));
				return;
			}
			else
			{
				OutLInfo.PipePointArray.AddUnique(FATPipePointInfo(NextPoint, NextPlane->Normal));
				CurrentPoint = NextPoint;
			}
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignWaterPipeLine()
{

}
void FArmyAutoDesignPipeLine::AutoDesignSocketPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo,int32 InLimitCount)
{
	//获取对应房间所有的普通插座
	//获取配电箱位置
	//插座与配电箱的连线得到回路的指导向量
	//算出所有插座的最短路径（将所有插座按路径距离排序）

	//填充本地回路信息
	int32 PointType = 1;
	if (InLoopInfo.IsValid())
	{
		TSharedPtr<FAutoDesignResultInfo> RInfo = MakeShareable(new FAutoDesignResultInfo(true, InLoopInfo->LoopName, TEXT("")));
		ResultList.Add(RInfo);

		if (!OriginalPoint.IsValid())
		{
			RInfo->bSuccessed = false;
			RInfo->ResultInfo = TEXT("布置失败！找不到配电箱");
			return;
		}
		//////////////////////////////////////////////////////////////////////
		int32 LocalLoopIndex = INDEX_NONE;
		switch (InLoopInfo->LoopType)
		{
		case FArmyHydropowerDataManager::PSP_WX://普通插座
		{
			PointType = 1;
			LocalLoopIndex = PipeLoopArray.AddUnique(FATPipeLoopInfo(InLoopInfo->LoopID, InLoopInfo->LoopType, InLoopInfo->PipeID));
		}
			break;
		case FArmyHydropowerDataManager::PSP_WK://空调插座
		{
			PointType = 2;
			LocalLoopIndex = PipeLoopArray.AddUnique(FATPipeLoopInfo(InLoopInfo->LoopID, InLoopInfo->LoopType, InLoopInfo->PipeID));
		}
		break;
		case FArmyHydropowerDataManager::PSP_WB://冰箱插座
		{
			PointType = 3;
			LocalLoopIndex = PipeLoopArray.AddUnique(FATPipeLoopInfo(InLoopInfo->LoopID, InLoopInfo->LoopType, InLoopInfo->PipeID));
		}
		break;
		case FArmyHydropowerDataManager::PSP_WP://动力插座
		{
			PointType = 4;
			LocalLoopIndex = PipeLoopArray.AddUnique(FATPipeLoopInfo(InLoopInfo->LoopID, InLoopInfo->LoopType, InLoopInfo->PipeID));
		}
		break;
		default:
			break;
		}

		if (LocalLoopIndex != INDEX_NONE)
		{
			FATPipeLoopInfo& LoopInfo = PipeLoopArray[LocalLoopIndex];
			for (auto& RoomV : InLoopInfo->RoomArray)
			{
				for (auto RPtr : AllRoomArray)
				{
					if (RPtr->RoomID == RoomV->ObjectID)
					{
						LoopInfo.RelevanceRoomArray.AddUnique(RPtr);
						break;
					}
				}
			}
			if (LoopInfo.RelevanceRoomArray.Num() == 0)
			{
				RInfo->bSuccessed = false;
				RInfo->ResultInfo = TEXT("布置失败！找不到匹配空间");
				return;
			}
			//对当前回路所占用的空间做排序
			TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>> RouteMap;
			AutoDesignRange(LoopInfo.RelevanceRoomArray, RouteMap);//step 4  weight:1 * plane number
			SyncProgress += 100 * Step4Weight / CountWeight;

			///////////////////////
			int32 TypePointCount = 0;
			TArray<TSharedPtr<FATRoomPlane>> AllPlaneSortArray;
			AllPlaneSortArray.AddUnique(OriginalPlane);
			TArray<TPair<TSharedPtr<FATPipePointInfo>,TSharedPtr<FATRoomPlane>>> AllPipePointArray;
			AutoDesignRouteAllPoint(LoopInfo.RelevanceRoomArray,OriginalRoom, OriginalPlane,OriginalPoint, RouteMap, AllPipePointArray, AllPlaneSortArray, TypePointCount,PointType,InLimitCount,true);//step 5 weight:2 * point number
			AllPlaneSortArray.Add(OriginalPlane);
			SyncProgress += 100 * Step5Weight / CountWeight;
			if (AllPipePointArray.Num() == 0)
			{
				RInfo->bSuccessed = false;
				RInfo->ResultInfo = TEXT("布置失败！找不到匹配插座");
				return;
			}

			FVector PreNormal = OriginalPlane->Normal;
			FVector PrePoint = OriginalPoint->InnerPos;

			///////////////////////////////查找配电箱未被占用的连接点（按连接方向去查找）///////////////////////////////// start
			if ((OriginalIndexRangeDir + OriginalPlane->RangeDirection).Size() > 1)
			{
				for (int32 i = 9; i > 0; --i)
				{
					if (OriginalPointMap[i].Value)//被占用
					{
						continue;
					}
					PrePoint = OriginalPointMap[i].Key;
					OriginalPointMap[i].Value = true;
					break;
				}
			}
			else
			{
				for (int32 i = 1; i < 10; ++i)
				{
					if (OriginalPointMap[i].Value)//被占用
					{
						continue;
					}
					PrePoint = OriginalPointMap[i].Key;
					OriginalPointMap[i].Value = true;
					break;
				}
			}
			///////////////////////////////////////////////////////////////// end

			TSharedPtr<FATRoomPlane> PreRoomPlane = OriginalPlane;
			TSharedPtr<FATRoom> PreRoom = OriginalRoom;

			///////////////////////////////剔除末尾是门洞的冗余点////////////////////////////////// start

			while (AllPipePointArray.Num() > 0 && PointPassMap.Contains(AllPipePointArray.Last().Key))
			{
				AllPipePointArray.Pop();
			}

			if (AllPipePointArray.Num() == 0)
			{
				RInfo->bSuccessed = false;
				RInfo->ResultInfo = TEXT("布置失败！找不到匹配插座");
				return;
			}
			///////////////////////////////////////////////////////////////// end
			struct FBorePointInfo //穿墙管的点位信息
			{
				FBorePointInfo(int32 InStartIndex,int32 InEndIndex,const TSharedPtr<FATPipePointInfo> InStartP,const TSharedPtr<FATPipePointInfo> InEndP):
					StartPipePointIndex(InStartIndex),EndPipePointIndex(InEndIndex),StartPointPtr(InStartP),EndPointPtr(InEndP)
				{
				}
				int32 StartPipePointIndex = -1;//在穿墙管起始点AllPipePointArray中的索引值
				int32 EndPipePointIndex = -1;//在穿墙管终止点AllPipePointArray中的索引值

				TSharedPtr<FATPipePointInfo> StartPointPtr;
				TSharedPtr<FATPipePointInfo> EndPointPtr;
			};
			TArray<FBorePointInfo> BorePointArray;
			int32 PreIndex = -1;
			for (int32 Index = 0;Index < AllPipePointArray.Num();++Index)
			{
				TSharedPtr<FATPipePointInfo> PointPtr = AllPipePointArray[Index].Key;
				
				if (PointPtr->PointType == PointType)//普通插座
				{
					if (PreIndex != -1)
					{
						TSharedPtr<FATPipePointInfo> PrePointPtr = AllPipePointArray[PreIndex].Key;
						if ((PrePointPtr->Normal + PointPtr->Normal).Size() < 0.001 && (PrePointPtr->InnerPos - PointPtr->InnerPos).Size() < 60)//墙厚加一个容差值 先定义成60cm
						{
							BorePointArray.Add(FBorePointInfo(PreIndex,Index,PrePointPtr,PointPtr));
						}
					}
					PreIndex = Index;
				}
			}


			for (int32 Index = 0; Index < AllPipePointArray.Num(); ++Index)
			{
				auto & PointObj = AllPipePointArray[Index];

				TSharedPtr<FATPipePointInfo> PointPtr = PointObj.Key;
				int32 PIndex = LoopInfo.PipeLineArray.AddUnique(FATPipeLineInfo());
				FATPipeLineInfo& LInfo = LoopInfo.PipeLineArray[PIndex];
				if (PointPtr->PointType == PointType)//普通插座
				{
					bool CeilingRoom = PreRoom->RoomType == 1 || PreRoom->RoomType == 2 || PreRoom->RoomType == 3;
					int32 LIndex = CeilingRoom ? 3 : 1;
					int32 RIndex = CeilingRoom ? 4 : 2;

					AutoDesignInterpolation(LInfo, AllPlaneSortArray, PreRoom, PreRoomPlane, PrePoint, PreNormal, NULL, PointObj.Value, PointPtr);//step 6 weight:2 * point number
					PreRoomPlane = PointObj.Value;

					//////////////////////////判断下当前点与下一个点是否需要穿墙连接//////////////////////////////////
					for (auto & BorePoint : BorePointArray)
					{
						if (BorePoint.StartPointPtr == PointPtr)
						{
							int32 BoreIndex = LoopInfo.PipeLineArray.AddUnique(FATPipeLineInfo());
							FATPipeLineInfo& BoreLInfo = LoopInfo.PipeLineArray[BoreIndex];

							BoreLInfo.PipePointArray.AddUnique(*BorePoint.StartPointPtr);
							BoreLInfo.PipePointArray.AddUnique(*BorePoint.EndPointPtr);

							Index = BorePoint.EndPipePointIndex;
							PointPtr = BorePoint.EndPointPtr;
							PreRoomPlane = AllPipePointArray[Index].Value;
							break;
						}
					}
					//////////////////////////////////////////////////////////////
					PrePoint = PointPtr->InnerPos + PreRoomPlane->RangeDirection * 2.5;
					PreNormal = PreRoomPlane->Normal;

					TPair<FVector, bool>* BLP = PointPtr->LinkPointMap.Find(LIndex);
					TPair<FVector, bool>* BRP = PointPtr->LinkPointMap.Find(RIndex);
					if (BLP && BRP)
					{
						if (BLP->Value)
						{
							PrePoint = BRP->Key;
						}
						else
						{
							PrePoint = BLP->Key;
						}
					}
				}
				else if (PointPassMap.Contains(PointPtr))//门洞
				{
					TSharedPtr<FATRoom> NextRoom = PreRoom;
					TSharedPtr<FATRoomPlane> NextPlane = PreRoomPlane;

					TSharedPtr<FATPass> Pass = PointPassMap.FindRef(PointPtr);
					TArray<TSharedPtr<FATRoom>> LinkRoomArray;
					PassRoomMap.MultiFind(Pass, LinkRoomArray);
					for (auto R : LinkRoomArray)
					{
						if (R != PreRoom)
						{
							NextRoom = R;
							break;
						}
					}
					for (auto Plane : NextRoom->PlaneArray)
					{
						if (Plane->PassArray.Contains(Pass))
						{
							NextPlane = Plane;
							break;
						}
					}

					bool CeilingRoom = PreRoom->RoomType == 1 || PreRoom->RoomType == 2 || PreRoom->RoomType == 3 ||
									   NextRoom->RoomType == 1 || NextRoom->RoomType == 2 || NextRoom->RoomType == 3;
					if (CeilingRoom)
					{
						auto & NextPointObj = AllPipePointArray[Index + 1];
						TSharedPtr<FATPipePointInfo> NextPointPtr = NextPointObj.Key;
						float NextPointHeight = NextPointPtr->InnerPos.Z;

						TSharedPtr<FATRoomPlane> FirstPlane = PreRoomPlane;
						TSharedPtr<FATRoomPlane> SecondPlane = NextPlane;

						FVector BorePointFirst = NextPointPtr->InnerPos;
						FVector BorePointSecond = NextPointPtr->InnerPos;
						TSharedPtr<FATPipePointInfo> SharePoint;
						if (PreRoom != NextRoom)
						{
							bool Stop = false;
							bool ExistSharePlane = false;
							bool BackDir = false;
							int32 MinStep = AllPlaneSortArray.Num();

							for (auto TempPlane : NextRoom->PlaneArray)
							{
								for (auto Plane : PreRoom->PlaneArray)
								{
									if ((Plane->Normal + TempPlane->Normal).Size() < 0.001)
									{
										float TempStartDis = FMath::PointDistToLine(TempPlane->StartPos, Plane->RangeDirection, Plane->StartPos);
										float TempEndDis = FMath::PointDistToLine(TempPlane->EndPos, Plane->RangeDirection, Plane->StartPos);

										if (TempStartDis < 60 && FMath::Abs(TempStartDis - TempEndDis) < 0.001)
										{
											FVector StartToPlane = FMath::ClosestPointOnLine(Plane->StartPos, Plane->EndPos, TempPlane->StartPos);
											FVector EndToPlane = FMath::ClosestPointOnLine(Plane->StartPos, Plane->EndPos, TempPlane->EndPos);

											FVector StartToTempPlane = FMath::ClosestPointOnLine(TempPlane->StartPos, TempPlane->EndPos, Plane->StartPos);
											FVector EndToTempPlane = FMath::ClosestPointOnLine(TempPlane->StartPos, TempPlane->EndPos, Plane->EndPos);

											float MinLen = (Plane->EndPos - Plane->StartPos).Size();
											FVector OPoint = Plane->EndPos;
											if ((!BackDir) &&((Plane->EndPos - Plane->StartPos).GetSafeNormal() - Plane->RangeDirection).Size() < 0.05)
											{
												OPoint = Plane->StartPos;
											}


											int32 FrontIndex = AllPlaneSortArray.Find(Plane);
											int32 BackIndex = AllPlaneSortArray.FindLast(Plane);

											int32 LastStep = AllPlaneSortArray.Num()- 1 - BackIndex;

											int32 TempMinStep = LastStep < FrontIndex ? LastStep : FrontIndex;

											if (ExistSharePlane && MinStep <= TempMinStep)
											{
												continue;
											}
											
											if ((StartToPlane - TempPlane->StartPos).Size() - TempStartDis < 0.001)
											{
												ExistSharePlane = true;

												if ((StartToPlane - OPoint).Size() < MinLen)
												{
													BackDir = LastStep < FrontIndex;

													MinStep = TempMinStep;
													FirstPlane = Plane;
													SecondPlane = TempPlane;

													MinLen = (StartToPlane - OPoint).Size();
													FVector PlaneDir = (TempPlane->EndPos - TempPlane->StartPos).GetSafeNormal();
													BorePointFirst = StartToPlane + PlaneDir * 5;
													BorePointSecond = TempPlane->StartPos + PlaneDir * 5;
												}
											}
											if ((EndToPlane - TempPlane->EndPos).Size() - TempStartDis < 0.001)
											{
												ExistSharePlane = true;
												if ((EndToPlane - OPoint).Size() < MinLen)
												{
													BackDir = LastStep < FrontIndex;

													MinStep = TempMinStep;
													FirstPlane = Plane;
													SecondPlane = TempPlane;
													MinLen = (EndToPlane - OPoint).Size();
													FVector PlaneDir = (TempPlane->StartPos - TempPlane->EndPos).GetSafeNormal();
													BorePointFirst = EndToPlane + PlaneDir * 5;
													BorePointSecond = TempPlane->EndPos + PlaneDir * 5;
												}
											}
											if ((StartToTempPlane - Plane->StartPos).Size() - TempStartDis < 0.001)
											{
												ExistSharePlane = true;
												if ((Plane->StartPos - OPoint).Size() < MinLen)
												{
													BackDir = LastStep < FrontIndex;

													MinStep = TempMinStep;
													FirstPlane = Plane;
													SecondPlane = TempPlane;
													MinLen = (Plane->StartPos - OPoint).Size();
													FVector PlaneDir = (Plane->EndPos - Plane->StartPos).GetSafeNormal();
													BorePointFirst = Plane->StartPos + PlaneDir * 5;
													BorePointSecond = StartToTempPlane + PlaneDir * 5;
												}
											}
											if ((EndToTempPlane - Plane->EndPos).Size() - TempStartDis < 0.001)
											{
												ExistSharePlane = true;
												if ((Plane->EndPos - OPoint).Size() < MinLen)
												{
													BackDir = LastStep < FrontIndex;

													MinStep = TempMinStep;
													FirstPlane = Plane;
													SecondPlane = TempPlane;
													MinLen = (Plane->EndPos - OPoint).Size();
													FVector PlaneDir = (Plane->StartPos - Plane->EndPos).GetSafeNormal();
													BorePointFirst = Plane->EndPos + PlaneDir * 5;
													BorePointSecond = EndToTempPlane + PlaneDir * 5;
												}
											}
											if (ExistSharePlane)
											{
												float LLen = (StartToPlane - EndToPlane).Size();
												if (NextPointObj.Value == TempPlane && (StartToPlane - NextPointPtr->InnerPos).Size() <= LLen && (EndToPlane - NextPointPtr->InnerPos).Size() <= LLen)
												{
													SharePoint = MakeShareable(new FATPipePointInfo(NextPointPtr->InnerPos - TempPlane->Normal * TempStartDis, Plane->Normal));
													SharePoint->PointType = PointType;
													Stop = true;
													break;
												}
												else if (Plane == PreRoomPlane)
												{
													FVector CloseToTempPlane = FMath::ClosestPointOnLine(TempPlane->StartPos, TempPlane->EndPos, PrePoint);
													CloseToTempPlane.Z = PrePoint.Z;
													if ((CloseToTempPlane - PrePoint).Size() < 60)
													{
														NextPointPtr = MakeShareable(new FATPipePointInfo(CloseToTempPlane, TempPlane->Normal));
														SharePoint = MakeShareable(new FATPipePointInfo(PrePoint, PreRoomPlane->Normal));
														SharePoint->PointType = PointType;
													}
													Stop = true;
													break;
												}
											}
										}
									}
								}
								if (Stop)
								{
									break;
								}
							}
						}

						if (SharePoint.IsValid())
						{
							AutoDesignInterpolation(LInfo, AllPlaneSortArray, PreRoom, PreRoomPlane, PrePoint, PreNormal, NULL, FirstPlane, SharePoint);
							LInfo.PipePointArray.AddUnique(*NextPointPtr);
						}
						else
						{
							BorePointFirst.Z = FArmySceneData::WallHeight;
							BorePointSecond.Z = FArmySceneData::WallHeight;

							TSharedPtr<FATPipePointInfo> TempSharePoint = MakeShareable(new FATPipePointInfo(BorePointFirst, FirstPlane->Normal));
							TempSharePoint->PointType = PointType;
							AutoDesignInterpolation(LInfo, AllPlaneSortArray, PreRoom, PreRoomPlane, PrePoint, PreNormal, NULL, FirstPlane, TempSharePoint);
							LInfo.PipePointArray.AddUnique(FATPipePointInfo(BorePointSecond, SecondPlane->Normal));
						}

						NextPlane = SecondPlane;
					}
					else
					{
						AutoDesignInterpolation(LInfo, AllPlaneSortArray, PreRoom, PreRoomPlane, PrePoint, PreNormal, NULL, PointObj.Value, PointPtr);
					}

					PreRoom = NextRoom;
					PreRoomPlane = NextPlane;
					FATPipePointInfo& LastPoint = LInfo.PipePointArray.Last();
					PrePoint = LastPoint.InnerPos;// PointPtr->InnerPos + PointPtr->InnerPos - OnWallPoint;
					PreNormal = LastPoint.Normal;// -PreRoomPlane->Normal;
				}
			}
			SyncProgress += 100 * Step6Weight / CountWeight;
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignSortOriginalPoint()
{
	struct FOriginalSortInfo
	{
		FOriginalSortInfo(int32 InLoopIndex,int32 InPointIndex,FVector InOffset,float InOffsetNormal):PipeLoopIndex(InLoopIndex),OffsetPointIndex(InPointIndex),OffsetToOriginal(InOffset),OffsetNormal(InOffsetNormal)
		{}
		int32 PipeLoopIndex = -1;
		int32 OffsetPointIndex = -1;
		float OffsetNormal = 0;
		FVector OffsetToOriginal = FVector::ZeroVector;
	};
	TArray<FOriginalSortInfo> SortArray;
	for (int32 Index = 0; Index < PipeLoopArray.Num(); ++Index)
	{
		if (PipeLoopArray[Index].PipeLineArray.Num() == 0) continue;
		auto& LInfo = PipeLoopArray[Index].PipeLineArray.HeapTop();
		for (int32 i = 0; i < LInfo.PipePointArray.Num() - 1; ++i)
		{
			FVector Dir = LInfo.PipePointArray[i + 1].InnerPos - LInfo.PipePointArray[i].InnerPos;
			if (Dir.Size() < 0.001)
			{
				continue;
			}
			Dir.Normalize();
			if (FMath::Abs(Dir.Z) < 0.0001 && (Dir - OriginalPlane->Normal).Size() > 0.01)
			{
				FVector ProjectionPoint = FMath::ClosestPointOnInfiniteLine(OriginalPlane->StartPos, OriginalPlane->EndPos, LInfo.PipePointArray[i + 1].InnerPos);
				float OffsetNormal = (LInfo.PipePointArray[i + 1].InnerPos - ProjectionPoint).Size();
				FVector Offset = ProjectionPoint - FVector(OriginalPoint->InnerPos.X, OriginalPoint->InnerPos.Y, ProjectionPoint.Z);
				SortArray.Add(FOriginalSortInfo(Index,i + 1,Offset, OffsetNormal));
				break;
			}
		}
	}
	SortArray.Sort([&](const FOriginalSortInfo& A,const FOriginalSortInfo& B) 
	{
		FVector ADir = A.OffsetToOriginal.GetSafeNormal();
		FVector BDir = B.OffsetToOriginal.GetSafeNormal();

		float AOffset = A.OffsetToOriginal.Size();
		float BOffset = B.OffsetToOriginal.Size();

		if ((OriginalIndexRangeDir + ADir).Size() < 0.5)
		{
			AOffset *= -1;
		}
		if ((OriginalIndexRangeDir + BDir).Size() < 0.5)
		{
			BOffset *= -1;
		}
		if (AOffset > 0 && BOffset > 0)
		{
			if (A.OffsetNormal > B.OffsetNormal)
			{
				return false;
			}
			else if (A.OffsetNormal < B.OffsetNormal)
			{
				return true;
			}
			else
			{
				return AOffset > BOffset;
			}
		}
		else if (AOffset < 0 && BOffset < 0)
		{
			if (A.OffsetNormal < B.OffsetNormal)
			{
				return false;
			}
			else if (A.OffsetNormal > B.OffsetNormal)
			{
				return true;
			}
			else
			{
				return AOffset > BOffset;
			}
		}
		return AOffset > BOffset;
	});

	for (size_t i = 0; i < SortArray.Num(); i++)
	{
		auto& LInfo = PipeLoopArray[SortArray[i].PipeLoopIndex].PipeLineArray.HeapTop();
		FVector StartPoint = LInfo.PipePointArray[0].InnerPos;
		FVector OriginalV = i > 8 ? OriginalPointMap[1].Key : OriginalPointMap[9 - i].Key;//崩溃问题：修改OriginalPointMap下标的错误使用。

		FVector Offset = OriginalV - StartPoint;

		for (size_t j = 0; j < SortArray[i].OffsetPointIndex; j++)
		{
			LInfo.PipePointArray[j].InnerPos += Offset;
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignAvoidOverlap()
{
	struct FOverlapLine
	{
		int32 LoopIndex = -1;
		int32 LineIndex = -1;
		int32 StartPointIndex = -1;
		int32 EndPointIndex = -1;

		FVector StartV = FVector::ZeroVector;
		FVector EndV = FVector::ZeroVector;

		float OffsetNormal = 0;
		FVector OffsetToOriginal = FVector::ZeroVector;

		FVector OffsetDirection = FVector::ZeroVector;

		FOverlapLine(int32 InLoopIndex, int32 InLineIndex, int32 InStartPoint, int32 InEndPoint,FVector InOffset,float InOffsetNoraml) :
			LoopIndex(InLoopIndex),
			LineIndex(InLineIndex),
			StartPointIndex(InStartPoint),
			EndPointIndex(InEndPoint),
			OffsetToOriginal(InOffset),
			OffsetNormal(InOffsetNoraml){}

		bool operator==(const FOverlapLine& InOther) const
		{
			return LoopIndex == InOther.LoopIndex && LineIndex == InOther.LineIndex && StartPointIndex == InOther.StartPointIndex && EndPointIndex == InOther.EndPointIndex;
		}
	};
	TArray<TArray<FOverlapLine>> OverLapLinesArray;
	auto CompareOverlap = [&](FOverlapLine& InOverlap)
	{
		FVector InDirection = (InOverlap.EndV - InOverlap.StartV).GetSafeNormal();
		bool InExist = false;
		TArray<FOverlapLine>* CurrentArray = NULL;
		for (int32 LoopIndex = 0; LoopIndex < PipeLoopArray.Num(); ++LoopIndex)
		{
			if (InOverlap.LoopIndex == LoopIndex) continue;

			auto& LoopInfo = PipeLoopArray[LoopIndex];
			for (int32 LineIndex = 0; LineIndex < LoopInfo.PipeLineArray.Num(); ++LineIndex)
			{
				auto& L = LoopInfo.PipeLineArray[LineIndex];
				for (int32 i = 0; i < L.PipePointArray.Num() - 1; ++i)
				{
					const FVector& StartV = L.PipePointArray[i].InnerPos;
					const FVector& EndV = L.PipePointArray[i + 1].InnerPos;
					L.PipePointArray[i].Normal;
					L.PipePointArray[i + 1].Normal;

					if (FMath::Abs(StartV.Z - EndV.Z) > 0.01 || FMath::Abs(StartV.Z - InOverlap.StartV.Z) > 0.01 || FMath::Abs(StartV.Z - InOverlap.EndV.Z) > 0.01)
					{
						continue;
					}
					FVector TempDirecton = (EndV - StartV).GetSafeNormal();

					FVector ProjectionDirecton(-TempDirecton.Y, TempDirecton.X, 0);
					if ((ProjectionDirecton + L.PipePointArray[i].Normal).Size() < 0.01 || (ProjectionDirecton - L.PipePointArray[i].Normal).Size() < 0.01)
					{
						ProjectionDirecton = L.PipePointArray[i].Normal;
					}
					else if ((ProjectionDirecton + L.PipePointArray[i + 1].Normal).Size() < 0.01 || (ProjectionDirecton - L.PipePointArray[i + 1].Normal).Size() < 0.01)
					{
						ProjectionDirecton = L.PipePointArray[i + 1].Normal;
					}

					FVector NextV = EndV;
					float OffsetNormal = 0;
					for (int32 j = i + 2; j < L.PipePointArray.Num(); j++)
					{
						NextV = L.PipePointArray[j].InnerPos;
						if (FMath::Abs(NextV.Z - EndV.Z) > 0.01)
						{
							break;
						}
						if (((NextV - EndV).GetSafeNormal() - TempDirecton).Size() > 0.1)
						{
							FVector BasePos = FMath::ClosestPointOnInfiniteLine(StartV,EndV,FVector(0,0,0));
							OffsetNormal = (EndV - BasePos).Size();

							if (((EndV - BasePos).GetSafeNormal() + TempDirecton).Size() < 0.01)
							{
								OffsetNormal *= -1;
							}
							break;
						}
					}

					if ((TempDirecton - InDirection).Size() < 0.01 || (TempDirecton + InDirection).Size() < 0.01)//平行
					{
						if ((EndV - InOverlap.StartV).Size() + (EndV - InOverlap.EndV).Size() - (InOverlap.EndV - InOverlap.StartV).Size() < 0.01 ||
							(StartV - InOverlap.StartV).Size() + (StartV - InOverlap.EndV).Size() - (InOverlap.EndV - InOverlap.StartV).Size() < 0.01)//在线段内  说明有重叠
						{
							FOverlapLine OverlapL(LoopIndex,LineIndex,i,i + 1, NextV - EndV,OffsetNormal);
							OverlapL.OffsetDirection = ProjectionDirecton;
							InOverlap.OffsetDirection = ProjectionDirecton;
							if (CurrentArray)
							{
								CurrentArray->AddUnique(OverlapL);
							}
							else
							{
								for (auto& TheArray : OverLapLinesArray)
								{
									if (TheArray.Contains(InOverlap))
									{
										InExist = true;
										if (!TheArray.Contains(OverlapL))
										{
											TheArray.AddUnique(OverlapL);
										}
										CurrentArray = &TheArray;
										break;
									}
									else if (TheArray.Contains(OverlapL))
									{
										InExist = true;
										if (!TheArray.Contains(InOverlap))
										{
											TheArray.AddUnique(InOverlap);
										}
										CurrentArray = &TheArray;
										break;
									}
								}
								if (!InExist)
								{
									int32 Index = OverLapLinesArray.Add(TArray<FOverlapLine>{InOverlap, OverlapL});
									CurrentArray = &(OverLapLinesArray[Index]);
								}
							}
						}
					}
				}
			}
		}
	};
	for (int32 LoopIndex = 0; LoopIndex < PipeLoopArray.Num();++LoopIndex)
	{
		auto& LoopInfo = PipeLoopArray[LoopIndex];
		for (int32 LineIndex = 0; LineIndex < LoopInfo.PipeLineArray.Num();++LineIndex)
		{
			auto& L = LoopInfo.PipeLineArray[LineIndex];
			for (int32 i = 0; i < L.PipePointArray.Num() - 1; ++i)
			{
				FVector StartV = L.PipePointArray[i].InnerPos;
				FVector EndV = L.PipePointArray[i + 1].InnerPos;
				if (FMath::Abs(L.PipePointArray[i].InnerPos.Z - L.PipePointArray[i + 1].InnerPos.Z) > 0.01) continue;//暂时不考虑垂直重叠的问题

				FVector TempDirecton = (EndV - StartV).GetSafeNormal();
				FVector NextV = EndV;
				float OffsetNormal = 0;
				for (int32 j = i + 2; j < L.PipePointArray.Num(); j++)
				{
					NextV = L.PipePointArray[j].InnerPos;
					if (FMath::Abs(NextV.Z - EndV.Z) > 0.01)
					{
						break;
					}
					if (((NextV - EndV).GetSafeNormal() - TempDirecton).Size() > 0.1)
					{
						FVector BasePos = FMath::ClosestPointOnInfiniteLine(StartV, EndV, FVector(0, 0, 0));
						OffsetNormal = (EndV - BasePos).Size();

						if (((EndV - BasePos).GetSafeNormal() + TempDirecton).Size() < 0.01)
						{
							OffsetNormal *= -1;
						}
						break;
					}
				}

				FOverlapLine OverlapL(LoopIndex, LineIndex, i, i + 1, NextV - EndV, OffsetNormal);
				OverlapL.StartV = StartV;
				OverlapL.EndV = EndV;
				CompareOverlap(OverlapL);
			}
		}
	}

	for (auto& TheArray : OverLapLinesArray)
	{
		FVector BaseDirection = TheArray.HeapTop().OffsetDirection;
		TheArray.Sort([&](const FOverlapLine& A, const FOverlapLine& B)
		{
			FVector ADir = A.OffsetToOriginal.GetSafeNormal();
			FVector BDir = B.OffsetToOriginal.GetSafeNormal();

			float AOffset = A.OffsetToOriginal.Size();
			float BOffset = B.OffsetToOriginal.Size();

			if ((BaseDirection + ADir).Size() < 0.5)
			{
				AOffset *= -1;
			}
			if ((BaseDirection + BDir).Size() < 0.5)
			{
				BOffset *= -1;
			}
			if (AOffset > 0 && BOffset > 0)
			{
				if (A.OffsetNormal > B.OffsetNormal)
				{
					return true;
				}
				else if (A.OffsetNormal < B.OffsetNormal)
				{
					return false;
				}
				else
				{
					return AOffset > BOffset;
				}
			}
			else if (AOffset < 0 && BOffset < 0)
			{
				if (A.OffsetNormal < B.OffsetNormal)
				{
					return true;
				}
				else if (A.OffsetNormal > B.OffsetNormal)
				{
					return false;
				}
				else
				{
					return AOffset > BOffset;
				}
			}
			return AOffset < 0;
		});

		for (int32 i = 0; i < TheArray.Num();i++)
		{
			auto& LInfo = TheArray[i];
			auto& TargetInfo = PipeLoopArray[LInfo.LoopIndex].PipeLineArray[LInfo.LineIndex];
			TargetInfo.PipePointArray[LInfo.StartPointIndex].InnerPos += (BaseDirection * i * 2.5);
			TargetInfo.PipePointArray[LInfo.EndPointIndex].InnerPos += (BaseDirection * i * 2.5);
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignGeneratePipeLines()
{
	AutoDesignSortOriginalPoint();
	AutoDesignAvoidOverlap();

	for (auto& LoopInfo : PipeLoopArray)
	{
		TArray<TSharedPtr<FContentItemSpace::FContentItem>> OutArray;

		TSharedPtr<FContentItemSpace::FContentItem> PipeProductItem = NULL;
		FArmyHydropowerDataManager::Get()->GetProductInfo(LoopInfo.LoopType, OutArray);
		for (auto Item : OutArray)
		{
			if (Item->ID == LoopInfo.LoopPipeID)
			{
				PipeProductItem = Item;
				break;
			}
		}
		if (PipeProductItem.IsValid())
		{
			TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(PipeProductItem->ResObjArr[0]);
			EObjectType LineType = EObjectType(Res->ObjectType);
			for (auto & L : LoopInfo.PipeLineArray)
			{
				for (int32 i = 0; i < L.PipePointArray.Num() - 1; ++i)
				{
					if ((L.PipePointArray[i].InnerPos - L.PipePointArray[i + 1].InnerPos).Size() < 0.001)
					{
						continue;
					}
					TSharedPtr<FArmyPipePoint> P1 = FArmyHydropowerDataManager::Get()->MakeShareablePoint(L.PipePointArray[i].InnerPos, LineType, Res->PointColor, Res->PointReformColor);
					TSharedPtr<FArmyPipePoint> P2 = FArmyHydropowerDataManager::Get()->MakeShareablePoint(L.PipePointArray[i + 1].InnerPos, LineType, Res->PointColor, Res->PointReformColor);

					FArmyHydropowerDataManager::Get()->CreatePipeline(P1, P2, Res);//step 7 weight 3 : point number - 1
				}
			}
			SyncProgress += 100 * Step7Weight / CountWeight;
		}
	}
}
void FArmyAutoDesignPipeLine::AutoDesignLightingPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo)
{

}
void FArmyAutoDesignPipeLine::AutoDesignRefrigeratorPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo)
{

}
void FArmyAutoDesignPipeLine::AutoDesignAirConditionerPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo)
{

}
void FArmyAutoDesignPipeLine::AutoDesignPowerPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo)
{

}
void FArmyAutoDesignPipeLine::AutoDesignELVPipeLine(const TSharedPtr<FHydropowerPowerSytemItem> InLoopInfo)
{

}
void FArmyAutoDesignPipeLine::AutoDesignAdjust()
{

}
#undef LOCTEXT_NAMESPACE 