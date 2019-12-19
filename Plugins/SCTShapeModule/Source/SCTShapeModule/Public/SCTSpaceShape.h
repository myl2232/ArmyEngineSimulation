/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTSpaceShape.h
* @Description 空间：用于对柜子中进行空间分割
*
* @Author 赵志强
* @Date 2018年9月8日
* @Version 1.0
*/
#pragma once

#include "SCTShape.h"
#include "SCTShapeData.h"

class FCabinetShape;
class FFrameShape;
class FUnitShape;
class FDoorGroup;
class FSideHungDoor;
class FSlidingDoor;
class FBoardShape;
class FSCTOutline;
class ASCTSpaceActor;
class FSlidingDrawerShape;
class FDrawerGroupShape;
class FAccessoryShape;
class FEmbededElectricalGroup;

//柜体空间
class SCTSHAPEMODULE_API FSpaceShape : public FSCTShape
{
public:
	struct FRangeAndStandValue
	{
		TArray<float> StandValueList;
		float MinValue = 0.0f;
		float MaxValue = 0.0f;
	};
public:
	FSpaceShape();
	FSpaceShape(FCabinetShape* InCabinetShape);
	virtual ~FSpaceShape();

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

	/** 重载型录尺寸设置 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeHeight(float InValue) override;

	/** 重新计算子空间 */
	bool RebuildSpaceWidth();
	bool RebuildSpaceHeight();
	bool RebuildSpaceDepth();

	/** 计算柜体空间尺寸变化范围 */
	float GetShapeWidthRange(float& OutMinValue, float& OutMaxValue);
	float GetShapeDepthRange(float& OutMinValue, float& OutMaxValue);
	float GetShapeHeightRange(float& OutMinValue, float& OutMaxValue);

	/** 判断输入空间尺寸值是否有效 */
	bool IsValidForWidth(float InValue);
	bool IsValidForDepth(float InValue);
	bool IsValidForHeight(float InValue);

	/** 获取空间的绝对位置 */
	float GetSpaceAbsolutePosX();
	float GetSpaceAbsolutePosY();
	float GetSpaceAbsolutePosZ();

public:
	/** 获取空间Actor */
	ASCTSpaceActor* GetSpaceShapeActor();
	/** 生成空间Actor */
	ASCTSpaceActor* SpawnSpaceShapeActor(FName InProfileName);
	/** 销毁空间Actor */
	void DestroySpaceShapeActor();

	/** 空间轮廓 */
	bool SetSpaceOutline(TSharedPtr<FSCTOutline> InOutline);
	TSharedPtr<FSCTOutline> GetSpaceOutline() const;
	bool SetOutlineParamValueByRefName(const FString &InRefName, float InValue);
	/** 计算空间轮廓的柜体板数量 */
	TArray<int32> CalculateFrameBoardCount();
	
	/** 柜体空间尺寸是否固定 */
	FORCEINLINE void SetSpaceModifyScale(float InValue) { ScaleValue = InValue; };
	float GetSpaceModifyScale();

	/** 隐藏空间中所有的ShapeActor */
	void HiddenSpaceShapeActors(bool bHidden);

public:
	/** 归属的柜体 */
	FCabinetShape* GetCabinetShapeData();
	void SetCabinetShapeData(FCabinetShape* InCabinetShape);

	/** 创建默认状态空间 */
	bool MakeDefaultSpace();

	/** 清除当前空间 */
	void EmptySpaceShape();
	/** 判断空间是否为空 */
	bool IsSpaceShapeEmpty();

	/** 板件空间（保存分割板件） */
	bool IsSplitBoardSpace();
	/** 框体空间（保存框体和内部空间） */
	bool IsFrameShapeSpace();
	/** 抽屉空间 */
	bool IsDrawerGroupSpace();
	/** 电子组空间 */
	bool IsEmbededElectricalGroup()const;
	/** 底部空间 */
	bool IsBottomSpace();
	bool IsInsideContainBottomSpace();

	/** 内部空间 */
	bool IsInsideSpace();
	/** 顶层内胆空间 */
	bool IsUnitSpace();
	/** 内部空间带有子级空间（顶层内胆为空） */
	bool IsChildrenSpace();
	/** 清空内部空间的所有子级空间 */
	void EmptyChildrenSpace();

