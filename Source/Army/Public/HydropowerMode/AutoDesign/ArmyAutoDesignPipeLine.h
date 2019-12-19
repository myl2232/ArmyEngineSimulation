// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyAutoDesignPipeLine.h
* @Description 水电模式 自动设计管线
*
* @Author huaen
* @Date 2018年11月15日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
/**
* 自动布线错误
*/
enum EAutoDesignErrorType
{
	EAD_NONE = 0,
  	EAD_ERROR_SPACENAME = 1<<0, // 未设置房间名
	EAD_ERROR_ELE_STRONG = 1<<1,// 强电箱
	EAD_ERROR_ELE_WEAK = 1<<2, // 弱电箱
	EAD_ERROR_ELE_SOCKET_STRONG = 1<<3, // 强电插座
	EAD_ERROR_POWERSYSTEM = 1<<4 , // 未布置配电箱系统
};

namespace EAutoDesignState
{
 enum Type
 {
	 AutoDesigning = 1,
	 AutoDesignSuccessed = 2,
	 AutoDesignFailed = 3,
 };
}
class FArmyAutoDesignPipeLine : public TSharedFromThis<FArmyAutoDesignPipeLine>
{
public:
	struct FAutoDesignErrorInfo
	{
		FString ErrorTitle;
		FString ErrorICon;
		FString ErrorContent;
		FString ErrorImage;
		FAutoDesignErrorInfo(FString InErrorTitle,FString InErrorICon,FString InErrorContent,FString InErrorImage):
			ErrorTitle(InErrorTitle),
			ErrorICon(InErrorICon),
			ErrorContent(InErrorContent),
			ErrorImage(InErrorImage)
		{

		}
	};
	
	struct FATPipePointInfo : public TSharedFromThis<FATPipePointInfo>
	{
		int32 PointType = -1;//区分插座开关灯位，弱电箱等 1:普通插座 2:空调插座 3：冰箱插座 4：动力插座 5:开关 6：灯具 7：强电箱 8：弱电箱
		int32 Index = -1;//在回路中的序号
		FVector InnerPos = FVector(0, 0, 0);
		FVector Normal = FVector(0, 0, 0);

		TMap<int32, TPair<FVector, bool>> LinkPointMap;//int32 意义：1表示左下方点 2表示右下方点 3表示左上方点 4表示右上方点，5表示左侧点 6表示右侧点，bool值表示是否已经连线

		FATPipePointInfo(FVector InPos, FVector InNormal) :InnerPos(InPos), Normal(InNormal){}

		bool operator==(const FATPipePointInfo& InOther) const
		{
			return InnerPos == InOther.InnerPos;
		}
	};
	struct FATPass : public TSharedFromThis<FATPass>
	{
		FVector StartPos = FVector(0, 0, 0);
		FVector EndPos = FVector(0, 0, 0);
		TSharedPtr<FATPipePointInfo> DoorCenterPoint = NULL;

		TArray<TPair<FVector, bool>> LinkLinePointArray;

