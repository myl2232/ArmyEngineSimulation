#pragma once

#include "ArmyObject.h"
#include "ArmyCircle.h"
#include "ArmyPolygon.h"
#include "ArmyEditPoint.h"

/**
 * 户型绘制元素基类
 */
class ARMYSCENEDATA_API FArmySymbol : public FArmyObject
{
public:
	enum ETextType
	{
		ETT_DIRTEXT = 1,  //方向标记文本
		ETT_OWNERTEXT = 2 //符号所属文本
	};
	struct FTextInfo
	{
		FTextInfo(ETextType InType, const FString& InValue, int32 InFontSize = 15) :TextType(InType), TextValue(InValue), FontSize(InFontSize) {};
		ETextType TextType = ETextType::ETT_DIRTEXT;
		FString TextValue;
		int32 FontSize = 15;
		FVector Position = FVector(ForceInitToZero);

		bool operator==(const FTextInfo& InOther) const
		{
			return TextType == InOther.TextType;
		}
	};
	FArmySymbol();

	virtual ~FArmySymbol();

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

	void SetText(const FString& InText, FArmySymbol::ETextType InType = FArmySymbol::ETT_DIRTEXT) 
	{
		int32 Index = TextValueArray.AddUnique(FTextInfo(InType, InText));
		TextValueArray[Index].TextValue = InText;
	};

	void SetTextSize(int32 InTextSize, FArmySymbol::ETextType InType = FArmySymbol::ETT_DIRTEXT) 
	{
		int32 Index = TextValueArray.AddUnique(FTextInfo(InType, ""));
		TextValueArray[Index].FontSize = InTextSize;
	};

	void SetDrawArrows(bool bDrawArrows = true) {DrawArrows = bDrawArrows;};

	void SetDrawSpliteLine(bool bDrawSpliteLine = true) { DrawSpliteLine = bDrawSpliteLine; };
	// 获得绘制CAD的线段列表
	void GetDrawCadLines(TArray<TPair<FVector, FVector>>& OutLineList);
	// 获得绘制CAD的圆
	TSharedPtr<class FArmyCircle> GetDrawCadCircle();
	// 获得绘制的文字
	FScaleCanvasText GetCanvasText();
	// 是否绘制箭头
	bool GetIsDrawArrows() { return DrawArrows; };
private:
	/**	变换矩阵更改后的参数更新*/
	void Update();
private:
	/**	变换矩阵*/
	FTransform LocalTransform;

	FLinearColor BorderCircleColor = FLinearColor::White;

	bool DrawArrows = true;

	bool DrawSpliteLine = false;

	float CircleRadius;

	FVector CirclePoint;

	TArray<FTextInfo> TextValueArray;

	TArray<FVector> BaseArrowsVertices;

	TArray<struct FSimpleElementVertex> ArrowsVertexArray;
};
REGISTERCLASS(FArmySymbol)