	/** 内部空间是否为空 */
	bool IsInsideSpaceEmpty();
	/** 清空内部空间 */
	void EmptyInsideSpace();

	/** 更新空间 */
	bool UpdateSpaceShape();

	/** 设置空间类型 */
	void SetSpaceSubType(ESpaceSubType InType) { SpaceSubType = InType; };
	ESpaceSubType GetSpaceSubType() { return SpaceSubType; };

	/** 设置/获取空间方向类型 */
	FORCEINLINE void SetDirectType(ESpaceDirectType InType) { DirectType = InType; };
	FORCEINLINE ESpaceDirectType GetDirectType() const { return DirectType; };

public:
	/** 设置/获取顶层内胆 */
	void SetTopInsideShape(TSharedPtr<FUnitShape> InShape);
	FORCEINLINE TSharedPtr<FUnitShape> GetTopInsideShape() const { return TopInsideShape; };
	void DeleteTopInsideShape();

	/** 顶层内胆内缩值 */
	float GetUnitShapeRetractValue();
	bool SetUnitShapeRetractValue(float InValue);
	int32 GetUnitShapeRetractType();
	void SetUnitShapeRetractType(int32 InType);
	/** 计算顶层内胆的位置和尺寸 */
	bool CalUnitShapePosDim(FVector& UnitPosition, FVector& UnitDimention);

	/** 顶层内胆中活动层板内缩值 */
	bool SetUnitShapeActiveRetract(float InValue);

	/** 门组外盖内缩值和计数 */
	float GetDoorCoverRetractValue() { return DoorCoverRetractValue; }
	int32 GetDoorCoverRetractCount() { return DoorCoverRetractCount; }
	bool AddDoorCoverRetractValue(float InValue);
	void RemoveDoorCoverRetractValue();
	/** 顶层内胆门组内缩值处理 */
	float GetDoorNestedRetractValue() { return DoorNestedRetractValue; }
	int32 GetDoorNestedRetractCount() { return DoorNestedRetractCount; }
	bool AddDoorNestedRetractValue(float InValue);
	void RemoveDoorNestedRetractValue();

public:
	/** 设置/获取分割侧板 */
	void SetSplitBoard(TSharedPtr<FBoardShape> InShape);
	FORCEINLINE TSharedPtr<FBoardShape> GetSplitBoard() const { return SplitBoardShape; };
	void DeleteSplitBoard();
	/** 分割板件外延值设置 */
	bool AddSplitBoardFrontExtension(float InValue);
	void RemoveSplitBoardFrontExtension();

public:
	/** 外盖抽屉组 */
	TSharedPtr<FDrawerGroupShape> GetCoveredDrawerGroup();
	void SetCoveredDrawerGroup(TSharedPtr<FDrawerGroupShape> InDrawerGroup);
	void DeleteCoveredDrawerGroup();
	/** 添加外盖抽屉组 */
	bool AddCoveredDrawerGroup(TSharedPtr<FDrawerGroupShape> InDrawerGroup);
	/** 更新电器饰条组 */
	bool UpdateEmbededElectricalGroup();
	/** 获取电器饰条组 */
	TSharedPtr<FEmbededElectricalGroup> GetEmbededElecticalGroup() { return EmbededElecticalGroup; }

	/** 添加嵌入式电器 */
	void SetEmbededElectricalGroup(TSharedPtr<FEmbededElectricalGroup> InEmbededElectricalGroup);
	bool AddEmbededElectricalGroup(TSharedPtr<FEmbededElectricalGroup> InEmbededElectricalGroup);

	/** 更新外盖抽屉组 */
	bool UpdateCoveredDrawerGroup();
	/**  重新计算嵌入式电器对相邻板件产生的影响，主要用于调整饰条的时候调用*/
	bool RecalEmbedElectricalGroupInfluences();
	void DeleteEmbededElectricalGroup();

	/** 判断输入抽屉是否可以有效添加到当前空间 */
	bool IsSlidingDrawerToCurSpaceMatching(FSlidingDrawerShape* InDrawerShape);

