#include "ArmyDownloadCad.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformFile.h"
#include "ArmyAutoCad.h"
#include "ArmyConstructionFrame.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyWallLine.h"
#include "ArmyHardware.h"
#include "ArmyRectBayWindow.h"
#include "ArmyBearingWall.h"
#include "ArmyWindow.h"
#include "ArmyFloorWindow.h"
#include "ArmyRectBayWindow.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyCornerBayWindow.h"
#include "ArmyDimensions.h"
#include "ArmyTextLabel.h"
#include "ArmyModifyWall.h"
#include "ArmyAddWall.h"
#include "ArmyDownLeadLabel.h"
#include "ArmyCompass.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmySecurityDoor.h"
#include "ArmyBeam.h"
#include "ArmyAirFlue.h"
#include "ArmyPillar.h"
#include "ArmyEntrance.h"
#include "ArmyDownLeadLabel.h"
#include "ResTools.h"
#include "ArmyBoardSplitline.h"
#include "ArmyPass.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyRectArea.h"
#include "ArmyRegularPolygonArea.h"
#include "ArmyFreePolygonArea.h"
#include "ArmyBaseArea.h"
#include "ArmyBodaArea.h"
#include "ArmyCircleArea.h"
#include "ArmyBrickUnit.h"
#include "ArmyObject.h"
#include "HardModeData/DrawStyle/XRBaseEditStyle.h"
#include "ArmyLampControlLines.h"
#include "ArmyPackPipe.h"
#include "ArmyNewPass.h"
#include "ArmyPunch.h"
#include "ArmySymbol.h"
#include "ArmyWHCTableObject.h"
#include "ArmyCustomDefine.h"
#include "ArmyConstructionLayerManager.h"


#define MIN_PRECISION_NUMBER 0.01
#define HATCH_SCALE_2				   2.0
#define HATCH_SCALE_4				  16.0
const TSharedRef<FArmyDownloadCad>& FArmyDownloadCad::Get()
{
	static const TSharedRef<FArmyDownloadCad> Instance = MakeShareable(new FArmyDownloadCad);
	return Instance;
}

FArmyDownloadCad::FArmyDownloadCad()
{
}


// 生成CAD文件测试
bool FArmyDownloadCad::CreateCad(const TMap<EConstructionFrameType, TSharedPtr<class FArmyConstructionFrame>>& InFrameMap, const TSharedPtr<class FArmyConstructionLayerManager>& InLayerManager, FString CadFilePath, const TArray<FName>& CadChosenList, FString &OutPath)
{
	if (CadFilePath.IsEmpty())
	{
		return false;
	}

	if (InFrameMap.Num() == 0)
	{
		return false;
	}

	FString DirPath = FResTools::GetAutoCadDir() / CadFilePath;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*DirPath))
	{
		PlatformFile.CreateDirectoryTree(*DirPath);
		if (!PlatformFile.DirectoryExists(*DirPath))
		{
			return false;
		}
	}
	FString FileName = DirPath/TEXT("平面图") + CadFilePath +TEXT(".dxf");
	OutPath = DirPath;

	this->LayerManager = InLayerManager;
	// 初始化cad要导出的图纸
	InitBimLayerArray(CadChosenList);
	MapFrameInfo = InFrameMap;
	InitLayersMap();

	// 清空画布
	FArmyAutoCad::Get()->ClearCanvas();
	// 创建图层
	FArmyAutoCad::Get()->CreateLayer(LayersMap);
	
	// 绘制基础数据
	//TestCad();
	// 绘制CAD
	DrawCAD();


	//const FString FilePath = FPaths::ProjectSavedDir() / TEXT("AutoCad") / TEXT("test.dxf");
	FArmyAutoCad::Get()->Save(FileName);

	return true;
}

bool FArmyDownloadCad::TestCad()
{
	//FArmyAutoCad::Get()->DrawMultipleText(CT_Original, GetLayerName(LT_Default), FVector::ZeroVector, TEXT("面积：30平方米\n周长：45.56米"), 20);
	//FArmyAutoCad::Get()->DrawAlignedDimension(CT_Original, GetLayerName(LT_Default), FVector(0, 0, 0), FVector(0, 100, 0), FVector(10, 0, 0));
	//FArmyAutoCad::Get()->DrawAlignedDimension(CT_Original, GetLayerName(LT_Default), FVector(100, 0, 0), FVector(100, 5, 0), FVector(110, 0, 0));

	return true;
}
// 绘制CAD
bool FArmyDownloadCad::DrawCAD()
{
	for(int index = 0; index < BimLayerArray.Num();index++)
	{
		TSharedPtr<FArmyLayer> CurrentBimLayer = BimLayerArray[index];
		if (!CurrentBimLayer.IsValid())
		{
			continue;
		}

		// 绘制图框
		if (!DrawCadFrame(CurrentBimLayer,index))
		{
			return false;
		}
		// 绘制指北针
		DrawCompass(index);
		// 根据bim图纸绘制cad
		DrawCadByLayer(CurrentBimLayer, index);
		/*
		// 绘制填充
		DrawFill(It);
		// 绘制窗梁柱风烟道
		DrawHardware(It);
		// 绘制门
		DrawDoor(It);
		// 绘制构件
		DrawCompoent(It);
		// 绘制标尺
		DrawDimension(It);
		// 绘制标签
		DrawLabel(It);
		// 绘制家具
		DrawFurniture(It);
		*/
	}

	return true;
}

// 根据bim图纸绘制图框
bool FArmyDownloadCad::DrawCadFrame(TSharedPtr<class FArmyLayer> BimLayer, int32 CadIndex)
{
	if(BimLayer.IsValid())
	{
		//获取指定bim图纸下的全部Class
		TMap<ObjectClassType, TSharedPtr<FArmyClass>> ClassMap;
		ClassMap = BimLayer->GetAllClass();

		// 存在图框
		if (ClassMap.Contains(Class_ConstructionFrame))
		{
			TSharedPtr<FArmyClass> ClassObj = ClassMap.FindRef(Class_ConstructionFrame);
			TArray<TSharedPtr<FArmyObject>> ObjItList;
			if (!ClassObj.IsValid())
			{
				return false;
			}


			// 绘制当前layer的图框
			ObjItList = ClassObj->GetAllObjects();
			FObjectPtr NewObj = nullptr;
			for (auto It : ObjItList)
			{
				if (It->GetType() == OT_ConstructionFrame)
				{
					TSharedPtr<FArmyConstructionFrame> NewFrame = StaticCastSharedPtr<FArmyConstructionFrame>(It);
					NewObj = NewFrame->CopySelf();
					break;
				}
			}

			if (!NewObj.IsValid())
			{
				if (!BimLayer->IsStaticLayer())
				{
					if (MapFrameInfo.Contains(E_Frame_Cupboard))
					{
						NewObj = MapFrameInfo.FindRef(E_Frame_Cupboard)->CopySelf();
					}
					else
					{
						if (MapFrameInfo.Contains(E_Frame_Default))
						{
							NewObj = MapFrameInfo.FindRef(E_Frame_Default)->CopySelf();
						}
					}
				}
				else
				{
					if (MapFrameInfo.Contains(E_Frame_Default))
					{
						NewObj = MapFrameInfo.FindRef(E_Frame_Default)->CopySelf();
					}
				}
			}

			if (!NewObj.IsValid())
			{
				return false;
			}

			TSharedPtr<FArmyConstructionFrame> CurrentInfo = StaticCastSharedPtr<FArmyConstructionFrame>(NewObj);
			CurrentInfo->FillAreaText(FRAME_AREA_CONSTRUCTIONNAME, BimLayer->GetName().ToString());
			return FArmyAutoCad::Get()->DrawFrame(GetLayerName(LT_Title_Gdg), CadIndex, CurrentInfo);
	
		}

		return false;
	}
	else
	{
		return false;
	}
}

// 绘制指北针
bool FArmyDownloadCad::DrawCompass(int32 CadIndex)
{
	TArray<TWeakPtr<FArmyObject>> CompassArray;
	FArmySceneData::Get()->GetObjects(E_ConstructionModel, OT_Compass, CompassArray);
	if (CompassArray.Num() < 1)
	{
		return false;
	}
	TSharedPtr<FArmyCompass> Compass = StaticCastSharedPtr<FArmyCompass>(CompassArray[0].Pin());
	FArmyAutoCad::Get()->DrawCompass(CadIndex, GetLayerName(LT_Default), Compass);

	return true;
}



