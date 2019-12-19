#include "ArmyAutoDesignPoint.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "EngineUtils.h"
#include "SArmyAutoDesignPoint.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"
#include "ArmyTypes.h"
#include "ArmyAutoDesignSettings.h"
#include "ArmyAutoDesignDataManager.h"
#include "ArmyResourceModule.h"
#include "ArmyDataTools.h"
#include "ArmyWallLine.h"
#include "ArmyActorConstant.h"
#include "ArmyAutoDesignSettings.h"
#include "ArmyRoomEntity.h"
#include "SArmyHydropowerAutoDesignError.h"
#include "ArmyHardware.h"
#include "ArmyAutoDesignPipeLine.h"
//
#include "ArmyFurnitureActor.h"

//开关
static const float Switch_AI_Door_DisFloor = 130.f;//开关距离地面距离   门
static const float Switch_AI_Door_DisModelH = 15.f;//开关距离门水平距离  门
static const float Switch_AI_Bed_DisFloor = 70.f;//开关距离地面距离  床
static const float Switch_AI_Bed_DisModelH = 15.f;//开关距离门水平距离  床

//插座
static const float Socket_AI_Desk_DisFloor = 95.f; //书桌距离地面
static const float Socket_AI_Desk_DisWall = 25.f;//桌子距离墙的最远距离

static const float Socket_AI_DiningTable_DisFloor = 30.f; //长方型餐桌距离地面
static const float Socket_AI_DiningTable_DisWall = 25.f;//长方型餐桌距离墙的最远距离

static const float Socket_AI_WashingMachine_DisFloor = 130.f; //洗衣机插座距地面距离
static const float Socket_AI_WashingMachine_LorRDis = 20.f;	//洗衣机插座 在模型中线 偏左/右 距离
static const float Socket_AI_WashingMachine_DisLorRWall = 40.f; //洗衣机侧墙的距离

static const float Socket_AI_Hood_DisFloor = 200.f; //油烟机插座距离地面的距离
static const float Socket_AI_Hood_LorRDis = 15.f;	  //油烟机插件在模型中线 偏左/右 距离

static const float Socket_AI_Dresser_DisFloor = 95.f; //梳妆台插座距地距离

static const float Socket_AI_TVWall_DisofBottom = 10.f; //电视机插座，布置在距离电视机上方距 底部的距离
static const float Socket_AI_TVBench_DisofTop = 10.f;   //电视柜插座，布置在距离电视柜上方的距离

static const float Socket_AI_Closestool_DisFloor = 60.f; //智能马桶的插座距离地面的距离
static const float Socket_AI_Closestool_DisLorR = 10.f; //智能马桶的插座在包围盒左右的距离

static const float Socket_AI_AirConditionedHang_DisFloor = 200.f; //壁挂空调的插座距离地面的距离
static const float Socket_AI_AirConditionedHang_DisLorR = 10.f; //壁挂空调的插座在包围盒左右的距离

static const float Socket_AI_Sofa_Master_DisFloor = 30.f; //主位沙发的插座距地面的距离
static const float Socket_AI_Sofa_Master_DisLorR = 10.f; //主卫沙发距离中线左右的距离

static const float Socket_AI_Heater_DisFloor = 200.f; //热水器的插座距地面的位置
static const float Socket_AI_Heater_DisLorR = 15.f;  //热水器的插座在包围盒左右的距离

static const float Socket_AI_Refrigerator_DisFloor = 30.f; //冰箱的插座距地面的位置
static const float Socket_AI_Refrigerator_DisLorR = 30.f;  //冰箱的插座在背立面中线左右的距离

static const float Socket_AI_AirConditionedVert_DisFloor = 30.f; //柜式空调的插座距地面的位置
static const float Socket_AI_AirConditionedVert_DisLorR = 35.f;  //柜式空调的插座在背立面中线左右的距离

static const float Socket_AI_Bed_NOTable_DisFloor = 30.f; //无床头柜的床侧插座距地面的位置
static const float Socket_AI_Bed_Table_DisFloor = 70.f; //有床头柜的床侧插座距地面的位置
static const float Socket_AI_Bed_DisBox = 25.f;  //插座距离床包围盒两侧的距离
static const float Socket_AI_Bed_DisCheck = 25.f;  //检测床侧一定距离是否存在床头柜

//水位
static const float WaterPos_AI_AIWashing_DisFloor = 120.f; //洗衣机水位距离地面的高度
static const float WaterPos_AI_AIWashing_DisLorR = 20.f; //洗衣机水位距离洗衣机中线的距离
static const float WaterPos_AI_AIWashing_DisLorRWall = 9.f; //洗衣机侧墙的距离

static const float WaterPos_Heater_DisFloor = 170.f; //电热水器距离地面的高度
static const float WaterPos_Gas_Heater_DisFloor = 130.f; //燃气热水器距离地面的高度
static const float WaterPos_Heater_DisLorR = 7.5f; //热水器距中线的距离

static const float WaterPos_AI_IntelClosestool_DisFloor = 20.f; //智能马桶距离地面的高度
static const float WaterPos_AI_IntelClosestool_DisLorR = 25.f; //马桶距中线的距离

static const float WaterPos_AI_Sprinkler_DisFloor = 115.f; //花洒距离地面的高度
static const float WaterPos_AI_Sprinkler_DisLorR = 7.5f; //花洒距离中线的距离

static const float WaterPos_AI_BathroomArk_DisFloor = 50.f; //浴室柜距离地面的高度
static const float WaterPos_AI_BathroomArk_DisLorR = 5.f; //浴室柜距中线的距离

static const float FloorDrain_AI_BathroomArk_DisWall = 20.f; //浴室柜水盆下水距离产品背立面的距离

static const float FloorDrain_AI_Washing_DisWall = 10.f; //洗衣机地漏距离产品背立面的距离
static const float FloorDrain_AI_Washing_DisLOrR = 10.f; //洗衣机地漏距离产品中线的左右距离

static const float FloorDrain_AI_Sprinkler_DisWall = 20.f; //花洒地漏距离产品背立面的距离

//以上是基础常量数据定义

//清空自动布点
bool FArmyAutoDesignPoint::Empty()
{
	/*清空room数据*/
	RoomTableArr.Reset();
	/*清空自动布点数据*/
	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num() > 0 && ActorItr->ActorHasTag(XRActorTag::AutoDesignPoint))
		{
			ActorItr->Destroy();
		}
	}
	FailInfoArr.Reset();
	FailInfoType = 0;
	/*清空门的状态*/
	DoorStateArray.Reset();
	bSecurityPlaceSuccess = false;
	/*结束*/
	return true;
}

//自动布点方法入口
bool FArmyAutoDesignPoint::AutoDesign()
{
	/*清空room数据*/
	RoomTableArr.Reset();
	/*检查是否存在“未命名”的房间等，并获取房间数据*/
	bool isRoomValid = true;
	TArray<FObjectWeakPtr> AllRooms;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, AllRooms);
	for (auto& RoomIt : AllRooms)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomIt.Pin());
		if (Room.IsValid())
		{
			if (Room->GetSpaceName() == TEXT("未命名"))
			{
				//异常数据
				isRoomValid = false;
				break;
			}
			//正常数据
			RoomTableArr.Add(MakeShareable(new FArmyRoomEntity(Room)));
		}
	}
	//
	if (!isRoomValid)
	{
		//显示房间未命名对话框
		GVC->ViewportOverlayWidget->AddSlot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Top)
			.Padding(0, 86, 20, 0)
			[
				SNew(SArmyHydropowerAutoDesignError)
				.Title(FText::FromString(TEXT("请命名房间")))
				.TitleIcon(FName(TEXT("HydropowerMode.namespace")))
				.Content(FText::FromString(TEXT("请在【原始户型】或【拆改户型】下为所有房间命名")))
				.ContentIcon(FName(TEXT("HydropowerMode.namespaceInfo")))
				.Visibility((EVisibility::Visible))
			];
	}
	else
	{
		//创建对话框，并设置确定回调方法
		AutoDesignWid = SNew(SArmyAutoDesignPoint).OnClickDelegate(this, &FArmyAutoDesignPoint::OnClick);
		//显示布点选择（水位、开关、插座）对话框
		GGI->Window->PresentModalDialog(
			AutoDesignWid->AsShared()
		);
	}

	return isRoomValid;
}

void FArmyAutoDesignPoint::OnClick(int32 inN)
{
	/*点击确定按钮*/
	if (inN == 1)
	{
		/*构造点位数据管理器*/
		if (!AutoDesignPointManager.IsValid())
		{
			AutoDesignPointManager = MakeShareable(new FArmyAutoDesignDataManager());
			AutoDesignPointManager->OnDownLoadFinished = FBoolDelegate::CreateRaw(this, &FArmyAutoDesignPoint::DataDownloadState);//确定下载回调方法
		}
		AutoDesignPointManager->RequestData();
	}
	/*关闭对话框*/
	GGI->Window->DismissModalDialog();
}

//下载完成，进行自动点位布置
void FArmyAutoDesignPoint::DataDownloadState(bool bSuccess)
{
	//关闭等待框
	GGI->Window->HideThrobber();

	if (bSuccess)
	{
		// 数据下载完成，将解析的数据存储起来
		FArmyResourceModule::Get().GetResourceManager()->AppendSynList(AutoDesignPointManager->GetAutoDesignContentItems());

		//执行自动布点
		ExecuteAutoPointDesign();

		GGI->Window->ShowMessage(MT_Success, TEXT("完成自动布点"));
	}
	else
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("自动布点数据下载失败"));
	}
}

///////////////////////////////////////以下是完全的数据运算/////////////////////////

//自动布置水位、开关和插座
bool FArmyAutoDesignPoint::ExecuteAutoPointDesign()
{
	bool isAutoWater = AutoDesignWid->GetIsAutoWaterInfo();
	bool isAutoSwitch = AutoDesignWid->GetIsAutoSwitchInfo();
	bool isAutoSocket = AutoDesignWid->GetIsAutoSocketInfo();
	if (isAutoWater || isAutoSwitch || isAutoSocket)
	{
		for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Num() > 0 && ActorItr->ActorHasTag(XRActorTag::AutoDesignPoint))
			{
				ActorItr->Destroy();
			}
		}
	}
	FailInfoArr.Reset();
	FailInfoType = 0;
	bSecurityPlaceSuccess = false;

	if (isAutoWater)//水位
	{
		RunWaterPosAutoDesign();
	}
	
	if(isAutoSwitch)//开关
	{
		RunSwitchAutoDesign();
	}
	
	if (isAutoSocket)//插座
	{
		RunSocketAutoDesign();
	}

	//检测柱子等碰撞
	CheckWallCornerCollision();

	//显示布置失败提示语对话框
	ShowDialogFailInfo();

	return true;
}

//自动布置水位
bool FArmyAutoDesignPoint::RunWaterPosAutoDesign()
{
	/** 布置洗衣机*/
	PlaceWaterPointOrSocketInWashing(EAutoDesignModelID::ADM_Water_Cold_Water_OutLet);
	/** 布置普通的水位*/

	//对所有房间的普通规则布置水位
	for (TSharedPtr<FArmyRoomEntity>& RoomIt : RoomTableArr)
	{
		//EAIComponentCode AICompnentCode, float AIDisFloor, float AILorR, EAutoDesignModelID AutoDesignId)
		/** 电热水器*/
		PlaceColdHotWaterPoint(AI_Electric_Heater, WaterPos_Heater_DisFloor, WaterPos_Heater_DisLorR, ADM_Water_Cold_Water_OutLet, ADM_Water_Hot_Water_OutLet, RoomIt);
		/** 燃气热水器*/
		PlaceColdHotWaterPoint(AI_Gas_Heater, WaterPos_Gas_Heater_DisFloor, WaterPos_Heater_DisLorR, ADM_Water_Cold_Water_OutLet, ADM_Water_Hot_Water_OutLet, RoomIt);
		/*  智能马桶 */ //应该是 AI_IntelClosestool 但是模型ID对应不符
		PlaceNormalWaterPoint(AI_Closestool, WaterPos_AI_IntelClosestool_DisFloor, WaterPos_AI_IntelClosestool_DisLorR, ADM_Water_Cold_Water_OutLet, RoomIt);

		/** 花洒*/
		PlaceColdHotWaterPoint(AI_Sprinkler, WaterPos_AI_Sprinkler_DisFloor, WaterPos_AI_Sprinkler_DisLorR, ADM_Water_Cold_Water_OutLet, ADM_Water_Hot_Water_OutLet, RoomIt);

		/** 浴室柜 */
		PlaceColdHotWaterPoint(AI_BathroomArk, WaterPos_AI_BathroomArk_DisFloor, WaterPos_AI_BathroomArk_DisLorR, ADM_Water_Cold_Water_OutLet, ADM_Water_Hot_Water_OutLet, RoomIt);
		PlaceColdHotWaterPoint(AI_BathroomArk_Wall, WaterPos_AI_BathroomArk_DisFloor, WaterPos_AI_BathroomArk_DisLorR, ADM_Water_Cold_Water_OutLet, ADM_Water_Hot_Water_OutLet, RoomIt);

		/** 布置浴室柜水盆下水*/
		PlaceFloorDrain(AI_BathroomArk, FloorDrain_AI_BathroomArk_DisWall, 0, ADM_Water_Launching_Of_A_Basin, ADM_None, RoomIt);
		PlaceFloorDrain(AI_BathroomArk_Wall, FloorDrain_AI_BathroomArk_DisWall, 0, ADM_Water_Launching_Of_A_Basin, ADM_None, RoomIt);

		/** 布置洗衣机地漏*/
		PlaceFloorDrain(AI_WashingMachine, FloorDrain_AI_Washing_DisWall, FloorDrain_AI_Washing_DisLOrR, ADM_Water_Floor_Drain, ADM_Water_The_Floor_Drain, RoomIt);

		/** 布置花洒地漏 */
		PlaceFloorDrain(AI_Sprinkler, FloorDrain_AI_Sprinkler_DisWall, 0, ADM_Water_Floor_Drain, ADM_Water_The_Floor_Drain, RoomIt);
	}
	return true;
}

