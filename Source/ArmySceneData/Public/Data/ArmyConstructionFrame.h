#pragma once

#include "ArmyObject.h"
#include "ArmyRect.h"
#include "dxfFile.h"
#include "ArmyEditPoint.h"

 
class ARMYSCENEDATA_API FArmyConstructionFrame : public FArmyObject
{
public:
	struct FillAreaInfo
	{
		FillAreaInfo(TSharedPtr<FArmyRect> InArea, uint32 InFontSize)
		{
			RectArea = InArea;
			FontSize = InFontSize;
		}
		FLinearColor BaseTextColor = FLinearColor::White;
		FLinearColor CurrentTextColor = FLinearColor::White;

		uint32 FontSize = 12;
		FString TextContent;
		TSharedPtr<FArmyFurniture> DXFData;
		TSharedPtr<FArmyRect> RectArea;
		bool operator ==(const FillAreaInfo& Other)const
		{
			return (RectArea == Other.RectArea);
		}
	};
public:
	FArmyConstructionFrame(bool IsEditModel = false);

	virtual ~FArmyConstructionFrame();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	virtual void SetState(EObjectState InState);

 
	virtual void Refresh();

 
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

 
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

 
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

 
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

 
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);
 
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

 
	virtual void GetVertexes(TArray<FVector>& OutVertexes);

 
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);

 
	virtual void RemoveLine(TSharedPtr<FArmyLine> InLine);

 
	virtual void ApplyTransform(const FTransform& Trans);

	 
	virtual const FVector GetBasePos();

 
	virtual const FBox GetBounds();

	
	void AddFrameLine(TSharedPtr<FArmyLine> InLine);

 
	void DeleteFrameLine(TSharedPtr<FArmyLine> InLine);


	void AddFillArea(int32 InType,TSharedPtr<FArmyRect> InRectArea,uint32 InFontSize);

 
	void DeleteFillArea(int32 InType);

	 
	FBox GetAreaBox(int32 InType) const;

	void FillAreaText(int32 InType, FString InContent);

 
	void FillAreaDxf(int32 InType,const TSharedPtr<FArmyFurniture> InDXFData);

	void SetTranFormModel(bool InABSModel = false);

	void SetEditorModel(bool IsEdit) { EditState = IsEdit; }

	void BuildOperationPoint();

	// 获得绘制cad的相关数据
	void GetFrameBaseLines(TArray<TSharedPtr<FArmyLine>> &OutLines);
	void GetFillAreasMap(TMap<int32, FillAreaInfo> &OutFillAreasMap);
	FTransform GetLocalTransform();

	// 获得图框类型
	EConstructionFrameType GetFrameType() { return FrameType; }
	// 设置图框类型
	void SetFrameType(EConstructionFrameType InFrameType) { FrameType = InFrameType; }
private:
 
	void Update();
private:
 
	// 图框类型 0 默认  1：橱柜
	EConstructionFrameType FrameType;
	bool EditState = false;

	 
	bool ApplyABSTransform = false;

 
	FTransform LocalTransform;

	TSharedPtr<FArmyEditPoint> MoveOperationPoint;

	TSharedPtr<FArmyEditPoint> ScaleOperationPointLeftTop;
	TSharedPtr<FArmyEditPoint> ScaleOperationPointLeftBottom;
	TSharedPtr<FArmyEditPoint> ScaleOperationPointRightTop;
	TSharedPtr<FArmyEditPoint> ScaleOperationPointRightBottom;

 
	TArray<TSharedPtr<FArmyLine>> FrameBaseLines;

 
	TMap<int32, FillAreaInfo> FillAreaMap;

	//TMap<int32, TSharedPtr<FArmyFurniture>> FillAreaDxfMap; 
};
REGISTERCLASS(FArmyConstructionFrame)