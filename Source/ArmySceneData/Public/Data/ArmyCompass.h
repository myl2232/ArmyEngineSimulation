#pragma once

#include "ArmyObject.h"
#include "ArmyCircle.h"
#include "ArmyPolygon.h"
#include "ArmyEditPoint.h"

/**
 * 户型绘制元素基类
 */
class ARMYSCENEDATA_API FArmyCompass : public FArmyObject
{
public:
	FArmyCompass();

	virtual ~FArmyCompass();

	void Init();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	virtual void SetState(EObjectState InState);

	/** 刷新 */
	virtual void Refresh();

	/** 绘制 */
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** HUD绘制 */
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 选中操作点 */
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮操作点 */
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有顶点*/
	virtual void GetVertexes(TArray<FVector>& OutVertexes);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);
	/** 移动位置 */
	virtual void ApplyTransform(const FTransform& Trans);

	/**	获取基点*/
	virtual const FVector GetBasePos();

	/** 获取包围盒*/
	virtual const FBox GetBounds();

	void SetBorderCircleRadius(float InRadius);

	void SetBorderCircleColor(FLinearColor InColor);

	void SetArrows(TSharedPtr<FArmyPolygon> InArrows);

	void SetTranFormModel(bool InABSModel = false);

	void BuildOperationPoint();

	TSharedPtr<FArmyEditPoint> GetMoveOperationPoint() { return MoveOperationPoint; };

	TSharedPtr<FArmyEditPoint> GetScaleOperationPoint() { return ScaleOperationPoint; };

	TSharedPtr<FArmyEditPoint> GetRotateOperationPoint() { return RotateOperationPoint; };

	// 获得圆-CAD
	TSharedPtr<FArmyCircle> GetCircleInfo();
	// 获得填充多边形-CAD
	TSharedPtr<FArmyPolygon> GetArrows() { return Arrows; };
	// 获得填充文字-CAD
	FScaleCanvasText GetCanvasText();
private:
	/**	变换矩阵更改后的参数更新*/
	void Update();
private:
	/**	应用变换是否是绝对变换，false则为增量变换*/
	bool ApplyABSTransform = false;

	/**	变换矩阵*/
	FTransform LocalTransform;

	TSharedPtr<FArmyEditPoint> MoveOperationPoint;

	TSharedPtr<FArmyEditPoint> ScaleOperationPoint;

	TSharedPtr<FArmyEditPoint> RotateOperationPoint;

	float BorderCircleRadius = 100;

	FLinearColor BorderCircleColor = FLinearColor::White;

	TArray<FVector> BaseArrowsVertices;

	TSharedPtr<FArmyPolygon> Arrows;
};
REGISTERCLASS(FArmyCompass)