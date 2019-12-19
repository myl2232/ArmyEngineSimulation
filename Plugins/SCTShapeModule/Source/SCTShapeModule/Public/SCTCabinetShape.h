/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 柜子
*
* @Author 赵志强
* @Date 2018年9月8日
* @Version 1.0
*/
#pragma once

#include "SCTShape.h"

class FSpaceShape;
class FBoardShape;
class FSCTOutline;
class FVeneerdSudeBoardGroup;

//柜子
class SCTSHAPEMODULE_API FCabinetShape : public FSCTShape
{
public:
	FCabinetShape();
	virtual ~FCabinetShape();

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
	/** 创建用于选择的Actor */
	void SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName);
	FName GetShapeSpaceProfileName();
	FName GetBoundSpaceProfileName();

	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 设置型录尺寸 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeHeight(float InValue) override;

	/** 计算柜体空间尺寸变化范围 */
	float GetShapeWidthRange(float& OutMinValue, float& OutMaxValue);
	float GetShapeDepthRange(float& OutMinValue, float& OutMaxValue);
	float GetShapeHeightRange(float& OutMinValue, float& OutMaxValue);

public:
	/** 设置/获取顶层空间 */
	void SetTopSpaceShape(const TSharedPtr<FSpaceShape> &InShape);
	const TSharedPtr<FSpaceShape>& GetTopSpaceShape() const;

	/** 设置/获取见光板组 */
	void SetVeneerdSudeBoardGroup(const TSharedPtr<FVeneerdSudeBoardGroup> &InShape);
	const TSharedPtr<FVeneerdSudeBoardGroup>& GetVeneerdSudeBoardGroup() const { return VeneerdSudeBoardGroup; }

	/** 修改指定空间的尺寸 */
	bool SetSpaceShapeWidth(FSpaceShape* InSpace, float InValue);
	bool SetSpaceShapeDepth(FSpaceShape* InSpace, float InValue);
	bool SetSpaceShapeHeight(FSpaceShape* InSpace, float InValue);

public:
	/** 内胆内缩值 */
	bool SetInsideRetractValue(float InValue);
	float GetInsideRetractValue() { return RetractValue; };

	/** 活动层板内缩值 */
	bool SetActiveRetractValue(float InValue);
	float GetActiveRetractValue() { return ActiveRetract; }

	/** 底部空间的高度 */
	float GetBottomSpaceHeight();
	bool SetBottomSpaceHeight(float InValue);

	/** 显示/隐藏门组Actor */
	void ShowDoorGroup(bool bShow);
	bool IsDoorGroupShow();
	/** 判断柜子中是否包含门组 */
	bool IsContainDoorGroup();

	/** 显示/隐藏所有抽面Actor */
	void ShowDrawer(const bool bInShow);
	bool IsDrawerShow() const { return bShowDrawer; }

	/** 隐藏隐藏抽面门组*/


	/** 隐藏柜体中所有的ShapeActor */
	void HiddenCabinetShapeActors(bool bHidden);

	/** 柜体更新 */
	void UpdateCabinet();

	/** 隐藏门板缩略图 */
	void SetNoDoorThumbnailUrl(FString InUrl) { NoDoorThumbnailUrl = InUrl; };
	FString GetNoDoorThumbnailUrl() { return NoDoorThumbnailUrl; };

	/** 柜体尺寸标准值 */
	TArray<float> GetStandardWidths() { return StandardWidths; };
	void SetStandardWidths(TArray<float> InValues) { StandardWidths = InValues; };
	TArray<float> GetStandardDepths() { return StandardDepths; };
	void SetStandardDepths(TArray<float> InValues) { StandardDepths = InValues; };
	TArray<float> GetStandardHeights() { return StandardHeights; };
	void SetStandardHeights(TArray<float> InValues) { StandardHeights = InValues; };

	/* 门厚缝 */
	float GetDoorBackGap() const { return DoorBackGap; }
	void SetDoorBackGap(const float InDoorBackGap) { DoorBackGap = InDoorBackGap; }

	/** 门上下缝 */
	float GetDoorUpAndDownGap() const { return DoorUpAndDownGap; }
	void SetDoorUpAndDownGap(const float InDoorUpAndDownGap) { DoorUpAndDownGap = InDoorUpAndDownGap; }

	/** 门左右缝 */
	float GetDoorLeftAndRightGap() const { return DoorLeftAndRightGap; }
	void SetDoorLeftAndRightGap(const float InDoorLeftAndRightGap) { DoorLeftAndRightGap = InDoorLeftAndRightGap; }


public:
	/** 顶层空间轮廓 */
	bool SetTopSpaceOutline(TSharedPtr<FSCTOutline> InOutline, bool bForce = false);
	TSharedPtr<FSCTOutline> GetTopSpaceOutline();
	bool SetOutlineParamValueByRefName(const FString &InRefName, float InValue);

	/** 递归移除空间中受影响的板件 */
	void RemoveInfluenceBoardInSpace(FBoardShape* InBoard);

	//调整指定内胆的位置
	bool ModifyInsidePosition(class FInsideShape* InInside, const FVector &InDelta, bool bIgnoreZero, bool bContinuedOperator);

	//调整指定内胆的尺寸
	bool ModifInsideDimention(FInsideShape* InInside, const FVector &InDelta, bool bPositive, bool bIgnoreZero, bool bContinuedOperator);

private:
	//顶层柜体空间
	TSharedPtr<FSpaceShape> TopSpaceShape = nullptr;

	// 见光板组
	TSharedPtr<FVeneerdSudeBoardGroup>	VeneerdSudeBoardGroup;
	
	//顶层内胆内缩值
	float RetractValue = 2.0;
	//活动层板内缩值
	float ActiveRetract = 20.0;

	//底部空间的高度
	float BottomHeight = 100.0;

	//柜体中所有门组的显隐
	bool bShowDoorGroup = true;

	// 柜体中所有抽面的显隐
	bool bShowDrawer = true;

	//隐藏柜体Actor
	bool bHiddenCabinetActor = false;

	//柜体缩略图（隐藏门板）URL
	FString NoDoorThumbnailUrl;

private:
	//空间Actor的ProfileName
	FName ShapeSpaceProfileName;   //用于点选
	FName BoundSpaceProfileName;   //用于拖拽

private:
	//联动操作中受影响的相邻内胆
	int32 LoBIndex = -1;
	int32 RoUIndex = -1;

	// 尺寸调整中剩余偏移量
	FVector SurplusDelta = FVector(0.0);

	float DoorBackGap = 1.0f;			 /*!< 门后缝*/
	float DoorUpAndDownGap = 1.0f;		 /*!< 门上下缝*/
	float DoorLeftAndRightGap = 1.0f;	 /*!< 门左右缝*/										 
	TArray<float> StandardWidths;
	TArray<float> StandardDepths;
	TArray<float> StandardHeights;
};

