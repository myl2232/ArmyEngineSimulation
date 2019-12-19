/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 空间轮廓：用于描述空间形状
*
* @Author 赵志强
* @Date 2018年9月21日
* @Version 1.0
*/
#pragma once
#include "SCTShapeData.h"
#include "SCTShape.h"

class FSCTShape;
class FBoardShape;
class FShapeAttribute;

//空间轮廓
class SCTSHAPEMODULE_API FSCTOutline : public FSCTShape
{
public:
	FSCTOutline();
	virtual ~FSCTOutline();

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

public:
	/** 轮廓名称 */
	FORCEINLINE void SetOutlineName(const FString& InName) { OutlineName = InName; };
	FORCEINLINE FString GetOutlineName() const { return OutlineName; };

	/** 轮廓类型 */
	void SetOutlineType(EOutlineType InType);
	FORCEINLINE EOutlineType GetOutlineType() { return OutlineType; };

	/** 轮廓方向 */
	FORCEINLINE void SetOutlineDirect(int32 InType) { DirectType = InType; };
	FORCEINLINE int32 GetOutlineDirect() { return DirectType; };

	/** 轮廓空间尺寸 */
	bool SetSpaceWidth(float InValue);
	bool SetSpaceDepth(float InValue);
	bool SetSpaceHeight(float InValue);
	FORCEINLINE float GetSpaceWidth() { return SpaceWidth; };
	FORCEINLINE float GetSpaceDepth() { return SpaceDepth; };
	FORCEINLINE float GetSpaceHeight() { return SpaceHeight; };
	/** 判断输入空间尺寸值是否有效 */
	bool IsValidForWidth(float InValue);
	bool IsValidForDepth(float InValue);
	bool IsValidForHeight(float InValue);
	/** 计算轮廓尺寸变化范围 */
	float GetOutlineWidthRange(float& OutMinValue, float& OutMaxValue);
	float GetOutlineDepthRange(float& OutMinValue, float& OutMaxValue);
	float GetOutlineHeightRange(float& OutMinValue, float& OutMaxValue);

	/** 虚拟分割轮廓 */
	bool SplitSpaceByVirtual(TArray<TSharedPtr<FSCTOutline>> ResultOutlines, ESpaceDirectType InDirect, TArray<float> InPartValues);
	/** 板件分割轮廓 */
	bool SplitSpaceByBoard(TArray<TSharedPtr<FSCTOutline>> ResultOutlines, FBoardShape* InBoard, ESpaceDirectType InDirect, TArray<float> InPartValues);

public:
	/** 轮廓形状参数 */
	float GetOutlineParamValueByIndex(int32 InIndex);
	float GetOutlineParamValueByRefName(const FString &InRefName);
	bool SetOutlineParamValueByRefName(const FString &InRefName, float InValue);
	TSharedPtr<FShapeAttribute> GetOutlineParamAttriByRefName(const FString &InRefName);
	TSharedPtr<FShapeAttribute> GetOutlineParamAttriByIndex(int32 InIndex);
	void AddOutlineParamAttri(TSharedPtr<FShapeAttribute> InAttri);
	void ClearOutlineParamAttris();
	FORCEINLINE int32 GetOutlineParamCount() const { return OutlineParameters.Num(); }

private:
	//FAny* FSCTShape::OnFindOwnValFunc(const FString& ValName);
	
private:
	//名称
	FString OutlineName = TEXT("Rectangle");
	//轮廓类型
	EOutlineType OutlineType = OLT_None;
	//轮廓方向: 1-X轴，2-Y轴，3-Z轴
	int32 DirectType = 0;
	//轮廓参数
	TArray<TSharedPtr<FShapeAttribute>> OutlineParameters;

	//轮廓尺寸
	float SpaceWidth = 100.0;
	float SpaceDepth = 100.0;
	float SpaceHeight = 100.0;
};


