#include "HardModeData/GridSystemCore/ArmyBrickUnit.h"
#include "ArmyTools/Public/ArmyClipper.h"

#include "../Mathematics/EarcutTesselator.h"
#include "ArmyEditorViewportClient.h"

#include<vector.h>

FArmyBrickUnit::FArmyBrickUnit()
{
	BrickWidthDirection = FVector(1, 0, 0);
	BrickHeightDirection = FVector(0, -1, 0);
	M_Rectf = MinMaxRect<float>(0, 0, 1, 1);
}

FArmyBrickUnit::FArmyBrickUnit(FArmyBrickUnit* Copy)
{
	MainTextureId = Copy->MainTextureId;
	MainTextureUrl = Copy->MainTextureUrl;
	BrickWidthDirection = Copy->BrickWidthDirection;
	BrickHeightDirection = Copy->BrickHeightDirection;

	BrickWidth = Copy->BrickWidth;
	BrickHeight = Copy->BrickHeight;

}

FArmyBrickUnit::FArmyBrickUnit(FVector InPos, float  InWidth, float InHeight)
{
	BrickWidthDirection = FVector(1, 0, 0);
	BrickHeightDirection = FVector(0, -1, 0);
	M_Rectf = MinMaxRect<float>(0, 0, 1, 1);
	BrickWidth = InWidth;
	BrickHeight = InHeight;
	BrickCenterPosition = InPos;
}

void FArmyBrickUnit::SetPoisition(FVector centerPosition)
{
	BrickCenterPosition = centerPosition;
}

void FArmyBrickUnit::SetBrickWidth(float InWidth)
{
	BrickWidth = InWidth;
}

void FArmyBrickUnit::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{

	int number = CutResults.Num();

	for (int i = 0; i < number; ++i)
	{
		int number0 = CutResults[i].Num();
		for (int j = 0; j < number0; ++j)
		{
			PDI->DrawLine(CutResults[i][j%number0], CutResults[i][(j + 1) % number0], FLinearColor(FColor(0XFF666666)), 1);
		}
	}
}

void FArmyBrickUnit::SetBrickHeight(float InHeight)
{
	BrickHeight = InHeight;
}

void FArmyBrickUnit::ApplyTransform(FTransform trans)
{
	for (auto& iter : ClipperResults)
	{
		iter.Position = trans.TransformPosition(iter.Position);
	}
	for (auto& iter0 : CutResults)
	{
		for (auto& itr : iter0)
		{
			itr = trans.TransformPosition(itr);
		}
	}
}

void FArmyBrickUnit::RotateAroundCenterPos(float angle)
{
	BrickWidthDirection = FVector(1, 0, 0).RotateAngleAxis(angle, FVector(0, 0, 1));
	BrickHeightDirection = FVector(0, -1, 0).RotateAngleAxis(angle, FVector(0, 0, 1));

	RotateAngle = angle;
}

