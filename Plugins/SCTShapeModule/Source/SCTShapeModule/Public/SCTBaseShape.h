/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTBaseShape.h
 * @Description 基础组件
 *
 * @Author 赵志强
 * @Date 2018年5月15日
 * @Version 1.0
 */
#pragma once

#include "SCTShape.h"

class FBoardShape;
class FAccessoryShape;
class FDrawerDoorShape;
class SlidingDoor;

// 基本组件型录
// 由板件、五金模型构成
class SCTSHAPEMODULE_API FBaseShape : public FSCTShape
{
public:
	FBaseShape();
	~FBaseShape();

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
	/** 用于创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 隐藏组件中所有的ShapeActor */
	void HiddenBaseShapeActors(bool bHidden);

public:
	virtual bool SetShapeWidth(const FString &InStrValue) override;	
	virtual bool SetShapeWidth(float InIntValue) override { return FSCTShape::SetShapeWidth(InIntValue); }
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	virtual bool SetShapeDepth(float InIntValue) override { return FSCTShape::SetShapeDepth(InIntValue); }
	virtual bool SetShapeHeight(const FString &InStrValue) override;
	virtual bool SetShapeHeight(float InIntValue) override { return FSCTShape::SetShapeHeight(InIntValue); }
private:
	void UpdateChildrenDimensions();	
public:
	/** 基础组件类型 */
	//统一使用基类的ShapeTag变量
	//void SetBaseShapeType(int32 InType);
	//int32 GetBaseShapeType();

	/** 基础组件中添加子对象 */
	void AddBaseChildShape(const TSharedPtr<FSCTShape> &InShape);

	/** 基础组件中删除子对象 */
	void DeleteBaseChildShape(FSCTShape* InShape);
	
	/** 获取板件型录集合 */
	const TArray<TSharedPtr<FBoardShape>>& GetBoardShapes() const;
	/** 获取五金型录集合 */
	const TArray<TSharedPtr<FAccessoryShape>>& GetAccessoryShapes() const;

	/** 获取抽面型录集合 */
	const TArray<TSharedPtr<FDrawerDoorShape>> & GetDrawerDoorShapes() const { return DrawerDoorShapes; }

private:
	//基本组件类型
	//EBaseShapeType BaseShapeType = BST_None;
	//板件型录集合
	TArray<TSharedPtr<FBoardShape>> BoardShapes;
	//五金型录集合
	TArray<TSharedPtr<FAccessoryShape>> AccessoryShapes;	
	// 抽面集合
	TArray<TSharedPtr<FDrawerDoorShape>> DrawerDoorShapes;
};

