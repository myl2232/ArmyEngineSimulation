/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRSplitLine.h
* @Description 分割线
*
* @Author 常远
* @Date 2018年12月17日
* @Version 1.0
*/
#pragma once
#include "ArmyObject.h"

#define SPlITLINE_WIDTH 2.0f

class ARMYSCENEDATA_API FArmySplitLine :public FArmyObject
{
public:

	FArmySplitLine();
	~FArmySplitLine() {};
	FArmySplitLine(FArmySplitLine* Copy);


	// 序列化为josn数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	// 反序列化为对象
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	// 重载绘制
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	// 设置分割线状态
	virtual void SetState(EObjectState InState) override;

	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false)override;
	// 选中
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;


	friend class FArmyRegionOperation;

private:
	// 设置依附的分割线id
	void SetDependentIdList();
protected:
	FLinearColor Color;

	// 房间编号
	FString SpaceObjectId;
	// 组成分割线的线段
	TArray<TSharedPtr<class FArmyLine>> LineList;
	// 依附的分割线
	TArray<TSharedPtr<class FArmySplitLine>> DependentList;
	// 依附的分割线的ID，保存时使用
	TArray<FGuid> DependentIdList;
	// 分割线索引，求闭合区域需要
	int  SplitLineIndex;

};

REGISTERCLASS(FArmySplitLine)