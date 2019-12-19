#pragma once

#include "Primitive2D/ArmyPrimitive.h"

class ARMYDRAWCORE_API FArmyPolygon :public FArmyPrimitive
{
public:
	FArmyPolygon();
    FArmyPolygon(FArmyPolygon* Copy);
	~FArmyPolygon() {}

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	void DrawOutLine(FPrimitiveDrawInterface* PDDI, const FSceneView* View);
	bool IsSelected(const FBox2D& box) override;
	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	void SetVertices(const TArray<FVector>& InVertices);
	void AddVertice(FVector InVertice);

	void RemoveAllVertexes();

	void RemoveVertice(FVector InVertice);

	void SetZoomFactor(float InFactor) { ZoomFactor = InFactor; }

	void SetZoomWithCameraHeight(bool Enable) { ZoomWithCameraHeight = Enable; }

	void GetLines(TArray< TSharedPtr<class FArmyLine> >& OutLines);

	void SetXDirRepeatDist(float InDist) { XDirRepeatDist = InDist; }

	void SetYDirRepeatDist(float InDist) { YDirRepeatDist = InDist; }

	void SetDrawAlignment(uint32 inAlignment);

	void SetPolygonOffset(float InOffset);

	void SetAlignPoint(FVector2D InAlignMentPoint) { AlignPoint = InAlignMentPoint; }

	//@ 获取多边形边线颜色
	FLinearColor GetOutLineColor() const { return OutLineColor; }
	//@ 设置多边形边线颜色
	void SetOutLineColor(FLinearColor InOutLineColor) { OutLineColor = InOutLineColor; }

	/**
	* @brief 射线法，判断某个点时候在一个polygon内
	* @param Position--->点的位置，该点默认是在polygon内不能在polygon边上并且Z=0
	* @param ContainInsied--->该点是否在polygon边上
	*/
	bool IsInside(const FVector& Position, bool ContainInsied = false) const;
	/**
	* @brief 判断两条线段是否相交
	*/
	static bool IsIntersected(const FVector& Line1StartPos, const FVector& Line1EndPos, const FVector& Line2StartPos, const FVector& Line2EndPos);
public:

	FLinearColor FillColor;

	FMaterialRenderProxy* MaterialRenderProxy;

	float Thickness;

	TArray<FVector> Vertices;

	TArray<FVector2D> UsePoints;

protected:
	float PolygonOffset = 0.0f;
	void PrePareCalculate(const TArray<FVector>& Vertices);
	uint32 CurrentAlginMent;
	bool ZoomWithCameraHeight = true;
	float ZoomFactor;
	float XDirRepeatDist;
	float YDirRepeatDist;
	FVector2D MinPoint;
	FVector2D MaxPoint;
	FVector2D AlignPoint;

	//@ 多边形边线颜色
	FLinearColor OutLineColor;
};