// 根据bim的layer绘制每张cad图
bool FArmyDownloadCad::DrawCadByLayer(TSharedPtr<class FArmyLayer> BimLayer, int32 CadIndex)
{
	if (BimLayer.IsValid())
	{
		// 设置当前cad模式
		InitCadDrawModel(BimLayer);
		//获取指定bim图纸下的全部Class
		TMap<ObjectClassType, TSharedPtr<FArmyClass>> ClassMap;
		ClassMap = BimLayer->GetAllClass();

		EModelType ModelType = E_LayoutModel;
		// =1 是原始图层
		if (BimLayer->GetLayerType() == 1)
		{
			ModelType = E_HomeModel;
		}

		// =1 是原始图层
		if (GetCadDrawModel(CDM_DelWall))
		{
			ModelType = E_HomeModel;
		}

		// 含有拆除或新增墙体，墙体使用原始墙体绘制
		if (GetCadDrawModel(CDM_AddWall))
		{
			ModelType = E_LayoutModel;
		}

		// 橱柜图纸
		if (!BimLayer->IsStaticLayer())
		{
			ModelType = E_ConstructionModel;
		}

		for (auto ClassIt : ClassMap)
		{
			TArray<TSharedPtr<FArmyObject>> ObjItList;
			if (!ClassIt.Value.IsValid() || !ClassIt.Value->IsVisible())
			{
				continue;
			}
			else
			{
				ObjItList = ClassIt.Value->GetAllObjects();
			}

			if (ObjItList.Num() < 1)
			{
				continue;
			}

			switch (ClassIt.Key)
			{
			case Class_Begin:
				break;
			case Class_BaseWall:
				break;
			case Class_AddWall:
				DrawCadWall(CadIndex, Class_AddWall, ObjItList);
				break;
			case Class_BreakWall:
				DrawCadWall(CadIndex, Class_BreakWall, ObjItList);
				break;
			case Class_Door: // 门相关数据存储到垭口中，待确定
				break;
			case Class_Pass:
				DrawCadDoor(CadIndex, Class_Pass, ObjItList);
				break;
			case Class_Window://窗户
				DrawCadWindow(CadIndex, Class_Window, ObjItList);
				break;
			case Class_Furniture:// 家具
				DrawCadComponent(CadIndex, Class_Furniture, ObjItList);
				break;
			case Class_Switch: // 开关
			case Class_Socket: // 插座
				DrawCadComponent(CadIndex, ClassIt.Key, ObjItList);
				DrawCadLampControlLines(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_ElectricBoxH: // 强电箱
			case Class_ElectricBoxL: // 弱电箱
				DrawCadComponent(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_Light: // 绘制灯具
				DrawCadComponent(CadIndex, Class_Light, ObjItList);
				break;
			case Class_AirConditionerPoint:// 空调点位
				break;
			case Class_HotWaterPoint://热水点位
			case Class_HotAndColdWaterPoint://冷热水点位
			case Class_ColdWaterPoint://冷水点位
			case Class_FloordRainPoint://地漏下水
			case Class_ClosestoolRainPoint://马桶下水
			case Class_WashBasinRainPoint://水盆下水
			case Class_RecycledWaterPoint://中水点位
			case Class_HomeEntryWaterPoint://进户水
			case Class_WaterRoutePoint:// 水路点位
				DrawCadComponent(CadIndex, Class_WaterRoutePoint, ObjItList);
				break;
			case Class_ElectricWire: // 灯控线
				DrawCadLampControlLines(CadIndex, Class_ElectricWire, ObjItList);
				break;
			case Class_AnnotationInSide: // 内墙标注
			case Class_AnnotationOutSide:// 外墙标注
			case Class_AnnotationDoor://门标注
			case Class_AnnotationWindow:// 窗标注
			case Class_AnnotationElectricBoxH:// 强电箱标注
			case Class_AnnotationElectricBoxL:// 弱电箱标注
				DrawCadDimensions(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_AnnotationAirConditionerPoint: // 空调点位标注
				break;
			case Class_AnnotationFurniture: // 家具家电标注
				DrawCadDimensions(CadIndex, Class_AnnotationFurniture, ObjItList);
				break;
			case Class_AnnotationSwitch: // 开关标注
				DrawCadDimensions(CadIndex, Class_AnnotationSwitch, ObjItList);
				break;
			case Class_AnnotationSocket: // 插座标注
				DrawCadDimensions(CadIndex, Class_AnnotationSocket, ObjItList);
				break;
			case Class_AnnotationLamp: // 灯具标注
				DrawCadDimensions(CadIndex, Class_AnnotationLamp, ObjItList);
				break;
			case Class_SpaceName: // 空间名称
				DrawCadTextLable(CadIndex, Class_SpaceName, ObjItList);
				break;
			case Class_Floorlayout:// 地面造型
				DrawCadFloor(CadIndex, Class_Floorlayout, ObjItList);
				DrawCadDimensions(CadIndex, Class_Floorlayout, ObjItList);
				break;
			case Class_Walllayout: // 墙面造型				
				break;
			case Class_ConstructionFrame: // 图框
				break;
			case Class_Flag34: // 新建拆除后实墙效果
				break;
			case Class_SpaceArea:
			case Class_SpacePerimeter:
			case Class_SpaceHeight:
				DrawCadTextLable(CadIndex, ClassIt.Key, ObjItList);
			case Class_AddWallPost: // 新建后效果
				break;
			case Class_DeleteWallPost: // 拆除后效果
				break;
			case Class_AnnotationDeleteWall:// 拆除墙标注
				DrawCadDimensions(CadIndex, Class_AnnotationDeleteWall, ObjItList);
				break;
			case Class_AnnotationAddWall: // 新建墙标注
				DrawCadDimensions(CadIndex, Class_AnnotationAddWall, ObjItList);
				break;
			case Class_LampStrip: // 灯带
				break;
			case Class_Ceilinglayout: // 顶面造型
				DrawCadCeiling(CadIndex, Class_Ceilinglayout, ObjItList);
				break;
			case Class_AnnotationCeilingObj:// 吊顶标注
				DrawCadDimensions(CadIndex, Class_AnnotationCeilingObj, ObjItList);
				break;
			case Class_Beam: // 梁
				DrawCadHardware(CadIndex, Class_Beam, ObjItList);
				break;
			case Class_Socket_H:// 强电插座
			case Class_Socket_L:// 强电插座
				DrawCadComponent(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_DiversityWater: // 分集水器
				DrawCadComponent(CadIndex, Class_WaterRoutePoint, ObjItList);
				break;
			case Class_WaterPipe: // 下水主管道
				DrawCadComponent(CadIndex, Class_WaterPipe, ObjItList);
				break;
			case Class_Heater: // 暖气
				break;
			case Class_GasMeter:// 燃气表
			case Class_GasPipe:// 燃气主管
				DrawCadComponent(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_AnnotationCurtainBox:// 窗帘盒标注
				break;
			case Class_AnnotationBeam:// 梁标注
				DrawCadDimensions(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_AnnotationElectricPoint:// 电路点位标注
			case Class_AnnotationSocket_H:// 强电插座标注
			case Class_AnnotationSocket_L:
				DrawCadDimensions(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_AnnotationWaterPoint://水路点位标注
			case Class_AnnotationSewerPipe:// 下水管道标注
			case Class_AnnotationHeater:// 暖气标注
				DrawCadDimensions(CadIndex, ClassIt.Key, ObjItList);
				break;
			case Class_AnnotationGas:// 燃气点位标注
				break;
			case Class_SCTCabinet:// 定制柜类
				DrawCadSCTCabinet(CadIndex, Class_SCTCabinet, ObjItList);
				break;
			case Class_SCTHardware:// 定制五金/电器类
				DrawCadSCTHardware(CadIndex, Class_SCTHardware, ObjItList);
				break;
			case Class_Other: // 风烟道柱子在这里
				DrawCadHardware(CadIndex, Class_Other, ObjItList);
				break;
			case Class_End:
				break;
			default:
				break;
			}
		}

		// 绘制户型
		if (ModelType == E_ConstructionModel && BimLayer->GetLayerType() == 20)
		{
			// 橱柜的台面图和立面图不绘制户型
			return true;
		}

		TArray<TSharedPtr<FArmyLine>> LineArray = GenerateChangedData(ModelType);
		for (auto It : LineArray)
		{
			FArmyAutoCad::Get()->DrawLine(CadIndex, GetLayerName(LT_Wall), It->GetStart(), It->GetEnd());
		}

	}

	return true;
}


/////////////////////////////// 绘制相关的私有方法////////////////////
#pragma region 绘制相关的私有方法

// 根据bim的layer绘制墙（新建墙体和拆除墙体填充）
bool FArmyDownloadCad::DrawCadWall(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	// 新增墙体
	if (InClassType == Class_AddWall)
	{
		for (auto It : InObjItList)
		{
			if (It->GetType() == OT_AddWall)
			{
				TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It);
				if (AddWall.IsValid())
				{
					TArray<TSharedPtr<FArmyPolygon>> PolygonArray;
					AddWall->CalCadWallsByHardwares(PolygonArray);
					for (auto PolygonIt : PolygonArray)
					{
						// 绘制填充
						LayerName = GetLayerName(LT_Wall_Add_Solid);
						FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, PolygonIt);
						// 绘制线
						LayerName = GetLayerName(LT_Wall_Add);
						FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, PolygonIt);
					}	
				}
			}
		}
	}

	// 拆除墙体
	if (InClassType == Class_BreakWall)
	{
		for (auto It : InObjItList)
		{
			if (It->GetType() == OT_ModifyWall)
			{
				TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It);
				if (ModifyWall.IsValid())
				{
					// 绘制填充
					LayerName = GetLayerName(LT_Wall_Del_Solid);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, ModifyWall->Polygon);
					// 绘制线
					LayerName = GetLayerName(LT_Wall_Del);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, ModifyWall->Polygon);
				}
			}
		}
	}

	return true;
}

// 根据bim的layer绘制门（推拉门、防盗门、标准门）
bool FArmyDownloadCad::DrawCadDoor(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	// 标准门
	if (InClassType == Class_Pass)
	{
		for (auto It : InObjItList)
		{
			if (It->GetType() == OT_Door)
			{
				TSharedPtr<FArmySingleDoor> SingleDoor = StaticCastSharedPtr<FArmySingleDoor>(It);
				
				// 显示门
				if (!GetCadDrawModel(CDM_DoorPass))
				{
					FArmyAutoCad::Get()->DrawSingleDoor(CadIndex, LayerName, SingleDoor);
				}

				// 显示通过门造成的拆改墙
				//0: 在原始墙体上 1：在新建墙体上
				// bHasConnectPass 是否链接了门洞
				if (GetCadDrawModel(CDM_DelWall)&& SingleDoor->InWallType == 0 && !SingleDoor->bHasConnectPass)
				{
					// 绘制成删除墙形式
					TSharedPtr<FArmyPolygon> DoorPolygon = MakeShareable(new FArmyPolygon);
					for (auto VectorIt : SingleDoor->ModifyWallRect->GetVertices())
					{
						DoorPolygon->AddVertice(VectorIt);
					}
					
					// 绘制填充
					LayerName = GetLayerName(LT_Wall_Del_Solid);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, DoorPolygon);
					// 绘制线
					LayerName = GetLayerName(LT_Wall_Del);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, DoorPolygon);
				}		
			}
			else if (It->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> SlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(It);
				
				// 显示门
				if (!GetCadDrawModel(CDM_DoorPass))
				{
					FArmyAutoCad::Get()->DrawSlidingDoor(CadIndex, LayerName, SlidingDoor);
				}
				//0: 在原始墙体上 1：在新建墙体上
				// bHasConnectPass 是否链接了门洞
				if (GetCadDrawModel(CDM_DelWall) && SlidingDoor->InWallType == 0 && !SlidingDoor->bHasConnectPass)
				{
					// 绘制成删除墙形式
					TSharedPtr<FArmyPolygon> DoorPolygon = MakeShareable(new FArmyPolygon);
					for (auto VectorIt : SlidingDoor->ModifyWallRect->GetVertices())
					{
						DoorPolygon->AddVertice(VectorIt);
					}

					// 绘制填充
					LayerName = GetLayerName(LT_Wall_Del_Solid);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, DoorPolygon);
					// 绘制线
					LayerName = GetLayerName(LT_Wall_Del);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, DoorPolygon);
				}
			}
			else if (It->GetType() == OT_SecurityDoor)
			{
				TSharedPtr<FArmySecurityDoor> SecurityDoor = StaticCastSharedPtr<FArmySecurityDoor>(It);
				FArmyAutoCad::Get()->DrawSecurityDoor(CadIndex, LayerName, SecurityDoor);
			}
			else if (It->GetType() == OT_DoorHole)
			{
				TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It);
				if (GetCadDrawModel(CDM_DelWall))
				{
					// 绘制成删除墙形式
					if (Pass->GetLeftDeleteWall()->Width > 0)
					{
						TSharedPtr<FArmyPolygon> LeftDelPassPolygon = MakeShareable(new FArmyPolygon);
						for (auto VectorIt : Pass->GetLeftDeleteWall()->GetVertices())
						{
							LeftDelPassPolygon->AddVertice(VectorIt);
						}

						// 绘制填充
						LayerName = GetLayerName(LT_Wall_Del_Solid);
						FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, LeftDelPassPolygon);
						// 绘制线
						LayerName = GetLayerName(LT_Wall_Del);
						FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, LeftDelPassPolygon);

					}

					// 绘制成删除墙形式
					if (Pass->GetRightDeleteWall()->Width > 0)
					{
						TSharedPtr<FArmyPolygon> RightDelPassPolygon = MakeShareable(new FArmyPolygon);
						for (auto VectorIt : Pass->GetRightDeleteWall()->GetVertices())
						{
							RightDelPassPolygon->AddVertice(VectorIt);
						}

						// 绘制填充
						LayerName = GetLayerName(LT_Wall_Del_Solid);
						FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, RightDelPassPolygon);
						// 绘制线
						LayerName = GetLayerName(LT_Wall_Del);
						FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, RightDelPassPolygon);
					}

					
				}

				if (GetCadDrawModel(CDM_AddWall))
				{
					// 绘制成删除墙形式
					if (Pass->GetLeftAddWall()->Width > 0)
					{
						TSharedPtr<FArmyPolygon> LeftAddPassPolygon = MakeShareable(new FArmyPolygon);
						for (auto VectorIt : Pass->GetLeftAddWall()->GetVertices())
						{
							LeftAddPassPolygon->AddVertice(VectorIt);
						}

						// 绘制填充
						LayerName = GetLayerName(LT_Wall_Add_Solid);
						FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, LeftAddPassPolygon);
						// 绘制线
						LayerName = GetLayerName(LT_Wall_Add);
						FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, LeftAddPassPolygon);

					}

					// 绘制成删除墙形式
					if (Pass->GetRightAddWall()->Width > 0)
					{
						TSharedPtr<FArmyPolygon> RightAddPassPolygon = MakeShareable(new FArmyPolygon);
						for (auto VectorIt : Pass->GetRightAddWall()->GetVertices())
						{
							RightAddPassPolygon->AddVertice(VectorIt);
						}

						// 绘制填充
						LayerName = GetLayerName(LT_Wall_Add_Solid);
						FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, RightAddPassPolygon);
						// 绘制线
						LayerName = GetLayerName(LT_Wall_Add);
						FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, RightAddPassPolygon);
					}
				}
			}
			else if (It->GetType() == OT_Pass)
			{
				TSharedPtr<FArmyPass> PassObj = StaticCastSharedPtr<FArmyPass>(It);
				
				// 删除墙模式
				if (GetCadDrawModel(CDM_DelWall))
				{
					TSharedPtr<FArmyRect> LeftDeleteRect = PassObj->GetLeftDeleteWall();
					TSharedPtr<FArmyRect> RightDeleteRect = PassObj->GetRightDeleteWall();
					TSharedPtr<FArmyPolygon> LeftDeletePolygon = MakeShareable(new FArmyPolygon);
					TSharedPtr<FArmyPolygon> RightDeletePolygon = MakeShareable(new FArmyPolygon);
					if (PassObj->bHasModify && LeftDeleteRect.IsValid() && LeftDeleteRect->Width > 0.01)
					{
						for (auto VectorIt : LeftDeleteRect->GetVertices())
						{
							LeftDeletePolygon->AddVertice(VectorIt);
						}
					}

					if (PassObj->bHasModify && RightDeleteRect.IsValid() && RightDeleteRect->Width > 0.01)
					{
						for (auto VectorIt : RightDeleteRect->GetVertices())
						{
							RightDeletePolygon->AddVertice(VectorIt);
						}
					}

					// 绘制填充
					LayerName = GetLayerName(LT_Wall_Del_Solid);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, LeftDeletePolygon);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, RightDeletePolygon);
					// 绘制线
					LayerName = GetLayerName(LT_Wall_Del);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, LeftDeletePolygon);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, RightDeletePolygon);
				}
				

				// 新增墙模式
				if (GetCadDrawModel(CDM_AddWall))
				{
					TSharedPtr<FArmyRect> LeftAddRect = PassObj->GetLeftAddWall();
					TSharedPtr<FArmyRect> RightAddRect = PassObj->GetRightAddWall();
					TSharedPtr<FArmyRect> FillRect = PassObj->GetFillPassWall();
					TSharedPtr<FArmyPolygon> LeftAddPolygon = MakeShareable(new FArmyPolygon);
					TSharedPtr<FArmyPolygon> RightAddPolygon = MakeShareable(new FArmyPolygon);
					TSharedPtr<FArmyPolygon> FillPolygon = MakeShareable(new FArmyPolygon);
					if (PassObj->bHasAdd && LeftAddRect.IsValid() && LeftAddRect->Width > 0.01)
					{
						for (auto VectorIt : LeftAddRect->GetVertices())
						{
							LeftAddPolygon->AddVertice(VectorIt);
						}
					}

					if (PassObj->bHasAdd && RightAddRect.IsValid() && RightAddRect->Width > 0.01)
					{
						for (auto VectorIt : RightAddRect->GetVertices())
						{
							RightAddPolygon->AddVertice(VectorIt);
						}
					}

					// 填充门洞
					if (PassObj->GetIfFillPass() && FillRect.IsValid() && FillRect->Width > 0.01)
					{
						for (auto VectorIt : FillRect->GetVertices())
						{
							FillPolygon->AddVertice(VectorIt);
						}
					}
					
					// 绘制填充
					LayerName = GetLayerName(LT_Wall_Add_Solid);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, LeftAddPolygon);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, RightAddPolygon);
					FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, FillPolygon);
					// 绘制线
					LayerName = GetLayerName(LT_Wall_Add);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, LeftAddPolygon);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, RightAddPolygon);
					FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, FillPolygon);
				}

				if (!GetCadDrawModel(CDM_DelWall) && !GetCadDrawModel(CDM_AddWall))
				{
					if (PassObj->GetIfGeneratePassModel())
					{
						FArmyAutoCad::Get()->DrawPassEdge(CadIndex, LayerName, PassObj);
					}
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

// 根据bim的layer绘制窗户（标准窗户等5类）
bool FArmyDownloadCad::DrawCadWindow(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	// 窗户
	if (InClassType == Class_Window)
	{
		for (auto It : InObjItList)
		{
			if (It->GetType() == OT_Window)
			{
				TSharedPtr<FArmyWindow> NormalWindow = StaticCastSharedPtr<FArmyWindow>(It);
				FArmyAutoCad::Get()->DrawWindow(CadIndex, LayerName, NormalWindow);
			}
			else if (It->GetType() == OT_FloorWindow)
			{
				TSharedPtr<FArmyFloorWindow> FloorWindow = StaticCastSharedPtr<FArmyFloorWindow>(It);
				FArmyAutoCad::Get()->DrawFloorWindow(CadIndex, LayerName, FloorWindow);
			}
			else if (It->GetType() == OT_RectBayWindow)
			{
				TSharedPtr<FArmyRectBayWindow> RectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(It);
				FArmyAutoCad::Get()->DrawRectBayWindow(CadIndex, LayerName, RectBayWindow);
			}
			else if (It->GetType() == OT_TrapeBayWindow)
			{
				TSharedPtr<FArmyTrapeBayWindow> TrapeBayWindow = StaticCastSharedPtr<FArmyTrapeBayWindow>(It);
				FArmyAutoCad::Get()->DrawTrapeBayWindow(CadIndex, LayerName, TrapeBayWindow);
			}
			else if (It->GetType() == OT_CornerBayWindow)
			{
				TSharedPtr<FArmyCornerBayWindow> CornerBayWindow = StaticCastSharedPtr<FArmyCornerBayWindow>(It);
				FArmyAutoCad::Get()->DrawCornerBayWindow(CadIndex, LayerName, CornerBayWindow);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

// 根据bim的layer绘制风烟道梁
bool FArmyDownloadCad::DrawCadHardware(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		if (It->GetType() == OT_Beam)
		{
			TSharedPtr<FArmyBeam> HardObj = StaticCastSharedPtr<FArmyBeam>(It);
			if (HardObj.IsValid())
			{
				FArmyAutoCad::Get()->DrawBeam(CadIndex, LayerName, HardObj);
			}
		}
		else if (It->GetType() == OT_AirFlue)
		{
			TSharedPtr<FArmyAirFlue> HardObj = StaticCastSharedPtr<FArmyAirFlue>(It);
			LayerName = GetLayerName(LT_Flue);
			if (HardObj.IsValid())
			{
				FArmyAutoCad::Get()->DrawAirFlue(CadIndex, LayerName, HardObj);
			}
		}
		else if (It->GetType() == OT_Pillar)
		{
			LayerName = GetLayerName(LT_Column);
			TSharedPtr<FArmyPillar> HardObj = StaticCastSharedPtr<FArmyPillar>(It);
			if (HardObj.IsValid())
			{
				FArmyAutoCad::Get()->DrawPillar(CadIndex, LayerName, HardObj);
			}
		}
		else if (It->GetType() == OT_BearingWall)
		{
			LayerName = GetLayerName(LT_Wall_Solid);
			TSharedPtr<FArmyBearingWall> HardObj = StaticCastSharedPtr<FArmyBearingWall>(It);
			if (HardObj.IsValid())
			{
				FArmyAutoCad::Get()->DrawHatchSolid(CadIndex, LayerName, HardObj->Polygon);
			}
		}
		else if (It->GetType() == OT_Entrance)
		{
			LayerName = GetLayerName(LT_Fp_Door);
			TSharedPtr<FArmyEntrance> Entrance = StaticCastSharedPtr<FArmyEntrance>(It);
			if (Entrance.IsValid())
			{
				FArmyAutoCad::Get()->DrawEntrance(CadIndex, LayerName, Entrance);
			}
		}
		else if (It->GetType() == OT_DownLeadLabel)
		{
			LayerName = GetLayerName(LT_Define_Text);
			TSharedPtr<FArmyDownLeadLabel> DownLeadLabel = StaticCastSharedPtr<FArmyDownLeadLabel>(It);
			if (DownLeadLabel.IsValid())
			{
				FArmyAutoCad::Get()->DrawDownLeadLabel(CadIndex, LayerName, DownLeadLabel);
			}
		}
		else if (It->GetType() == OT_TextLabel)
		{
			LayerName = GetLayerName(LT_Define_Text);
			TSharedPtr<FArmyTextLabel> TextLabel = StaticCastSharedPtr<FArmyTextLabel>(It);
			if (TextLabel.IsValid())
			{
				FArmyAutoCad::Get()->DrawTextLabel(CadIndex, LayerName, TextLabel);
			}
		}
		else if (It->GetType() == OT_Dimensions)
		{
			TSharedPtr<FArmyDimensions> Dimensions = StaticCastSharedPtr<FArmyDimensions>(It);
			if (Dimensions.IsValid() &&Dimensions->ClassType == -1)
			{
				LayerName = GetLayerName(LT_Define_Text);
				FArmyAutoCad::Get()->DrawDimensions(CadIndex, LayerName, Dimensions);
			}
		}
		else if (It->GetType() == OT_BoardSplitline)
		{
			TSharedPtr<FArmyBoardSplitline> BoardSplitline = StaticCastSharedPtr<FArmyBoardSplitline>(It);
			LayerName = GetLayerName(LT_Define_Text);
			FArmyAutoCad::Get()->DrawBoardSplitline(CadIndex, LayerName, BoardSplitline);
		}
		else if (It->GetType() == OT_PackPipe)
		{
			TSharedPtr<FArmyPackPipe>	PackPipe= StaticCastSharedPtr<FArmyPackPipe>(It);
			FArmyAutoCad::Get()->DrawPackPipe(CadIndex, LayerName, PackPipe);
		}
		else if (It->GetType() == OT_NewPass)  
		{
			// 开垭口当做拆除墙体绘制
			TSharedPtr<FArmyNewPass>	NewPass = StaticCastSharedPtr<FArmyNewPass>(It);

			TSharedPtr<FArmyRect> NewPassRect = NewPass->GetModifyRect();
			TSharedPtr<FArmyPolygon> NewPassPolygon = MakeShareable(new FArmyPolygon);
			if (!NewPassRect.IsValid())
			{
				continue;
			}

			for (auto VectorIt : NewPassRect->GetVertices())
			{
				NewPassPolygon->AddVertice(VectorIt);
			}

			// 0: 在原始墙体上 1：在新建墙体上
			if (NewPass->InWallType == 0 && GetCadDrawModel(CDM_DelWall))
			{
				// 绘制填充
				LayerName = GetLayerName(LT_Wall_Del_Solid);
				FArmyAutoCad::Get()->DrawHatch(CadIndex, LayerName, FILL_STYLE_3, HATCH_SCALE_2, NewPassPolygon);
				// 绘制线
				LayerName = GetLayerName(LT_Wall_Del);
				FArmyAutoCad::Get()->DrawPolygon(CadIndex, LayerName, NewPassPolygon);
			}
			
			if (!GetCadDrawModel(CDM_DelWall) && !GetCadDrawModel(CDM_AddWall))
			{
				if (NewPass->GetIfGeneratePassModel())
				{
					LayerName = GetLayerName(LT_Fp_Door);
					FArmyAutoCad::Get()->DrawNewPassEdge(CadIndex, LayerName, NewPass);
				}
			}
		}
		else if (It->GetType() == OT_Punch)
		{
			// 绘制开阳台
			TSharedPtr<FArmyPunch> Punch = StaticCastSharedPtr<FArmyPunch>(It);
			LayerName = GetLayerName(LT_Windows);
			FArmyAutoCad::Get()->DrawPunch(CadIndex, LayerName, Punch);
		}
	}

	return true;
}

// 根据bim的layer绘制点位
bool FArmyDownloadCad::DrawCadComponent(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		if (It->GetType() == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> Component = StaticCastSharedPtr<FArmyFurniture>(It);
			FArmyAutoCad::Get()->DrawComponent(CadIndex, LayerName, Component->ComponentType, Component);
		}
	}

	return true;
}

// 根据bim的layer绘制家具
bool FArmyDownloadCad::DrawCadFurniture(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	// 家具
	if (InClassType == Class_Furniture)
	{
		for (auto It : InObjItList)
		{
			if (It->GetType() == OT_ComponentBase)
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(It);
				// 添加到家具块管理器-也可以初始化添加
				FArmyAutoCad::Get()->AddFurniture(Furniture->ComponentID, Furniture);
				float angle = Furniture->LocalTransform.GetRotation().GetAngle();
				if (Furniture->LocalTransform.GetRotation().Z > 0)
				{
					angle = angle *-1;
				}
				FArmyAutoCad::Get()->DrawFurniture(CadIndex, LayerName, Furniture->ComponentID, Furniture->LocalTransform.GetLocation(), Furniture->LocalTransform.GetScale3D(), angle);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

// 根据bim的layer绘制标尺
bool FArmyDownloadCad::DrawCadDimensions(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		if (It->GetType() == OT_Dimensions ||
			It->GetType() == OT_InSideWallDimensions ||
			It->GetType() == OT_OutSideWallDimensions ||
			It->GetType() == OT_AddWallDimensions ||
			It->GetType() == OT_DeleteWallDimensions)
		{
			TSharedPtr<FArmyDimensions> Dimensions = StaticCastSharedPtr<FArmyDimensions>(It);
			if (Dimensions.IsValid())
			{
				FArmyAutoCad::Get()->DrawDimensions(CadIndex, LayerName, Dimensions);
			}
		}
		else if (It->GetType() == OT_TextLabel)
		{
			TSharedPtr<FArmyTextLabel> TextLabel = StaticCastSharedPtr<FArmyTextLabel>(It);
			if (TextLabel.IsValid())
			{
				FArmyAutoCad::Get()->DrawTextLabel(CadIndex, LayerName, TextLabel);
			}
		}
	}

	return true;
}

// 根据bim的layer绘制文本
bool FArmyDownloadCad::DrawCadTextLable(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		if (It->GetType() == OT_TextLabel)
		{
			TSharedPtr<FArmyTextLabel> TextLabel = StaticCastSharedPtr<FArmyTextLabel>(It);
			if (TextLabel.IsValid())
			{
				FArmyAutoCad::Get()->DrawTextLabel(CadIndex, LayerName, TextLabel);
			}
		}
	}

	return true;
}


// 根据bim的layer绘制地面数据
bool FArmyDownloadCad::DrawCadFloor(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));
	for (auto It : InObjItList)
	{
		if (!It.IsValid())
		{
			continue;
		}
		// 房间区域
		if (It->GetType() == OT_RoomSpaceArea)
		{
			// 绘制线框
			LayerName = GetLayerName(LT_Fp_Floor_M_Hatch);
			DrawCadWireFrame(CadIndex, LayerName, It);
			// 在图纸模式下绘制放样线条
			LayerName = GetLayerName(LT_Fp_Floor_M);
			DrawCadSkitLine(CadIndex, LayerName, It);
		}
		else if (It->GetType() == OT_RectArea)
		{
			// 绘制波打线
			TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(It);
			TSharedPtr<FArmyBodaArea> BodaArea = RectArea->GetBodaSurfaceArea();
			if (BodaArea.IsValid())
			{
				DrawCadBodaLine(CadIndex, LayerName,BodaArea);
			}

			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_PolygonArea)
		{
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_FreePolygonArea)
		{			
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_CircleArea)
		{
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_BodaArea)
		{
			TSharedPtr<FArmyBodaArea> BodaArea = StaticCastSharedPtr<FArmyBodaArea>(It);
			if (BodaArea.IsValid())
			{
				DrawCadBodaLine(CadIndex, LayerName, BodaArea);
			}
		}
	}

	return true;

}
// DrawCadRectBase内部方法，绘制线框
bool FArmyDownloadCad::DrawCadWireFrame(int32 CadIndex, FString InLayerName, TSharedPtr<FArmyObject> InObject)
{
	if (!InObject.IsValid())
	{
		return false;
	}

	if (!InObject->AsassignObj<FArmyBaseArea>())
	{
		return false;
	}

	TSharedPtr<FArmyBaseArea> BaseArea = StaticCastSharedPtr<FArmyBaseArea>(InObject);
	if (!BaseArea.IsValid())
	{
		return false;
	}

	TSharedPtr<class FArmyBaseEditStyle> MatStyle = BaseArea->GetMatStyle();
	if (!MatStyle.IsValid())
	{
		return false;
	}
	TMap<uint32, TArray<TSharedPtr<FArmyBrickUnit>>> ClipperBricks = MatStyle->GetClipperBricks();
	 
	for (auto& iter : ClipperBricks)
	{
		TArray<TSharedPtr<FArmyLine>>	lines;
		for (auto& iter0 : iter.Value)
		{
			TArray<TArray<FVector>> CutResults = iter0->GetCutResults();
			int number = CutResults.Num();
			for (int i = 0; i < number; ++i)
			{
				int number0 = CutResults[i].Num();
				for (int j = 0; j < number0; ++j)
				{
					TSharedPtr<FArmyLine>	Line = MakeShareable(new FArmyLine());
					Line->SetStart(CutResults[i][j%number0]);
					Line->SetEnd(CutResults[i][(j + 1) % number0]);
					lines.Add(Line);	 
				}
			}
		}
		if (!FArmyAutoCad::Get()->DrawLinesAsBlock(CadIndex, InLayerName, lines))
			return false;
	}
	return true;
}

// DrawCadFloor内部方法，绘制波打线
bool FArmyDownloadCad::DrawCadBodaLine(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyBodaArea> InBodaArea)
{
	if (!InBodaArea.IsValid())
		return false;

	TArray<TSharedPtr<FArmyBrickUnit>> TotoalBricks = InBodaArea->GetTotoalBricks();
	for (auto It : TotoalBricks)
	{
		TArray<TArray<FVector>> CutResults = It->GetCutResults();
		int number = CutResults.Num();
		for (int i = 0; i < number; ++i)
		{
			int number0 = CutResults[i].Num();
			for (int j = 0; j < number0; ++j)
			{
				FArmyAutoCad::Get()->DrawLine(CadIndex, InLayerName, CutResults[i][j%number0], CutResults[i][(j + 1) % number0]);
			}
		}
	}

	return true;
}

// DrawCadRectBase内部方法，绘制放样线条
bool FArmyDownloadCad::DrawCadSkitLine(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyObject> InObject)
{
	if (!InObject.IsValid())
	{
		return false;
	}

	TSharedPtr<FArmyBaseArea> BaseArea = StaticCastSharedPtr<FArmyBaseArea>(InObject);
	TArray<TArray<TArray<FVector>>> ConstructionSkitLineVerts = BaseArea->GetConstructionSkitLineVerts();
	
	if (ConstructionSkitLineVerts.Num() > 0)
	{
		// 0 表示地，1表示墙，2 表示顶
		if (BaseArea->SurfaceType == 0)
		{
			// 踢脚线两段线在门洞处闭合
			for (int i = 0; i < ConstructionSkitLineVerts[0].Num(); i++)
			{
				FArmyAutoCad::Get()->DrawLine(CadIndex, InLayerName, ConstructionSkitLineVerts[0][i][0], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][0]);
				FArmyAutoCad::Get()->DrawLine(CadIndex, InLayerName, ConstructionSkitLineVerts[0][i][ConstructionSkitLineVerts[0][i].Num() - 1], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][ConstructionSkitLineVerts[0][i].Num() - 1]);
			}
		}
		

		for (TArray<TArray<FVector>>& iter0 : ConstructionSkitLineVerts)
		{
			if ((iter0.Num() == 1) && (BaseArea->RoomDataStatus & FArmyBaseArea::RoomStatus::FLAG_CLOSESKITLINE))
			{
				TArray<FVector>& skitVerts = iter0[0];
				int number = skitVerts.Num();
				for (int i = 0; i < number; ++i)
				{
					FArmyAutoCad::Get()->DrawLine(CadIndex, InLayerName, skitVerts[i%number], skitVerts[(i + 1) % number]);
				}
			}
			else
			{
				for (TArray<FVector>& iter : iter0)
				{
					int number = iter.Num();
					for (int i = 0; i < number - 1; ++i)
					{
						FArmyAutoCad::Get()->DrawLine(CadIndex, InLayerName, iter[i], iter[i + 1]);
					}
				}
			}
		}

	}

	return true;
}

