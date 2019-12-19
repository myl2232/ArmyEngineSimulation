#pragma once
#include "Primitive2D/ArmyPrimitive.h"
class ARMYDRAWCORE_API FArmyArcLine :public FArmyPrimitive
{
public:
	FArmyArcLine();
	FArmyArcLine(FArmyArcLine* Copy);
	~FArmyArcLine();

	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)override;
	bool IsSelected(const FBox2D& rect)override;

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	void SetStartPos(const FVector& InPos);
	FVector GetStartPos()const { return ArcStartPoint; }

	void SetEndPos(const FVector& InPos);
	FVector GetEndPos()const { return  ArcEndPoint; }

	void SetArcMiddlePos(const FVector& InPos);
	FVector GetArcMiddlePos()const { return ArcMiddlePoint; }

	FVector GetPosition() { return Position; }
	void SetPosition(FVector InPosition) {Position = InPosition; }

	float GetRadius() { return Radius; }
	void SetRadius(float InRadius) { Radius = InRadius; }

	void SetClockwise(bool  clockwise) { bClockwise = clockwise; }
	bool IsClockwise() { return bClockwise; }

	void SetTreePointPos(const FVector& InStart, const FVector& InEnd, const FVector& InMiddle);

	bool GetUpDirection()const { return IsUp; }

	const TArray<FVector>& GetVertices()const { return TotalVertices; }

	float GetCircleLength();

	float GetArcAngle()const { return ArcAngle; }
	uint32 NumSides;
	// 设置cad绘制所需的点
	void SetCadPos(const FVector& InStart, const FVector& InEnd, const FVector& InCenter);
protected:
	void UpdateVertices();
private:
	float Radius;
	bool IsUp;
	TArray<FVector> TotalVertices;
	FVector ArcStartPoint;
	FVector ArcEndPoint;
	FVector ArcMiddlePoint;
	FVector Position;
	float ArcAngle;
	bool		 bClockwise;
};