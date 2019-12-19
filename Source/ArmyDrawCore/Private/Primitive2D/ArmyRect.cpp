#include "Primitive2D/ArmyRect.h"
#include "DynamicMeshBuilder.h"
#include "SceneManagement.h"
#include "ArmyEditorViewportClient.h"

FArmyRect::FArmyRect(FArmyRect* Copy)
{
	Pos = Copy->Pos;
	Height = Copy->Height;
	Width = Copy->Width;
	bIsFilled = Copy->bIsFilled;
	FillColor = Copy->FillColor;
	SetBaseColor(Copy->GetBaseColor());
	MaterialRenderProxy = Copy->MaterialRenderProxy;
	XDirection = Copy->XDirection;
	YDirection = Copy->YDirection;
	DrawBoundray = Copy->DrawBoundray;
	LineThickness = Copy->LineThickness;
}

FArmyRect::FArmyRect()
{
	Pos = FVector::ZeroVector;
	Width = 1;
	Height = 1;
	bIsFilled = false;
	FillColor = FLinearColor(1, 1, 1, 1);
	XDirection = FVector(1, 0, 0);
	YDirection = FVector(0, 1, 0);
	DrawBoundray = true;
}

FArmyRect::~FArmyRect()
{
}

FArmyRect::FArmyRect(FVector InPosition, float InWidth, float InHeight)
{
	Pos = InPosition;
	Width = InWidth;
	Height = InHeight;
	bIsFilled = false;
	FillColor = FLinearColor(1, 1, 1, 1);
}

bool FArmyRect::IsSelected(const FVector& Point, UArmyEditorViewportClient* InViewportClient)
{
	FVector2D V1 = FVector2D(Pos);
	FVector2D V2 = FVector2D(Pos + XDirection * Width + YDirection * Height);

	float ydis1 = FMath::PointDistToLine(Point,XDirection,Pos);
	float ydis2 = FMath::PointDistToLine(Point, XDirection, Pos + YDirection.GetSafeNormal() * Height);

	float xdis1 = FMath::PointDistToLine(Point, YDirection, Pos);
	float xdis2 = FMath::PointDistToLine(Point, YDirection, Pos + XDirection.GetSafeNormal() * Width);

	return (ydis1 <= Height && ydis2 <= Height && xdis1 <= Width && xdis2 <= Width);
}

bool FArmyRect::IsSelected(const FBox2D& Box)
{
	FBox2D BoundingBox;
	BoundingBox.Min = FVector2D(Pos.X, Pos.Y);
	BoundingBox.Max = FVector2D(Pos.X + Width, Pos.Y + Height);

	return Box.IsInside(BoundingBox);
}

bool FArmyRect::IsPointInRect(const FVector & Point)
{
	FVector2D V1 = FVector2D(Pos);
	FVector2D V2 = FVector2D(Pos + XDirection * Width + YDirection * Height);

	float ydis1 = FMath::PointDistToLine(Point, XDirection, Pos);
	float ydis2 = FMath::PointDistToLine(Point, XDirection, Pos + YDirection.GetSafeNormal() * Height);

	float xdis1 = FMath::PointDistToLine(Point, YDirection, Pos);
	float xdis2 = FMath::PointDistToLine(Point, YDirection, Pos + XDirection.GetSafeNormal() * Width);

	return (ydis1 <= Height && ydis2 <= Height && xdis1 <= Width && xdis2 <= Width);
}

void FArmyRect::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	FVector centerPos;
	if (InCenterPos != FVector::ZeroVector)
	{
		centerPos = InCenterPos;
	}
	else
	{
		centerPos = Pos + XDirection*Width / 2 + YDirection*Height / 2;
	}	
	FVector LeftTop = centerPos - XDirection * Width / 2 - YDirection * Height / 2;
	FVector LeftBottom = centerPos - XDirection * Width / 2 + YDirection * Height / 2;
	FVector RightTop = centerPos + XDirection * Width / 2 - YDirection * Height / 2;
	FVector RightBottom = centerPos + XDirection * Width / 2 + YDirection * Height / 2;		
    if(DrawBoundray)
	{
		PDI->DrawLine(LeftTop, LeftBottom, GetBaseColor(), DepthPriority, LineThickness, 0.0f, true);
		PDI->DrawLine(LeftBottom, RightBottom, GetBaseColor(), DepthPriority, LineThickness, 0.0f, true);
		PDI->DrawLine(RightBottom, RightTop, GetBaseColor(), DepthPriority, LineThickness, 0.0f, true);
		PDI->DrawLine(RightTop, LeftTop, GetBaseColor(), DepthPriority, LineThickness, 0.0f, true);
	}
	

	const float SizeX = View->ViewRect.Width();
	const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0]) / SizeX;

	if (bIsFilled && MaterialRenderProxy != NULL)
	{
		float ULength = (LeftTop - RightTop).Size();
		float VLength = (LeftTop - LeftBottom).Size();
		FVector2D UVs[4];
		if(bCalcUVs)
		{
			UVs[0] = FVector2D(0, 0);
			UVs[1] = FVector2D(0, VLength / (YUVRepeatDist*Zoom));
			UVs[2] = FVector2D(ULength / (XUVRepeatDist*Zoom), VLength / (YUVRepeatDist*Zoom));
			UVs[3] = FVector2D(ULength / (XUVRepeatDist*Zoom), 0);
		}
		else
		{
			UVs[0] = FVector2D(0, 0);
			UVs[1] = FVector2D(0, 1);
			UVs[2] = FVector2D(1, 1);
			UVs[3] = FVector2D(1, 0);
		}
		FDynamicMeshBuilder MeshBuilder;
		FVector Normal = FVector(0, 0, 1);
		FVector Tangent = FVector(1, 0, 0);

		TArray<FVector> VertexArr;
		VertexArr.Add(LeftTop);
		VertexArr.Add(LeftBottom);
		VertexArr.Add(RightBottom);
		VertexArr.Add(RightTop);
		
		MeshBuilder.AddVertex(FDynamicMeshVertex(LeftTop, Tangent, Normal, UVs[0], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(LeftBottom, Tangent, Normal, UVs[1], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(RightBottom, Tangent, Normal, UVs[2], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(RightTop, Tangent, Normal, UVs[3], FColor::White));

		FVector Normal1 = FVector::CrossProduct(VertexArr[1] - VertexArr[0], VertexArr[2] - VertexArr[1]).GetSafeNormal();
		FVector Normal2 = FVector::CrossProduct(VertexArr[2] - VertexArr[0], VertexArr[3] - VertexArr[2]).GetSafeNormal();
		if (FVector::DotProduct(Normal1,Normal) < 0)
		{
			MeshBuilder.AddTriangle(0, 1, 2);
			MeshBuilder.AddTriangle(0, 2, 3);
		}
		else
		{
			MeshBuilder.AddTriangle(0, 2, 1);
			MeshBuilder.AddTriangle(0, 3, 2);
		}
		MeshBuilder.Draw(PDI, FMatrix::Identity, MaterialRenderProxy, DepthPriority, false, false);
	}
	
	
}

TArray<FVector> FArmyRect::GetVertices()const 
{
	TArray<FVector> result = { Pos,
							   Pos + YDirection * Height,
							   Pos + XDirection * Width + YDirection * Height,
							   Pos + XDirection * Width
	};

	return result;
}