#define SCALE 1000000.0f
void FArmyBrickUnit::ApplyClipper(const TArray<FVector>& InClipperVerices, const TArray<TArray<FVector>>& Holes, const int32 useTextureIndex, const int32 totalNum, const float IntervalDistance)
{
	Clipper c;
	Paths ClipperAreas;
	Paths HolePaths;
	int holeNumbers = Holes.Num();
	ClipperAreas.resize(1);
	for (int i = 0; i < InClipperVerices.Num(); i++)
	{
		cInt x = (cInt)(InClipperVerices[i].X * SCALE);
		cInt y = (cInt)(InClipperVerices[i].Y * SCALE);
		ClipperAreas[0].push_back(IntPoint(x, y));
	}
	for (int i = 0; i < holeNumbers; i++)
	{
		Path holePath;
		for (int j = 0; j < Holes[i].Num(); j++)
		{
			cInt x = (cInt)(Holes[i][j].X * SCALE);
			cInt y = (cInt)(Holes[i][j].Y * SCALE);
			holePath.push_back(IntPoint(x, y));
		}
		HolePaths.push_back(holePath);
	}
	c.AddPaths(ClipperAreas, ptSubject, true);
	TArray<FVector> BrickVertices;
	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	FVector second = BrickCenterPosition - BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector third = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector fourth = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 - BrickHeightDirection * BrickHeight / 2;
	BrickVertices = { first,second,third,fourth };


	Paths sub;
	sub.resize(1);
	for (int j = 0; j < BrickVertices.Num(); j++)
	{
		cInt x = (cInt)(BrickVertices[j].X * SCALE);
		cInt y = (cInt)(BrickVertices[j].Y * SCALE);
		sub[0].push_back(IntPoint(x, y));
	}
	c.AddPaths(sub, ptClip, true);
	Paths ClipperSolution, tempResults;
	c.Execute(ctIntersection, tempResults, pftEvenOdd, pftEvenOdd);

	//@郭子阳
	//忽略比砖缝还窄的砖
	if (tempResults.size() > 0 && tempResults[0].size() == 4)
	{
		if ((((tempResults[0][1] - tempResults[0][0]) / SCALE)).Size() < IntervalDistance
			|| (((tempResults[0][2] - tempResults[0][1]) / SCALE)).Size() < IntervalDistance)
		{
			tempResults[0].clear();
		}
	}
	
	if (HolePaths.size() > 0)
	{
		c.Clear();
		c.AddPaths(tempResults, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		c.Execute(ctDifference, ClipperSolution, pftEvenOdd, pftEvenOdd);
	}
	else
	{
		ClipperSolution = tempResults;
	}
	CutResults.Empty();
	ClipperResults.Reset();
	points.Empty();
	FVector Tangent = PlaneXDir;
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);

	TArray<AreaCompare> tempAreas;
	for (int i = 0; i < ClipperSolution.size(); ++i)
	{
		Path tempPath = ClipperSolution[i];
		TArray<FVector> tempOutVertices;
		TArray<FVector> vertices;
		for (int j = 0; j < tempPath.size(); ++j)
		{

			IntPoint point = tempPath[j];
			FVector x = point.X / SCALE * PlaneXDir;
			FVector y = point.Y / SCALE*PlaneYDir;
			tempOutVertices.Push(FVector(point.X / SCALE, point.Y / SCALE, 0.0f));
			vertices.Push(x + y + PlaneCenterPos);
		}
		tempAreas.Push(AreaCompare(tempOutVertices));
		CutResults.Add(vertices);
	}
	tempAreas.Sort();
	TArray<TSharedPtr<AreaCompare>> OrginalAreas;
	for (int i = 0; i < tempAreas.Num(); ++i)
	{
		OrginalAreas.Push(MakeShareable(new AreaCompare(tempAreas[i].M_Area)));
	}
	TArray<TSharedPtr<AreaCompare>> Results;
	while (OrginalAreas.Num() > 0)
	{

		TSharedPtr<AreaCompare> start = OrginalAreas[0];

		OrginalAreas.Remove(start);
		TArray<TSharedPtr<AreaCompare>> RemoveAreas;
		for (int i = 0; i < OrginalAreas.Num(); ++i)
		{
			if (start->IsContainOther(OrginalAreas[i]))
			{
				RemoveAreas.Push(OrginalAreas[i]);
			}
		}
		for (int i = 0; i < RemoveAreas.Num(); ++i)
		{
			OrginalAreas.Remove(RemoveAreas[i]);
		}
		Results.Push(start);

	}
	for (TSharedPtr<AreaCompare> iter : Results)
	{
		TArray<TArray<FVector>> tempHoles;
		for (int i = 0; i < iter->InnearHoles.Num(); ++i)
		{
			tempHoles.Push(iter->InnearHoles[i].M_Area);
		}
		TriangleBrickWithHole(iter->M_Area, tempHoles, useTextureIndex, totalNum);
	}

}

void FArmyBrickUnit::SetUVRects(const Rectf& InRectf)
{
	M_Rectf = InRectf;
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);
	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	int number = ClipperResults.Num();
	if (number != points.Num())
		return;
	for (int i = 0; i < number; ++i)
	{
		FDynamicMeshVertex& vertex = ClipperResults[i];

		FVector tempPos = FVector(points[i].X, points[i].Y, 0.0) - FVector(first.X, first.Y, 0.0);
		float UProjection = FVector::DotProduct(tempPos, BrickWidthDirection) / BrickWidth;
		float VProjection = FVector::DotProduct(tempPos, BrickHeightDirection) / BrickHeight;

		UProjection = InRectf.x + UProjection* InRectf.width;
		VProjection = InRectf.y + VProjection* InRectf.height;
		FVector2D uv = FVector2D(UProjection, VProjection);
		uv -= FVector2D(0.5f, 0.5f);
		UProjection = uv.X * FMath::Cos(FMath::DegreesToRadians(RoateTextureUV)) - uv.Y * FMath::Sin(FMath::DegreesToRadians(RoateTextureUV));
		VProjection = uv.X * FMath::Sin(FMath::DegreesToRadians(RoateTextureUV)) + uv.Y * FMath::Cos(FMath::DegreesToRadians(RoateTextureUV));
		UProjection = (UProjection + 0.5f);
		VProjection = (VProjection + 0.5f);
		vertex.TextureCoordinate = FVector2D(UProjection, VProjection);
		vertex.Position = PlaneXDir * points[i].X + PlaneYDir * points[i].Y + PlaneCenterPos + Normal * polyOffset;
	}

}

