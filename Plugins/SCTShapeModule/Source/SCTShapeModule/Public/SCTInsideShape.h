/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTInsideShape.h
 * @Description 内胆
 *
 * @Author 赵志强
 * @Date 2018年5月15日
 * @Version 1.0
 */
#pragma once

#include "SCTShape.h"

class FBoardShape;
class FBaseShape;
class FUnitShape;

// 内胆型录
// 通过单元柜的空间分割自动创建
class SCTSHAPEMODULE_API FInsideShape : public FSCTShape
{
public:
	FInsideShape();
	FInsideShape(FUnitShape* InUnitShape);
	virtual ~FInsideShape();

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
	/** 创建子型录用于点选和Hover的Actor */
	void SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 隐藏内胆中所有的ShapeActor */
	void HiddenInsideShapeActors(bool bHidden);

public:
	/** 重载型录位置设置 */
	virtual bool SetShapePosX(float InValue) override;
	virtual bool SetShapePosY(float InValue) override;
	virtual bool SetShapePosZ(float InValue) override;

	/** 重载型录尺寸设置 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeHeight(float InValue) override;

	/** 获取W、D、H的变化范围，同时返回当前值 */
	float GetInsideWidthRange(float& MinValue, float& MaxValue);
	float GetInsideDepthRange(float& MinValue, float& MaxValue);
	float GetInsideHeightRange(float& MinValue, float& MaxValue);

	/** 获取型录绝对位置 */
	float GetAbsolutePosX();
	float GetAbsolutePosY();
	float GetAbsolutePosZ();

	/** 生成包围盒Actors */
	void SpawnBoundingBoxActors(FName InProfileName);
	/** 销毁包围盒Actors*/
	void DestroyBoundingBoxActors();

	/** 设置活动层板内缩值 */
	bool SetBoardActiveRetract(bool bActive);
	void UpdateActiveBoardRetract();

public:
	/** 归属的单元柜 */
	FUnitShape* GetUnitShapeData();
	void SetUnitShapeData(FUnitShape* InUnitShape);

	/** 方向类型 */
	FORCEINLINE EInsideDirectType GetDirectType() const { return DirectType; }
	FORCEINLINE void SetDirectType(EInsideDirectType InType) { DirectType = InType; }

	/** 判断内胆是否为空 */
	bool IsInsideShapeEmpty() const;
	/** 设置内胆为空 */
	void EmptyInsideShape();

	/** 获取/设置关联子型录 */
	const TSharedPtr<FSCTShape>& GetSelfShape() const;
	void SetSelfShape(const TSharedPtr<FSCTShape> &InShape);
	/** 根据内胆尺寸更新关联子型录 */
	bool UpdateSelfShape();

	/** 增加复制子型录 */
	void AddCopyShape(const TSharedPtr<FSCTShape> &InShape);
	/** 移除指定复制子型录 */
	void RemoveCopyShapeAt(int32 InIndex);
	/** 复制出的子型录集合 */
	FORCEINLINE const TArray<TSharedPtr<FSCTShape>>& GetCopyShapes() const { return CopyShapes; }
	/** 清空复制子型录 */
	void ClearAllCopyShapes();

	/** 设置复制数量 */
	bool SetCopyCount(int32 InCount);
	FORCEINLINE int32 GetCurrentCopyCount() const { return CopyShapes.Num() + 1; }
	TArray<int32> GetValidCopyCounts();

	/** 设置/获取可复制板件内胆复制阈值 */
	bool SetBoardCopyThreshold(float InValue);
	FORCEINLINE float GetBoardCopyThreshold() const { return CopyThreshold;}

	/** 设置板件内胆可复制 */
	bool SetBoardCopyValid(bool bValid);
	FORCEINLINE bool IsBoardCopyValid() const { return bBoardCopyValid; }

	/** 设置内胆中的板件厚度 */
	bool SetBoardHeight(const FString& InValue);
	/** 设置内胆中板件的材质 */
	bool SetBoardMaterial();

	/** 分割出的子对象集合 */
	FORCEINLINE TArray<TSharedPtr<FInsideShape>>& GetChildInsideShapes() { return ChildInsideShapes; }
	FORCEINLINE const TArray<TSharedPtr<FInsideShape>>& GetChildInsideShapes() const { return ChildInsideShapes; }
	/** 获取子对象的索引 */
	int32 GetChildInsideShapeIndex(FInsideShape* InShape) const;
	/** 插入子对象 */
	bool InsertChildInsideShape(FInsideShape* InCurShape, TSharedPtr<FInsideShape> InNewShape);
	/** 删除子对象 */
	void DeleteChildInsideShape(FInsideShape* InShape);

	/** 设置/获取父级内胆 */
	FORCEINLINE void SetParentInsideShape(FInsideShape* InParent) { ParentInsideShape = InParent; }
	FORCEINLINE FInsideShape* GetParentInsideShape() const { return ParentInsideShape; }

private:
	/** 设置内胆宽度 */
	bool SetShapeWidthWithBoard(float InValue);
	bool SetShapeWidthWithBase(float InValue);
	bool SetShapeWidthWithChildren(float InValue);
	/** 设置内胆深度 */
	bool SetShapeDepthWithBoard(float InValue);
	bool SetShapeDepthWithBase(float InValue);
	bool SetShapeDepthWithChildren(float InValue);
	/** 设置内胆厚度 */
	bool SetShapeHeightWithBoard(float InValue);
	bool SetShapeHeightWithBase(float InValue);
	bool SetShapeHeightWithChildren(float InValue);

	/** 按比例处理子内胆的宽度W （当前内胆为横向）*/
	bool ScaleChildrenWidth(float InValue);
	/** 按比例处理子内胆的厚度H （当前内胆为纵向）*/
	bool ScaleChildrenHeihgt(float InValue);

	/** 利用内胆尺寸处理基础组件复制对象 */
	bool ProcessBaseShapeCopies();
	/** 利用内胆尺寸处理板件复制对象 */
	bool ProcessBoardShapeCopies();

private:
	//方向类型
	EInsideDirectType DirectType = IDT_None;

	//关联的子型录
	TSharedPtr<FSCTShape> SelfShape;
	//复制出的子型录集合
	TArray<TSharedPtr<FSCTShape>> CopyShapes;

	//板件复制辅助变量
	bool bBoardCopyValid = false;
	float CopyThreshold = 300.0;

	//父级对象
	FInsideShape* ParentInsideShape = nullptr;
	//分割出的子对象
	TArray<TSharedPtr<FInsideShape>> ChildInsideShapes;

	//归属的单元柜
	FUnitShape* UnitShapeData;
};

