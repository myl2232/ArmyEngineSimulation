#pragma once
#include "ArmyObject.h"
#include "FArmyConstructionItemInterface.h"

class ARMYSCENEDATA_API FArmyPackPipe : public FArmyObject
{
public:

	FArmyPackPipe();
	~FArmyPackPipe() {};

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	virtual const FBox GetBounds();
	virtual TSharedPtr<FArmyLine> HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	// FObject2D Interface End

	virtual void ApplyTransform(const FTransform& Trans) override;

	virtual void Delete() override;

	virtual bool IsPointInObj(const FVector & Pos);

	void SetVertexes(TArray<FVector>& Vertexes, bool bDeserialization = false);

	TSharedPtr<FArmyLine> GetParallelLine() const;

	void SetHeight(float InValue) { Height = InValue; }
	float GetHeight() { return Height; }

	void SetThickness(float InValue) { Thickness = InValue; }
	float GetThickness() { return Thickness; }

	void SetLength(float InValue) { Length = InValue; }
	float GetLength() { return Length; }

	void PreCalTransformIllegal(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag);
	bool PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag);

	void GetAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void SetInsideVertices(TArray<FVector> InVertexes);

	void SetOutsideVertices(TArray<FVector> InVertexes);

	void GetInsideVertices(TArray<FVector>& Vertexes);

	void GetOutsideVertices(TArray<FVector>& Vertexes);

	void GetFillOutsideVertices(TArray<FVector>& Vertexes);

	/** @纪仁泽 获得立面投影box */ 
	TArray<struct FLinesInfo> GetFacadeBox(TSharedPtr<class FArmyLine> InLine);
	
	/** @欧石楠 设置额外绘制的线*/
	void SetExternalLines(TArray< TSharedPtr<FArmyLine> >& InLines);
private:
	void UpdatePolyVertices(bool bDeserialization = false);

	void UpdateSeletedRulerLine();

	bool CalInternalWallIntersection(TArray<FVector> Points);

	void OnLineInputBoxCommitted(float InLength);

	void OnLineInputBoxCommittedCancel();

	void OnLineInputBoxCommittedCommon(const FText& InText, const ETextCommit::Type InTextAction);

public:
	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	TSharedPtr<class FArmyPolygon> Polygon;

	bool bModified = false;
protected:
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;	

	TArray<TSharedPtr<class FArmyLine>> PolyLines;


	TSharedPtr<class FArmyRulerLine> SelectedRulerLine;
	TSharedPtr<class FArmyLine> SelectedLine;
	FVector CenterPos;

	TSharedPtr<class FArmyAxisRuler>AxisRuler;

	float Height;
	float Length;
	float Thickness;

private:

	/**@欧石楠 平面视图绘制*/
	TSharedPtr<class FArmyPolygon> PlaneViewPolygon;
	UMaterialInstanceDynamic* MI_PlaneView;

	/** @欧石楠 额外绘制的线条用于补足房间被截掉的线*/
	TArray<TSharedPtr<FArmyLine>> ExternalLines;

	TSharedPtr<class FArmyPolygon> FillPolygon;
	//UMaterialInstanceDynamic* MI_FillPolygon;

	//判断选择的线是否为新建墙端点线
	bool bIsStartOrEndLine = false;

	//@ 选择单边时，存储标尺线的方向
	FVector SelectedRulerDir;

	TArray<FVector> InsideVertices;
	TArray<FVector> OutsideVertices;

	TArray<FVector> FillOutSideVertices;
};

REGISTERCLASS(FArmyPackPipe)