//自动布置洗衣机相关的水位或者插座
void FArmyAutoDesignPoint::PlaceWaterPointOrSocketInWashing(EAutoDesignModelID ModelID)
{
	//对所有房间的洗衣机布置水位或者插座
	float Dis_Floor = WaterPos_AI_AIWashing_DisFloor;
	float Dis_LorR = WaterPos_AI_AIWashing_DisLorR;
	float Dis_LorRWall = WaterPos_AI_AIWashing_DisLorRWall;
	float Length_Wall = 11;
	if (ModelID == EAutoDesignModelID::ADM_Water_Cold_Water_OutLet)
	{
		Dis_Floor = WaterPos_AI_AIWashing_DisFloor;
		Dis_LorR = WaterPos_AI_AIWashing_DisLorR;
		Dis_LorRWall = WaterPos_AI_AIWashing_DisLorRWall;
		Length_Wall = 11;
	}
	else if (ModelID == EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A)
	{
		Dis_Floor = Socket_AI_WashingMachine_DisFloor;
		Dis_LorR = Socket_AI_WashingMachine_LorRDis;
		Dis_LorRWall = Socket_AI_WashingMachine_DisLorRWall;
		Length_Wall = 10;
	}
	//
	for (TSharedPtr<FArmyRoomEntity>& RoomIt : RoomTableArr)
	{
		if (RoomIt->IsExistObj(EAIComponentCode::AI_WashingMachine))
		{
			TArray<TSharedPtr<FArmyModelEntity>> WashingMachineArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_WashingMachine, WashingMachineArr);
			//
			TArray<TSharedPtr<FArmyHardware>> WindowArr;
			WindowArr = RoomIt->GetWindowsActor();
			for (TSharedPtr<FArmyModelEntity>& WashingMachine : WashingMachineArr)
			{
				TSharedPtr<FArmyWallLine> WMBackWall = FArmyDataTools::GetWallLineByModelInDir(*WashingMachine->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
				if (WMBackWall.IsValid())
				{
					//背后的墙找到
					FVector VecDirH = WMBackWall->GetDirection();
					FVector VecDirV = RoomIt->GetRoom()->GetWallLineNormal(WMBackWall);
					VecDirV = VecDirV.GetSafeNormal();
					FVector PosCenterBack(0, 0, 0);
					bool isPosValid = FArmyDataTools::PointIntervalGroundOnDirProjPoint(*WashingMachine->Actor, Dis_Floor, PosCenterBack, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
					if (isPosValid)
					{
						TArray<FVector> PointArr;
						TArray<FVector> PointDirArr;
						TSet<AActor*> IgnoreActor;
						IgnoreActor.Add(WashingMachine->Actor);
						AActor* WaterActor = AutoDesignPointManager->CreateActor(ModelID);
						if (WaterActor)
						{
							if (ModelID == EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A)
							{
								// 如果是洗衣机插座则需要设置"标记"字段，成"普通插座"。
								SetSocketPipeLineFlag(WaterActor, ESF_Normal);
							}
							WaterActor->SetActorLocation(PosCenterBack);
							WaterActor->SetActorRotation(VecDirV.Rotation() + FRotator(0, -90, 0));
							if (FArmyDataTools::CalModelIntersectWithOtherModel(WaterActor, RoomIt, IgnoreActor) && !FArmyDataTools::CalModelIntersectWithOtherUnWindowsModel(WaterActor, RoomIt, IgnoreActor))
							{
								//跟窗户相交了
								TSharedPtr<FArmyHardware> InterWindow;
								FBox InActorBox = GVC->GetActorCollisionBox(WaterActor);
								for (TSharedPtr<FArmyHardware>& Window : WindowArr)
								{
									TArray<FVector> WindowVec1 = Window->RectImagePanel->GetVertices();
									for (FVector & VecIT : WindowVec1)
									{
										VecIT.Z = Window->GetHeightToFloor();
									}
									TArray<FVector> WindowVec2 = WindowVec1;
									for (FVector & VecIT : WindowVec2)
									{
										VecIT.Z += Window->GetHeight();
									}
									WindowVec1.Append(WindowVec2);
									FBox ModelActorBox(WindowVec1);
									ModelActorBox = ModelActorBox.ExpandBy(2);//将窗的包围盒适当增大，以便和模型进行碰撞检测
									if (InActorBox.Intersect(ModelActorBox))
									{
										//找到了相交的窗户
										InterWindow = Window;
										break;
									}
								}
								//
								if (InterWindow.IsValid())
								{
									FVector StartPos = InterWindow->GetStartPos(); StartPos.Z = 0;
									FVector EndPos = InterWindow->GetEndPos(); EndPos.Z = 0;
									FVector WindowCenter = (StartPos + EndPos) / 2 + FVector(0, 0, Dis_Floor);
									WindowCenter += VecDirV * (InterWindow->GetWidth() / 2);
									float WindowLength = (EndPos - StartPos).Size();
									//
									TArray<FVector> VecDirHArr = { VecDirH , VecDirH * -1 };
									for (auto& VecDirHTemp : VecDirHArr)
									{
										FVector PosTemp = WindowCenter + VecDirHTemp * (Dis_LorRWall + WindowLength / 2);
										if (RoomIt->GetRoom()->IsPointInRoom(PosTemp + VecDirHTemp * (Length_Wall)))
										{
											PointArr.Add(PosTemp);
											PointDirArr.Add(VecDirV);
										}
										else
										{
											//墙体长度不够，则考虑侧墙
											FVector WMForward = FArmyDataTools::GetModelForwardVector(WashingMachine->Actor);
											TArray<FArmyDataTools::EDirection> DirArr = { FArmyDataTools::EDirection::AI_DT_MODEL_LEFT , FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT };
											for (auto Direction : DirArr)
											{
												TSharedPtr<FArmyWallLine> WMWall = FArmyDataTools::GetWallLineByModelInDir(*WashingMachine->Actor, Direction);
												if (WMWall.IsValid())
												{
													FVector VecWallDirV = RoomIt->GetRoom()->GetWallLineNormal(WMWall);
													VecWallDirV = VecWallDirV.GetSafeNormal();
													FVector VecWallDirH = WMWall->GetDirection();
													FVector WallStart = WMWall->GetCoreLine()->GetStart();
													if (FVector::DotProduct(WMForward, VecWallDirH) < 0)
													{
														VecWallDirH *= -1;
														WallStart = WMWall->GetCoreLine()->GetEnd();
													}
													WallStart += Dis_LorRWall * VecWallDirH;
													WallStart.Z = Dis_Floor;
													PointArr.Add(WallStart);
													PointDirArr.Add(VecWallDirV);
												}
											}
										}
									}
									//
								}
							}
							else
							{
								//没有跟窗户相交
								FVector PosLeft = PosCenterBack + VecDirH * Dis_LorR;
								FVector PosRight = PosCenterBack + VecDirH * Dis_LorR * -1;
								if (ModelID == EAutoDesignModelID::ADM_Water_Cold_Water_OutLet)
								{
									PointArr.Add(PosLeft);
									PointArr.Add(PosRight);
								}
								else if (ModelID == EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A)
								{
									PointArr.Add(PosRight);
									PointArr.Add(PosLeft);
								}
								PointDirArr.Add(VecDirV);
								PointDirArr.Add(VecDirV);
							}
							//
							if (PointArr.Num() > 0)
							{
								TArray<FVector> PosArr;
								TArray<FVector> PosDirArr;
								for (int i = 0; i < PointArr.Num(); i++)
								{
									auto& Point = PointArr[i];
									WaterActor->SetActorLocation(Point);
									WaterActor->SetActorRotation(PointDirArr[i].Rotation() + FRotator(0, -90, 0));
									if (!FArmyDataTools::CalModelIntersectWithOtherModel(WaterActor, RoomIt, IgnoreActor) && !CheckWallCornerCollision(WaterActor))
									{
										PosArr.Add(Point);
										PosDirArr.Add(PointDirArr[i]);
									}
								}
								FVector PointMin = FVector::ZeroVector;
								FVector PointDirMin = FVector::ZeroVector;
								float DisMin = FLT_MAX;
								for (int i = 0; i < PosArr.Num(); i++)
								{
									auto& Point = PosArr[i];
									float Dis = FVector::Distance(PosCenterBack, Point);
									if (Dis < DisMin && RoomIt->GetRoom()->IsPointInRoom(Point))
									{
										DisMin = Dis;
										PointMin = Point;
										PointDirMin = PosDirArr[i];
									}
								}
								if (!FVector::PointsAreSame(PointMin, FVector::ZeroVector))
								{
									WaterActor->SetActorLocation(PointMin);
									WaterActor->SetActorRotation(PointDirMin.Rotation() + FRotator(0, -90, 0));
								}
								else
								{
									WaterActor->Destroy();
									//点位（冷水口或者插座）模型布置失败（没有满足的布置要求的点位）
									AddFailInfoDesign(RoomIt, AI_WashingMachine, ModelID);
								}
							}
							else
							{
								WaterActor->Destroy();
								//点位（冷水口或者插座）模型布置失败（没有满足的布置要求的点位）
								AddFailInfoDesign(RoomIt, AI_WashingMachine, ModelID);
							}
						}
						else
						{
							//点位（冷水口或者插座）模型创建失败（模型或者构件下载失败）
							AddFailInfoDownload(RoomIt, AI_WashingMachine, ModelID);
						}
					}
				}
			}
		}
	}
}

void FArmyAutoDesignPoint::PlaceColdHotWaterPoint(EAIComponentCode AICompnentCode, float AIDisFloor, float AILorR, EAutoDesignModelID ColdWaterPointID, EAutoDesignModelID HotWaterPointID, TSharedPtr<class FArmyRoomEntity> RoomIt)
{
	if (RoomIt->IsExistObj(AICompnentCode))
	{
		TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
		RoomIt->GetModelEntityArrayByComponentId(AICompnentCode, ActorItemArr);
		for (TSharedPtr<FArmyModelEntity>& ActorItem : ActorItemArr)
		{
			//特殊处理，花洒、电热水器、燃气热水器，水位高度是依赖模型原点高度
			if (AICompnentCode == AI_Sprinkler || AICompnentCode == AI_Electric_Heater || AICompnentCode == AI_Gas_Heater)
			{
				AIDisFloor = ActorItem->Actor->GetActorLocation().Z;
			}
			//特殊处理结束

			//依赖模型的数据
			TSet<AActor*> IgnoreActor;
			IgnoreActor.Add(ActorItem->Actor);
			if (AICompnentCode == AI_Sprinkler)
			{
				//花洒布置水位需要过滤淋浴房
				TArray<TSharedPtr<FArmyModelEntity>> ActorArr;
				RoomIt->GetModelEntityArrayByComponentId(AI_ShowerRoom, ActorArr);
				for (TSharedPtr<FArmyModelEntity>& Item : ActorArr)
				{
					IgnoreActor.Add(Item->Actor);
				}
			}
			TSharedPtr<FArmyWallLine> RelyModelBackWall = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
			if (RelyModelBackWall.IsValid())
			{
				FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), RelyModelBackWall);
				TArray<EAutoDesignModelID> WaterPointArr = { HotWaterPointID , ColdWaterPointID};
				TArray<float> WaterPointPosArr = { AILorR , -AILorR };
				for (int32 i = 0; i < WaterPointArr.Num(); ++i)
				{
					//布置水位
					AActor* WaterPointActor = AutoDesignPointManager->CreateActor(WaterPointArr[i]);
					if (WaterPointActor)
					{
						WaterPointActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
						bool IsValidPos = FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, WaterPointPosArr[i], AIDisFloor, *WaterPointActor);
						if (IsValidPos)
						{
							FVector ActorPos = WaterPointActor->GetActorLocation();
							if (FArmyDataTools::CalModelIntersectWithOtherModel(WaterPointActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(ActorPos) || CheckWallCornerCollision(WaterPointActor))
							{
								WaterPointActor->Destroy();
								//点位（水位）模型布置失败（没有满足的布置要求的点位）
								AddFailInfoDesign(RoomIt, AICompnentCode, WaterPointArr[i]);
							}
						}
					}
					else
					{
						//点位（水位）模型创建失败（模型或者构件下载失败）
						AddFailInfoDownload(RoomIt, AICompnentCode, WaterPointArr[i]);
					}
				}
			}
		}
	}
}

void FArmyAutoDesignPoint::PlaceNormalWaterPoint(EAIComponentCode AICompnentCode, float AIDisFloor, float AILorR, EAutoDesignModelID AutoDesignId, TSharedPtr<class FArmyRoomEntity> RoomIt)
{
	if (RoomIt->IsExistObj(AICompnentCode))
	{
		TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
		RoomIt->GetModelEntityArrayByComponentId(AICompnentCode, ActorItemArr);
		for (TSharedPtr<FArmyModelEntity>& ActorItem : ActorItemArr)
		{
			TSet<AActor*> IgnoreActor;
			IgnoreActor.Add(ActorItem->Actor);
			TSharedPtr<FArmyWallLine> RelyModelBackWall = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
			if (RelyModelBackWall.IsValid())
			{
				FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), RelyModelBackWall);
				//布置水位
				AActor* WaterPointActor = AutoDesignPointManager->CreateActor(AutoDesignId);
				if (WaterPointActor)
				{
					WaterPointActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
					bool IsValidPos = FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, AILorR, AIDisFloor, *WaterPointActor);
					if (IsValidPos)
					{
						FVector ActorPos = WaterPointActor->GetActorLocation();
						if (FArmyDataTools::CalModelIntersectWithOtherModel(WaterPointActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(ActorPos) || CheckWallCornerCollision(WaterPointActor))
						{
							IsValidPos = FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -AILorR, AIDisFloor, *WaterPointActor);
							if (IsValidPos)
							{
								ActorPos = WaterPointActor->GetActorLocation();
								if (FArmyDataTools::CalModelIntersectWithOtherModel(WaterPointActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(ActorPos) || CheckWallCornerCollision(WaterPointActor))
								{
									WaterPointActor->Destroy();
									//点位（水位）模型布置失败（没有满足的布置要求的点位）
									AddFailInfoDesign(RoomIt, AICompnentCode, AutoDesignId);
								}
							}
						}
					}
				}
				else
				{
					//点位（水位）模型创建失败（模型或者构件下载失败）
					AddFailInfoDownload(RoomIt, AICompnentCode, AutoDesignId);
				}
			}
		}
	}
}