		FATPass(const FVector& InStart, const FVector& InEnd) :
			StartPos(InStart), EndPos(InEnd) 
		{
			DoorCenterPoint = MakeShareable(new FATPipePointInfo((InStart + InEnd)/2,FVector(0,0,1)));
			FVector WidthV = InEnd - InStart;
			FVector Dir = WidthV.GetSafeNormal();
			float AvailableWidth = WidthV.Size() - 40;//门两边总剩余量
			int32 LinkNum = AvailableWidth / 2.5;
			for (int32 i = 0;i < LinkNum;++i)
			{
				LinkLinePointArray.Add(TPair<FVector,bool>(InStart + Dir * (20/*门边剩余量*/ + i * 2.5),false));
			}
		}
		const FVector& GetLinkPoint(const FVector& InDir)
		{
			FVector Dir = (EndPos - StartPos).GetSafeNormal();
			if((Dir + InDir).Size() > 1)//同向
			{
				for (int32 i = 0; i < LinkLinePointArray.Num(); i++)
				{
					auto& P = LinkLinePointArray[i];
					if (!P.Value)
					{
						P.Value = true;
						return P.Key;
					}
				}
			}
			else
			{
				for (int32 i = LinkLinePointArray.Num() - 1; i >= 0; i--)
				{
					auto& P = LinkLinePointArray[i];
					if (!P.Value)
					{
						P.Value = true;
						return P.Key;
					}
				}
			}
			return DoorCenterPoint->InnerPos;
		}
		bool operator==(const FATPass& InOther) const
		{
			return StartPos == InOther.StartPos && EndPos == InOther.EndPos;
		}
	};
	struct FATRoomPlane : public TSharedFromThis<FATRoomPlane>
	{
		int32 PlaneType = -1;//区分墙顶地 1墙 2顶 3地
		int32 Index = -1;//在某一房间内的排序
		FVector Normal = FVector(0,0,0);
		FVector StartPos = FVector(0, 0, 0);
		FVector EndPos = FVector(0, 0, 0);

		FVector RangeDirection = FVector(0,0,0);//当前墙体排序方向

		TArray<TSharedPtr<FATPass>> PassArray;//在该面墙上的门或者垭口的数组
		TArray<TSharedPtr<FATPipePointInfo>> PipePointArray;

		FATRoomPlane(int32 InPlaneType,const FVector& InStart,const FVector& InEnd,const FVector& InNormal):
			PlaneType(InPlaneType),StartPos(InStart),EndPos(InEnd),Normal(InNormal)
		{
			MakeShareable(this);
		}
		bool operator==(const FATRoomPlane& InOther) const
		{
			return PlaneType == InOther.PlaneType && StartPos == InOther.StartPos && EndPos == InOther.EndPos && Normal == InOther.Normal;
		}
	};
	struct FATRoom : public TSharedFromThis<FATRoom>
	{
		int32 RoomType = -1;//1厨房 2卫生间 3阳台（其他类型暂时不需要考虑）
		int32 Index = -1;
		FGuid RoomID;
		FString RoomName;

		TArray<TSharedPtr<FATRoomPlane>> PlaneArray;
		FATRoom(const FGuid& InID) :RoomID(InID) 
		{
			MakeShareable(this);
		}
		bool operator==(const FATRoom& InOther) const
		{
			return RoomID == InOther.RoomID;
		}
	};
	struct FATPipeLineInfo
	{
		TArray<FATPipePointInfo> PipePointArray;
		bool operator==(const FATPipeLineInfo& InOther) const
		{
			return this == &InOther;
		}
	};
	struct FATPipeLoopInfo
	{
		int32 LoopID = -1;
		int32 LoopType = -1;
		int32 LoopPipeID = -1; // 管线商品
		TArray<FATPipeLineInfo> PipeLineArray;
		TArray<TSharedPtr<FATRoom>> RelevanceRoomArray;

			FATPipeLoopInfo(int32 InID, int32 InType,int32 InPipeID) :LoopID(InID), LoopType(InType), LoopPipeID(InPipeID) {}

		bool operator==(const FATPipeLoopInfo& InOther) const
		{
			return LoopID == InOther.LoopID && LoopType == InOther.LoopType;
		}
	};
	struct RoomRouteInfo : public TSharedFromThis<RoomRouteInfo>
	{
		RoomRouteInfo(TSharedPtr<FATRoom> InRoom, TSharedPtr<FATRoomPlane> InPlane, TSharedPtr<FATPass> InPass):
			RouteRoom(InRoom),RoutePlane(InPlane),RoutePass(InPass){};

		bool operator==(const RoomRouteInfo& InOther) const
		{
			return RouteRoom == InOther.RouteRoom && RoutePlane == InOther.RoutePlane && RoutePass == InOther.RoutePass;
		}
		bool operator<(const RoomRouteInfo& Other) const
		{
			return false;
		}