// 根据bim的layer绘制顶面数据
bool FArmyDownloadCad::DrawCadCeiling(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));
	for (auto It : InObjItList)
	{
		if (!It.IsValid())
		{
			continue;
		}
		// 房间区域
		if (It->GetType() == OT_RoomSpaceArea)
		{
			// 如果不绘制顶面除灯带外的其他东西，直接停止绘制
			if (!GetCadDrawModel(CDM_OtherCeilingObj))
			{
				continue;
			}
			// 在图纸模式下绘制放样线条
			DrawCadSkitLine(CadIndex, LayerName, It);

			if (!It->AsassignObj<FArmyRoomSpaceArea>())
			{
				continue;
			}

			TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(It);
			TSharedPtr<class FArmyBaseEditStyle> MatStyle = RoomSpaceArea->GetMatStyle();
			if (!MatStyle.IsValid())
			{
				continue;
			}
			// @是顶&&无缝铺 
			if (MatStyle->GetEditType() == S_SeamlessStyle)
			{
				//&&进行了下吊，斜线绘制下吊区域
				LayerName = GetLayerName(LT_Ceil_Hatch);
				DrawCadCeilingFill(CadIndex, LayerName, It);
			}
			else
			{
				// 绘制线框
				DrawCadWireFrame(CadIndex, LayerName, It);
			}
		}
		else if (It->GetType() == OT_RectArea)
		{
			// 绘制波打线
			TSharedPtr<FArmyRectArea> RectArea = StaticCastSharedPtr<FArmyRectArea>(It);
			TSharedPtr<FArmyBodaArea> BodaArea = RectArea->GetBodaSurfaceArea();
			if (BodaArea.IsValid())
			{
				DrawCadBodaLine(CadIndex, LayerName, BodaArea);
			}

			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_PolygonArea)
		{
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_FreePolygonArea)
		{
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_CircleArea)
		{
			// 绘制共通区域数据
			DrawCadRectBase(CadIndex, InClassType, It);
		}
		else if (It->GetType() == OT_BodaArea)
		{
			TSharedPtr<FArmyBodaArea> BodaArea = StaticCastSharedPtr<FArmyBodaArea>(It);
			if (BodaArea.IsValid())
			{
				DrawCadBodaLine(CadIndex, LayerName, BodaArea);
			}
		}
	}
	return true;
}