void FArmyAutoDesignPoint::PlaceFloorDrain(EAIComponentCode AICompnentCode, float AIDisWall, float AILorR, EAutoDesignModelID FloorDrainID, EAutoDesignModelID FloorDrainID2, TSharedPtr<class FArmyRoomEntity> RoomIt)
{
	if (RoomIt->IsExistObj(AICompnentCode))
	{
		TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
		RoomIt->GetModelEntityArrayByComponentId(AICompnentCode, ActorItemArr);
		for (TSharedPtr<FArmyModelEntity>& ActorItem : ActorItemArr)
		{
			TSet<AActor*> IgnoreActor;
			IgnoreActor.Add(ActorItem->Actor);
			TSharedPtr<FArmyWallLine> RelyModelBackWall = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
			if (RelyModelBackWall.IsValid())
			{
				FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), RelyModelBackWall);
				//布置地漏
				AActor* FloorDrainActor = AutoDesignPointManager->CreateActor(FloorDrainID);
				if (FloorDrainActor)
				{
					FloorDrainActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
					bool IsValidPos = FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, AILorR, 0, *FloorDrainActor);
					if (IsValidPos)
					{
						FVector ActorPos = FloorDrainActor->GetActorLocation();
						ActorPos += AIDisWall * NormalInRoom;
						FloorDrainActor->SetActorLocation(ActorPos);
						if (FArmyDataTools::CalModelIntersectWithOtherModel(FloorDrainActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(ActorPos) || CheckWallCornerCollision(FloorDrainActor))
						{
							IsValidPos = FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -AILorR, 0, *FloorDrainActor);
							if (IsValidPos)
							{
								ActorPos = FloorDrainActor->GetActorLocation();
								ActorPos += AIDisWall * NormalInRoom;
								FloorDrainActor->SetActorLocation(ActorPos);
								if (FArmyDataTools::CalModelIntersectWithOtherModel(FloorDrainActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(ActorPos) || CheckWallCornerCollision(FloorDrainActor))
								{
									FloorDrainActor->Destroy();
									//点位（地漏）模型布置失败（没有满足的布置要求的点位）
									AddFailInfoDesign(RoomIt, AICompnentCode, FloorDrainID);
								}
								else
								{
									AActor* FloorDrainActor2 = AutoDesignPointManager->CreateActor(FloorDrainID2);
									if (FloorDrainActor2)
									{
										FloorDrainActor2->SetActorLocation(ActorPos + FVector(0, 0, 0.6));
										FloorDrainActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
									}
								}
							}
						}
						else
						{
							AActor* FloorDrainActor2 = AutoDesignPointManager->CreateActor(FloorDrainID2);
							if (FloorDrainActor2)
							{
								FloorDrainActor2->SetActorLocation(ActorPos + FVector(0, 0, 0.6));
								FloorDrainActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
							}
						}
					}
				}
			}
			else
			{
				//点位（地漏）模型创建失败（模型或者构件下载失败）
				AddFailInfoDownload(RoomIt, AICompnentCode, FloorDrainID);
			}
		}
	}
}

//自动布置开关
bool FArmyAutoDesignPoint::RunSwitchAutoDesign()
{
	//显示下载进度
	CurrentAutoDesignStr = TEXT("开关");
	TAttribute<FText> SyncProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FArmyAutoDesignPoint::GetAutoDesignProgressText));
	GGI->Window->ShowThrobber(SyncProgressTextAttr);
	//清空门的状态
	DoorStateArray.Reset();
	//int32 --单个房间内门的数量
	TMultiMap<int32, TSharedPtr<FArmyRoomEntity>> RoomEntityDoorInfo;
	for (TSharedPtr<FArmyRoomEntity>& RoomIt : RoomTableArr)
	{
		TArray<TSharedPtr<FArmyHardware>> DoorArr = RoomIt->GetDoorsActor();
		//遍历整个屋子所有的门，将门的信息存储到数组DoorStateArray里
		for (TSharedPtr<FArmyHardware> Door : DoorArr)
		{
			//
			int32 itemIndex = DoorStateArray.IndexOfByPredicate([&](const DoorState& ds) {
				return ds.DoorInfo == Door;
			});
			if (itemIndex == INDEX_NONE)
			{
				//没有找到
				if (Door->GetType() == OT_SecurityDoor)
				{
					//安全门（入户门）优先级为0
					DoorStateArray.Add(DoorState(Door, 0, false));
				}
				else if (Door->GetType() == OT_Door || Door->GetType() == OT_SlidingDoor)
				{
					//标准门和推拉门优先级为99
					DoorStateArray.Add(DoorState(Door, 99, false));
				}
			}
		}

		//获取单个房间的各种门的数量
		int32 DoorNum = 0;
		for (TSharedPtr<FArmyHardware> Door : DoorArr)
		{
			if (Door->GetType() == OT_SecurityDoor)
			{
				//安全门（入户门）
				DoorNum = 0;
				break;
			}
			else if (Door->GetType() == OT_Door || Door->GetType() == OT_SlidingDoor)
			{
				//标准门和推拉门
				DoorNum++;
			}
		}
		RoomEntityDoorInfo.Add(DoorNum, RoomIt);
	}
	RoomEntityDoorInfo.KeySort(TLess<int32>());//排序

	//按房间门数量从少到多的顺序进行开关的布置
	for (auto& MapRoomIt : RoomEntityDoorInfo)
	{
		ArrangeSwitchByRoomType(MapRoomIt.Value);
	}
	//结束

	GGI->Window->AlwaysHideThrobber();

	return true;
}

void FArmyAutoDesignPoint::ArrangeSwitchByRoomType(TSharedPtr<FArmyRoomEntity> RoomIt)
{
	switch (RoomIt->GetAiRoomType())
	{
	case RT_NoName:
		break;
	case RT_Bedroom_Master://主卧/次卧/书房/老人房/儿童房
	case RT_Bedroom_Second:
	case RT_StudyRoom:
		PlaceSwitch(RoomIt);
		break;
	case RT_Balcony://阳台
	case RT_Balcony_Kitchen://厨房阳台
		PlaceSwitchOutWall(RoomIt);
		break;
	case RT_Kitchen://厨房
		PlaceSwitch(RoomIt);
		break;
	case RT_Hallway://玄关
		PlaceSwitch(RoomIt);
		break;
	case RT_Bathroom://卫生间
		PlaceSwitchToilet(RoomIt);
		break;
	case RT_LivingRoom://客厅
	case RT_DiningRoom://餐厅
		break;
	default:
		break;
	}
}

//常规房间布置开关
bool FArmyAutoDesignPoint::PlaceSwitch(TSharedPtr<FArmyRoomEntity> InRoomTable)
{
	TSharedPtr<FArmyHardware> FirstDoor = GetFirstPriorityDoorInRoom(InRoomTable);
	if (FirstDoor.IsValid() && IsExistLight(InRoomTable))
	{
		EAutoDesignModelID ModelType = GetModelTypeByLightAndBedNum(GetLightTypeNum(InRoomTable), IsExistBed(InRoomTable));
		if (!bSecurityPlaceSuccess && FirstDoor->GetType() == OT_SecurityDoor)
		{
			//安全门
			PlaceSwitchInDoor(FirstDoor, InRoomTable, ADM_Switch2O1_Point, true);
			SetDoorUsedAndOtherPrority(FirstDoor);
			bSecurityPlaceSuccess = true;
		}
		else if (FirstDoor->GetType() == OT_Door || FirstDoor->GetType() == OT_SlidingDoor)
		{
			//标准门或者推拉门
			PlaceSwitchInDoor(FirstDoor, InRoomTable, ModelType, true);
			PlaceSwitchInBed(FirstDoor, InRoomTable, ModelType, true);
			SetDoorUsedAndOtherPrority(FirstDoor);
		}
		return true;
	}
	return false;
}

//特殊房间（卫生间）布置开关
bool FArmyAutoDesignPoint::PlaceSwitchToilet(TSharedPtr<FArmyRoomEntity> InRoomTable)
{
	TSharedPtr<FArmyHardware> FirstDoor = GetFirstPriorityDoorInRoom(InRoomTable);
	if (FirstDoor.IsValid())
	{
		//非入户门（非安全门）,才进行开关的布置
		if (FirstDoor->GetType() == OT_Door || FirstDoor->GetType() == OT_SlidingDoor)
		{
			if (InRoomTable->IsExistObj(/*AI_300_600_301_Bath_Heater*/AI_LampBathHeater))
			{
				//放置浴霸开关，门内
				PlaceSwitchInDoor(FirstDoor, InRoomTable, ADM_Switch_BathHeater, false);
			}
			//放置灯、排风开关，门外
			PlaceSwitchOutWall(InRoomTable);
			SetDoorUsedAndOtherPrority(FirstDoor);
			return true;
		}
	}
	return false;
}

//特殊房间（阳台和卫生间），外墙布置开关
bool FArmyAutoDesignPoint::PlaceSwitchOutWall(TSharedPtr<FArmyRoomEntity> InRoomTable)
{
	TSharedPtr<FArmyHardware> FirstDoor = GetFirstPriorityDoorInRoom(InRoomTable);
	if (FirstDoor.IsValid() && IsExistLight(InRoomTable))
	{
		EAutoDesignModelID ModelType = GetModelTypeByLightAndBedNum(GetLightTypeNum(InRoomTable), false);
		//阳台和卫生间一般不存在防盗门，如果认为放置防盗门，则不布置开关
		if (FirstDoor->GetType() == OT_Door || FirstDoor->GetType() == OT_SlidingDoor)
		{
			PlaceSwitchOutDoor(FirstDoor, InRoomTable, ModelType, true);
			SetDoorUsedAndOtherPrority(FirstDoor);
			return true;
		}
	}
	return false;
}