		TSharedPtr<FATRoom> RouteRoom;
		TSharedPtr<FATRoomPlane> RoutePlane;
		TSharedPtr<FATPass> RoutePass;
	};
public:
	static TSharedPtr<FArmyAutoDesignPipeLine> GetInstance()
	{
		static TSharedPtr<FArmyAutoDesignPipeLine> Instance = MakeShareable(new FArmyAutoDesignPipeLine);
		return Instance;
	}

	/** 同步相关 */
	FText GetSyncProgressText() const { return FORMAT_TEXT("智能布线中%d%%...",FMath::FloorToInt(SyncProgress)); }

	void AutoDesign(const TSharedPtr<struct FHydropowerPowerSytem> InLoopInfo);

	void AutoDesignUpdateRoom();
	//清空
	void Empty();
public:
	~FArmyAutoDesignPipeLine() {};
private:
	FArmyAutoDesignPipeLine();

	//自动布线前的条件判断
	bool AutoDesignPreJudge(const TSharedPtr<FHydropowerPowerSytem> InLoopInfo);

	//排序回路关联的空间
	void AutoDesignRange(TArray<TSharedPtr<FATRoom>>& InRoomArray, TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>> &InRouteMap);
	//判断是路由门还是直连门 1：直连门 2：路由门
	int32 AutoDesignGetPassType(const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATPass> InPass, const TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>>& RouteMap,TSharedPtr<FATRoom>& OutRoom);
	//排序当前回路所有点，包含路由门洞的中间点
	void AutoDesignRouteAllPoint(const TArray<TSharedPtr<FATRoom>>& InRelateRoomArray, const TSharedPtr<FArmyAutoDesignPipeLine::FATRoom> InCurrentRoom, const TSharedPtr<FArmyAutoDesignPipeLine::FATRoomPlane> InStartPlane, const TSharedPtr<FArmyAutoDesignPipeLine::FATPipePointInfo> InStartPoint, TMap<TSharedPtr<FATRoom>, TArray<RoomRouteInfo>>& InRouteMap, TArray<TPair<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATRoomPlane>>>& RoutePointArray, TArray<TSharedPtr<FATRoomPlane>>& OutSortPlaneArray, int32& InCount,int32 InPointType = 1, int32 InLimitCount = -1, bool InStart = false);
	//排序墙面
	void AutoDesignPlaneRange(TArray<TSharedPtr<FATRoomPlane>>& InPlaneArray, TSharedPtr<FATRoomPlane> RoomStartPlane, TSharedPtr<FATRoomPlane>& PrePlane, TSharedPtr<FATRoomPlane>& PassPlane, int32& MaxIndex);

	bool AutoDesignRouteRoom(TArray<TSharedPtr<FATRoom>>& InRoomArray, TSharedPtr<FATRoom> InRoom, TSharedPtr<FATRoomPlane> InPlane,TSharedPtr<FATPass> InPass, TArray<TSharedPtr<FATPass>>& InRoutePassArray, TArray<RoomRouteInfo>& InRouteArray);
	//计算点到一个面的连路插值
	void AutoDesignInterpolationToPlane(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InPreRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const FVector& InPreNormal, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InRoomPlane, const float InHeight);
	//计算两点位间的连路点
	void AutoDesignInterpolation(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InPreRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const FVector& InPreNormal, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InRoomPlane, const TSharedPtr<FATPipePointInfo> InPointInfo);
	
