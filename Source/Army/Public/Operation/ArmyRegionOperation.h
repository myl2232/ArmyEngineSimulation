
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File AliyunOssHttp.h
* @Description 区域生成操作
*
* @Author  fengxiaoyi
* @Date 2018年11月21日
* @Version 1.0
*/
#pragma once
#include "ArmyOperation.h"
#include "ArmyRegion.h"
#include "ArmySplitLine.h"
#include "ArmyRoom.h"
 

class  FArmyRegionOperation :public FArmyOperation
{
public:
	FArmyRegionOperation(EModelType InBelongModel);
	virtual ~FArmyRegionOperation() {}
	
	/** 开始区域划分操作 */
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	
	/** 结束区域划分操作 */
	virtual void EndOperation() override;

	/** 绘制缓存线 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	/** 每帧捕捉Slide Line */
	virtual void Tick() override;
	
	/** 处理鼠标输入 */
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

	/** 鼠标移动 */
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	
	/** 选中的构件 */
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive) override;
private:
	/*------------------------------------------------------------------------------------
	*
	*	   @非法绘制判断逻辑:在此处添加
	*
	-------------------------------------------------------------------------------------*/
	//在区域内
	bool IsPointInRegion(FVector vector, TSharedPtr<FArmyRegion> region);
	
	//在区域线上
	bool IsPointOnRegionLine(FVector vector, TSharedPtr<FArmyRegion> region);
	
	//在房间内
	bool IsPointOnInternelRoom(FVector vector, TSharedPtr<FArmyRoom> region);
	
	//在房间线上
	bool IsPointOnRoomLines(FVector vector, TSharedPtr<FArmyRoom> region);
	
	//自相交
	bool IsSelfIntersection(const FVector&Vector);
	
	//与区域重叠
	bool IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRegion> region);
	
	//与房间重叠
	bool IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRoom> room);
	
	//与上次点重合
	bool IsSameWithLastPoint(const FVector&Vector);
	
	//回线重合
	bool IsOverlayWithSelf(const FVector&Vector);

	// 横跨区域
	bool IsSpanRegion(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RegionList);
protected:
	
	void ReDraw();

	// 删除分割线
	void DeleteExecute();
	// 清理缓存数据 
	void ClearCacheData();
	
	//添加实时缓存点
	bool AddCachePoint(const FVector& vector);
	
	//根据缓存点查询区域
	TSharedPtr<FArmyRegion> SearchRegionByCachePoint(const FVector& vector , TArray<TWeakPtr<FArmyObject>> &RegionList);
	
	//根据缓存点查询房间
	TSharedPtr<FArmyRoom> SearchRoomByCachePoint(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RoomList);

	//添加切分线
	void ProduceSplitLine(FString spaceId);
	
	//读取房间线,填充到区域
	void FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRegion> room);
	
	//读取房间线,填充到区域
	void FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRoom> room);
	
	//线段长度回调
	void OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
 
	//计算某区域下分割线的个数
	 int  CalculateNumberOfSplitLinesOnRegion(FString SpaceId);
	
	 //收集相同唯一标识符号的所有分割线并根据分割线的索引排序,拿到有顺序的分割
	 TArray<TSharedPtr<FArmySplitLine>> SearchSplitLinesWithSameSpaceIdOrderByIndex(FString SpaceId);
	
	 //收集相同唯一标识符下的区域数据集合
	 TArray<TSharedPtr<FArmyRegion>>	SearchRegionWithSameSpaceId(FString SpaceId);
	
	 //添加切割线
	 void AddSplitLine(TSharedPtr<FArmySplitLine> SplitLine,FString SpaceId);

	 //删除切割线
	 void DeleteSplitLineAndSort(TSharedPtr<FArmySplitLine> SplitLine);

	 //获取切割线相关联的线
	 TArray<TSharedPtr<FArmySplitLine>> GetRelativeSplitLines(TSharedPtr<FArmySplitLine> SplitLine);

	 //搜索关联的线
	 void SearchRelativeSplitLines(TSet<int> &Lines,TSharedPtr<FArmySplitLine> SplitLine, TArray<TSharedPtr<FArmySplitLine>>&  LinesArray);
	  
	//移除一个房间内所有的区域
	 void RemoveAllRegionInRoom(const TArray<TSharedPtr<FArmyRegion>> &RegionList);
	
	 //刷新指定区域
	 void RealTimeFlushRegionList(TSharedPtr<FArmyRoom> WhichRoom);
	
	 //从房间生成缓存区域
	 void ProduceCacheRegionByRoom(TSharedPtr<FArmyRoom> WhichRoom, TArray<TSharedPtr<FArmyRegion>> &RegionList,TSharedPtr<FArmySplitLine> Lines);
	
	 //从区域中生成缓存区域
	 void ProduceCacheRegionByRegionList(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines);
	
	 //检查切分线的区域
	 int SearchSplitLinesRegion(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines);
	 
	 //比较两个区域的,布置名称
	 void CompareSignature(TArray<TSharedPtr<FArmyRegion>> &Src, TArray<TSharedPtr<FArmyRegion>> &Dest);
	 
	 // 是否相同区域
	 bool IsSameRegion( TSharedPtr<FArmyRegion>  &Src,  TSharedPtr<FArmyRegion> &Dest);
	
	 //是否相同区域判断
	 bool IsSameRegionPrecisely(TSharedPtr<FArmyRegion>  &Src, TSharedPtr<FArmyRegion> &Dest);

	 //更新缓冲区数据
	 void UpdateRegionList(FString id);
	 
	 //处理区域与分割线的关系
	 void RelativeSplitLinesWithRegions(TArray<TSharedPtr<FArmyRegion>>&Regions, TArray<TSharedPtr<FArmySplitLine>>&SplitLines);

private:
	FLinearColor Color;

	// 缓存待删除关联的分割线
	TArray<TSharedPtr<FArmySplitLine>> RelativeSplitLines;
	// 选中的分割线
	TSharedPtr<FArmySplitLine> SelectedSplitLine;
	//缓存一次操作的中的点坐标 
	TArray<FVector> CachedPoints;
	//鼠标捕捉线 
	TSharedPtr<class FArmyLine>		 SolidLine;
	//参数
	XRArgument CurrentArg;
	//标尺
	TSharedPtr<class FArmyRulerLine>	RulerLine;
};