// 根据bim的layer绘制顶面填充
bool FArmyDownloadCad::DrawCadCeilingFill(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyObject> InObject)
{
	if (!InObject.IsValid())
	{
		return false;
	}

	TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(InObject);
	//&&进行了下吊，斜线绘制下吊区域
	if (RoomSpaceArea->GetExtrusionHeight() != 0.0f)
	{
		// 吊顶填充绘制
		TArray<TSharedPtr<FArmyPolygon>> PolygonList;
		if (!RoomSpaceArea->GetOutArea().IsValid())
		{
			return false;
		}

		// 生成最外围的多边形轮廓
		TSharedPtr<FArmyPolygon> PolygonObj = MakeShareable(new FArmyPolygon);
		for (auto It : RoomSpaceArea->GetOutArea()->Vertices)
		{
			PolygonObj->AddVertice(RoomSpaceArea->PlaneToTranlate(It));
		}
		PolygonList.Add(PolygonObj);

		// 生成内部的多边形轮廓
		for (auto EditBase : RoomSpaceArea->GetEditAreas())
		{
			if (EditBase.IsValid())
			{
				TSharedPtr<FArmyPolygon> PolygonIt = MakeShareable(new FArmyPolygon);
				for (auto It2 : EditBase->GetOutArea()->Vertices)
				{
					PolygonIt->AddVertice(EditBase->PlaneToTranlate(It2));
				}
				PolygonList.Add(PolygonIt);
			}
		}

		FArmyAutoCad::Get()->DrawHatch(CadIndex, InLayerName, FILL_STYLE_2,  HATCH_SCALE_4, PolygonList);
		
		//填充天花轮廓线
		for (auto Polygon : PolygonList)
		{
			InLayerName = GetLayerName(LT_Ceiling);
			FArmyAutoCad::Get()->DrawPolygon(CadIndex, InLayerName, Polygon);
		} 
	}
	return true;
}
// DrawCadCeiling内部方法-共通绘制
bool FArmyDownloadCad::DrawCadRectBase(int32 CadIndex, ObjectClassType InClassType, TSharedPtr<class FArmyObject> InObject)
{
	if (!InObject.IsValid())
	{
		return false;
	}

	if (!InObject->AsassignObj<FArmyBaseArea>())
	{
		return false;
	}

	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));
	TSharedPtr<FArmyBaseArea> BaseArea = StaticCastSharedPtr<FArmyBaseArea>(InObject);
	// 绘制灯带
	if (BaseArea->SurfaceType == 2 && GetCadDrawModel(CDM_LampStrip))
	{
		TArray<FVector> LampLightDashLines = BaseArea->GetLampLightDashLines();

		int number = LampLightDashLines.Num();
		for (int i = 0; i < number; ++i)
		{
			LayerName = GetLayerName(LT_Ceil_Light);
			FArmyAutoCad::Get()->DrawLine(CadIndex, LayerName, LampLightDashLines[i%number], LampLightDashLines[(i + 1) % number]);
		}
	}

	//如果不绘制顶面除灯带外的其他东西，直接停止绘制
	if(BaseArea->SurfaceType == 2 && !GetCadDrawModel(CDM_OtherCeilingObj))
	{
		return true;
	}
	

	// @是顶&&无缝铺 
	TSharedPtr<class FArmyBaseEditStyle> MatStyle = BaseArea->GetMatStyle();
	if (!MatStyle.IsValid())
	{
		return false;
	}
	if(BaseArea->SurfaceType == 2 && MatStyle->GetEditType() == S_SeamlessStyle)
	{
		//&&进行了下吊  ，斜线绘制下吊区域
		if (BaseArea->GetExtrusionHeight() != 0.0f)
		{
			//DrawRoof(PDI, View);
		}
			
	}
	else
	{
		if (InClassType == Class_Floorlayout)
		{
			LayerName = GetLayerName(LT_Fp_Floor_M_Hatch);
		}

		if (InClassType == Class_Ceilinglayout)
		{
			LayerName = GetLayerName(LT_Ceil_Hatch);
		}

		DrawCadWireFrame(CadIndex, LayerName, InObject);
	}

	// DrawCadRectBase内部方法，绘制放样线条
	DrawCadSkitLine(CadIndex, LayerName, InObject);

	return true;
}

