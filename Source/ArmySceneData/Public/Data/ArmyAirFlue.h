/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRAirFlue.h
 * @Description 风道
 *
 * @Author 欧石楠
 * @Date 2018年11月8日
 * @Version 1.0
 */
#pragma once
#include "ArmySimpleComponent.h"

class ARMYSCENEDATA_API FArmyAirFlue : public FArmySimpleComponent
{

public:
	FArmyAirFlue();
	FArmyAirFlue(FArmyAirFlue* Copy);
	virtual ~FArmyAirFlue();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	//~ Begin FObject2D Interface
	//virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	//virtual bool Hover(const FVector& Pos) override;
	//virtual void ApplyTransform(const FTransform& Trans) override;
	/** 获取包围盒*/
	virtual const FBox GetBounds() override;
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	//~ End FObject2D Interface

	/** 更新2D显示 */
	virtual void Update() override;

	/*@欧石楠 拖拽top point时单独更新Top point**/
	virtual void UpdateTopPoint(FVector Pos) override;

	/*@欧石楠 拖拽bottom point时单独更新Bottom point**/
	virtual void UpdateBottomPoint(FVector Pos) override;

	/*@欧石楠 拖拽left point时单独更新left point**/
	virtual void UpdateLeftPoint(FVector Pos) override;

	/*@欧石楠 拖拽right point时单独更新right point**/
	virtual void UpdateRightPoint(FVector Pos) override;

	/** @欧石楠 当被清空时的调用，与上面的删除不一致*/
	virtual void OnDelete() override;

	/**@欧石楠 自适应*/
	virtual void AutoAdapt() override;

	/**@欧石楠 自吸附*/
	virtual void AutoAttach(float Invalue = 0) override;

	virtual bool CheckAllPointsInRoom(FVector Pos) override;

	virtual void SetAutoAdapt(bool bValue) override;
protected:

	/**@欧石楠 选中控件时的标注虚线框*/	
	virtual void OnTopInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;
	
	virtual void OnBottomInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;
	
	virtual void OnLeftInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;
	
	virtual void OnRightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;

private:
	TSharedPtr<FArmyLine> RightCenterToRightTopLine;
	TSharedPtr<FArmyLine> LeftBottomToRightCenterLine;

	TSharedPtr<FArmyLine> OtherModeRightCenterToRightTopLine;
	TSharedPtr<FArmyLine> OtherModeLeftBottomToRightCenterLine;
};
REGISTERCLASS(FArmyAirFlue)