/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRSymbolRectRegion.h
* @Description 立面索引图范围编辑框
*
* @Author 花恩
* @Date 2019年6月6日
* @Version 1.0
*/
#pragma once
#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmySymbolRectRegion :public FArmyObject
{
public:
	enum EDirectionType
	{
		EDT_LEFT = 1,
		EDT_RIGHT = 1 << 1,
		EDT_TOP = 1 << 2,
		EDT_BOTTOM = 1 << 3
	};

	FArmySymbolRectRegion();
	~FArmySymbolRectRegion();
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

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	const FBox GetBounds() override;

	void Init(const FVector& InBasePos,FArmySymbolRectRegion::EDirectionType InType);

	void Update(TSharedPtr<FArmyEditPoint> InEditPoint,const FVector& InOffset);

	const TSharedPtr<FArmyLine> GetMainLine() const { return MainLine; };

	//@杨云鹤 初始化选择框的宽高
	void SetSelectRectData(const float InNewLength, const float InNewWidth);

	//@杨云鹤 设置基点
	void SetBasePos(const FVector InBasePos);
private:
	EDirectionType DirectionType;
	TSharedPtr<FArmyLine> LeftLine;
	TSharedPtr<FArmyLine> RightLine;
	TSharedPtr<FArmyLine> FaceLine;
	TSharedPtr<FArmyLine> MainLine;

	FVector m_VInBasePos;

	float DefaultLenth = 172;
	float DefaultWidth = 64;
};
REGISTERCLASS(FArmySymbolRectRegion)