	bool AutoDesignLinkToPlane(const TSharedPtr<FATRoom> InRoom, const FVector& InPrePoint, const FVector& InPreNormal, const TSharedPtr<FATRoomPlane> InTargetPlane, FATPipeLineInfo& OutLInfo);
	bool AutoDesignLinkTwoPoint(const TSharedPtr<FATRoom> InRoom, const FVector& InPrePoint, const FVector& InPreNormal, const FVector& InNextPoint, const FVector& InNextNormal, FATPipeLineInfo& OutLInfo);
	//路由下一个面路径
	void AutoDesignRouteNextPlane(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const TSharedPtr<FATRoomPlane> InRoomPlane);
	//路由下一个点位路径
	void AutoDesignRouteNextPoint(FATPipeLineInfo& OutLInfo, const TArray<TSharedPtr<FATRoomPlane>>& RoutePlaneArray, const TSharedPtr<FATRoom> InRoom, const TSharedPtr<FATRoomPlane> InPreRoomPlane, const FVector& InPrePos, const TSharedPtr<FATRoomPlane> InRoomPlane, const FVector& InTargetPoint);
	//水路
	void AutoDesignWaterPipeLine();
	//插座
	void AutoDesignSocketPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo, int32 InTypeCount = 0);
	//照明
	void AutoDesignLightingPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo);
	//冰箱
	void AutoDesignRefrigeratorPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo);
	//空调
	void AutoDesignAirConditionerPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo);
	//动力
	void AutoDesignPowerPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo);
	//弱电
	void AutoDesignELVPipeLine(const TSharedPtr<struct FHydropowerPowerSytemItem> InLoopInfo);
	//对起始点进行排序
	void AutoDesignSortOriginalPoint();
	//消除重叠
	void AutoDesignAvoidOverlap();
	//生成管线
	void AutoDesignGeneratePipeLines();
	//所有线路避让、排布调整（优化）
	void AutoDesignAdjust();

	TSharedPtr<FArmyAutoDesignPipeLine::FATPass> AutoDesignGetOrCreatePass(FArmyAutoDesignPipeLine::FATPass& InPass);

	//更新UI状态
	void SetUIState(EAutoDesignState::Type InState);

	void AutoDesignStart();

	void AutoDesignEnd();

	void OnAutoDesignPre();

	void OnAutoDesignPost();

	void OnAutoDesign_Delayed(const TSharedPtr<FHydropowerPowerSytem> InLoopInfo);
private:
	TArray<TSharedPtr<FATRoom>> AllRoomArray;
	TArray<TSharedPtr<FATPass>> AllPassArray;
	TArray<FATPipeLoopInfo> PipeLoopArray;
	TMultiMap<TSharedPtr<FATPass>, TSharedPtr<FATRoom>> PassRoomMap;
	TMultiMap<TSharedPtr<FATPass>, TSharedPtr<FATRoomPlane>> PassPlaneMap;
	TMap<TSharedPtr<FATPipePointInfo>, TSharedPtr<FATPass>> PointPassMap;

	FVector OriginalIndexRangeDir;
	TMap<int32, TPair<FVector,bool>> OriginalPointMap;

	TSharedPtr<FATPipePointInfo> OriginalPoint = NULL;//配电箱位置 回路的起始位置
	TSharedPtr<FATRoomPlane> OriginalPlane = NULL;//配电箱所在墙面
	TSharedPtr<FATRoom> OriginalRoom = NULL;//配电箱所在房间
	TMap<EAutoDesignErrorType,FAutoDesignErrorInfo> ErrorMap;
	TArray< TSharedPtr< struct FAutoDesignResultInfo> > ResultList; //结果
	TSharedPtr<class SArmyHydropowerAutoDesignFinish> ResultInfoView;
	float SyncProgress = 0.f; // 智能布线进程
	EAutoDesignState::Type CurrentState;

	FTimerHandle DesignTimeHandle;
	FTimerHandle DesignBeginTimeHandle;
	int32 Step1Weight = 3;
	int32 Step2Weight = 1;
	int32 Step4Weight = 4;//*loop num
	int32 Step5Weight = 4 * 2;//*loop num
	int32 Step6Weight = 4 * 2;//*loop num
	int32 Step7Weight = 3 * 3;//*loop num
	int32 CountWeight = 100;
};
