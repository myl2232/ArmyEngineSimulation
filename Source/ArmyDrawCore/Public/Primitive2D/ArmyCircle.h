#pragma once

#include "Primitive2D/ArmyPrimitive.h"

class ARMYDRAWCORE_API FArmyCircle :public FArmyPrimitive
{
public:
	FArmyCircle();
    FArmyCircle(FArmyCircle* Copy);
	~FArmyCircle();

	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)override;
	bool IsSelected(const FBox2D& rect)override;

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	void SetStartPos(const FVector& InPos);
	FVector GetStartPos()const { return ArcStartPoint; }
	
	void SetEndPos(const FVector& InPos);
	FVector GetEndPos()const { return  ArcEndPoint; }

	void SetArcMiddlePos(const FVector& InPos);
	FVector GetArcMiddlePos()const { return ArcMiddlePoint; }

	void SetPosition(const FVector& InPos);
	FVector GetPosition()const { return Position; }

	void SetTreePointPos(const FVector& InStart, const FVector& InEnd, const FVector& InMiddle);

	bool GetUpDirection()const { return IsUp; }

	TArray<FVector> GetVertices() { return TotalVertices; }

    /** @欧石楠 获得绘制顶点数据 */
    TArray<FVector> GetDrawingVertexes();

	float GetCircleLength();

    /** 更新顶点数据 */
    void UpdateVertices();
	
public:
	FVector Position;
	FVector XDirection;
	FVector YDirection;
	float Radius;
	uint32 NumSides;
	float Thickness;
	FLinearColor FillColor;
	bool MDrawDashCircle;
	float ArcAngle;

private:
	bool IsUp;
	TArray<FVector> TotalVertices;
	FVector ArcStartPoint;
	FVector ArcEndPoint;
	FVector ArcMiddlePoint;

    /** @欧石楠 顶点绘制数据 */
    TArray<FVector> DrawingVertexes;
};

