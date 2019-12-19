#pragma once

#include "ArmyObject.h"

/**
 * 自定义绘制样式
 */
class ARMYSCENEDATA_API FArmyCustomDefine : public FArmyObject
{
public:
	FArmyCustomDefine();
	FArmyCustomDefine(const TArray<struct FSimpleElementVertex>& InVertexArray, const FTransform& Trans, int32 InBelongClass);
	virtual ~FArmyCustomDefine();

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

	void SetBaseVertexArray(const TArray<struct FSimpleElementVertex>& InVertexArray);

	void SetBelongClass(int32 InBelongClass) {BelongClass = InBelongClass;}

	int32 GetBelongClass() { return BelongClass; }
	// 获得绘制CAD的线段列表
	void GetDrawCadLines(TArray<TSharedPtr<FArmyLine>>& OutLineList);
private:
	/**	变换矩阵更改后的参数更新*/
	void Update();
private:
	int32 BelongClass = -1;
	/**	变换矩阵*/
	FTransform LocalTransform;
	TArray<TSharedPtr<FArmyLine>> LineArray;
	TArray<struct FSimpleElementVertex> BaseVertexArray;
	TArray<struct FSimpleElementVertex> DrawVertexArray;

	FBox Bounds;
};
REGISTERCLASS(FArmyCustomDefine)