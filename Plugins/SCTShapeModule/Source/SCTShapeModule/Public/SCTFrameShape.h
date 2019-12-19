/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTFrameShape.h
* @Description 柜体外框
*
* @Author 赵志强
* @Date 2018年9月8日
* @Version 1.0
*/
#pragma once

#include "SCTShape.h"

class FBoardShape;
class FSpaceShape;
class FSCTOutline;

//框体板件
class SCTSHAPEMODULE_API FrameBoardInfo
{
public:
	//板件类型：
	//顶板 Type==0//底板 Type==1//左板 Type==2
	//右板 Type==3//背板 Type==4//前板 Type==5
	int32 BoardType = 0;
	//板件索引
	int32 BoardIndex = 0;
	//前向外延计数
	int32 FrontCount = 0;
	//前向外延值
	float FrontRetract = 0.0;
	//板件数据
	TSharedPtr<FBoardShape> FrameBoardData = nullptr;
};

//柜体外框
class SCTSHAPEMODULE_API FFrameShape : public FSCTShape
{
public:
	FFrameShape();
	virtual ~FFrameShape();

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
	void SpawnActorsForSelected(FName InSelectProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 隐藏框体中所有的ShapeActors */
	void HiddenFrameShapeActors(bool bHidden);

public:
	//所属的柜体空间
	FORCEINLINE void SetSpaceShapeData(FSpaceShape* InSpaceShape) { SpaceShapeData = InSpaceShape; };
	FORCEINLINE FSpaceShape* GetSpaceShapeData() const { return SpaceShapeData; };

	/** 判断框体板件是否为空 */
	bool IsFrameBoardEmpty();
	/** 清空框体板件 */
	void EmptyFrameBoard();

	//框体板件
	/** 添加框体板件 */
	FBoardShape* AddFrameBoardShape(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	/** 添加框体板件信息 */
	FBoardShape* AddFrameBoardInfo(const FrameBoardInfo& InFrameBoardInfo);
	/** 替换框体板件 */
	bool ChangeFrameBoardShape(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	/** 移除指定的框体板件 */
	bool RemoveFrameBoardShape(FBoardShape* InBoard);
	/** 移除指定类型和索引的框体板件 */
	bool RemoveFrameBoardShapeByTypeIndex(int32 InType, int32 InIndex);
	/** 获取所有框体板件信息 */
	TArray<FrameBoardInfo>& GetAllFrameBoardInfos() { return FrameBoardInfos; }
	/** 获取所有顶底左右侧板 */
	void GetUpDownLeftRightFrameBoardShapes(TArray<FBoardShape*>& OutFrameBoardShapes);
	/** 获取指定类型和索引的框体板件 */
	TSharedPtr<FBoardShape> GetFrameBoardShape(int32 InType, int32 InIndex);
	/** 获取指定板件的类型和索引 */
	void GetFrameBoardTypeIndex(int32& InOutType, int32& InOutIndex, FBoardShape* InBoard);
	/** 计算框体板件的位置和尺寸以及轮廓信息 */
	bool CalFrameBoardPositionDimension(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex, float InFrontExtension);
	/**  计算前板对顶底左右板子的外盖关系 */
	TTuple<bool, bool, bool, bool> GetFrontBoardCoverdTopBottomLeftRightBoard();
	//更新柜体框
	bool UpdateAllFrameShapes();

	/** 修改框体板件的内移和厚度值 */
	bool ModifyFrameBoardRetrack(int32 InType, int32 InIndex, float InValue);
	bool ModifyFrameBoardHeight(int32 InType, int32 InIndex, FString InValue);

	/** 设置框体板件前方向外延值 */
	bool AddBoardFrontExtension(FBoardShape* InBoard, float InValue);
	void RemoveBoardFrontExtension(FBoardShape* InBoard);
	/** 设置框体板件上方向外延值 */
	bool AddBoardUpExtension(FBoardShape* InBoard, float InValue);
	void RemoveBoardUpExtension(FBoardShape* InBoard);

public:
	/** 计算框体内部空间的位置和尺寸 */
	bool CalInsideSpacePosDim(FVector& InsidePosition, FVector& InsideDimention);
	/** 计算框体内部空间轮廓的参数 */
	bool CalInsideOutlineParm(TSharedPtr<FSpaceShape> InSpace);

private:
	/** 判断给定两个板件的添加顺序 */
	bool IsBoardInSequence(TSharedPtr<FBoardShape> InBoardShape0, TSharedPtr<FBoardShape> InBoardShape1);
	float GetSpaceConfluenceCoverdBoardValue() const;
	/** 计算不同轮廓框体内部空间轮廓的参数 */
	bool CalFrameBoardPosDimenForRectangle(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForLeftGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForRightGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForBackGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForLeftPiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForRightPiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);
	bool CalFrameBoardPosDimenForMiddlePiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex);

private:
	//柜体框板件
	TArray<FrameBoardInfo> FrameBoardInfos;
	//所属的柜体空间
	FSpaceShape* SpaceShapeData;
};

