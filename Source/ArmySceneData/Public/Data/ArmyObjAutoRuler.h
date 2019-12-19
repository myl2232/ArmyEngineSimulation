#pragma once

#include "CoreMinimal.h"
#include "ArmyObject.h"
//#include "ArmyBaseModel.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "ArmyPolygon.h"

class ARMYSCENEDATA_API FArmyObjAutoRuler : public FArmyObject
{
public:
	enum LineID
	{
		XFRONT = 1,
		XBACK = 1 << 1,
		YFRONT = 1 << 2,
		YBACK = 1 << 3,
		WIDTH = 1 << 4,
		LENGTH = 1 << 5
	};
    FArmyObjAutoRuler();
    virtual ~FArmyObjAutoRuler() {}

	void Update(TArray<FObjectPtr>& InObjects);
    // FObject2D Interface Begin
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
    virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	virtual void SetForceVisible(bool InV) override;
	void SetLineVisible(LineID InID, bool InV);
	bool IsLineVisible(LineID InID) {return (LineVisible &InID) > 0; };
	/**	设置绑定对象*/
	void SetBoundObj(FObjectPtr InObj);
	void SetAutoCheck(bool InCheck) { bAutoCheck = InCheck; };

	FObjectPtr GetBoundObj() { return BindObject; }

	/**	@梁晓菲 更新标注箭头*/
	void UpdateArrows(TSharedPtr<FArmyLine> MainLine, TSharedPtr<FArmyPolygon> LeftArrows, TSharedPtr<FArmyPolygon> RightArrows);
	/**	@梁晓菲 标尺箭头*/
	TSharedPtr<FArmyPolygon> XFrontStartArrows;
	TSharedPtr<FArmyPolygon> XFrontEndArrows;
	TSharedPtr<FArmyPolygon> XBackStartArrows;
	TSharedPtr<FArmyPolygon> XBackEndArrows;
	TSharedPtr<FArmyPolygon> YFrontStartArrows;
	TSharedPtr<FArmyPolygon> YFrontEndArrows;
	TSharedPtr<FArmyPolygon> YBackStartArrows;
	TSharedPtr<FArmyPolygon> YBackEndArrows;
private:
	TArray<EObjectType> IgoneObjectTypeArray;
	TSharedPtr<FArmyObject> BindObject;

	//ATextRenderActor* XFrontDimensionText;
	//ATextRenderActor* XBackDimensionText;
	//ATextRenderActor* YFrontDimensionText;
	//ATextRenderActor* YBackDimensionText;
	//ATextRenderActor* WidthDimensionText;
	//ATextRenderActor* LengthDimensionText;

	FScaleCanvasText XFrontDimensionText;
	FScaleCanvasText XBackDimensionText;
	FScaleCanvasText YFrontDimensionText;
	FScaleCanvasText YBackDimensionText;
	FScaleCanvasText WidthDimensionText;
	FScaleCanvasText LengthDimensionText;

	uint32 LineVisible;

	TSharedPtr<FArmyLine> XFrontLine;
	TSharedPtr<FArmyLine> XBackLine;

	TSharedPtr<FArmyLine> YFrontLine;
	TSharedPtr<FArmyLine> YBackLine;

	TSharedPtr<FArmyLine> WidthLine;
	TSharedPtr<FArmyLine> LengthLine;

	bool bAutoCheck = false;
};
REGISTERCLASS(FArmyObjAutoRuler)