bool FArmyBrickUnit::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	FVector2D PosPix, LeftPix, RightPix, LeftBottomPix, RightBottomPix;
	InViewportClient->WorldToPixel(Pos, PosPix);

	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	FVector second = BrickCenterPosition - BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector third = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector fourth = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 - BrickHeightDirection * BrickHeight / 2;

	InViewportClient->WorldToPixel(first, LeftPix);
	InViewportClient->WorldToPixel(second, RightPix);
	InViewportClient->WorldToPixel(third, LeftBottomPix);
	InViewportClient->WorldToPixel(fourth, RightBottomPix);
	TArray<FVector2D> boudingBox = { LeftPix,RightPix,LeftBottomPix,RightBottomPix };
	FBox2D box(boudingBox);
	return box.IsInside(PosPix);
}

void FArmyBrickUnit::SetPlaneOffset(float Offset)
{
	int number = ClipperResults.Num();
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);
	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	if (number != points.Num())
		return;
	polyOffset = Offset;
	for (int i = 0; i < number; ++i)
	{
		FDynamicMeshVertex& vertex = ClipperResults[i];
		vertex.Position = PlaneXDir * points[i].X + PlaneYDir * points[i].Y + PlaneCenterPos + Normal * Offset;
	}
}

void FArmyBrickUnit::SetPlaneInfo(const FVector& InCenter, const FVector& InXDir, const FVector& InYdir)
{
	PlaneCenterPos = InCenter;
	PlaneXDir = InXDir;
	PlaneYDir = InYdir;
}


TArray<FVector> FArmyBrickUnit::GetBrickVerts()
{
	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	FVector second = BrickCenterPosition - BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector third = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 + BrickHeightDirection * BrickHeight / 2;
	FVector fourth = BrickCenterPosition + BrickWidthDirection * BrickWidth / 2 - BrickHeightDirection * BrickHeight / 2;

	TArray<FVector> result = { first,second,third,fourth };
	return MoveTemp(result);
}

void FArmyBrickUnit::TriangleBrickWithHole(const TArray<FVector>& BrickVertexs, const TArray<TArray<FVector>>& Holes, const int32 CurrentUseIndex, const int32 TotalNum)
{
	TArray<FVector2D> tempPoints;
	Translate::TriangleAreaWithHole(BrickVertexs, Holes, tempPoints);
	int verticeNumber = tempPoints.Num();
	FArmyMath::ReverPointList(tempPoints);
	points.Append(tempPoints);
	FVector first = BrickCenterPosition - BrickWidthDirection*BrickWidth / 2 - BrickHeightDirection* BrickHeight / 2;
	FVector Tangent = PlaneXDir;
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);
	float offset = 0;
	if (TotalNum > 1)
	{
		offset = (CurrentUseIndex - 1) / (float)TotalNum;
	}
	float pivotV = 0.5f;
	float scale = 1;
	if (TotalNum > 1)
	{
		pivotV = (CurrentUseIndex - 0.5) / (float)TotalNum;
	}
	if (RoateTextureUV != 0)
	{
		scale = (float)TotalNum;
	}
	for (int index = 0; index < verticeNumber; index++)
	{
		FVector tempPos = FVector(tempPoints[index], 0.0) - FVector(first.X, first.Y, 0.0);
		float UProjection = FVector::DotProduct(tempPos, BrickWidthDirection) / BrickWidth;
		float VProjection = FVector::DotProduct(tempPos, BrickHeightDirection) / (float)(BrickHeight*TotalNum) + offset;

		FVector2D uv = FVector2D(UProjection, VProjection);
		uv -= FVector2D(0.5f, pivotV);
		UProjection = uv.X * FMath::Cos(FMath::DegreesToRadians(RoateTextureUV)) - uv.Y * FMath::Sin(FMath::DegreesToRadians(RoateTextureUV));
		VProjection = uv.X * FMath::Sin(FMath::DegreesToRadians(RoateTextureUV)) + uv.Y * FMath::Cos(FMath::DegreesToRadians(RoateTextureUV));
		UProjection = (UProjection + 0.5f);
		VProjection = (VProjection / scale + pivotV);
		ClipperResults.Push(FDynamicMeshVertex(PlaneXDir * tempPoints[index].X + PlaneYDir * tempPoints[index].Y + PlaneCenterPos + Normal * polyOffset, Tangent, Normal, FVector2D(UProjection, VProjection), FColor::White));
	}

}

