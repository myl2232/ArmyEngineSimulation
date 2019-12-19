#pragma once

#include "ArmyObject.h"
#include "ArmyPolyline.h"

class ARMYSCENEDATA_API FArmyBoardSplitline :
	public FArmyObject
{
public:
	/**	构造函数*/
	FArmyBoardSplitline();

	/**	析构函数*/
	virtual ~FArmyBoardSplitline();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual void SetState(EObjectState InState) override;

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);

	/**	获取所有单独的点*/
	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

	/**	获取分割线的全部线集合*/
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;

	/**设置基础颜色*/
	void SetBaseColor(const FLinearColor& InColor);

	/**获取基础颜色*/
	const FLinearColor& GetBaseColor() const;

	/**	设置基线起始点*/
	void SetBaseStart(FVector InStartPoint);

	/**	设置基线结束点*/
	void SetBaseEnd(FVector InEndPoint);
private:
	void Update();
private:
	TSharedPtr<FArmyPoint> LeftStaticPoint;
	TSharedPtr<FArmyPoint> RightStaticPoint;
	/** 表示切割线 */
	TSharedPtr<FArmyPolyline> MainLine;

	FLinearColor BaseColor = FLinearColor(1,1,1);
};
REGISTERCLASS(FArmyBoardSplitline)