	/** 判断电器组是否可以有效的加入到当前空间 */
	bool IsEmbededEletricalGroupSpaceMatching(FAccessoryShape * InEmbededEletricalShape);
public:
	/** 设置/获取柜体框 */
	void SetFrameShape(TSharedPtr<FFrameShape> InShape);
	FORCEINLINE TSharedPtr<FFrameShape> GetFrameShape() const { return FrameShape; };
	void DeleteFrameShape();

	/** 柜体框板件 */
	void AddFrameBoard(TSharedPtr<FBoardShape> InShape, int32 InType, int32 InIndex = 0);
	void ChangeFrameBoard(TSharedPtr<FBoardShape> InShape, int32 InType, int32 InIndex = 0);
	void DeleteFrameBoardByTypeIndex(int32 InType, int32 InIndex = 0);
	void DeleteFrameBoard(FBoardShape* InBoard);
	TSharedPtr<FBoardShape> GetFrameBoard(int32 InType, int32 InIndex = 0);

	/** 处理框体板件的内移和厚度 */
	bool ModifyFrameBoardRetractByTypeIndex(int32 InType, int32 InIndex, float InValue);
	bool ModifyFrameBoardHeightByTypeIndex(int32 InType, int32 InIndex, FString InValue);
	bool ModifyFrameBoardRetract(FBoardShape* InBoard, float InValue);
	bool ModifyFrameBoardHeight(FBoardShape* InBoard, FString InValue);

	/** 更新框体板件 */
	bool UpdateFrameBoard();
public:
	/** 设置/获取门组 */
	FORCEINLINE TSharedPtr<FDoorGroup> GetDoorShape() const { return DoorShape; };
	/** 添加（掩门、移门）门组 */
	bool AddSideHungDoor(TSharedPtr<FSideHungDoor> InDoorShape,const bool InCreateBoundBox = true);
	bool AddSlidingDoor(TSharedPtr<FSlidingDoor> InDoorShape, const bool InCreateBoundBox = true);
	/** 显示、隐藏门组(递归处理子级空间) */
	void ShowDoorGropActor();
	void HideDoorGropActor();
	bool IsDoorGroupShow();
	/** 判断空间中是否包含门组 */
	bool IsContainDoorGroup();
	/** 移除已经添加的门组 */
	void RemoveDoorGroup();
	/** 更新门组 */
	bool UpdateDoorGroup();
	/** 显示/隐藏抽面 */
	void ShowDrawerActor();
	void HideDrawerActor();
public:
	const FRangeAndStandValue & GetWidthExportValue() const { return WidthExportValue; }
	void SetWidthExportValue(const FRangeAndStandValue & InValue) { WidthExportValue = InValue; }
	const FRangeAndStandValue & GetHeightExportValue() const { return HeightExportValue; }
	void SetHeightExportValue(const FRangeAndStandValue & InValue) { HeightExportValue = InValue; }
	const FRangeAndStandValue & GetDepthExportValue() const { return DepthExportValue; }
	void SetDepthExportValue(const FRangeAndStandValue & InValue) { DepthExportValue = InValue; }
private:
	/** 添加、删除门组 */
	void SetDoorShape(TSharedPtr<FDoorGroup> InShape);  //内部调用
	void DeleteDoorShape();  //内部调用
public:
	/** 内部空间 */
	TSharedPtr<FSpaceShape> GetInsideSpace();
	bool SetInsideSpace(TSharedPtr<FSpaceShape> InSpace);
	void DeleteInsideSpace();
	/** 更新内部空间 */
	bool UpdateInsideSpace();

	/** 底部空间 */
	TSharedPtr<FSpaceShape> GetBottomSpace();
	void SetBottomSpace(TSharedPtr<FSpaceShape> InSpace);
	bool CreateBottomSpace(bool bForce = false);
	void DeleteBottomSpace();
	/** 设置底部空间高度 */
	bool SetBottomSpaceHeight(float InValue);
	float GetBottomSpaceHeight();
	bool IsValidForBottomHeight(float InValue);
	/** 更新底部空间 */
	//bool UpdateBottomSpace();

public:
	/** 虚拟分割指定空间 */
	bool SplitSpaceByVirtual(ESpaceDirectType InDirect, TArray<float> InPartValues, bool bForce = false);
	/** 板件分割指定空间 */
	bool SplitSpaceByBoard(FBoardShape* SplitBoard, ESpaceDirectType InDirect, TArray<float> InPartValues, bool bForce = false);

