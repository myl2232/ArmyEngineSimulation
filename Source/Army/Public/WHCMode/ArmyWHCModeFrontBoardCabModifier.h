#pragma once
#include "CoreMinimal.h"

struct FShapeInRoom;

struct FFrontBoardCabInfo
{
	FShapeInRoom *FrontBoardCab;
	int32 NumDoors = 0;
	float StartPoint = 0.0f;
	float EndPoint = 0.0f;
	FFrontBoardCabInfo(FShapeInRoom *InFrontBoardCab, int32 InNumDoors, float InStartPoint, float InEndPoint)
	: FrontBoardCab(InFrontBoardCab)
	, NumDoors(InNumDoors)
	, StartPoint(InStartPoint)
	, EndPoint(InEndPoint) {}
};

class FArmyWHCFrontBoardCabModifier
{
public:
	// 添加一个插脚柜
	void AddFrontBoardCab(FShapeInRoom *InCabinet);
	// 添加一个柜子，返回被修改了的柜子列表
	TArray<FFrontBoardCabInfo> AddCabinet(FShapeInRoom *InCabinet);
	// 移动一个柜子，返回被修改了的柜子列表
	TArray<FFrontBoardCabInfo> MoveCabinet();
	// 删除一个柜子
	void DeleteCabinet(FShapeInRoom *InCabinet);
	// 清空所有插脚柜
	void ClearFrontBoardCabs();
	// 是否是插脚柜
	bool IsFrontBoardCab(FShapeInRoom *InCabinet) const;

private:
	// 所有已知的插脚柜
	TArray<FShapeInRoom*> FrontBoardCabs;

	// 处理某个插脚柜
	bool ProcessFrontBoardCab(FShapeInRoom *InFrontBoardCab, int32 &OutNumDoors, float &OutStartPoint, float &OutEndPoint);
	bool CheckFrontBoardCabinetHitOnlyOneSide(FShapeInRoom *InFrontBoardCab, const FVector &CabForward, const FVector &CabRight, int8 &OutLeftOrRight, class AXRShapeFrame *&OutHitOtherCab, FVector &HitPoint) const;
    bool ReverseHit(FShapeInRoom *InFrontBoardCab, FShapeInRoom *InOtherCab, int8 nFlag, const FVector &InHitPoint, const FVector &InReverseDir, FVector &OutReverseHitPoint) const;
	// 自动变化某个插脚柜
	bool ModifyFrontBoardCab(FShapeInRoom *InFrontBoardCab, FShapeInRoom *InOtherCab, int8 InLeftOrRight, int8 nFlag, const FVector &InHitPoint, const FVector &InReverseHitDir, int32 &OutNumDoors, float &OutStartPoint, float &OutEndPoint);
	FVector GetCabFrontLowerLeft(FShapeInRoom *InCab) const;
	FVector GetCabFrontLowerRight(FShapeInRoom *InCab) const;
};