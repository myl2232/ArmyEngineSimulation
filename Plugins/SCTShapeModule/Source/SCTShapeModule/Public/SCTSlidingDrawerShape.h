/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTBaseShape.h
 * @Description 抽屉
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

// 抽类型录
// 由板件、五金模型和抽面构成
class SCTSHAPEMODULE_API FSlidingDrawerShape : public FSCTShape
{
public:
	FSlidingDrawerShape();
	virtual ~FSlidingDrawerShape();

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

	/** 隐藏抽屉中所有的ShapeActor */
	void HiddenDrawerShapeActors(bool bHidden);

	void ShowDrawer();
	void HideDrawer();

public:
	/** 设置门组宽度 */
	virtual bool SetShapeWidth(float InIntValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	/** 设置门组深度 */
	virtual bool SetShapeDepth(float InIntValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	/** 设置门组厚度 */
	virtual bool SetShapeHeight(float InIntValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;

	/** 判断输入空间尺寸值是否有效(抽屉数量可以改变情况下计算) */
	bool IsValidForWidth(float InValue);
	bool IsValidForDepth(float InValue);
	bool IsValidForHeight(float InValue);

	/** 获取抽屉的抽面深度 */
	float GetDrawerDoorTotalDepth();

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
	const TArray<TSharedPtr<FDrawerDoorShape>> & GetDrawerDoorShapes() const;

public:
	/** 抽面四边外延值 */
	void SetUpDoorExtensionValue(float InValue);
	void SetDownDoorExtensionValue(float InValue);
	void SetLeftDoorExtensionValue(float InValue);
	void SetRightDoorExtensionValue(float InValue);

	/** 抽屉动画标识 */
	bool IsDrawerCanAnimation() { return bCanAnimation; }
	void SetDrawerAcanAnimation(bool bState) { bCanAnimation = bState; }

private:
	//板件型录集合
	TArray<TSharedPtr<FBoardShape>> BoardShapes;
	//五金型录集合
	TArray<TSharedPtr<FAccessoryShape>> AccessoryShapes;
	// 抽面集合
	TArray<TSharedPtr<FDrawerDoorShape>> DrawerDoorShapes;

	//标识抽屉是否可动画
	bool bCanAnimation = true;
};

//抽屉组型录
class SCTSHAPEMODULE_API FDrawerGroupShape : public FSCTShape
{
public:
	FDrawerGroupShape();
	~FDrawerGroupShape();

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

	/** 隐藏框体中所有的ShapeActor */
	void HiddenDrawerGroupShapeActors(bool bHidden);

public:
	/** 重载型录位置设置 */
	virtual bool SetShapePosX(float InValue) override;
	virtual bool SetShapePosY(float InValue) override;
	virtual bool SetShapePosZ(float InValue) override;

	/** 重载型录尺寸设置 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeHeight(float InValue) override;

	/** 同时设置抽屉组的尺寸和外延值 */
	bool SetGroupDimensionExtension(float InWidth, float InDepth, float InHeight, 
		float InUp, float InDown, float InLeft, float InRight);

	/** 获取抽屉组抽面深度 */
	float GetDrawerDoorTotalDepth();

	/** 获取W、D、H的变化范围，同时返回当前值(抽屉数量不变的情况下计算) */
	float GetShapeWidthRange(float& MinValue, float& MaxValue);
	float GetShapeDepthRange(float& MinValue, float& MaxValue);
	float GetShapeHeightRange(float& MinValue, float& MaxValue);

	/** 判断输入空间尺寸值是否有效(抽屉数量可以改变情况下计算) */
	bool IsValidForWidth(float InValue);
	bool IsValidForDepth(float InValue);
	bool IsValidForHeight(float InValue);

	/** 根据抽屉组当前尺寸修改抽屉数量 */
	bool RebuildDrawerCopies();

	/** 更新抽屉组的尺寸与坐标 */
	void UpdateDrawerGroup();
public:
	void ShowDrawer();
	void HideDrawer();

public:
	/** 抽屉组四边掩盖方式 */
	int32 GetUpCorverType() { return UpCorverType; };
	void SetUpCorverType(int32 InType);
	int32 GetDownCorverType() { return DownCorverType; };
	void SetDownCorverType(int32 InType);
	int32 GetLeftCorverType() { return LeftCorverType; };
	void SetLeftCorverType(int32 InType);
	int32 GetRightCorverType() { return RightCorverType; };
	void SetRightCorverType(int32 InType);

	/** 抽面四边外延值 */
	void SetDrawerUpExtensionValue(float InValue);
	void SetDrawerDownExtensionValue(float InValue);
	void SetDrawerLeftExtensionValue(float InValue);
	void SetDrawerRightExtensionValue(float InValue);
	/** 抽屉组上下外延值 */
	void SetGroupUpExtensionValue(float InValue);
	float GetGroupUpExtensionValue();
	void SetGroupDownExtensionValue(float InValue);
	float GetGroupDownExtensionValue();

public:
	/** 设置原始抽屉 */
	void SetPrimitiveDrawer(TSharedPtr<FSlidingDrawerShape> InShape);
	/** 获取原始抽屉 */
	TSharedPtr<FSlidingDrawerShape> GetPrimitiveDrawer() const;

	/** 获取复制抽屉 */
	TArray<TSharedPtr<FSlidingDrawerShape>> GetCopyDrawerShapes() const;
	/** 添加复制抽屉 */
	void AddCopyDrawer(const TSharedPtr<FSlidingDrawerShape> &InShape);
	/** 移除指定位置的复制抽屉 */
	void RemoveCopyDrawerAt(int32 InIndex);
	/** 清楚所有复制抽屉 */
	void ClearAllCopyDrawers();
	/** 设置复制抽屉数量 */
	bool SetCopyDrawerCount(int32 InCount);
	/** 计算当前尺寸可实现的复制抽屉数量 */
	TArray<int32> GetValidCopyDrawerCounts();
	/** 获取当前抽屉数量 */
	int32 GetCurrentCopyCount();

private:
	//原始抽屉
	TSharedPtr<FSlidingDrawerShape> PrimitiveDrawerShape = nullptr;
	//拷贝抽屉
	TArray<TSharedPtr<FSlidingDrawerShape>> CopyDrawerShapes;

	//四边掩盖方式:0-未定义，1-全盖，2-半盖，3-内嵌
	int32 UpCorverType = 1;
	int32 DownCorverType = 1;
	int32 LeftCorverType = 1;
	int32 RightCorverType = 1;
	//抽屉组抽面四方向外延值
	float UpExtension = 0.0;
	float DownExtension = 0.0;
	float LeftExtension = 0.0;
	float RightExtension = 0.0;
	//抽屉组上、下外延值
	float GroupUpExtension = 0.0;
	float GroupDownExtension = 0.0;
};