// 根据bim的layer绘制灯控线
bool FArmyDownloadCad::DrawCadLampControlLines(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		if (It->GetType() == OT_LampControlLine)
		{
			LayerName = GetLayerName(LT_Fp_Electrical);
			TSharedPtr<FArmyLampControlLines> LampControlLines = StaticCastSharedPtr<FArmyLampControlLines>(It);
			if (LampControlLines.IsValid())
			{
				FArmyAutoCad::Get()->DrawLampControlLines(CadIndex, LayerName, LampControlLines);
			}
		}
	}

	return true;
}

// 绘制定制柜类
bool FArmyDownloadCad::DrawCadSCTCabinet(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));
	for (auto It : InObjItList)
	{
		// 绘制柜体
		if (It->GetType() == OT_ComponentBase)
		{
			// 获得柜体类型，分别放置不同图层
			LayerName = GetLayerName(LT_Floor_Cabinet_Line);
			TSharedPtr<FArmyFurniture> Component = StaticCastSharedPtr<FArmyFurniture>(It);
			FArmyAutoCad::Get()->DrawCabinet(CadIndex, LayerName,Component);
		}
		// 绘制索引图标
		else if (It->GetType() == OT_Symbol)
		{
			LayerName = GetLayerName(LT_Cabinet_Symbol);
			TSharedPtr<FArmySymbol> SctSymbol = StaticCastSharedPtr<FArmySymbol>(It);
			FArmyAutoCad::Get()->DrawSymbol(CadIndex, LayerName, SctSymbol);
		}
		// 绘制台面外围轮廓
		else if (It->GetType() == OT_CupboardTable)
		{
			LayerName = GetLayerName(LT_Cabinet_Countertops);
			TSharedPtr<FArmyWHCTableObject> WHCTableObject = StaticCastSharedPtr<FArmyWHCTableObject>(It);
			// 绘制基本轮廓线
			TArray<TSharedPtr<FArmyLine>> DrawLines;
			WHCTableObject->GetCadLines(DrawLines, 0);
			for (auto It : DrawLines)
			{
				FArmyAutoCad::Get()->DrawLine(CadIndex, LayerName, It);
			}

			// 前轮廓线-虚线
			if (WHCTableObject->GetIsDrawFrontOutLine())
			{
				DrawLines.Empty();
				WHCTableObject->GetCadLines(DrawLines, 1);
				TSharedPtr<LayerInfo> CurrentLayer = MakeShareable(new LayerInfo(LayerName, FColor(0XFFFF0000), LineType_02, LW_009));
				for (auto It : DrawLines)
				{
					FArmyAutoCad::Get()->DrawLine(CadIndex, CurrentLayer, It);
				}
			}

			// 后轮廓线-实线
			if (WHCTableObject->GetIsDrawBackOutLine())
			{
				DrawLines.Empty();
				WHCTableObject->GetCadLines(DrawLines, 2);
				for (auto It : DrawLines)
				{
					FArmyAutoCad::Get()->DrawLine(CadIndex, LayerName, It);
				}
			}

			// 电器构件在台面图上
			if (WHCTableObject->GetIsDrawComponent())
			{
				LayerName = GetLayerName(LT_Fp_Kitchen_Furn);
				TArray<FArmyWHCTableObject::FComponentInfo> ComponentInfoArray = WHCTableObject->GetDrawComponentArray();
				for (auto It : ComponentInfoArray)
				{
					FArmyAutoCad::Get()->DrawWHCComponent(CadIndex, LayerName, It);
					// 绘制电器虚线
					TSharedPtr<LayerInfo> CurrentLayer = MakeShareable(new LayerInfo(LayerName, FColor(0XFFFF0000), LineType_02, LW_009));
					FArmyAutoCad::Get()->DrawLine(CadIndex, CurrentLayer, It.CenterLine.DrawLine);
				}
			}

		}
		// 绘制封板
		else if (It->GetType() == OT_UserDefine || 
			     It->GetType() == OT_CabinetFace ||
			     It->GetType() == OT_DoorSheetFace ||
				 It->GetType() == OT_CupboardTableFace ||
			     It->GetType() == OT_SunBoardFace ||
			     It->GetType() == OT_ResizeFace ||
			     It->GetType() == OT_TopLineFace ||
			     It->GetType() == OT_FootLineFace)
		{
			LayerName = GetLayerName(LT_Floor_Cabinet_Solid);
			if (It->AsassignObj<FArmyCustomDefine>())
			{
				TSharedPtr<FArmyCustomDefine> CustomDefine = StaticCastSharedPtr<FArmyCustomDefine>(It);
				FArmyAutoCad::Get()->DrawCustomDefine(CadIndex, LayerName, CustomDefine);
			}	
		}
	}

	return true;
}