	/** 设置/获取父级柜体空间 */
	FORCEINLINE void SetParentSpaceShape(FSpaceShape* InShape) { ParentSpaceShape = InShape; };
	FORCEINLINE FSpaceShape* GetParentSpaceShape() const { return ParentSpaceShape; };
	/** 获取子级柜体空间 */
	FORCEINLINE TArray<TSharedPtr<FSpaceShape>>& GetChildSpaceShapes() { return ChildSpaceShapes; }
	FORCEINLINE const TArray<TSharedPtr<FSpaceShape>>& GetChildSpaceShapes() const { return ChildSpaceShapes; }
	/** 获取子对象的索引 */
	int32 GetChildSpaceShapeIndex(FSpaceShape* InShape) const;
	/** 插入子对象 */
	bool InsertChildSpaceShape(FSpaceShape* InCurShape, TSharedPtr<FSpaceShape> InNewShape);
	/** 删除子对象 */
	void DeleteChildSpaceShape(FSpaceShape* InShape);

	/** 获取子级空间的分割方式 */
	FORCEINLINE void SetChildrenSpaceSplitMode(bool InMode) { ChildrenSplitMode = InMode; };
	FORCEINLINE bool GetChildrenSpaceSplitMode() { return ChildrenSplitMode; };
	/** 获取子级空间的分割方向 */
	FORCEINLINE void SetChildrenSpaceSplitDirect(ESpaceDirectType InType) { ChildrenSplitDirect = InType; };
	FORCEINLINE ESpaceDirectType GetChildrenSpaceDirectType() { return ChildrenSplitDirect; };

	/** 递归移除受影响的板件 */
	void RemoveInfluenceBoard(FBoardShape* InBoard);

public:
	/** 仅仅用来判断门组尺寸是否满足门洞尺寸，不对当前空间产生任何影响 */
	bool FitSideHungDoorGroupToCurSpaceShapeWithoutconfluence(FSideHungDoor* InDoorGroup);
private:
	/** 获取相邻空间的分割板件和相邻空间 */
	bool GetUpSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape);
	bool GetDownSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape);
	bool GetLeftSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape);
	bool GetRightSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape);
	/** 获取父级空间的框体板件和相邻空间 */
	bool GetUpBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes);
	bool GetDownBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes);
	bool GetLeftBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes);
	bool GetRightBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes);
	/** 获取子级空间的框体板件 */
	bool GetLeftBoardInChildrenSpace(TArray<FBoardShape*>& OutFrameBoards, FBoardShape*& OutTopFrameBoard);
	bool GetRightBoardInChildrenSpace(TArray<FBoardShape*>& OutFrameBoards, FBoardShape*& OutTopFrameBoard);
	/** 计算给定框体板件的厚度最大值 */
	float GetFrameBoardHeight(TArray<TSharedPtr<FBoardShape>>& OutResult);
	/** 设置框体板件的前向外延值 */
	bool SetFrameBoardFrontExtension(TArray<TSharedPtr<FBoardShape>>& OutResult, float InValue);

	/** 统计空间中所有的框体板件 */
	void GetAllBoardsInSpaceShape(TArray<FBoardShape*>& OutFrameBoards);

	/** 门组外盖引起的框体板件和内部空间的变化 */
	void ExecuteCoveredSpaceInfluences(float InValue);
	void CancelCoveredSpaceInfluences();
	void ExecuteCoveredBoardInfluences(float InValue);
	void CancelCoveredBoardInfluences();
	void ExecuteInsetedBoardInfluences(float InValue);
	void CancelInsetedBoardInfluences();
	void ClearDoorGroupInfluences();

	/** 计算当前空间中顶层内胆因门组添加引起的内缩值 */
	float GetDoorGroupRetractValue();

	/** 门组适配当前空间 */
	bool FitSideHungDoorGroupToCurSpaceShape(FSideHungDoor* InDoorGroup);

	bool FitSlidingDoorGroupToCurSpaceShape(FSlidingDoor* InDoorGroup);
	/** 外盖抽屉组适配当前空间 */
	bool FitCoveredDrawerGroupToCurSpaceShape(FDrawerGroupShape* InDrawerGroup);

	/** 嵌入式电器组适配当前空间 */
	bool FitEmbededElectricalGroupToCurSpaceShape(FEmbededElectricalGroup * InEmbededElectricalGroup);	

