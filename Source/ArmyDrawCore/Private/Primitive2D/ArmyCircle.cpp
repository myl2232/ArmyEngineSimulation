#include "ArmyCircle.h"
#include "Math/UnrealMathUtility.h"
#include "SceneManagement.h"

FArmyCircle::FArmyCircle()
    : Radius(1.f)
    , NumSides(32)
    , Position(FVector::ZeroVector)
    , Thickness(1.f)
    , XDirection(FVector(1, 0, 0))
    , YDirection(FVector(0, 1, 0))
    , MDrawDashCircle(false)
    , IsUp(true)
{
}

FArmyCircle::FArmyCircle(FArmyCircle* Copy)
{
	Position = Copy->Position;
	XDirection = Copy->XDirection;
	YDirection = Copy->YDirection;
	Radius = Copy->Radius;
	IsUp = Copy->IsUp;
	NumSides = Copy->NumSides;
	Thickness = Copy->Thickness;
	FillColor = Copy->FillColor;
	MDrawDashCircle = Copy->MDrawDashCircle;
	ArcAngle = Copy->ArcAngle;
	TotalVertices = Copy->TotalVertices;
    DrawingVertexes = Copy->DrawingVertexes;
	ArcStartPoint = Copy->ArcStartPoint;
	ArcEndPoint = Copy->ArcEndPoint;
	ArcMiddlePoint = Copy->ArcMiddlePoint;
	SetBaseColor(Copy->GetBaseColor());
}

FArmyCircle::~FArmyCircle()
{
}

bool FArmyCircle::IsSelected(const FBox2D& rect)
{
	FVector2D left = FVector2D(Position.X, Position.Y) + FVector2D(1, 0)*Radius;
	FVector2D bottom = FVector2D(Position.X, Position.Y) + FVector2D(0, -1)*Radius;
	FVector2D top = FVector2D(Position.X, Position.Y) + FVector2D(0, 1)*Radius;
	FVector2D right = FVector2D(Position.X, Position.Y) + FVector2D(1, 0)*Radius;
	return rect.IsInside(left) && rect.IsInside(bottom) && rect.IsInside(top) && rect.IsInside(right);
}

bool FArmyCircle::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return FVector::Distance(Pos, Position) <= Radius;
}

void FArmyCircle::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (MDrawDashCircle)
	{
		float AngleStep = (ArcAngle) / ((float)(NumSides));
		float CurrentAngle = 0;

		FVector LastVertex = Position + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * XDirection + FMath::Sin(CurrentAngle * (PI / 180.0f)) * YDirection);
		CurrentAngle += AngleStep;

		for (uint32 i = 0; i < NumSides; i++)
		{
			if (CurrentAngle <= ArcAngle)
			{
				FVector ThisVertex = Position + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * XDirection + FMath::Sin(CurrentAngle * (PI / 180.0f)) * YDirection);
				PDI->DrawLine(LastVertex, ThisVertex, GetBaseColor(), DepthPriority);
				FVector tempVertex = Position + Radius * (FMath::Cos((CurrentAngle + AngleStep) * (PI / 180.0f)) * XDirection + FMath::Sin((CurrentAngle + AngleStep) * (PI / 180.0f)) * YDirection);

				LastVertex = tempVertex;
				CurrentAngle += 2 * AngleStep;
			}
		}
	}
    else
    {
        float AngleStep = (ArcAngle) / ((float)(NumSides * 2));
        float CurrentAngle = 0;

        FVector LastVertex = Position + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * XDirection + FMath::Sin(CurrentAngle * (PI / 180.0f)) * YDirection);
        CurrentAngle += AngleStep;

        for (uint32 i = 0; i < NumSides * 2; i++)
        {
            if (CurrentAngle <= ArcAngle)
            {
                FVector ThisVertex = Position + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * XDirection + FMath::Sin(CurrentAngle * (PI / 180.0f)) * YDirection);
                PDI->DrawLine(LastVertex, ThisVertex, GetBaseColor(), DepthPriority);

                LastVertex = ThisVertex;
                CurrentAngle += AngleStep;
            }
        }
    }
}

void FArmyCircle::SetStartPos(const FVector& InPos)
{
	ArcStartPoint = InPos;
	UpdateVertices();
}

void FArmyCircle::SetEndPos(const FVector& InPos)
{
	ArcEndPoint = InPos;
	UpdateVertices();
}

void FArmyCircle::SetArcMiddlePos(const FVector& InPos)
{
	ArcMiddlePoint = InPos;
	UpdateVertices();
}

void FArmyCircle::SetPosition(const FVector& InPos)
{
	Position = InPos;
	UpdateVertices();
}

void FArmyCircle::SetTreePointPos(const FVector& InStart, const FVector& InEnd, const FVector& InMiddle)
{
	ArcStartPoint = InStart;
	ArcMiddlePoint = InMiddle;
	ArcEndPoint = InEnd;
	UpdateVertices();
}

TArray<FVector> FArmyCircle::GetDrawingVertexes()
{
    DrawingVertexes.Empty();

    DrawingVertexes.Add(Position);

    float AngleStep = (ArcAngle) / ((float)(NumSides * 2));
    float CurrentAngle = 0;

    for (uint32 i = 0; i <= NumSides * 2; ++i)
    {
        FVector TheVertex = Position + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * XDirection + FMath::Sin(CurrentAngle * (PI / 180.0f)) * YDirection);
        DrawingVertexes.Add(TheVertex);
        CurrentAngle += AngleStep;
    }

    return DrawingVertexes;
}

float FArmyCircle::GetCircleLength()
{
	float arcLength = (ArcAngle / 360.0f)*(2 * PI * Radius);
	return arcLength;
}

void FArmyCircle::UpdateVertices()
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

