/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRRegion.h
* @Description 区域
*
* @Author 常远
* @Date 2018年12月17日
* @Version 1.0
*/
#pragma once
#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmyRegion :public FArmyObject
{
public:

	FArmyRegion();
	~FArmyRegion(){}
	FArmyRegion(FArmyRegion* Copy);
	// 序列化json数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	// 反序列化为对象
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	// 设置状态
	virtual void SetState(EObjectState InState) override;
	// 重绘
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	// 选中
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	//区域是否闭合
	bool IsClosed();
	// 获得组成区域的所有线段
	TArray<TSharedPtr<class FArmyLine>> GetRegionLines();
	// 生成有效的闭合区域的点
	bool CreatePolygonPoints();
	//获得区域空间名称
	FString GetRegionSpaceName(); 
	// 获得区域的分割线ID
	TArray<FGuid> GetSplitLineIdList() { return SplitLineIdList; };
	// 添加区域的分割线
	void AddSplitLine(TSharedPtr<class FArmySplitLine> SplitLine) { SplitLineList.Add(SplitLine); };

	// 获得区域的房间编号
	FString GetSpaceObjectId() { return SpaceObjectId; };

	/** 
	*  分割区域操作 @纪仁泽 
	*  区域名称 区域ID 区域周长 区域面积
	*/
	FString GetSplitRegionName()const { return RegionName; } //得到区域名称
	void SetSplitRegionName(FString InRegionName) { RegionName = InRegionName; }; //设置区域名称
	const int32 GetSplitRegionId() const { return RegionId; } //得到区域ID 
	void SetSplitRegionId(const int32 InRegionId) { RegionId = InRegionId; } //设置区域ID
	void GetSplitAreaAndPerimeter(float& OutArea, float& OutPerimeter); //计算区域周长和面积

	// 设置是否虚拟区域
	void SetIsVirtual(bool InIsVirtual);
	// 获得区域id
	int32 GetRegionId();

	friend class FArmyRegionOperation;
protected:
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;

	TSharedPtr<class FArmyPolygon> SelectedRoomSpace;
	
	// 房间编号
	FString SpaceObjectId;
	// 区域名称
	FString RegionName;
	// 区域ID
	int32 RegionId;
	//组成区域的分割线
	TArray< TSharedPtr<class FArmySplitLine> > SplitLineList;
	//组成区域的分割线ID
	TArray<FGuid> SplitLineIdList;
	//组成区域的虚线
	TArray< TSharedPtr<class FArmyLine> > VirtualLineList;
	// 组成区域的点的集合
	TArray< TSharedPtr<class FArmyEditPoint> > PointList;
	// 组成区域的墙的集合
	TArray< TSharedPtr<class FArmyWallLine> > WallList;
	// 业务需要
	bool IsVirtual;
private:
	// 把编辑的点装换成vector
	TArray<FVector> EditPointToVector();
	// 设置组成区域的分割线ID
	void SetSplitLineIdList();
};
REGISTERCLASS(FArmyRegion)