private:
	/** 按比例处理子空间的宽度W （子空间为X方向）*/
	bool ScaleChildrenWidth(float InValue);
	/** 按比例处理子空间的深度D （子空间为Y方向）*/
	bool ScaleChildrenDepth(float InValue);
	/** 按比例处理子空间的厚度H （子空间为Z方向）*/
	bool ScaleChildrenHeight(float InValue);

private:
	//空间类型
	ESpaceSubType SpaceSubType = SST_None;
	//空间方向类型
	ESpaceDirectType DirectType = SDT_None;
	//空间尺寸变化比例(用于柜子尺寸变化时)
	//小于0为比例值；大于0为固定值
	float ScaleValue = 0;
	//空间轮廓
	TSharedPtr<FSCTOutline> SpaceOutline = nullptr;

	//分割侧板
	TSharedPtr<FBoardShape> SplitBoardShape = nullptr;
	int32 FrontExtensionCount = 0;

	//柜体框
	TSharedPtr<FFrameShape> FrameShape = nullptr;

	//外盖抽屉组
	bool bShowDrawerGroup = true; //标识是否显示门组
	TSharedPtr<FDrawerGroupShape> CoveredDrawerGroup = nullptr;

	// 电器组
	TSharedPtr<FEmbededElectricalGroup> EmbededElecticalGroup;

	//门组
	bool bShowDoorGroup = true; //标识是否显示门组
	TSharedPtr<FDoorGroup> DoorShape = nullptr;

	//内空间(柜体框内部的空间)
	TSharedPtr<FSpaceShape> InsideSpace = nullptr;

	//顶层内胆
	TSharedPtr<FUnitShape> TopInsideShape = nullptr;
	//顶层内胆工艺内缩方向和数值
	int32 RetractType = 5; //0-顶，1-底，2-左，3-右，4-背，5-前	
	//门组外盖内缩值及计数
	float DoorCoverRetractValue = 0.0;
	int32 DoorCoverRetractCount = 0;
	//门组内嵌内缩值及计数
	float DoorNestedRetractValue = 0.0;
	int32 DoorNestedRetractCount = 0;

	//底部空间
	TSharedPtr<FSpaceShape> BottomSpace = nullptr;
	//父级柜体空间
	FSpaceShape* ParentSpaceShape = nullptr;
	//子级柜体空间
	TArray<TSharedPtr<FSpaceShape>> ChildSpaceShapes;
	//子级空间分割方式 -- true：板件分割；false：虚拟分割
	bool ChildrenSplitMode = false;
	//子级空间分割方向
	ESpaceDirectType ChildrenSplitDirect = SDT_None;

	//归属的柜体
	FCabinetShape* CabinetShapeData;

private:
	//空间ShapeActor
	ASCTSpaceActor* SpaceShapeActor = nullptr;	
	bool bNestedAdded = false; /*!< 标识门组、外盖抽屉组、电器饰条组是否为内嵌方式添加*/
	//当前空间中门组外盖受影响的空间和框体板件
	TArray<FSpaceShape*> CoverInfluencedSpaces;
	TArray<FBoardShape*> CoverInfluencedBoards;
	TArray<FBoardShape*> InsetInfluencedBoards;

private:
	FRangeAndStandValue WidthExportValue;  /*!< 宽度导出值，用于外部使用*/
	FRangeAndStandValue HeightExportValue; /*!< 高度导出值，用于外部使用*/
	FRangeAndStandValue DepthExportValue;  /*!< 深度导出值，用于外部使用*/
};