//以下为真实的开关布置算法
//布置门内的开关
bool FArmyAutoDesignPoint::PlaceSwitchInDoor(const TSharedPtr<FArmyHardware> & InDoor, const TSharedPtr<FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights)
{
	//找到门所在的墙体
	TSharedPtr<FArmyWallLine> DoorWallLine = FArmyDataTools::GetWallLineByHardware(InDoor, InRoomTable->GetRoom());
	if (!DoorWallLine.IsValid())
	{
		return false;
	}
	//得到距离门的最近墙角点
	TArray<TSharedPtr<FArmyEditPoint>> RoomPoints = InRoomTable->GetRoom()->GetPoints();
	float MinDis = FLT_MAX;
	TSharedPtr<FArmyEditPoint> DoorMinPoint;
	for (TSharedPtr<FArmyEditPoint> PointIt : RoomPoints)
	{
		float Temp = FVector::Distance(PointIt->GetPos(), InDoor->GetStartPos());
		if (Temp < MinDis)
		{
			MinDis = Temp;
			DoorMinPoint = PointIt;
		}
	}
	if (!DoorMinPoint.IsValid())
	{
		return false;
	}
	//
	//创建双联单控开关
	AActor* SwitchActor = AutoDesignPointManager->CreateActor(InModelID);
	if (SwitchActor)
	{
		FVector NearestPos;
		FVector ModelPlaceDir = FVector::ZeroVector;
		float ResultMinDis = FLT_MAX;
		TSharedPtr<FArmyWallLine> WallLineV;
		TArray<TSharedPtr<FArmyWallLine>> WallLines = InRoomTable->GetRoom()->GetWallLines();
		for (TSharedPtr<FArmyWallLine> LineIt : WallLines)
		{
			if (LineIt != DoorWallLine && (LineIt->GetCoreLine()->GetStart() == DoorMinPoint->GetPos() || LineIt->GetCoreLine()->GetEnd() == DoorMinPoint->GetPos()))
			{
				if (LineIt->GetCoreLine()->GetStart() == DoorMinPoint->GetPos())
				{
					NearestPos = LineIt->GetCoreLine()->GetStart();
					ModelPlaceDir = (LineIt->GetCoreLine()->GetEnd() - LineIt->GetCoreLine()->GetStart()).GetSafeNormal();
				}
				else
				{
					NearestPos = LineIt->GetCoreLine()->GetEnd();
					ModelPlaceDir = (LineIt->GetCoreLine()->GetStart() - LineIt->GetCoreLine()->GetEnd()).GetSafeNormal();
				}
				//起点在墙线上的投影点
				FVector DoorOpenStart = FArmyMath::GetProjectionPoint(InDoor->GetStartPos(), DoorWallLine->GetCoreLine()->GetStart(), DoorWallLine->GetCoreLine()->GetEnd());
				ResultMinDis = FVector::Distance(DoorOpenStart, NearestPos);
				WallLineV = LineIt;
				break;
			}
		}
		//
		FBox SwitchBox = GVC->GetActorCollisionBox(SwitchActor);
		float SwitchLength = SwitchBox.GetSize().X;
		//模型是否到墙角，需要加上模型长度的一半
		if (ResultMinDis > (Switch_AI_Door_DisModelH + SwitchLength / 2))
		{
			//模型放置到当前墙上
			WallLineV = DoorWallLine;
			//门的开启方向起点，可简单理解为门锁芯所在位置，靠近内墙一侧
			FVector DoorOpenStart = FArmyMath::GetProjectionPoint(InDoor->GetStartPos(), WallLineV->GetCoreLine()->GetStart(), WallLineV->GetCoreLine()->GetEnd());
			//开关放置方向
			FVector SwitchPlaceDir = (InDoor->GetStartPos() - InDoor->GetEndPos()).GetSafeNormal();
			NearestPos = DoorOpenStart;
			ModelPlaceDir = SwitchPlaceDir;
		}
		FVector ResultPos = NearestPos + ModelPlaceDir * Switch_AI_Door_DisModelH + FVector(0, 0, Switch_AI_Door_DisFloor);
		FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(InRoomTable->GetRoom(), WallLineV);
		SwitchActor->SetActorLocation(ResultPos);
		SwitchActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
		if (isLinkLights)
		{
			//布置灯控
			PlaceSwitchConnectCouplet(SwitchActor, InModelID, InRoomTable);
		}
		//当前模型放置位置存在其他模型
		TSet<AActor*> IgnoreActor;
		if (FArmyDataTools::CalModelIntersectWithOtherModel(SwitchActor, InRoomTable, IgnoreActor) || CheckWallCornerCollision(SwitchActor))
		{
			WallLineV = DoorWallLine;
			//门的开启方向起点，可简单理解为门锁芯所在位置，靠近内墙一侧
			FVector DoorOpenStart = FArmyMath::GetProjectionPoint(InDoor->GetEndPos(), WallLineV->GetCoreLine()->GetStart(), WallLineV->GetCoreLine()->GetEnd());
			//开关放置方向
			FVector SwitchPlaceDir = (InDoor->GetEndPos() - InDoor->GetStartPos()).GetSafeNormal();
			NearestPos = DoorOpenStart;
			ModelPlaceDir = SwitchPlaceDir;

			FVector SwitchResultPos = NearestPos + ModelPlaceDir * Switch_AI_Door_DisModelH + FVector(0, 0, Switch_AI_Door_DisFloor);

			FVector SwitchNormalInRoom = FArmyDataTools::GetWallNormalInRoom(InRoomTable->GetRoom(), WallLineV);
			SwitchActor->SetActorLocation(SwitchResultPos);
			SwitchActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, SwitchNormalInRoom));
			if (isLinkLights)
			{
				//布置灯控
				PlaceSwitchConnectCouplet(SwitchActor, InModelID, InRoomTable);
			}

			if (FArmyDataTools::CalModelIntersectWithOtherModel(SwitchActor, InRoomTable, IgnoreActor) || CheckWallCornerCollision(SwitchActor))
			{
				//此处表示，门的开启方向和背方向，以及垂直墙体都不满足条件，不再布置开关
				SwitchActor->Destroy();
				//点位（门内开关）模型布置失败（没有满足的布置要求的点位）
				AddFailInfoDesign(InRoomTable, InDoor->GetType(), InModelID);
				return false;
			}

			return true;
		}
	}
	else
	{
		//点位（门内开关）模型创建失败（模型或者构件下载失败）
		AddFailInfoDownload(InRoomTable, InDoor->GetType(), InModelID);
	}

	return false;
}

//布置门外的开关
bool FArmyAutoDesignPoint::PlaceSwitchOutDoor(const TSharedPtr<FArmyHardware> & InDoor, const TSharedPtr<FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights)
{
	//找到门所在的墙体
	TSharedPtr<FArmyWallLine> DoorWallLine = FArmyDataTools::GetWallLineByHardware(InDoor, InRoomTable->GetRoom());
	if (!DoorWallLine.IsValid())
	{
		return false;
	}
	//内线法向量
	FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(InRoomTable->GetRoom(), DoorWallLine);
	//
	AActor* SwitchActor = AutoDesignPointManager->CreateActor(InModelID);
	if (SwitchActor)
	{
		FBox ActorBox = GVC->GetActorCollisionBox(SwitchActor);
		FVector ActorSize = ActorBox.GetSize();
		//外墙线
		TSharedPtr<FArmyLine> OutLine;
		if (DoorWallLine->GetCoreLine()->GetStart() == InDoor->LinkFirstLine->GetStart() && DoorWallLine->GetCoreLine()->GetEnd() == InDoor->LinkFirstLine->GetEnd())
		{
			OutLine = InDoor->LinkSecondLine;
		}
		else
		{
			OutLine = InDoor->LinkFirstLine;
		}
		//起点在外墙线上的投影点
		FVector DoorOpenStartOut = FArmyMath::GetProjectionPoint(InDoor->GetStartPos(), OutLine->GetStart(), OutLine->GetEnd());
		FVector DoorOpenEndOut = FArmyMath::GetProjectionPoint(InDoor->GetEndPos(), OutLine->GetStart(), OutLine->GetEnd());

		FVector DoorOpenDirTemp = (DoorOpenStartOut - DoorOpenEndOut).GetSafeNormal();
		FVector SwitchPos = DoorOpenStartOut + DoorOpenDirTemp * (Switch_AI_Door_DisModelH + ActorSize.X / 2);
		bool bPointInLineOpen = FArmyMath::IsPointOnLine(SwitchPos, OutLine->GetStart(), OutLine->GetEnd());

		//得到门关联的另一个房间（一个门有且仅有两个关联房间）
		TArray<TSharedPtr<FArmyRoom>> Rooms = FArmyDataTools::GetRoomsByHardware(InDoor);
		TSharedPtr<FArmyRoom> Room2 = InRoomTable->GetRoom() == Rooms[0] ? Rooms[1] : Rooms[0];
		TSharedPtr<FArmyRoomEntity> RoomEntity2;
		for (TSharedPtr<FArmyRoomEntity> RoomEnIt : RoomTableArr)
		{
			if (RoomEnIt->GetRoom() == Room2)
			{
				RoomEntity2 = RoomEnIt;
				break;
			}
		}
		bool bPlaceSuccess = false;
		if (bPointInLineOpen)
		{
			FVector ResultPos = SwitchPos + FVector(0, 0, Switch_AI_Door_DisFloor);
			FVector SwitchOutNormalInRoom = FVector::CrossProduct(DoorOpenDirTemp, FVector(0, 0, 10)/*ResultPos - SwitchPos*/).GetSafeNormal();
			FVector CheckPos = InDoor->GetPos() + (FVector::Distance(DoorOpenStartOut, InDoor->GetStartPos()) + 2) * SwitchOutNormalInRoom;
			if (InRoomTable->GetRoom()->IsPointInRoom(CheckPos))
			{
				SwitchOutNormalInRoom = -SwitchOutNormalInRoom;
			}

			SwitchActor->SetActorLocation(ResultPos);
			SwitchActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, SwitchOutNormalInRoom));
			if (isLinkLights)
			{
				//布置灯控
				PlaceSwitchConnectCouplet(SwitchActor, InModelID, InRoomTable);
			}

			TSet<AActor*> IgnoreActor;
			if (!FArmyDataTools::CalModelIntersectWithOtherModel(SwitchActor, RoomEntity2, IgnoreActor) && !CheckWallCornerCollision(SwitchActor))
			{
				bPlaceSuccess = true;
			}
		}

		if (!bPlaceSuccess)
		{

			FVector SwitchPosReverse = DoorOpenEndOut + (-DoorOpenDirTemp) * (Switch_AI_Door_DisModelH + ActorSize.X / 2);
			bool bPointInLineOpenReverse = FArmyMath::IsPointOnLine(SwitchPosReverse, OutLine->GetStart(), OutLine->GetEnd());
			if (bPointInLineOpenReverse)
			{
				FVector ResultPosRev = SwitchPosReverse + FVector(0, 0, Switch_AI_Door_DisFloor);
				FVector NormalInRoomRev = FVector::CrossProduct(DoorOpenDirTemp, FVector(0, 0, 10)/*ResultPosRev - SwitchPosReverse*/).GetSafeNormal();
				FVector CheckPos = InDoor->GetPos() + (FVector::Distance(DoorOpenStartOut, InDoor->GetStartPos()) + 2) * NormalInRoomRev;
				if (InRoomTable->GetRoom()->IsPointInRoom(CheckPos))
				{
					NormalInRoomRev = -NormalInRoomRev;
				}
				SwitchActor->SetActorLocation(ResultPosRev);
				SwitchActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoomRev));

				if (isLinkLights)
				{
					//布置灯控
					PlaceSwitchConnectCouplet(SwitchActor, InModelID, InRoomTable);
				}

				TSet<AActor*>  IgnoreActorArr;
				if (!FArmyDataTools::CalModelIntersectWithOtherModel(SwitchActor, RoomEntity2, IgnoreActorArr) && !CheckWallCornerCollision(SwitchActor))
				{
					bPlaceSuccess = true;
				}
			}
		}
		if (!bPlaceSuccess)
		{
			//此处表示，门的外墙两侧都不满足条件，不再布置开关
			SwitchActor->Destroy();
			//点位（门外开关）模型布置失败（没有满足的布置要求的点位）
			AddFailInfoDesign(InRoomTable, InDoor->GetType(), InModelID);
			return false;
		}
	}
	else
	{
		//点位（门外开关）模型创建失败（模型或者构件下载失败）
		AddFailInfoDownload(InRoomTable, InDoor->GetType(), InModelID);
	}

	return false;
}

//布置卧室带床的开关
bool FArmyAutoDesignPoint::PlaceSwitchInBed(const TSharedPtr<FArmyHardware> & InDoor, const TSharedPtr<FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights)
{
	if (IsExistBed(InRoomTable))
	{
		TArray<TSharedPtr<FArmyModelEntity>> BedArrSingle;
		TArray<TSharedPtr<FArmyModelEntity>> BedArrDouble;
		InRoomTable->GetModelEntityArrayByComponentId(AI_SingleBed, BedArrSingle);
		InRoomTable->GetModelEntityArrayByComponentId(AI_DoubleBed, BedArrDouble);
		TArray<TSharedPtr<FArmyModelEntity>> BedArr;
		BedArr.Append(BedArrSingle);
		BedArr.Append(BedArrDouble);
		//
		for (TSharedPtr<FArmyModelEntity>& ActorItemIt : BedArr)
		{
			FBox ActorBox = GVC->GetActorCollisionBox(ActorItemIt->Actor);
			FVector Size = ActorBox.GetSize();
			//
			FVector OutPosLeft;
			FVector OutPosRight;
			FArmyDataTools::PointIntervalGroundOnLeftOfBackProjPoint(*ActorItemIt->Actor, (Size.X / 2) + Switch_AI_Bed_DisModelH, Switch_AI_Bed_DisFloor, OutPosLeft);
			FArmyDataTools::PointIntervalGroundOnRightOfBackProjPoint(*ActorItemIt->Actor, (Size.X / 2) + Switch_AI_Bed_DisModelH, Switch_AI_Bed_DisFloor, OutPosRight);
			float LeftDis = FVector::Distance(InDoor->GetPos(), OutPosLeft);
			float RightDis = FVector::Distance(InDoor->GetPos(), OutPosRight);
			//
			TSharedPtr<FArmyWallLine> BedBackWall = FArmyDataTools::GetWallLineByModelInDir(*ActorItemIt->Actor);
			if (BedBackWall.IsValid())
			{
				FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(InRoomTable->GetRoom(), BedBackWall);
				FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, BedBackWall);
				FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, BedBackWall);
				//
				AActor* SwitchActor = AutoDesignPointManager->CreateActor(InModelID);
				if (SwitchActor)
				{
					FBox ModelBox = GVC->GetActorCollisionBox(SwitchActor);
					FVector ModelSize = ModelBox.GetSize();
					FVector LeftModelSize = ModelSize.X / 2 * LeftNormal;
					FVector RightModelSize = ModelSize.X / 2 * RightNormal;
					//
					FVector ResultPos;
					if (LeftDis >= RightDis)//放置在距离门最远侧床头
					{
						if (InRoomTable->GetRoom()->IsPointInRoom(OutPosLeft + LeftModelSize))//检测放置位置是否在房间内
						{
							ResultPos = OutPosLeft;
						}
						else if (InRoomTable->GetRoom()->IsPointInRoom(OutPosRight + RightModelSize))//放置在床的另一侧
						{
							ResultPos = OutPosRight;
						}
					}
					else if (LeftDis < RightDis)
					{
						if (InRoomTable->GetRoom()->IsPointInRoom(OutPosRight + RightModelSize))
						{
							ResultPos = OutPosRight;
						}
						else if (InRoomTable->GetRoom()->IsPointInRoom(OutPosLeft + LeftModelSize))
						{
							ResultPos = OutPosLeft;
						}
					}
					//
					bool bPlaceSuccess = false;
					TSet<AActor*> IgnoreActor;
					if (!FArmyDataTools::CalModelIntersectWithOtherModel(SwitchActor, InRoomTable, IgnoreActor) && !CheckWallCornerCollision(SwitchActor))
					{
						bPlaceSuccess = true;
					}
					if (bPlaceSuccess)
					{
						SwitchActor->SetActorLocation(ResultPos);
						SwitchActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
						if (isLinkLights)
						{
							//布置灯控
							PlaceSwitchConnectCouplet(SwitchActor, InModelID, InRoomTable);
						}
					}
					else
					{
						SwitchActor->Destroy();
						//点位（床头开关）模型布置失败（没有满足的布置要求的点位）
						AddFailInfoDesign(InRoomTable, ActorItemIt->ComponentId, InModelID);
						return false;
					}
				}
				else
				{
					//点位（床头开关）模型创建失败（模型或者构件下载失败）
					AddFailInfoDownload(InRoomTable, ActorItemIt->ComponentId, InModelID);
				}

				return true;
			}
		}
	}
	return false;
}

