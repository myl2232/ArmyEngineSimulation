/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 单元柜
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "SCTShape.h"

class FInsideShape;

// 单元柜型录
// 由内胆构成，默认为空内胆，添加子型录后可被分割为纵向和横向的嵌套内胆集合
class SCTSHAPEMODULE_API FUnitShape : public FSCTShape
{
public:
	FUnitShape();
	virtual ~FUnitShape();

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 用于创建子型录用于点选和Hover的Actor */
	void SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName);
	/** 设置内胆包围盒ProfileName */
	FName GetSelfInsideProfileName();
	FName GetNullInsideProfileName();
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 隐藏单元柜中所有的ShapeActor */
	void HiddenUnitShapeActors(bool bHidden);

	/** 设置型录尺寸 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeHeight(float InValue) override;

	/** 获取W、D、H的变化范围，同时返回当前值 */
	float GetUnitWidthRange(float& MinValue, float& MaxValue);
	float GetUnitDepthRange(float& MinValue, float& MaxValue);
	float GetUnitHeightRange(float& MinValue, float& MaxValue);

public:
	/** 单元柜类型 */
	//统一使用基类的ShapeTag变量
	//void SetUnitShapeType(int32 InType);
	//int32 GetUnitShapeType();

	/** 设置/获取顶层内胆 */
	void SetTopInsideShape(const TSharedPtr<FInsideShape> &InShape);
	const TSharedPtr<FInsideShape>& GetTopInsideShape() const;
	void DeleteTopTinsideShape();

	/** 活层内缩方向 */
	void SetRetractDirect(int32 InDirect) { RetractDirect = InDirect; }
	int32 GetRetractDirect() { return RetractDirect; }
	/** 活动层板内缩值 */
	void SetActiveRetractValue(float InValue);
	float GetActiveRetractValue() { return ActiveRetract; }

	/** 分割当前内胆（添加子级型录(板件或基础组件)） */
	FInsideShape* SplitSpecialInsideShape(const TSharedPtr<FSCTShape> &InChild, EInsideDirectType DirectType, FInsideShape* InInside);
	/** 子型录适配指定内胆 */
	bool FitChildShapeToInsideShape(const TSharedPtr<FSCTShape> &InChild, EInsideDirectType DirectType, FInsideShape* InInside);

	/** 调整指定内胆的位置 */
	bool ModifyInsidePosition(FInsideShape* InInside, const FVector &InDelta, bool bIgnoreZero, bool bContinuedOperator);
	/** 调整指定内胆的尺寸 */
	bool ModifInsideDimention(FInsideShape* InInside, const FVector &InDelta, bool bPositive, bool bIgnoreZero, bool bContinuedOperator);

	/** 清空调整缓存状态量 */
	void ClearModifyCacheStatus();

	/** 设置指定内胆宽度、厚度 */
	bool SetInsideWidth(FInsideShape* InInside, float Invalue);
	bool SetInsideHeight(FInsideShape* InInside, float Invalue);
	/** 设置指定内胆的x位置、z位置 */
	bool SetInsidePosX(FInsideShape* InInside, float Invalue);
	bool SetInsidePosZ(FInsideShape* InInside, float Invalue);

	/** 设置内胆中子对象的复制数量 */
	bool SetInsideCopyCount(FInsideShape* InInside, int32 InCount);

	/** 移除指定子级型录 */
	void DeleteSpecialInsideShape(FInsideShape* InShape);

	/** 判断单元柜是否为空 */
	bool IsTopInsideShapeEmpty();
	/** 清空顶层内胆 */
	void EmptyTopInsideShape();

private:
	/** 水平分割当前内胆 */
	void SplitInsideShapeByHorizontal(TArray<FInsideShape*>& ChildInsideShapes, TSharedPtr<FSCTShape> InChild, FInsideShape* InInside);
	/** 垂直分割当前内胆 */
	void SplitInsideShapeByVertical(TArray<FInsideShape*>& ChildInsideShapes, TSharedPtr<FSCTShape> InChild, FInsideShape* InInside);
	/** 异向分割当前内胆 */
	void SplitInsideShapeByIncongruous(TArray<FInsideShape*>& ChildInsideShapes, int32 DirectType, FInsideShape* InInside);

	/** 递归处置相邻空内胆 */
	void MergeAdjacentEmptyInsides(FInsideShape* CurInside);

private:
	//单元柜类型
	//EUnitShapeType UnitShapeType = UST_None;

	//顶层内胆型录
	TSharedPtr<FInsideShape> TopInsideShape;

private:
	//活层内缩方向
	int32 RetractDirect = 0;
	//活动层板内缩值
	float ActiveRetract = 18.0;

	//ProfileName
	FName SelfInsideProfileName;
	FName NullInsideProfileName;

private:
	//联动操作中受影响的相邻内胆
	int32 LoBIndex = -1;
	int32 RoUIndex = -1;

	// 尺寸调整中剩余偏移量
	FVector SurplusDelta = FVector(0.0);
};

