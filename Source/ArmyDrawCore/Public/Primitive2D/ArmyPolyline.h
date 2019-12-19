#pragma once

#include "ArmyPrimitive.h"

class ARMYDRAWCORE_API FArmyPolyline :public FArmyPrimitive
{
public:
	enum ELineType
	{
		Lines,
		LineStrip,
		Line_Loop,
	};

	FArmyPolyline();
	FArmyPolyline(FArmyPolyline* Copy);
	~FArmyPolyline();

	void SetLineType(ELineType InType) { mLineType = InType; }
	ELineType GetLineType() { return mLineType; }
	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	bool IsSelected(const FBox2D& box)override;
	bool IsSelected(const FVector& Point, class UArmyEditorViewportClient* InViewportClient)override;

	void SetVertices(TArray<FVector>& Vertices) { LineVertices = Vertices; }
	void AddVertice(FVector Pos) { LineVertices.Push(Pos); }
	TArray<FVector>& GetAllVertices() { return LineVertices; }

	void SetBasePoint(const FVector& pivtPoint) override { mPivotPoint = pivtPoint; }
	FVector GetBasePoint()const { return mPivotPoint; }
	void SetTransform(const FTransform& InTransform) { mLocalTransform = InTransform; }
	const FTransform& GetTransform() { return mLocalTransform; }
	void SetLineWidth(const float InWidth) { LineWidth = InWidth; }
	float GetLineWidth()const { return LineWidth; }
	void SetMaterial(UMaterial* InMaterial) { TriangleMaterial = InMaterial; }
	void SetColor(const FLinearColor& PolyLineColor); 
	const FLinearColor& GetColor() { return GetBaseColor(); }

protected:
    void DrawWithTriangle(FPrimitiveDrawInterface* PDI, const FSceneView* View);

public:
	TArray<FVector> LineVertices;

protected:
	struct VertextInfo
	{
		FVector Position;
		FVector2D UV;
		FVector Normal;
		FVector Tangent;
		FColor VerticeColor;
	};

	TArray<VertextInfo> TotalTriangleVertexs;
	FVector mPivotPoint;
	ELineType mLineType;
	FTransform mLocalTransform;
	float LineWidth;
	UMaterial* TriangleMaterial;
	UMaterialInstanceDynamic* mDynamicMaterial;
};