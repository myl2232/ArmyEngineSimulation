#pragma once

#include "ArmyObject.h"
#include "ArmyPolygon.h"

//#include "Engine/TextRenderActor.h"
//#include "Components/TextRenderComponent.h"

class FArmyEditorInterface;

class ARMYSCENEDATA_API FArmyDimensions :
	public FArmyObject
{
public:
	enum ArrowsType
	{
		Default, //默认箭头
		Diagonal //建筑斜线
	};
public:
	/**	构造函数*/
	FArmyDimensions();

	/**	析构函数*/
	virtual ~FArmyDimensions();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual void SetState(EObjectState InState) override;

	virtual void SetForceVisible(bool InV) override;

	virtual void DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);
	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);

	//方位转换
	virtual void ApplyTransform(const FTransform& Trans) override;

	/**	获取包围盒*/
	virtual const FBox GetBounds();

	/**设置基础颜色*/
	void SetBaseColor(const FLinearColor& InColor);

	/**获取基础颜色*/
	const FLinearColor& GetBaseColor() const;

	/**设置文本字体大小*/
	void SetTextSize(int32 InSize);

	/**	获取文本字体大小*/
	int32 GetTextSize();

	/**	设置延伸最大值*/
	void SetExtentMaxValue(float InExtent) { ExtentMaxV = InExtent; };

	/**	设置箭头样式*/
	void SetArrowsType(ArrowsType InType);

	/**	设置标注基线起始点*/
	void SetBaseStart(FVector InStartPoint);

	/**	设置标注基线结束点*/
	void SetBaseEnd(FVector InEndPoint);

	/**	设置标注起始点*/
	void SetDimensionsStart(FVector InStartPoint);

	/**	设置标注结束点*/
	void SetDimensionsEnd(FVector InEndPoint);

	/**	设置连续标注*/
	void SetSeries(FVector InStartPoint, FVector InExtentPoint,FVector InDir);

	/**	随鼠标位置更新*/
	void UpdateMouse(FVector InMouseWorldPos);

	/**	更新标注箭头*/
	void UpdateArrows();

	/**	更新标注文本*/
	void UpdateText();

	/**	更新标注线*/
	void UpdateLines();

    /** 标注文字 */
    void CreateText();
    void DeleteText();

public:
	int32 ClassType = -1;

	int32 CurrentState;//-1 未开始，1已绘制第一个基点，2已绘制第二个基点，3 连续标注状态, 0已完成绘制

	float ExtentMaxV = 15;
	ArrowsType CArrowsType;
	int32 AreaType;//标注的范围类型，1、水平标注、2、垂直标注、0、原线标注
	float DimensionValue;

	TArray<TSharedPtr<FArmyLine>> InnerOutLines;

	FVector SeriesDir;
	/** 表示标尺的2D线段 */
	TSharedPtr<FArmyLine> MainLine;
	/**	标尺左侧界线*/
	TSharedPtr<FArmyLine> LeftLine;
	/**	标尺右侧界线*/
	TSharedPtr<FArmyLine> RightLine;
	/**	标尺左侧箭头*/
	TSharedPtr<FArmyPolygon> LeftArrows;
	/**	标尺左侧箭头*/
	TSharedPtr<FArmyPolygon> RightArrows;

	bool UseLeadLine = false;

	bool NeedLeadLine = false;

	bool ForceUseLeadLine = false;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);
	virtual void Refresh();
	void SetLeadPoint(const FVector& InPos);
	void SetEndPoint(const FVector& InPos);
	/* @梁晓菲  强制使用引线*/
	void SetForceUseLeadLine();
	/* @梁晓菲 更新标注引线*/
	void UpdateDownLeadLine();
	/* @梁晓菲 停止使用引线*/
	void StopUseLeadLine();

	/** 操作点 */
	TSharedPtr<FArmyPoint> LeftStaticPoint;
	TSharedPtr<FArmyPoint> RightStaticPoint;

	TSharedPtr<FArmyPoint> LeftExtentPoint;
	TSharedPtr<FArmyPoint> RightExtentPoint;

	TSharedPtr<FArmyPoint> TextControlPoint;

	FScaleCanvasText DimensionCanvasText;
	//ATextRenderActor* DimensionText;

	//UArmyEditorViewportClient* LocalViewPortClient;
private:
	FLinearColor BaseColor = FLinearColor(1,1,1);

	/*@梁晓菲 操作点*/
	TSharedPtr<class FArmyEditPoint> LeadEditPoint;
	TSharedPtr<class FArmyEditPoint> EndEditPoint;
	TSharedPtr<FArmyLine> LeadEditLine;
	TSharedPtr<FArmyLine> MainEditLine;
	bool BDefaultLeadLine;
	//设置两个操作点的默认位置
	void SetDefaulePos();
};
REGISTERCLASS(FArmyDimensions)