//布置浴霸的开关
bool FArmyAutoDesignPoint::PlaceBathHeaterSwitch(TSharedPtr<FArmyHardware> InDoor, TSharedPtr<FArmyRoomEntity> InRoomTable)
{
	TSharedPtr<FArmyWallLine> DoorWallLine = FArmyDataTools::GetWallLineByHardware(InDoor, InRoomTable->GetRoom());
	if (!DoorWallLine.IsValid())
	{
		return false;
	}
	FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(InRoomTable->GetRoom(), DoorWallLine);
	//
	//起点
	FVector DoorStart = FArmyMath::GetProjectionPoint(InDoor->GetStartPos(), DoorWallLine->GetCoreLine()->GetStart(), DoorWallLine->GetCoreLine()->GetEnd());
	//方向
	FVector SwitchDir = (InDoor->GetStartPos() - InDoor->GetEndPos()).GetSafeNormal();
	//放置位置
	FVector ResultPos = DoorStart + SwitchDir * Switch_AI_Door_DisModelH;
	ResultPos.Z = Switch_AI_Door_DisFloor;
	//
	AActor* AutoSwitchBathHeater = CreateAutoSwitchByBathHeaterType(InRoomTable);
	if (AutoSwitchBathHeater)
	{
		//设置浴霸开关的位置
		AutoSwitchBathHeater->SetActorLocation(ResultPos);
		AutoSwitchBathHeater->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));

		return true;
	}
	else
	{
		//点位（浴霸开关）模型创建失败（模型或者构件下载失败）
		AddFailInfoDownload(InRoomTable, InDoor->GetType(), ADM_Switch_BathHeater);
	}

	return false;
}

//浴霸的开关创建
AActor* FArmyAutoDesignPoint::CreateAutoSwitchByBathHeaterType(TSharedPtr<FArmyRoomEntity> InRoomTable)
{
	AActor* BathHeaterAutoSwitch = nullptr;
	if (InRoomTable->IsExistObj(AI_300_600_301_Bath_Heater))
	{
		BathHeaterAutoSwitch = AutoDesignPointManager->CreateActor(ADM_Switch_BathHeater);
	}

	if (InRoomTable->IsExistObj(AI_300_300_301_Bath_Heater))
	{
		BathHeaterAutoSwitch = AutoDesignPointManager->CreateActor(ADM_Switch_BathHeater);
	}

	return BathHeaterAutoSwitch;
}

// 布置灯控开始
TArray<TArray<TSharedPtr<FArmyModelEntity>>> FArmyAutoDesignPoint::GetLightCategoryArrInRoom(TSharedPtr<FArmyRoomEntity> InRoom)
{
	//主灯
	TArray<TSharedPtr<FArmyModelEntity>> LampMainArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampMain, LampMainArr);
	//射灯
	TArray<TSharedPtr<FArmyModelEntity>> LampRefArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampReflector, LampRefArr);
	//筒灯和防雾筒灯
	TArray<TSharedPtr<FArmyModelEntity>> LampTubeArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampTube, LampTubeArr);
	TArray<TSharedPtr<FArmyModelEntity>> LampTubeFogArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampTubeFog, LampTubeFogArr);
	if (LampTubeFogArr.Num() > 0)
	{
		LampTubeArr.Append(LampTubeFogArr);
	}
	//壁灯和地灯
	TArray<TSharedPtr<FArmyModelEntity>> LampWallArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampWall, LampWallArr);
	TArray<TSharedPtr<FArmyModelEntity>> LampFloorArr;
	InRoom->GetModelEntityArrayByComponentId(EAIComponentCode::AI_LampFloor, LampFloorArr);
	if (LampFloorArr.Num() > 0)
	{
		LampWallArr.Append(LampFloorArr);
	}
	//
	TArray<TArray<TSharedPtr<FArmyModelEntity>>> LampGroupArr;
	if (LampMainArr.Num() > 0)
	{
		LampGroupArr.Add(LampMainArr);
	}
	if (LampRefArr.Num() > 0)
	{
		LampGroupArr.Add(LampRefArr);
	}
	if (LampTubeArr.Num() > 0)
	{
		LampGroupArr.Add(LampTubeArr);
	}
	if (LampWallArr.Num() > 0)
	{
		LampGroupArr.Add(LampWallArr);
	}
	return LampGroupArr;
}

bool FArmyAutoDesignPoint::PlaceSwitchConnect(TSharedPtr<FArmyFurniture> SwitchFurniture, TSharedPtr<FArmyRoomEntity> InRoom)
{
	//
	TArray<TArray<TSharedPtr<FArmyModelEntity>>> LampGroupArr = GetLightCategoryArrInRoom(InRoom);
	for (int i = 0; i < SwitchFurniture->SwitchCoupletNum && i < LampGroupArr.Num(); ++i)
	{
		TArray<FGuid> EngagedLightArr;
		SwitchFurniture->RelatedFurnitureMap.MultiFind(i + 1, EngagedLightArr);
		TArray<TSharedPtr<FArmyModelEntity>> LampArr = LampGroupArr[i];
		for (auto& LightEntity : LampArr)
		{
			AActor* LightActor = LightEntity->Actor;
			if (LightActor->IsValidLowLevel())
			{
				AXRActor* LightOwner = Cast<AXRActor>(LightActor->GetOwner());
				if (LightOwner->IsValidLowLevel()) {
					TWeakPtr<FArmyObject> LightObject = LightOwner->GetRelevanceObject();
					if (LightObject.IsValid() && !EngagedLightArr.Contains(LightObject.Pin()->GetUniqueID()))
					{
						SwitchFurniture->RelatedFurnitureMap.Add(i + 1, LightObject.Pin()->GetUniqueID());
					}
				}
			}
		}
	}
	return true;
}

bool FArmyAutoDesignPoint::PlaceSwitchConnectCouplet(AActor* SwitchActor, EAutoDesignModelID InModelID, TSharedPtr<FArmyRoomEntity> InRoom)
{
	//布置灯控
	TSharedPtr<FArmyFurniture> SwitchFurniture;
	AXRFurnitureActor* FurnitureActor = Cast<AXRFurnitureActor>(SwitchActor->GetOwner());
	if (FurnitureActor && FurnitureActor->IsValidLowLevel()) {
		TSharedPtr<FArmyFurniture> SwitchFurnitureTemp = StaticCastSharedPtr<FArmyFurniture>(FurnitureActor->GetRelevanceObject().Pin());
		if (SwitchFurnitureTemp.IsValid() && SwitchFurnitureTemp->IsSwitch()) {
			SwitchFurniture = SwitchFurnitureTemp;
		}
	}
	if (SwitchFurniture.IsValid())
	{
		switch (InModelID)
		{
		case ADM_Switch1O1_Point:
		case ADM_Switch1O2_Point:
		{
			SwitchFurniture->SwitchCoupletNum = 1;
			PlaceSwitchConnect(SwitchFurniture, InRoom);
			break;
		}
		case ADM_Switch2O1_Point:
		case ADM_Switch2O2_Point:
		{
			SwitchFurniture->SwitchCoupletNum = 2;
			PlaceSwitchConnect(SwitchFurniture, InRoom);
			break;
		}
		case ADM_Switch3O1_Point:
		case ADM_Switch3O2_Point:
		{
			SwitchFurniture->SwitchCoupletNum = 3;
			PlaceSwitchConnect(SwitchFurniture, InRoom);
			break;
		}
		case ADM_Switch4O1_Point:
		case ADM_Switch4O2_Point:
		{
			SwitchFurniture->SwitchCoupletNum = 4;
			PlaceSwitchConnect(SwitchFurniture, InRoom);
			break;
		}
		default:
			break;
		}
	}
	return true;
}
// 布置灯控结束

bool FArmyAutoDesignPoint::IsExistBed(TSharedPtr<class FArmyRoomEntity> InRoomTable)
{
	if (InRoomTable->IsExistObj(AI_SingleBed) || InRoomTable->IsExistObj(AI_DoubleBed))
	{
		return true;
	}
	return false;
}

bool FArmyAutoDesignPoint::IsExistLight(TSharedPtr<class FArmyRoomEntity> InRoomTable)
{
	if (InRoomTable->IsExistObj(AI_LampFloor)
		|| InRoomTable->IsExistObj(AI_LampWall)
		|| InRoomTable->IsExistObj(AI_LampTube)
		|| InRoomTable->IsExistObj(AI_LampTubeFog)
		|| InRoomTable->IsExistObj(AI_LampReflector)
		|| InRoomTable->IsExistObj(AI_LampMain)
		)
	{
		return true;
	}
	return false;
}

int32 FArmyAutoDesignPoint::GetLightTypeNum(TSharedPtr<class FArmyRoomEntity> InRoomTable)
{
	//获取单个房间，灯的种类
	int32 num = 0;
	if (InRoomTable->IsExistObj(AI_LampMain))
	{
		num++;
	}
	if (InRoomTable->IsExistObj(AI_LampReflector))
	{
		num++;
	}
	if (InRoomTable->IsExistObj(AI_LampTube) || InRoomTable->IsExistObj(AI_LampTubeFog))
	{
		num++;
	}
	if (InRoomTable->IsExistObj(AI_LampWall) || InRoomTable->IsExistObj(AI_LampFloor))
	{
		num++;
	}

	//特殊处理，如果是卫生间并且包含排风扇则需要开关多加一联
	if (InRoomTable->GetAiRoomType() == RT_Bathroom && InRoomTable->IsExistObj(AI_ExhaustFan))
	{
		num += 1;
	}
	//特殊处理结束

	return num;
}

EAutoDesignModelID FArmyAutoDesignPoint::GetModelTypeByLightAndBedNum(int32 InLightNum, bool InIsExistBed)
{
	switch (InLightNum)
	{
	case 1:
	{
		return InIsExistBed ? ADM_Switch1O2_Point : ADM_Switch1O1_Point;
	}
	case 2:
	{
		return InIsExistBed ? ADM_Switch2O2_Point : ADM_Switch2O1_Point;
	}
	case 3:
	{
		return InIsExistBed ? ADM_Switch3O2_Point : ADM_Switch3O1_Point;
	}
	case 4:
	{
		return InIsExistBed ? ADM_Switch4O2_Point : ADM_Switch4O1_Point;
	}
	default:
		break;
	}

	return EAutoDesignModelID::ADM_None;
}

TSharedPtr<FArmyHardware> FArmyAutoDesignPoint::GetFirstPriorityDoorInRoom(TSharedPtr<FArmyRoomEntity> RoomIt)
{
	//过滤符合要求的房间内所有的门和窗
	TArray<TSharedPtr<FArmyHardware>> InHomeAllDoorAndWin = RoomIt->GetDoorsActor();
	TArray<TSharedPtr<FArmyHardware>> InHomeAllDoor;
	for (TSharedPtr<FArmyHardware>& HardwareIt : InHomeAllDoorAndWin)
	{
		if (HardwareIt->GetType() == EObjectType::OT_Door
			|| HardwareIt->GetType() == EObjectType::OT_SecurityDoor
			|| HardwareIt->GetType() == EObjectType::OT_SlidingDoor)
		{
			InHomeAllDoor.Add(HardwareIt);
		}
	}

	//Map，存储门的优先级，与门的地址
	TMultiMap<int32, TSharedPtr<FArmyHardware>> DoorInfoMap;
	for (auto& InHomeAllDoorIt : InHomeAllDoor)
	{
		for (auto& DoorStateIt : DoorStateArray)
		{
			if (DoorStateIt.DoorInfo == InHomeAllDoorIt && DoorStateIt.bUsed == false)
			{
				DoorInfoMap.Add(DoorStateIt.DoorPriority, InHomeAllDoorIt);
			}
		}
	}
	DoorInfoMap.KeySort(TLess<int32>());//排序

	TSharedPtr<FArmyHardware> FirstPriorityDoor;
	for (auto &DoorInfoMapIt : DoorInfoMap)
	{
		FirstPriorityDoor = DoorInfoMapIt.Value;
		break;
	}

	return FirstPriorityDoor;
}

void FArmyAutoDesignPoint::SetDoorUsedAndOtherPrority(TSharedPtr<FArmyHardware> FirstDoor)
{
	for (auto & DoorStateIt : DoorStateArray)
	{
		if (DoorStateIt.DoorInfo == FirstDoor)
		{
			DoorStateIt.bUsed = true;
		}
		else
		{
			DoorStateIt.DoorPriority--;
		}
	}
}

