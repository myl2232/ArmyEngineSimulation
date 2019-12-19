/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyEntrance.h
* @Description 施工图入口绘制
*
* @Author 刘俊良
* @Date 2018年11月1日
* @Version 1.0
*/

#pragma once
#include "ArmyObject.h"
#include "ArmyPolygon.h"
#include "Map.h"
/*
*              /|\---->顶点
*             / | \
*            /__|__\
*              |_|
*               =	--->底部划线
*/

#define UP_DIS   10.0f
#define DOWN_DIS 10.0f
#define LONG_DIS 25.0f
#define NEAR_DIS 20.0f
#define FIRSTLINE_DOWN_DIS 14.0f
#define SECONDLINE_DOWN_DIS 18.0f
#define THREELINE_DOWN_DIS 22.0f

class ARMYSCENEDATA_API FArmyEntrance : public FArmyObject
{
public:
	//入口图标各个顶点位置
	struct ConstructHouse
	{
		FVector Top              = FVector(0, UP_DIS, 0);
		FVector Long_Left        = FVector(-LONG_DIS, 0, 0);
		FVector Long_Right       = FVector(LONG_DIS, 0, 0);
		FVector Middle_Left      = FVector(-NEAR_DIS, 0, 0);
		FVector Middle_Right     = FVector(NEAR_DIS, 0, 0);
		FVector Long_BottomLeft  = FVector(-NEAR_DIS, -DOWN_DIS, 0);
		FVector Long_BottomRight = FVector(NEAR_DIS, -DOWN_DIS, 0);
		FVector Bottom           = FVector(0, -DOWN_DIS, 0);
		
		FVector FirstLine_Left   = FVector(-NEAR_DIS, -FIRSTLINE_DOWN_DIS, 0);
		FVector FirstLine_Right  = FVector(NEAR_DIS, -FIRSTLINE_DOWN_DIS, 0);
		FVector SecondLine_Left  = FVector(-NEAR_DIS, -SECONDLINE_DOWN_DIS, 0);
		FVector SecondLine_Right = FVector(NEAR_DIS, -SECONDLINE_DOWN_DIS, 0);
		FVector ThreeLine_Left   = FVector(-NEAR_DIS, -THREELINE_DOWN_DIS, 0);
		FVector ThreeLine_Right  = FVector(NEAR_DIS, -THREELINE_DOWN_DIS, 0);
	};

public:
	FArmyEntrance();
	virtual ~FArmyEntrance();

	//序列化
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	//反序列化
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	//绘制
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	//HUD绘制
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;

	virtual void Refresh() override;

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual void SetState(EObjectState InState) override;

	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual const FBox GetBounds() override;

	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual void ApplyTransform(const FTransform& Trans) override;

private:
	//初始化
	void Init();
	/**
	* @brief 获取房屋外框线
	* @Param InVertices--> 在世界坐标下的位置数组
	* @Param OutBoundLines--> 根据位置数组获取得到各个点的连线数组并OUT出来
	*/
	static void GetOutLine(const TArray<FVector>& InVertices, TArray<TSharedPtr<FArmyLine>>& OutBoundLines);

public:
	/**
	* @brief 设置绘制的位置
	* @Param 在世界坐标下的位置
	* @Param 在世界坐标下的方向
	*/
	void SetWorldTransform(const FVector& Pos, const FVector& Rotate);

	/**
	* @brief 获取填充列表
	* @Param 返回出填充列表 Key为是否为白色填充，Value为多边形对象
	*/
	void GetPolygon(TArray<TPair<bool, TSharedPtr<class FArmyPolygon>>>& OutPolygonMap) const
	{
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(true, WhiteArrows));
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(false, BlackArrows));
	}

	/**
	* @brief 获取原始填充列表
	* @Param 返回出填充列表 Key为是否为白色填充，Value为多边形对象
	*/
	void GetOriginPolygon(TArray<TPair<bool, TSharedPtr<class FArmyPolygon>>>& OutPolygonMap) const
	{
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(true, OriginWhiteArrows));
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(false, OriginBlackArrows));
	}

	/**
	* @brief 获取入户门线段列表
	* @Param 线段列表集合
	*/
	void GetBoundLineList(TArray< TSharedPtr<FArmyLine>>& OutLineList) const
	{
		OutLineList.Append(BoundList);
		//底部三根线段TODO
	}

	/**
	* @brief 获取原始入户门线段列表
	* @Param 线段列表集合
	*/
	void GetOriginBoundLineList(TArray< TSharedPtr<FArmyLine>>& OutLineList) const
	{
		GetOutLine(WhiteVertices, OutLineList);
		GetOutLine(BlackVertices, OutLineList);
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.FirstLine_Left, CH.FirstLine_Right)));
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.SecondLine_Left, CH.SecondLine_Right)));
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.ThreeLine_Left, CH.ThreeLine_Right)));
	}

	/**
	* @brief 获取入户门中心位置
	*/
	FVector GetWorldPosition() const
	{
		return WorldPosition;
	}

	FVector GetOriginPosition() const
	{
		return WorldOriginPosition;
	}

	/**
	* @brief 获取方位转换
	*/
	FTransform GetTransform() const
	{
		return Transform;
	}

private:
	//绘制一半的房子-->白色
	TSharedPtr<FArmyPolygon> WhiteArrows;
	TSharedPtr<FArmyPolygon> OriginWhiteArrows;
	TArray<FVector> WhiteVertices;

	//绘制一半的房子-->黑色
	TSharedPtr<FArmyPolygon> BlackArrows;
	TSharedPtr<FArmyPolygon> OriginBlackArrows;
	TArray<FVector> BlackVertices;

	//绘制房屋标志图标以及底下三根划线
	TArray< TSharedPtr<FArmyLine>> BoundList;

	//房屋结构坐标
	ConstructHouse CH;

	//编辑点
	TSharedPtr<class FArmyEditPoint> EditPoint;

private:
	//图标的位置
	FVector WorldPosition = FVector::ZeroVector;
	FVector WorldOriginPosition = FVector::ZeroVector;
	//图标的旋转方位
	FVector WorldRotate   = FVector::ZeroVector;
	//方位转换
	FTransform Transform;
};
REGISTERCLASS(FArmyEntrance)