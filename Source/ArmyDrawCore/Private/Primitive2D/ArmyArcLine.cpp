#include "ArmyArcLine.h"
#include "ArmyEditorViewportClient.h"
FArmyArcLine::FArmyArcLine()
{
	NumSides = 32;
	Position = FVector::ZeroVector;
}

FArmyArcLine::FArmyArcLine(FArmyArcLine* Copy)
{
	Position = Copy->Position;
	IsUp = Copy->IsUp;
	TotalVertices = Copy->TotalVertices;
	ArcStartPoint = Copy->ArcStartPoint;
	ArcMiddlePoint = Copy->ArcMiddlePoint;
	ArcEndPoint = Copy->ArcEndPoint;
	NumSides = Copy->NumSides;
	Radius = Copy->Radius;
	ArcAngle = Copy->ArcAngle;
}

FArmyArcLine::~FArmyArcLine()
{

}

bool FArmyArcLine::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return false;
}

bool FArmyArcLine::IsSelected(const FBox2D& rect)
{
	return false;
}

void FArmyArcLine::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	int number = TotalVertices.Num();
	for (int i = 1; i < number; i++)
	{
		PDI->DrawLine(TotalVertices[i], TotalVertices[i - 1], GetBaseColor(), DepthPriority, 1.0f, 0.0f, true);
	}
}

void FArmyArcLine::SetStartPos(const FVector& InPos)
{
	ArcStartPoint = InPos;
	UpdateVertices();
}

void FArmyArcLine::SetEndPos(const FVector& InPos)
{
	ArcEndPoint = InPos;
	UpdateVertices();
}

void FArmyArcLine::SetArcMiddlePos(const FVector& InPos)
{
	ArcMiddlePoint = InPos;
	UpdateVertices();
}

void FArmyArcLine::SetTreePointPos(const FVector& InStart, const FVector& InEnd, const FVector& InMiddle)
{
	ArcStartPoint = InStart;
	ArcMiddlePoint = InMiddle;
	ArcEndPoint = InEnd;
	UpdateVertices();
}

float FArmyArcLine::GetCircleLength()
{
	float arcLength = (ArcAngle / 360.0f)*(2 * PI * Radius);
	return arcLength;
}

// 设置cad绘制所需的点
void FArmyArcLine::SetCadPos(const FVector& InStart, const FVector& InEnd, const FVector& InCenter)
{
	ArcStartPoint = InStart;
	ArcEndPoint = InEnd;
	Position = InCenter;
}
void FArmyArcLine::UpdateVertices()
{
	float OffSetHeight = (ArcMiddlePoint - (ArcStartPoint + ArcEndPoint) / 2.0f).Size();
	if ((ArcStartPoint - ArcEndPoint).IsNearlyZero() || FMath::IsNearlyZero(OffSetHeight, 0.01f))
	{
		TotalVertices.Empty();
		TotalVertices.Push(ArcStartPoint);
		TotalVertices.Push(ArcMiddlePoint);
		TotalVertices.Push(ArcEndPoint);
		return;
	}
	FVector horizontal0 = (ArcEndPoint - ArcStartPoint).GetSafeNormal();
	FVector vertical0 = horizontal0.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector projectionPoint = FMath::ClosestPointOnInfiniteLine(ArcStartPoint, ArcEndPoint, ArcMiddlePoint);
	FVector vertical1 = (ArcMiddlePoint - projectionPoint).GetSafeNormal();
	float angle = FVector::DotProduct(vertical1, vertical0);

	if (FMath::IsNearlyEqual(angle, 1, 0.002f))
	{
		IsUp = true;
	}
	else {
		IsUp = false;
	}
	float d = (ArcEndPoint - ArcStartPoint).Size();
	Radius = (OffSetHeight * OffSetHeight + d* d / 4.0f) / (2 * OffSetHeight);
	ArcAngle = FMath::RadiansToDegrees(FMath::Atan(d / (2 * (Radius - OffSetHeight))) * 2);
	if (ArcAngle < 0.0)
	{
		ArcAngle += 360.0;
	}
	float clockWise;
	if (IsUp)
	{
		Position = (ArcStartPoint + ArcEndPoint) / 2 - vertical0 * (Radius - OffSetHeight);
		clockWise = 1.0f;
	}
	else
	{
		Position = (ArcStartPoint + ArcEndPoint) / 2 + vertical0 *(Radius - OffSetHeight);
		clockWise = -1.0f;
	}

	float deltaAngle = ArcAngle / NumSides;
	float startAngle = 0.0f;

	TotalVertices.Empty();
	for (uint32 i = 0; i < NumSides; i++)
	{
		FVector result = FRotator(0, clockWise* startAngle, 0).RotateVector(ArcStartPoint - Position) + Position;
		startAngle += deltaAngle;
		TotalVertices.Push(result);
	}
	TotalVertices.Push(ArcEndPoint);
}