//自动布置插座
bool FArmyAutoDesignPoint::RunSocketAutoDesign()
{
	//洗衣机布置插座
	PlaceWaterPointOrSocketInWashing(EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A);
	//洗衣机布置插座结束
	
	//对所有房间布置插座
	for (TSharedPtr<FArmyRoomEntity> & RoomIt : RoomTableArr)
	{
		//书桌布置插座
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Desk))
		{
			TArray<TSharedPtr<FArmyModelEntity> >ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Desk, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor * NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);
					//书桌背立面
					FVector BackwardPosBack;
					FVector BackwardPosLeft;
					FVector BackwardPosRight;
					FVector ForwardPos;
					

					//计算指定方向、指定离地高度的位置
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Desk_DisFloor, BackwardPosBack, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Desk_DisFloor, BackwardPosLeft,FArmyDataTools::AI_DT_MODEL_RIGHT);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Desk_DisFloor, BackwardPosRight, FArmyDataTools::AI_DT_MODEL_LEFT);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Desk_DisFloor, ForwardPos, FArmyDataTools::AI_DT_MODEL_FORWARD);

					//static bool PointIntervalGroundOnDirProjPoint(const AActor&SrcModel, const float DistToGround, FVector&Out, EDirection InDir = AI_DT_MODEL_BACKWARD);
					//得到指定方向墙线
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					TSharedPtr<FArmyWallLine> WallLineLeft = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_RIGHT);
					TSharedPtr<FArmyWallLine> WallLineRight = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_LEFT);
					TSharedPtr<FArmyWallLine> WallLineForward = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_FORWARD);
					
					if (!WallLineBack.IsValid() && !WallLineLeft.IsValid() && !WallLineRight.IsValid() &!WallLineForward.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					
					float MinDisBack = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineBack);
					float MinDisLeft = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineLeft);
					float MinDisRight = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineRight);
					float MinDisForward = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineForward);

					FVector ResultNormal;
					FVector ResultPos;
					bool bPlaceSuccess = false;
					TSet<AActor *> ActorSet;
					ActorSet.Add(ActorItem->Actor);
					float BackOrForwardMinDis = MinDisBack < MinDisForward ? MinDisBack : MinDisForward;
					bool  IsBackOrForward = MinDisBack <= MinDisForward ? true : false;
					if (BackOrForwardMinDis < Socket_AI_Desk_DisWall)//检测背立面是否满足
					{
						if (IsBackOrForward)
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);
							ResultPos = BackwardPosBack;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet)||CheckWallCornerCollision(NewActor)) ? false : true;
						}
						else
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineForward);
							ResultPos = ForwardPos;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
						}
					}
					else
					{
						bPlaceSuccess = false;
					}
					
					if (!bPlaceSuccess)//检测背面和前面哪一侧离餐桌近，因为模型是对称的，默认的背立面可能在放置过程中被设为前面
					{
						if (MinDisLeft < Socket_AI_Desk_DisWall)
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineLeft);
							ResultPos = BackwardPosLeft;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
						}
						else
						{
							bPlaceSuccess = false;
						}
					}

					if (!bPlaceSuccess)//检测右侧是否满足
					{
						if (MinDisRight < Socket_AI_Desk_DisWall)
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineRight);
							ResultPos = BackwardPosRight;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
						}
						else
						{
							bPlaceSuccess = false;
						}
					}

					if (!bPlaceSuccess)
					{
						NewActor->Destroy();
						//点位（书桌插座）模型布置失败
						AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
						continue;
					}
				}
				else
				{
					//点位（书桌插座）模型创建失败（模型或者构件下载失败）
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}

		//长方型餐桌布置插座AI_DiningTable
		if (RoomIt->IsExistObj(EAIComponentCode::AI_DiningTable))
		{
			TArray<TSharedPtr<FArmyModelEntity> >ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_DiningTable, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor * NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);

					//书桌背立面
					FVector BackwardPosBack;
					FVector BackwardPosLeft;
					FVector BackwardPosRight;
					FVector ForwardPos;

					//计算指定方向、指定离地高度的位置
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_DiningTable_DisFloor, BackwardPosBack, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_DiningTable_DisFloor, BackwardPosLeft, FArmyDataTools::AI_DT_MODEL_RIGHT);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_DiningTable_DisFloor, BackwardPosRight, FArmyDataTools::AI_DT_MODEL_LEFT);
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Desk_DisFloor, ForwardPos, FArmyDataTools::AI_DT_MODEL_FORWARD);

					//得到指定方向墙线
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					TSharedPtr<FArmyWallLine> WallLineLeft = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_RIGHT);
					TSharedPtr<FArmyWallLine> WallLineRight = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_LEFT);
					TSharedPtr<FArmyWallLine> WallLineForward = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_FORWARD);
					if (!WallLineBack.IsValid() && !WallLineLeft.IsValid() && !WallLineRight.IsValid() && WallLineForward.IsValid())
					{
						NewActor->Destroy();
						continue;
					}

					float MinDisBack = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineBack);
					float MinDisLeft = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineLeft);
					float MinDisRight = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineRight);
					float MinDisForward = FArmyDataTools::CalModelFromWallDistance(ActorItem->Actor, WallLineForward);

					FVector ResultNormal;
					FVector ResultPos;
					bool bPlaceSuccess = false;
					TSet<AActor *> ActorSet;
					ActorSet.Add(ActorItem->Actor);
					if (!bPlaceSuccess)//检测左侧是否满足
					{
						if (MinDisLeft < Socket_AI_Desk_DisWall)
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineLeft);
							ResultPos = BackwardPosLeft;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
						}
						else
						{
							bPlaceSuccess = false;
						}
					}

					if (!bPlaceSuccess)//检测右侧是否满足
					{
						if (MinDisRight < Socket_AI_Desk_DisWall)
						{
							ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineRight);
							ResultPos = BackwardPosRight;

							NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
							NewActor->SetActorLocation(ResultPos);
							bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
						}
						else
						{
							bPlaceSuccess = false;
						}
					}

					if (!bPlaceSuccess)
					{
						float BackOrForwardMinDis = MinDisBack < MinDisForward ? MinDisBack : MinDisForward;
						bool  IsBackOrForward = MinDisBack <= MinDisForward ? true : false;
						if (BackOrForwardMinDis < Socket_AI_Desk_DisWall)//检测背面和前面哪一侧离餐桌近，因为模型是对称的，默认的背立面可能在放置过程中被设为前面
						{
							if (IsBackOrForward)
							{
								ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);
								ResultPos = BackwardPosBack;

								NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
								NewActor->SetActorLocation(ResultPos);
								bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
							}
							else
							{
								ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineForward);
								ResultPos = ForwardPos;

								NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
								NewActor->SetActorLocation(ResultPos);
								bPlaceSuccess = (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, ActorSet) || CheckWallCornerCollision(NewActor)) ? false : true;
							}
						}
						else
						{
							bPlaceSuccess = false;
						}



						if (!bPlaceSuccess)
						{
							NewActor->Destroy();
							//点位（餐桌插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
							continue;
						}
					}
				}
				else
				{
					//点位（书桌插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}

		//洗衣机布置插座
		/*if (RoomIt->IsExistObj(EAIComponentCode::AI_WashingMachine))
		{
			TArray<TSharedPtr<FArmyModelEntity> >ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_WashingMachine, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor * NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A);
				if (NewActor)
				{

					FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);

					FVector ModelBoxSize = ModelBox.GetSize();
					FRotator RotationData = ActorItem->Actor->GetActorRotation();
					FVector RegSize = RotationData.RotateVector(ModelBoxSize);

					float LRLocation = Socket_AI_WashingMachine_LorRDis;

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LRLocation, Socket_AI_WashingMachine_DisFloor, *NewActor);


					FVector SocketLocation = NewActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LRLocation * 2)* RightNormal;

					bool bSuccess = false;
					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					//FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_WashingMachine_DisFloor, *NewActor);
					if (FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_WashingMachine_DisFloor, *NewActor)
						&& (!FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) && RoomIt->GetRoom()->IsPointInRoom(LeftModelPos)))
					{
						bSuccess = true;
						//FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_WashingMachine_DisFloor, *NewActor);
					}
					else if (FArmyDataTools::ModelIntervalGroundOnRightOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_WashingMachine_DisFloor, *NewActor)
						&& (!FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) && RoomIt->GetRoom()->IsPointInRoom(RightModelPos)))
					{
						bSuccess = true;
					}

					if (!bSuccess)
					{
						NewActor->Destroy();
					}
				}
				else
				{
					TArray<EAutoDesignModelID> ModelIDArr;
					ModelIDArr.Add(ADM_Socket_Five_Point_10A);
					AddFailInfo(ModelIDArr, RoomIt->GetRoom()->GetSpaceName(), ActorItem->Actor->GetName());
				}
			}
		}*/

		//油烟机布置插座
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Hood))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Hood, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);

					FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);

					FVector ModelBoxSize = ModelBox.GetSize();
					FRotator RotationData = ActorItem->Actor->GetActorRotation();
					FVector RegSize = RotationData.RotateVector(ModelBoxSize);

					float LRLocation = fabs((RegSize.X) / 2) + Socket_AI_Hood_LorRDis;

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LRLocation, Socket_AI_Hood_DisFloor, *NewActor);

					FVector SocketLocation = NewActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LRLocation * 2)* RightNormal;
					TSet<AActor* > IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor,RoomIt,IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewActor)) //检测左侧是否满足
					{
						FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_Hood_DisFloor, *NewActor);
						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(RightModelPos) || CheckWallCornerCollision(NewActor))
						{
							NewActor->Destroy();
							//点位（油烟机插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
							continue;
						}
					}
					
				}
				else
				{
					//点位（油烟机插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}
		
		//梳妆台布置插座
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Dresser))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Dresser, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor *NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, 0, Socket_AI_Dresser_DisFloor, *NewActor);
					
					TSet<AActor* > IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || CheckWallCornerCollision(NewActor))
					{
						NewActor->Destroy();
						//点位（梳妆台插座）模型布置失败
						AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
						continue;
					}
				}
				else
				{
					//点位（梳妆台插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}

		//壁挂电视，电视柜布置插座AI_DiningTable AI_TVWall
		if (RoomIt->IsExistObj(EAIComponentCode::AI_TVWall))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_TVWall, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				FVector SocketLocationInOderToZ;
				FVector SocketLocation;
				FArmyDataTools::PosOnDirectionOfModel(*ActorItem->Actor, Socket_AI_TVWall_DisofBottom, SocketLocationInOderToZ, FArmyDataTools::EDirection::AI_DT_MODEL_UP, 0);
				FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVWall_DisofBottom, SocketLocation);
				SocketLocation.Z = SocketLocationInOderToZ.Z;

				FVector BackwardPosBack;
				TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
				if (!WallLineBack.IsValid())
				{
					continue;
				}
				FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);
				//公用电视背景墙组合插座
				if (AutoDesignPointManager->IsExistAutoDesignModel(ADM_Socket_Living_Couplet))
				{
					AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Living_Couplet);
					if (NewActor)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor, ESF_Normal);

						NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Living_Couplet);
					}
				}
				else if (AutoDesignPointManager->IsExistAutoDesignModel(ADM_Socket_Net_TV_Couplet))
				{

					AActor* NewActor1 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor2 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor3 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Net_TV_Couplet);

					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVBench_DisofTop, SocketLocation);
					SocketLocation.Z = (SocketLocationInOderToZ.Z) + 10;

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor1->Destroy();
						NewActor2->Destroy();
						NewActor3->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					if (NewActor2)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor2, ESF_Normal);

						NewActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor2->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor1)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor1, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor1);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector LeftSocketLocation = SocketLocation + OutLength * LeftNormal;

						NewActor1->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor1->SetActorLocation(LeftSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}
					
					if (NewActor3)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor3, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor3);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal;

						NewActor3->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor3->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Net_TV_Couplet);
					}
				}
				else
				{
					//新增规则，布置五孔插座*2、网络插座*1和电视插座*1
					AActor* NewActor1 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor2 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor3 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Computer);
					AActor* NewActor4 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_TV);

					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVBench_DisofTop, SocketLocation);
					SocketLocation.Z = (SocketLocationInOderToZ.Z) + 10;

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor1->Destroy();
						NewActor2->Destroy();
						NewActor3->Destroy();
						NewActor4->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					if (NewActor2)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor2, ESF_Normal);

						NewActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor2->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor1)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor1, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor1);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector LeftSocketLocation = SocketLocation + OutLength * LeftNormal;

						NewActor1->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor1->SetActorLocation(LeftSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor3)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor3, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor3);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal;

						NewActor3->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor3->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Computer);
					}

					if (NewActor4)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor4, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor4);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal + OutLength * RightNormal;

						NewActor4->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor4->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_TV);
					}
				}
			}
		}

		//只有电视柜，没有电视布置插座 AI_TVBench
		if (RoomIt->IsExistObj(EAIComponentCode::AI_TVBench) && !RoomIt->IsExistObj(EAIComponentCode::AI_TVWall))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_TVBench, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				FVector SocketLocationInOderToZ;
				
				FArmyDataTools::PosOnDirectionOfModel(*ActorItem->Actor, 10, SocketLocationInOderToZ, FArmyDataTools::EDirection::AI_DT_MODEL_UP, 0);
				

				FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);
				float ModelHeight = ModelBox.GetSize().Z;
				

				FVector BackwardPosBack;
				TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
				if (!WallLineBack.IsValid())
				{
					continue;
				}
				FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

				//如果有电视背景墙插座
				if (AutoDesignPointManager->IsExistAutoDesignModel(ADM_Socket_Living_Couplet))
				{
					//公牛电视背景墙组合插座，但是暂时没有模型，用10A五孔插座代替
					AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Living_Couplet);
					if (NewActor)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor, ESF_Normal);

						FVector SocketLocation;
						FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVBench_DisofTop, SocketLocation);
						SocketLocation.Z = (SocketLocationInOderToZ.Z) + ModelHeight;

						NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Living_Couplet);
					}
				}
				//如果没有电视背景墙组合插座，则布置10A五孔*2 + 网络/电视弱电插座
				else if (AutoDesignPointManager->IsExistAutoDesignModel(ADM_Socket_Net_TV_Couplet))
				{
					AActor* NewActor1 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor2 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor3 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Net_TV_Couplet);

					FVector SocketLocation;
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVBench_DisofTop, SocketLocation);
					SocketLocation.Z = (SocketLocationInOderToZ.Z) + ModelHeight;

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor1->Destroy();
						NewActor2->Destroy();
						NewActor3->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);
					
					TArray<EAutoDesignModelID> ModelIDArr;
					if (NewActor2)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor2, ESF_Normal);

						NewActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor2->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor1)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor1, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor1);
						float OutLength = (ModelBoxSocket.GetSize().X ) + 10 ;
						FVector LeftSocketLocation = SocketLocation + OutLength * LeftNormal;

						NewActor1->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor1->SetActorLocation(LeftSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor3)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor3, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor3);
						float OutLength = (ModelBoxSocket.GetSize().X ) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal;

						NewActor3->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor3->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Net_TV_Couplet);
					}
				}
				else
				{
					//新增规则，布置五孔插座*2、网络插座*1和电视插座*1
					AActor* NewActor1 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor2 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
					AActor* NewActor3 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Computer);
					AActor* NewActor4 = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_TV);

					FVector SocketLocation;
					FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_TVBench_DisofTop, SocketLocation);
					SocketLocation.Z = (SocketLocationInOderToZ.Z) + ModelHeight;

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor1->Destroy();
						NewActor2->Destroy();
						NewActor3->Destroy();
						NewActor4->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					TArray<EAutoDesignModelID> ModelIDArr;
					if (NewActor2)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor2, ESF_Normal);

						NewActor2->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor2->SetActorLocation(SocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor1)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor1, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor1);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector LeftSocketLocation = SocketLocation + OutLength * LeftNormal;

						NewActor1->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor1->SetActorLocation(LeftSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}

					if (NewActor3)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor3, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor3);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal;

						NewActor3->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor3->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Computer);
					}

					if (NewActor4)
					{
						// 设置“标记”字段
						SetSocketPipeLineFlag(NewActor4, ESF_Normal);

						FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor4);
						float OutLength = (ModelBoxSocket.GetSize().X) + 10;
						FVector RightSocketLocation = SocketLocation + OutLength * RightNormal + OutLength * RightNormal;

						NewActor4->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
						NewActor4->SetActorLocation(RightSocketLocation);
					}
					else
					{
						//点位（电视柜插座）模型创建失败
						AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_TV);
					}
				}
			}
		}

		//壁挂空调插座布置 ADM_Socket_Three_Point_16A
		if (RoomIt->IsExistObj(EAIComponentCode::AI_AirConditionedHang))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_AirConditionedHang, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Three_Point_16A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_AirConditioner);

					FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);

					FVector ModelBoxSize = ModelBox.GetSize();
					FRotator RotationData = ActorItem->Actor->GetActorRotation();
					FVector RegSize = RotationData.RotateVector(ModelBoxSize);

					float LeftLocation = fabs((RegSize.X))/2 + Socket_AI_AirConditionedHang_DisLorR;
					

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LeftLocation, Socket_AI_AirConditionedHang_DisFloor, *NewActor);
					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
					
					FVector SocketLocation = NewActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);
				
					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LeftLocation * 2)* RightNormal;


					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewActor))
					{
						FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LeftLocation, Socket_AI_AirConditionedHang_DisFloor, *NewActor);

						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(RightModelPos) || CheckWallCornerCollision(NewActor))
						{
							NewActor->Destroy();
							//点位（壁挂空调插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Point_16A);
						}
					}
				}
				else
				{
					//点位（壁挂空调插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Point_16A);
				}
			}
		}

		//电/燃气热水器 ADM_Socket_Three_Spatter_Point_16A 防溅三孔插座，不知道是不是16A,  AIComponent里暂没有热水器。
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Gas_Heater) || RoomIt->IsExistObj(EAIComponentCode::AI_Electric_Heater))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Gas_Heater, ActorItemArr);
			RoomIt->GetModelEntityArrayByComponentId(AI_Electric_Heater, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem :ActorItemArr)
			{
				AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Three_Spatter_Point_16A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);

					FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);

					FVector ModelBoxSize = ModelBox.GetSize();
					FRotator RotationData = ActorItem->Actor->GetActorRotation();
					FVector RegSize = RotationData.RotateVector(ModelBoxSize);

					float LRLocation = fabs((RegSize.X)/2)  + Socket_AI_Heater_DisLorR;

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LRLocation, Socket_AI_Heater_DisFloor, *NewActor);
					FVector HeaterLocation = NewActor->GetActorLocation();
					HeaterLocation.Z = Socket_AI_Heater_DisFloor;
					NewActor->SetActorLocation(HeaterLocation);
					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);
					
					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor); 
					float OutLength =  (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = HeaterLocation + (OutLength  ) *LeftNormal ;
					FVector RightModelPos = HeaterLocation + (OutLength + LRLocation*2)* RightNormal ;
		

					TSet<AActor*> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor,RoomIt,IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewActor))
					{
						FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_Heater_DisFloor, *NewActor);
						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(RightModelPos) || CheckWallCornerCollision(NewActor))
						{
							NewActor->Destroy();
							//点位（电/燃气热水器插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Spatter_Point_16A);
						}
					}
				}
				else
				{
					//点位（电/燃气热水器插座）模型布置失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Spatter_Point_16A);
				}
			}
		}

		//智能马桶自动布置插座
		if (RoomIt->IsExistObj(EAIComponentCode::AI_IntelClosestool))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_IntelClosestool, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				AActor* NewActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Three_Spatter_Point_16A);
				if (NewActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewActor, ESF_Normal);

					FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);

					FVector ModelBoxSize = ModelBox.GetSize();
					FRotator RotationData = ActorItem->Actor->GetActorRotation();
					FVector RegSize = RotationData.RotateVector(ModelBoxSize);

					float LRLocation = fabs((RegSize.X)) / 2 + Socket_AI_Closestool_DisLorR;

					FVector BackwardPosBack;
					TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
					if (!WallLineBack.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LRLocation, Socket_AI_Closestool_DisFloor, *NewActor);
					FVector SofaLocation = NewActor->GetActorLocation();
					SofaLocation.Z = Socket_AI_Closestool_DisFloor;
					NewActor->SetActorLocation(SofaLocation);
					NewActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));

					FVector SocketLocation = NewActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LRLocation * 2)* RightNormal;

					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor,RoomIt,IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewActor))
					{
						FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_Closestool_DisFloor, *NewActor);
						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(RightModelPos) || CheckWallCornerCollision(NewActor))
						{
							NewActor->Destroy();
							//点位（智能马桶插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Spatter_Point_16A);
						}
					}
				}
				else
				{
					//点位（智能马桶插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Spatter_Point_16A);
				}
			}
		}

		//主位沙发插座布置 ADM_Socket_Five_Point_10A 10A五孔插座x2
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Sofa_Master))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Sofa_Master, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				FBox ModelBox = GVC->GetActorCollisionBox(ActorItem->Actor);
				FVector ModelBoxSize = ModelBox.GetSize();
				FRotator RotationData = ActorItem->Actor->GetActorRotation();
				FVector RegSize = RotationData.RotateVector(ModelBoxSize);
				float LRLocation =fabs((RegSize.X)) / 2 + Socket_AI_Sofa_Master_DisLorR;

				FVector BackwardPosBack;
				TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
				if (!WallLineBack.IsValid())
				{
					continue;
				}
				FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

				AActor* NewLeftActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewLeftActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewLeftActor, ESF_Normal);

					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, LRLocation, Socket_AI_Closestool_DisLorR, *NewLeftActor);
					FVector SofaLocation = NewLeftActor->GetActorLocation();
					SofaLocation.Z = Socket_AI_Sofa_Master_DisFloor;
					NewLeftActor->SetActorLocation(SofaLocation);
					NewLeftActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));

					FVector SocketLocation = NewLeftActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewLeftActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewLeftActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LRLocation * 2)* RightNormal;

					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewLeftActor))
					{
						NewLeftActor->Destroy();
						//点位（主位沙发插座）模型布置失败
						AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}
				}
				else
				{
					//点位（主位沙发插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}

				AActor* NewRightActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewRightActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewRightActor, ESF_Normal);

					FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, -LRLocation, Socket_AI_Closestool_DisLorR, *NewRightActor);
					FVector SofaLocation = NewRightActor->GetActorLocation();
					SofaLocation.Z = Socket_AI_Sofa_Master_DisFloor;
					NewRightActor->SetActorLocation(SofaLocation);
					NewRightActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));


					FVector SocketLocation = NewRightActor->GetActorLocation();

					TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
					if (!WallLine.IsValid())
					{
						NewRightActor->Destroy();
						continue;
					}
					FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLine);

					FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
					FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

					FBox ModelBoxSocket = GVC->GetActorCollisionBox(NewRightActor);
					float OutLength = (ModelBoxSocket.GetSize().X / 2);

					FVector LeftModelPos = SocketLocation + OutLength * LeftNormal;
					FVector RightModelPos = SocketLocation + (OutLength + LRLocation * 2)* RightNormal;

					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewRightActor, RoomIt, IgnoreActor) || !RoomIt->GetRoom()->IsPointInRoom(LeftModelPos) || CheckWallCornerCollision(NewRightActor))
					{
						NewRightActor->Destroy();
						//点位（主位沙发插座）模型布置失败
						AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}
				} 
				else
				{
					//点位（主位沙发插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}

		//冰箱插座布置 ADM_Socket_Five_Point_10A 10A五孔插座x2
		if (RoomIt->IsExistObj(EAIComponentCode::AI_Refrigerator))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_Refrigerator, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
				if (!WallLineBack.IsValid())
				{
					continue;
				}
				FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

				FVector LeftPos;
				FArmyDataTools::PointIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, Socket_AI_Refrigerator_DisLorR, Socket_AI_Refrigerator_DisFloor, LeftPos);
				AActor* NewLeftActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
				if (NewLeftActor )
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewLeftActor, ESF_Refrigerator);

					NewLeftActor->SetActorLocation(LeftPos);
					NewLeftActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));

					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActor, RoomIt, IgnoreActor) || CheckWallCornerCollision(NewLeftActor))//检测左侧是否满足
					{
						FVector RightPos;
						FArmyDataTools::PointIntervalGroundOnRightOfBackProjPoint(*ActorItem->Actor, Socket_AI_Refrigerator_DisLorR, Socket_AI_Refrigerator_DisFloor, RightPos);
						NewLeftActor->SetActorLocation(RightPos);
						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActor, RoomIt, IgnoreActor) || CheckWallCornerCollision(NewLeftActor))//检测右侧是否满足
						{
							NewLeftActor->Destroy();
							//点位（冰箱插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
						}
					}
				}
				else
				{
					//点位（冰箱插座）模型创建失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
		}

		//柜式空调插座布置 ADM_Socket_Three_Point_16A 16A三孔插座
		if (RoomIt->IsExistObj(EAIComponentCode::AI_AirConditionedVert))
		{
			TArray<TSharedPtr<FArmyModelEntity>> ActorItemArr;
			RoomIt->GetModelEntityArrayByComponentId(AI_AirConditionedVert, ActorItemArr);
			for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemArr)
			{
				TSharedPtr<FArmyWallLine> WallLineBack = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor, FArmyDataTools::AI_DT_MODEL_BACKWARD);
				if (!WallLineBack.IsValid())
				{
					continue;
				}
				FVector ResultNormal = FArmyDataTools::GetWallNormalInRoom(RoomIt->GetRoom(), WallLineBack);

				FVector LeftDir;
				FVector RightDir;
				FArmyDataTools::ModelProjPointOnWallLeftDirection(*ActorItem->Actor, LeftDir);
				FArmyDataTools::ModelProjPointOnWallRightDirection(*ActorItem->Actor, RightDir);

				FVector LeftPos;
				FArmyDataTools::PointIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, Socket_AI_AirConditionedVert_DisLorR, Socket_AI_AirConditionedVert_DisFloor, LeftPos);
				AActor* NewLeftActor = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Three_Point_16A);
				if (NewLeftActor)
				{
					// 设置“标记”字段
					SetSocketPipeLineFlag(NewLeftActor, ESF_AirConditioner);

					FBox ActorBox = GVC->GetActorCollisionBox(NewLeftActor);
					FVector ActorSize = ActorBox.GetSize();

					FVector ResultLeftPos = LeftPos /*+ LeftDir * (ActorSize.X / 2)*/;
					ResultLeftPos.Z = 0;
					bool bPointInLineOpenReverseLeft = FArmyMath::IsPointOnLine(ResultLeftPos, WallLineBack->GetCoreLine()->GetStart(), WallLineBack->GetCoreLine()->GetEnd());
					//if (bPointInLineOpenReverse)

					NewLeftActor->SetActorLocation(LeftPos);
					NewLeftActor->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, ResultNormal));
					TSet<AActor *> IgnoreActor;
					IgnoreActor.Add(ActorItem->Actor);
					if (FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActor, RoomIt, IgnoreActor) || !bPointInLineOpenReverseLeft || CheckWallCornerCollision(NewLeftActor))//检测左侧是否满足
					{
						FVector RightPos;
						FArmyDataTools::PointIntervalGroundOnRightOfBackProjPoint(*ActorItem->Actor, Socket_AI_AirConditionedVert_DisLorR, Socket_AI_AirConditionedVert_DisFloor, RightPos);
						NewLeftActor->SetActorLocation(RightPos);

						FVector ResultRightPos = RightPos + RightDir * (ActorSize.X / 2);
						ResultRightPos.Z = 0;
						bool bPointInLineOpenReverseRight = FArmyMath::IsPointOnLine(ResultRightPos, WallLineBack->GetCoreLine()->GetStart(), WallLineBack->GetCoreLine()->GetEnd());

						if (FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActor, RoomIt, IgnoreActor) || !bPointInLineOpenReverseRight || CheckWallCornerCollision(NewLeftActor))//检测右侧是否满足
						{
							NewLeftActor->Destroy();
							//点位（立式空调插座）模型布置失败
							AddFailInfoDesign(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Point_16A);
						}
					}
				}
				else
				{
					//点位（冰箱插座）模型布置失败
					AddFailInfoDownload(RoomIt, ActorItem->ComponentId, ADM_Socket_Three_Point_16A);
				}
			}
		}

		//床侧方式插座
		PalceSocketInBed(RoomIt);
	}

	return true;

}