// 绘制定制五金/电器类
bool FArmyDownloadCad::DrawCadSCTHardware(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList)
{
	FString LayerName = GetLayerName(GetLayerTypeByObjectClassType(InClassType));

	for (auto It : InObjItList)
	{
		// 绘制电器
		if (It->GetType() == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> Component = StaticCastSharedPtr<FArmyFurniture>(It);
			FArmyAutoCad::Get()->DrawComponent(CadIndex, LayerName, Component->ComponentType, Component);
		}
	}

	return true;
}
#pragma endregion 绘制相关的私有方法

/////////////////////////////// 数据相关的私有方法////////////////////
#pragma region 数据相关的私有方法

// 初始化图纸的集合
void FArmyDownloadCad::InitBimLayerArray(const TArray<FName>& CadChosenList)
{
	BimLayerArray.Empty();
	if (!LayerManager.IsValid())
	{
		return;
	}

	for (auto It : CadChosenList)
	{
		TWeakPtr<FArmyLayer> LayerObj = LayerManager->GetLayer(It);
		if (LayerObj.Pin().IsValid())
		{
		    BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
		}
	}

	//获取指定bim图纸下的全部Class
	//TArray<TSharedPtr<FArmyLayer>> ConstructLayerList;
	//LayerManager->GetLayerMap().GenerateValueArray(ConstructLayerList);
	//for (auto It : ConstructLayerList)
	//{
	//	FString layerName = It->GetName().ToString();
	//	//if (layerName.Contains(TEXT("quanzhu")))
	//	if (layerName.Contains("橱柜"))
	//	{
	//		continue;
	//		//BimLayerArray.Add(It);
	//	}
	//	else
	//	{
	//		BimLayerArray.Add(It);
	//		//continue;
	//	}
	//}

	/*TWeakPtr<FArmyLayer> LayerObj = LayerManager->GetLayer(TEXT("天花布置图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}
	LayerObj = LayerManager->GetLayer(TEXT("地面铺装图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}

	LayerObj = LayerManager->GetLayer(TEXT("新建墙体图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}

	LayerObj = LayerManager->GetLayer(TEXT("平面布局图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}

	LayerObj = LayerManager->GetLayer(TEXT("地面铺装图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}
	LayerObj = LayerManager->GetLayer(TEXT("天花布置图"));
	if (LayerObj.Pin().IsValid())
	{
		BimLayerArray.Add(StaticCastSharedPtr<FArmyLayer>(LayerObj.Pin()));
	}*/

}
// 初始化图层信息
void FArmyDownloadCad::InitLayersMap()
{
	LayersMap.Add(LT_Default, MakeShareable(new LayerInfo(TEXT("0"), FColor(0XFFFFFFFF), LineType_01, LW_000)));
	LayersMap.Add(LT_Wall_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-承重墙填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Wall, MakeShareable(new LayerInfo(TEXT("DBJ-原始建筑墙体"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	LayersMap.Add(LT_Windows, MakeShareable(new LayerInfo(TEXT("DBJ-窗"), FColor(0XFF0000FF), LineType_01, LW_015)));
	LayersMap.Add(LT_LL, MakeShareable(new LayerInfo(TEXT("DBJ-顶面梁"), FColor(0XFFFF0000), LineType_02, LW_015)));
	LayersMap.Add(LT_Column, MakeShareable(new LayerInfo(TEXT("DBJ-柱"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	LayersMap.Add(LT_Column_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-柱填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Flue, MakeShareable(new LayerInfo(TEXT("DBJ-风道"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	LayersMap.Add(LT_Flue_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-风道填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Lvtry, MakeShareable(new LayerInfo(TEXT("DBJ-下水主管道"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Potential, MakeShareable(new LayerInfo(TEXT("DBJ-强弱电点位"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Water, MakeShareable(new LayerInfo(TEXT("DBJ-水路点位"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Utilities, MakeShareable(new LayerInfo(TEXT("DBJ-燃气点位"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Text, MakeShareable(new LayerInfo(TEXT("DBJ-原始标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Wall_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-内墙标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Grid_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-外墙标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Ceiling_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-天花标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Windows_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-门窗标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Potential_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-强弱电点位标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Water_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-水路点位标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Gas_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-燃气点位标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Define_Text, MakeShareable(new LayerInfo(TEXT("DBJ-自定义标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Title_Gdg, MakeShareable(new LayerInfo(TEXT("DBJ-图框"), FColor(0XFF00FF00), LineType_01, LW_020)));
	LayersMap.Add(LT_Wall_Del, MakeShareable(new LayerInfo(TEXT("DBJ-拆除墙体"), FColor(0XFF00FF00), LineType_01, LW_020)));
	LayersMap.Add(LT_Wall_Del_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-拆除墙体填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Wall_Del_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-拆除墙标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Wall_Add, MakeShareable(new LayerInfo(TEXT("DBJ-新建墙体"), FColor(0XFFFFFF00), LineType_01, LW_025)));
	LayersMap.Add(LT_Wall_Add_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-新建墙体填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Wall_Add_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-新建墙标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Door, MakeShareable(new LayerInfo(TEXT("DBJ-门"), FColor(0XFF00FF00), LineType_01, LW_020)));
	LayersMap.Add(LT_Fp_Furn, MakeShareable(new LayerInfo(TEXT("DBJ-平面活动家具"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Toilet_Furn, MakeShareable(new LayerInfo(TEXT("DBJ-洁具"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Light, MakeShareable(new LayerInfo(TEXT("DBJ-平面灯具"), FColor(0XFFFF00FF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Curtain, MakeShareable(new LayerInfo(TEXT("DBJ-窗帘"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Furn_Fl, MakeShareable(new LayerInfo(TEXT("DBJ-固定家具"), FColor(0XFF00FF00), LineType_01, LW_020)));
	LayersMap.Add(LT_Fp_Kitchen_Furn, MakeShareable(new LayerInfo(TEXT("DBJ-厨房电器"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Ceil_Light, MakeShareable(new LayerInfo(TEXT("DBJ-天花灯具"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Floor_M_Hatch, MakeShareable(new LayerInfo(TEXT("DBJ-地面拼花填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Fp_Floor_M, MakeShareable(new LayerInfo(TEXT("DBJ-地面拼花线"), FColor(0XFF0000FF), LineType_01, LW_015)));
	LayersMap.Add(LT_Wall_Hatch, MakeShareable(new LayerInfo(TEXT("DBJ-墙面造型"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Ceiling, MakeShareable(new LayerInfo(TEXT("DBJ-天花结构线"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_Ceil_Hatch, MakeShareable(new LayerInfo(TEXT("DBJ-天花填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Fp_Electrical, MakeShareable(new LayerInfo(TEXT("DBJ-开关及连线"), FColor(0XFFFF0000), LineType_01, LW_015)));
	LayersMap.Add(LT_FP_Furn_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-家具尺寸标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Ceil_Light_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-灯具标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	LayersMap.Add(LT_Fp_Electrical_Dim, MakeShareable(new LayerInfo(TEXT("DBJ-开关标注"), FColor(0XFF00FFFF), LineType_01, LW_015)));
	
	//LayersMap.Add(LT_Wall_Cabinet_Line, MakeShareable(new LayerInfo(TEXT("DBJ-吊柜线条"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	//LayersMap.Add(LT_Wall_Cabinet_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-吊柜填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Floor_Cabinet_Line, MakeShareable(new LayerInfo(TEXT("DBJ-柜体线条"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	LayersMap.Add(LT_Floor_Cabinet_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-柜体填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	//LayersMap.Add(LT_High_Cabinet_Line, MakeShareable(new LayerInfo(TEXT("DBJ-高柜线条"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	//LayersMap.Add(LT_High_Cabinet_Solid, MakeShareable(new LayerInfo(TEXT("DBJ-高柜填充"), FColor(0XFFBFBFBF), LineType_01, LW_009)));
	LayersMap.Add(LT_Board_Splitline, MakeShareable(new LayerInfo(TEXT("DBJ-分割线"), FColor(0XFF0000FF), LineType_01, LW_015)));
	LayersMap.Add(LT_Cabinet_Countertops, MakeShareable(new LayerInfo(TEXT("DBJ-柜体台面"), FColor(0XFFFFFFFF), LineType_01, LW_035)));
	LayersMap.Add(LT_Cabinet_Symbol, MakeShareable(new LayerInfo(TEXT("DBJ-柜体索引号"), FColor(0XFF00FFFF), LineType_01, LW_015)));

}

// 获得图层的名字通过编号
FString FArmyDownloadCad::GetLayerName(int32 LayerType)
{
	FString LayerName = "0";
	if (LayersMap.Contains(LayerType))
	{
		LayerName = (*LayersMap.Find(LayerType))->Name;
	}

	return LayerName;
}

//// 根据bim的图纸获得CAD的图纸
//ECadFrameType FArmyDownloadCad::GetCadTypeByLayer(int32 Layerindex)
//{
//	return (ECadFrameType)Layerindex;
//}

// 根据BIm的图层获得CAD的图层
ELayerType FArmyDownloadCad::GetLayerTypeByObjectClassType(ObjectClassType InClassType)
{
	ELayerType CadLayerType = LT_Default;
	switch (InClassType)
	{
	case Class_Begin:
		break;
	case Class_BaseWall:
		CadLayerType = LT_Wall;
		break;
	case Class_AddWall:
		CadLayerType = LT_Wall_Add_Solid;
		break;
	case Class_BreakWall:
		CadLayerType = LT_Wall_Del_Solid;
		break;
	case Class_Door:
		CadLayerType = LT_Fp_Door;
		break;
	case Class_Pass:
		CadLayerType = LT_Fp_Door;
		break;
	case Class_Window://窗
		CadLayerType = LT_Windows;
		break;
	case Class_Furniture:
		CadLayerType = LT_Fp_Furn;
		break;
	case Class_Switch: // 开关
	case Class_Socket: // 插座
	case Class_ElectricBoxH: // 强电箱
	case Class_ElectricBoxL: // 弱电箱
		CadLayerType = LT_Fp_Potential;
		break;
	case Class_Light:
		CadLayerType = LT_Ceil_Light;
		break;
	case Class_AirConditionerPoint:
		break;
	case Class_HotWaterPoint://热水点位
	case Class_HotAndColdWaterPoint://冷热水点位
	case Class_ColdWaterPoint://冷水点位
	case Class_FloordRainPoint://地漏下水
	case Class_ClosestoolRainPoint://马桶下水
	case Class_WashBasinRainPoint://水盆下水
	case Class_RecycledWaterPoint://中水点位
	case Class_HomeEntryWaterPoint://进户水
	case Class_WaterRoutePoint:// 水路点位
		CadLayerType = LT_Fp_Water;
		break;
	case Class_ElectricWire: // 灯控线
		CadLayerType = LT_Fp_Electrical;
		break;
	case Class_AnnotationInSide: // 内墙标注
		CadLayerType = LT_Wall_Dim;
		break;
	case Class_AnnotationOutSide:
		CadLayerType = LT_Grid_Dim;
		break;
	case Class_AnnotationDoor:// 门窗标注
	case Class_AnnotationWindow:
		CadLayerType = LT_Windows_Dim;
		break;
	case Class_AnnotationElectricBoxH:
	case Class_AnnotationElectricBoxL:
		CadLayerType = LT_Fp_Potential_Dim;
		break;
	case Class_AnnotationAirConditionerPoint: // 空调点位标注
		break;
	case Class_AnnotationFurniture: // 家具家电标注
		CadLayerType = LT_FP_Furn_Dim;
		break;
	case Class_AnnotationSwitch: // 开关
		CadLayerType = LT_Fp_Electrical_Dim;
		break;
	case Class_AnnotationSocket: // 插座
		CadLayerType = LT_Fp_Potential_Dim;
		break;
	case Class_AnnotationLamp: // 灯具标注
		CadLayerType = LT_Ceil_Light_Dim;
		break;
	case Class_SpaceName: // 空间名称
		CadLayerType = LT_Text;
		break;
	case Class_Floorlayout:// 地面造型
		CadLayerType = LT_Fp_Floor_M;
		break;
	case Class_Walllayout: // 墙面造型
		CadLayerType = LT_Wall_Hatch;
		break;
	case Class_ConstructionFrame: // 图框
		CadLayerType = LT_Title_Gdg;
		break;
	case Class_Flag34: // 新建拆除后实墙效果
		break;
	case Class_SpaceArea:
	case Class_SpacePerimeter:
	case Class_SpaceHeight:
		CadLayerType = LT_Text;
		break;
	case Class_AddWallPost: // 新建后效果
		break;
	case Class_DeleteWallPost: // 拆除后效果
		break;
	case Class_AnnotationDeleteWall:// 拆除墙标注
		CadLayerType = LT_Wall_Del_Dim;
		break;
	case Class_AnnotationAddWall: // 新建墙标注
		CadLayerType = LT_Wall_Add_Dim;
		break;
	case Class_LampStrip: // 灯带
		CadLayerType = LT_Ceil_Light;
		break;
	case Class_Ceilinglayout: // 顶面造型
		CadLayerType = LT_Ceiling;
		break;
	case Class_AnnotationCeilingObj:// 吊顶标注
		CadLayerType = LT_Ceiling_Dim;
		break;
	case Class_Beam: // 梁
		CadLayerType = LT_LL;
		break;
	case Class_Socket_H:// 强电插座
	case Class_Socket_L:// 强电插座
		CadLayerType = LT_Fp_Potential;
		break;
	case Class_DiversityWater: // 分集水器
		CadLayerType = LT_Fp_Water;
		break;
	case Class_WaterPipe: // 下水主管道
		CadLayerType = LT_Lvtry;
		break;
	case Class_Heater: // 暖气
		CadLayerType = LT_Fp_Water;
		break;
	case Class_GasMeter:// 燃气表
	case Class_GasPipe:// 燃气主管
		CadLayerType = LT_Fp_Utilities;
		break;
	case Class_AnnotationCurtainBox:// 窗帘盒标注
		break;
	case Class_AnnotationBeam:// 梁标注
		CadLayerType = LT_Ceiling_Dim;
		break;
	case Class_AnnotationElectricPoint:// 电路点位标注
	case Class_AnnotationSocket_H:// 强电插座标注
	case Class_AnnotationSocket_L:
		CadLayerType = LT_Fp_Potential_Dim;
		break;
	case Class_AnnotationHotWaterPoint://热水点位标注
	case Class_AnnotationHotAndColdWaterPoint://冷热水点位标注
	case Class_AnnotationColdWaterPoint://冷水点位标注
	case Class_AnnotationFloordRainPoint://地漏下水标注
	case Class_AnnotationClosestoolRainPoint://马桶下水标注
	case Class_AnnotationWashBasinRainPoint://水盆下水标注
	case Class_AnnotationRecycledWaterPoint://中水点位标注
	case Class_AnnotationHomeEntryWaterPoint://进户水标注
	case Class_AnnotationWaterPoint://水路点位标注
	case Class_AnnotationSewerPipe:// 下水管道标注
	case Class_AnnotationHeater:// 暖气标注
		CadLayerType = LT_Fp_Water_Dim;
		break;
	case Class_AnnotationGas:// 燃气点位标注
		CadLayerType = LT_Fp_Gas_Dim;
		break;
	case Class_SCTCabinet:// 定制柜类
		CadLayerType = LT_Fp_Furn_Fl;
		break;
	case Class_SCTHardware:// 定制五金/电器类
		CadLayerType = LT_Fp_Kitchen_Furn;
		break;
	case Class_End:
		break;
	default:
		break;
	}

	return CadLayerType;
}

// 生成拆改后的户型数据
TArray<TSharedPtr<FArmyLine>> FArmyDownloadCad::GenerateChangedData(EModelType Modeltype/* = EModelType::E_HomeModel*/)
{
	TArray<TSharedPtr<FArmyLine>> TargetHomeModelLineList, TempHardwareList, TempRoomLineList;
	TargetHomeModelLineList = TempRoomLineList = GetHomeModelData(Modeltype);
	TempHardwareList = GetHardwareData(Modeltype);

	for (TSharedPtr<FArmyLine> Hardware : TempHardwareList)
	{
		bool IsOnLine = false;
		for (TSharedPtr<FArmyLine> RoomLine : TempRoomLineList)
		{
			FVector ClosestToStartPos = FMath::ClosestPointOnLine(RoomLine->GetStart(), RoomLine->GetEnd(), Hardware->GetStart());
			FVector ClosestToEndPos = FMath::ClosestPointOnLine(RoomLine->GetStart(), RoomLine->GetEnd(), Hardware->GetEnd());
			bool bStart = (ClosestToStartPos - Hardware->GetStart()).Size() < MIN_PRECISION_NUMBER;
			bool bEnd = (ClosestToEndPos - Hardware->GetEnd()).Size() < MIN_PRECISION_NUMBER;
			if (bStart && bEnd)
			{
				IsOnLine = true;

				if (TargetHomeModelLineList.Contains(RoomLine))
				{
					TargetHomeModelLineList.Remove(RoomLine);

					TempRoomLineList.Remove(RoomLine);
				}
				const FVector CenterPos = (ClosestToStartPos + ClosestToEndPos)*0.5f;
				if (((CenterPos - RoomLine->GetStart()).GetSafeNormal() - (CenterPos - ClosestToStartPos).GetSafeNormal()).Size() < MIN_PRECISION_NUMBER)
				{
					TSharedPtr<FArmyLine> RoomToClosestStartLine = MakeShareable(new FArmyLine(RoomLine->GetStart(), ClosestToStartPos));
					TSharedPtr<FArmyLine> RoomToClosestEndLine = MakeShareable(new FArmyLine(RoomLine->GetEnd(), ClosestToEndPos));

					TargetHomeModelLineList.AddUnique(RoomToClosestStartLine);
					TargetHomeModelLineList.AddUnique(RoomToClosestEndLine);

					TempRoomLineList.AddUnique(RoomToClosestStartLine);
					TempRoomLineList.AddUnique(RoomToClosestEndLine);
				}
				else
				{
					TSharedPtr<FArmyLine> RoomToClosestEndLine = MakeShareable(new FArmyLine(RoomLine->GetStart(), ClosestToEndPos));
					TSharedPtr<FArmyLine> RoomToClosestStartLine = MakeShareable(new FArmyLine(RoomLine->GetEnd(), ClosestToStartPos));

					TargetHomeModelLineList.AddUnique(RoomToClosestEndLine);
					TargetHomeModelLineList.AddUnique(RoomToClosestStartLine);

					TempRoomLineList.AddUnique(RoomToClosestEndLine);
					TempRoomLineList.AddUnique(RoomToClosestStartLine);
				}

				break;
			}
		}
		if (!IsOnLine)
		{
			TargetHomeModelLineList.AddUnique(MakeShareable(new FArmyLine(Hardware->GetStart(), Hardware->GetEnd())));
		}
	}

	return MoveTemp(TargetHomeModelLineList);
}

// 获取原始户型数据
TArray<TSharedPtr<FArmyLine>> FArmyDownloadCad::GetHomeModelData(EModelType Modeltype)
{
	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(Modeltype, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(Modeltype, OT_OutRoom, RoomList);
	
	TArray<TSharedPtr<FArmyLine>> RoomLineList;
	for (FObjectWeakPtr WeakPtrObj : RoomList)
	{
		WeakPtrObj.Pin()->GetLines(RoomLineList);
	}
	
	return RoomLineList;
}

// 获取飘窗、门洞数据-中间函数
TArray<TSharedPtr<FArmyLine>> FArmyDownloadCad::GetHardwareData(EModelType Modeltype)
{
	TArray<TSharedPtr<FArmyLine>> ResLineList;

	TArray<FObjectWeakPtr> HomeModelObjectList;
	HomeModelObjectList.Append(FArmySceneData::Get()->GetObjects(Modeltype));
		
	for (FObjectWeakPtr WeakPtrObj : HomeModelObjectList)
	{
		switch (WeakPtrObj.Pin()->GetType())
		{
		case OT_Beam:
		{
			break;
		}
		case OT_DoorHole:
		{
			if (Modeltype != EModelType::E_HomeModel)//拆改后户型不需要拆改门洞
			{
				break;
			}
		}
		case OT_Pass:
		{
			FArmyPass* fPass = (FArmyPass*)WeakPtrObj.Pin()->AsassignObj<FArmyHardware>();
			if(fPass&&fPass->GetIfFillPass())
				break;
		}
		case OT_NewPass:
		case OT_SecurityDoor:
		case OT_Door:
// 		{
// 			FArmySingleDoor* Door = WeakPtrObj.Pin()->AsassignObj<FArmySingleDoor>();
// 			if (Door)
// 			{
// 				if (Door->bHasConnectPass)
// 				{
// 					break;
// 				}
// 			}
// 		}
		case OT_SlidingDoor:
		case OT_Window:
		case OT_FloorWindow:
		case OT_RectBayWindow:
		case OT_TrapeBayWindow:
		case OT_CornerBayWindow:
		{
			FArmyHardware* Window = WeakPtrObj.Pin()->AsassignObj<FArmyHardware>();
			//TSharedPtr<FArmyHardware> Window = WeakPtrObj.Pin();
			if (Window)
			{
				const FVector StartPos = Window->GetStartPos();
				const FVector EndPos   = Window->GetEndPos();
				const FVector WidthDirection = FVector::CrossProduct(StartPos - EndPos, FVector::UpVector).GetSafeNormal();
				const FVector CornerPos1 = StartPos + WidthDirection*Window->GetWidth()*0.5f;
				const FVector CornerPos2 = StartPos - WidthDirection*Window->GetWidth()*0.5f;
				const FVector CornerPos3 = EndPos - WidthDirection*Window->GetWidth()*0.5f;
				const FVector CornerPos4 = EndPos + WidthDirection*Window->GetWidth()*0.5f;
					
				ResLineList.Append
				({
					MakeShareable(new FArmyLine(CornerPos1,CornerPos2)),
					MakeShareable(new FArmyLine(CornerPos2,CornerPos3)),
					MakeShareable(new FArmyLine(CornerPos3,CornerPos4)),
					MakeShareable(new FArmyLine(CornerPos4,CornerPos1)),
				});
			}
			break;
		}
		default:
			break;
		}
	}
		
	return ResLineList;
}


// 初始化绘图模式通过bim图层
void FArmyDownloadCad::InitCadDrawModel(TSharedPtr<class FArmyLayer> BimLayer)
{
	if (!BimLayer.IsValid())
	{
		return;
	}

	CadModeArray.Empty();
	SetCadDrawModel(CDM_Normal);

	if (BimLayer->GetLayerType() == 1)
	{

	}
	//新建墙
	if (BimLayer->GetOrCreateClass(Class_AddWall)->IsVisible())
	{
		SetCadDrawModel(CDM_AddWall,true);
	}
	else
	{
		SetCadDrawModel(CDM_AddWall, false);
	}

	//删除墙
	if (BimLayer->GetOrCreateClass(Class_BreakWall)->IsVisible())
	{
		SetCadDrawModel(CDM_DelWall, true);
	}
	else
	{
		SetCadDrawModel(CDM_DelWall, false);
	}

	//没有门则显示门洞
	TSharedPtr<FArmyClass> ClassPtr = BimLayer->GetOrCreateClass(Class_Door);
	if (ClassPtr.IsValid() && ClassPtr->IsVisible())
	{
		SetCadDrawModel(CDM_DoorPass, false);
	}
	else
	{
		SetCadDrawModel(CDM_DoorPass,true);
	}

	// 设置灯带是否绘制
	SetCadDrawModel(CDM_LampStrip, BimLayer->GetFlag() & MODE_LAMPSTRIP ? true : false);
	SetCadDrawModel(CDM_OtherCeilingObj, BimLayer->GetFlag() & MODE_OTHERCEILINGOBJ ? true : false);

}
// 获取绘制模式
bool FArmyDownloadCad::GetCadDrawModel(ECadDrawMode InType)
{
	if (CadModeArray.Contains(InType))
	{
		return true;
	}
	else
	{
		return false;
	}
}
// 设置绘制模式
void FArmyDownloadCad::SetCadDrawModel(ECadDrawMode InType, bool InUse)
{
	if (InUse)
	{
		if (!GetCadDrawModel(InType))
		{
			CadModeArray.Add(InType);
		}
	}
	else
	{
		if (GetCadDrawModel(InType))
		{
			CadModeArray.Remove(InType);
		}
	}
}

#pragma endregion 数据相关的私有方法