void FArmyAutoDesignPoint::PalceSocketInBed(TSharedPtr<class FArmyRoomEntity> RoomEntity)
{
	//床头放置插座
	if (RoomEntity->IsExistObj(EAIComponentCode::AI_DoubleBed) || RoomEntity->IsExistObj(EAIComponentCode::AI_SingleBed))
	{
		TArray<TSharedPtr<FArmyModelEntity>> BedsideTable_L_Arr;
		TArray<TSharedPtr<FArmyModelEntity>> BedsideTableArr;
		RoomEntity->GetModelEntityArrayByComponentId(AI_BedsideTable_L, BedsideTable_L_Arr);
		RoomEntity->GetModelEntityArrayByComponentId(AI_BedsideTable_R, BedsideTableArr);
		BedsideTableArr.Append(BedsideTable_L_Arr);

		TArray<TSharedPtr<FArmyModelEntity>> ActorItemDoubleBedArr;
		TArray<TSharedPtr<FArmyModelEntity>> ActorItemSingleArr;
		TArray<TSharedPtr<FArmyModelEntity>> ActorItemBedArr;
		RoomEntity->GetModelEntityArrayByComponentId(AI_DoubleBed, ActorItemDoubleBedArr);
		RoomEntity->GetModelEntityArrayByComponentId(AI_SingleBed, ActorItemSingleArr);
		ActorItemBedArr.Append(ActorItemDoubleBedArr);
		ActorItemBedArr.Append(ActorItemSingleArr);
		for (TSharedPtr<FArmyModelEntity> & ActorItem : ActorItemBedArr)
		{
			//无床头柜
			FBox ActorBox = GVC->GetActorCollisionBox(ActorItem->Actor);
			FVector Size = ActorBox.GetSize();
			TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByModelInDir(*ActorItem->Actor);
			if (!WallLine.IsValid())
			{
				continue;
			}

			FVector NormalInRoom = FArmyDataTools::GetWallNormalInRoom(RoomEntity->GetRoom(), WallLine);

			FVector OutPosLeft;
			FVector OutPosRight;
			FArmyDataTools::PointIntervalGroundOnLeftOfBackProjPoint(*ActorItem->Actor, (Size.X / 2) + Socket_AI_Bed_DisBox, Socket_AI_Bed_NOTable_DisFloor, OutPosLeft);
			FArmyDataTools::PointIntervalGroundOnRightOfBackProjPoint(*ActorItem->Actor, (Size.X / 2) + Socket_AI_Bed_DisBox, Socket_AI_Bed_NOTable_DisFloor, OutPosRight);
				
			//获取背部中线一定高度的点
			FVector OutPosCenter;
			FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ActorItem->Actor, Socket_AI_Bed_NOTable_DisFloor, OutPosCenter);

			FVector LeftDir = (OutPosLeft - OutPosCenter).GetSafeNormal2D();
			FVector RightDir = (OutPosRight - OutPosCenter).GetSafeNormal2D();

			AActor* NewLeftActorL = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
			AActor* NewLeftActorR = AutoDesignPointManager->CreateActor(EAutoDesignModelID::ADM_Socket_Five_Point_10A);
			if (NewLeftActorL && NewLeftActorR)
			{
				// 设置“标记”字段
				SetSocketPipeLineFlag(NewLeftActorL, ESF_Normal);
				// 设置“标记”字段
				SetSocketPipeLineFlag(NewLeftActorR, ESF_Normal);

				FVector ModelCenterPos = ActorItem->Actor->GetActorLocation();
				//构造左侧检测包围盒
				FVector LeftPos = ModelCenterPos + LeftDir * Socket_AI_Bed_DisCheck;
				TArray<FVector> FboxVecL;
				FboxVecL.Add(ModelCenterPos);
				FboxVecL.Add(LeftPos);
				FboxVecL.Add(OutPosLeft);
				FBox NewBoxL(FboxVecL);

				for (TSharedPtr<FArmyModelEntity> ActorIt : BedsideTableArr)
				{
					FBox ActorBoxTemp = GVC->GetActorCollisionBox(ActorIt->Actor);

					if (NewBoxL.Intersect(ActorBoxTemp))
					{
						OutPosLeft.Z += Socket_AI_Bed_Table_DisFloor - Socket_AI_Bed_NOTable_DisFloor;
						break;
					}
				}

				//构造右侧检测包围盒
				FVector RightPos = ModelCenterPos + RightDir * Socket_AI_Bed_DisCheck;
				TArray<FVector> FboxVecR;
				FboxVecR.Add(ModelCenterPos);
				FboxVecR.Add(RightPos);
				FboxVecR.Add(OutPosRight);
				FBox NewBoxR(FboxVecR);

				for (TSharedPtr<FArmyModelEntity> ActorIt : BedsideTableArr)
				{
					FBox ActorBoxTemp = GVC->GetActorCollisionBox(ActorIt->Actor);

					if (NewBoxR.Intersect(ActorBoxTemp))
					{
						OutPosRight.Z += Socket_AI_Bed_Table_DisFloor - Socket_AI_Bed_NOTable_DisFloor;
						break;
					}
				}

				TSet<AActor *> IgnoreActor;
				bool bPlaceSoccess = false;

				NewLeftActorL->SetActorLocation(OutPosLeft);
				NewLeftActorL->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));

				FVector LeftNormal = FArmyDataTools::GetLeftDirectionByNormal(NormalInRoom, WallLine);
				FVector RightNormal = FArmyDataTools::GetRightDirectionByNormal(NormalInRoom, WallLine);

				FBox ModelBoxL = GVC->GetActorCollisionBox(NewLeftActorL);
				FBox ModelBoxR = GVC->GetActorCollisionBox(NewLeftActorR);

				FVector LeftModelPos = OutPosLeft + ModelBoxL.GetSize().X / 2 * LeftNormal;
				FVector RightModelPos = OutPosRight + ModelBoxR.GetSize().X / 2 * RightNormal;

				if (!FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActorL,RoomEntity, IgnoreActor) && RoomEntity->GetRoom()->IsPointInRoom(LeftModelPos) && !CheckWallCornerCollision(NewLeftActorL))
				{
					NewLeftActorL->SetActorLocation(OutPosLeft);
					NewLeftActorL->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
					bPlaceSoccess = true;
				}
				else
				{
					bPlaceSoccess = false;
					NewLeftActorL->Destroy();
					//点位（床头插座）模型布置失败
					AddFailInfoDesign(RoomEntity, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
					
				NewLeftActorR->SetActorLocation(OutPosRight);
				NewLeftActorR->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));

				if (!FArmyDataTools::CalModelIntersectWithOtherModel(NewLeftActorR, RoomEntity, IgnoreActor) && RoomEntity->GetRoom()->IsPointInRoom(RightModelPos) && !CheckWallCornerCollision(NewLeftActorR))
				{
					if (bPlaceSoccess && ActorItemSingleArr.Contains(ActorItem))//单人床
					{
						NewLeftActorR->Destroy();
						//点位（床头插座）模型布置失败
						AddFailInfoDesign(RoomEntity, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
					}
					else
					{
						NewLeftActorR->SetActorLocation(OutPosRight);
						NewLeftActorR->SetActorRotation(FQuat::FindBetweenVectors(FVector::RightVector, NormalInRoom));
					}
				}
				else
				{
					NewLeftActorR->Destroy();
					//点位（床头插座）模型布置失败
					AddFailInfoDesign(RoomEntity, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
				}
			}
			else
			{
				//点位（床头插座）模型布置失败
				AddFailInfoDownload(RoomEntity, ActorItem->ComponentId, ADM_Socket_Five_Point_10A);
			}
		}
	}
}

// 检测开关、插座、水位与柱子、包立管、风道的碰撞，如果碰撞了则移除
bool FArmyAutoDesignPoint::CheckWallCornerCollision()
{
	//收集柱子等墙角物件
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
	for (auto& InRoom : RoomTableArr)
	{
		TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
		TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
		TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
		ComponentObjArr.Append(PillarArr);
		ComponentObjArr.Append(PackPipeArr);
		ComponentObjArr.Append(AirFlueArr);
	}
	bool hasCollision = false;
	//碰撞检测
	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num() > 0 && ActorItr->ActorHasTag(XRActorTag::AutoDesignPoint))
		{
			//碰撞检测
			for (auto& InRoom : RoomTableArr)
			{
				FBox ActorBox = GVC->GetActorCollisionBox(*ActorItr);
				ActorBox.Min.Z = 0;
				ActorBox.Max.Z = 0;
				for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
				{
					FBox ModelBox = model->GetBounds();
					if (ModelBox.Intersect(ActorBox))
					{
						ActorItr->Destroy();
						hasCollision = true;
					}
				}
			}
		}
	}
	return hasCollision;
}

// 检测开关、插座、水位与柱子、包立管、风道的碰撞，如果碰撞了则移除
bool FArmyAutoDesignPoint::CheckWallCornerCollision(AActor* ActorItr)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	//收集柱子等墙角物件
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
	for (auto& InRoom : RoomTableArr)
	{
		TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
		TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
		TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
		ComponentObjArr.Append(PillarArr);
		ComponentObjArr.Append(PackPipeArr);
		ComponentObjArr.Append(AirFlueArr);
	}
	//碰撞检测
	if (ActorItr->Tags.Num() > 0 && ActorItr->ActorHasTag(XRActorTag::AutoDesignPoint))
	{
		//碰撞检测
		FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
		ActorBox.Min.Z = 0;
		ActorBox.Max.Z = 0;
		for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
		{
			FBox ModelBox = model->GetBounds();
			if (ModelBox.Intersect(ActorBox))
			{
				//ActorItr->Destroy();
				return true;
			}
		}
	}
	return false;
}

//提示语：房间名称+依赖物品+“缺失”+水位/开关/插座
//添加一条提示语
void FArmyAutoDesignPoint::AddFailInfoDownload(TSharedPtr<FArmyRoomEntity> Room, int32 RelyModelType, EAutoDesignModelID PointType)
{
	FString RoomName = Room->GetRoom()->GetSpaceName();//房间名称
	FString RelyModelTypeName = AutoDesignPointManager->GetRelyModelNameByModelID(RelyModelType);//依赖模型类型名称
	FString PointTypeName = AutoDesignPointManager->GetModelNameByModelID(PointType);//点位模型类型名称
	FString TempStr = RoomName + RelyModelTypeName + TEXT("后台缺失") + PointTypeName;//房间名称+依赖物品+“缺失”+水位/开关/插座
	FailInfoArr.Add(TempStr);
	FailInfoType = 1;
}

//添加一条提示语
void FArmyAutoDesignPoint::AddFailInfoDesign(TSharedPtr<FArmyRoomEntity> Room, int32 RelyModelType, EAutoDesignModelID PointType)
{
	FString RoomName = Room->GetRoom()->GetSpaceName();//房间名称
	FString RelyModelTypeName = AutoDesignPointManager->GetRelyModelNameByModelID(RelyModelType);//依赖模型类型名称
	FString PointTypeName = AutoDesignPointManager->GetModelNameByModelID(PointType);//点位模型类型名称
	FString TempStr = RoomName + RelyModelTypeName + TEXT("缺失") + PointTypeName;//房间名称+依赖物品+“缺失”+水位/开关/插座
	FailInfoArr.Add(TempStr);
}

//显示提示语对话框
void FArmyAutoDesignPoint::ShowDialogFailInfo()
{
	//提示语：房间名称+依赖物品+“缺失”+水位/开关/插座
	if (FailInfoArr.Num() > 0)
	{
		FailInfoArr.Sort();//默认排序
		//构件提示内容
		FString FialInfo;
		for (int32 i = 0; i < FailInfoArr.Num(); ++i)
		{
			FialInfo += FString::FromInt(i + 1) + TEXT(". ") + FailInfoArr[i] + (i != FailInfoArr.Num() - 1 ? TEXT("\n") : TEXT(""));
		}
		//显示对话框
		GGI->Window->PresentModalDialog(
			SNew(SArmyAutoDesignFailWid)
			.FailInfoNum(FailInfoArr.Num())
			.FailInfo(FialInfo)
			.FailInfoType(FailInfoType)
			.OnConfirm_Lambda([this]() {
				GGI->Window->DismissModalDialog();
				return FReply::Handled();
			})
		);
	}
}
//提示语结束

// 处理“标记”字段
void FArmyAutoDesignPoint::SetSocketPipeLineFlag(AActor* InActor, ESocketFlag InFlag)
{
	if (InFlag == ESF_Normal || InFlag == ESF_AirConditioner || InFlag == ESF_Refrigerator || InFlag == ESF_Power)
	{
		TSharedPtr<FArmyFurniture> Furniture;
		AXRFurnitureActor* FurnitureActor = Cast<AXRFurnitureActor>(InActor->GetOwner());
		if (FurnitureActor && FurnitureActor->IsValidLowLevel()) {
			TSharedPtr<FArmyFurniture> FurnitureTemp = StaticCastSharedPtr<FArmyFurniture>(FurnitureActor->GetRelevanceObject().Pin());
			if (FurnitureTemp.IsValid() /*&& FurnitureTemp->IsSocket()*/) {
				Furniture = FurnitureTemp;
			}
		}
		if (Furniture.IsValid())
		{
			//构件获取到了
			ESocketFlag flag = Furniture->GetSocketType();
			if (flag != InFlag && (flag == ESF_Normal || flag == ESF_AirConditioner || flag == ESF_Refrigerator || flag == ESF_Power))
			{
				Furniture->SetSocketType(InFlag);
			}
		}
	}
}

