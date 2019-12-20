#include "Data/HardModeData/DrawStyle/ArmyHerringBoneStyle.h"
#include "ArmyBrickUnit.h"

FArmyHerringBoneStyle::FArmyHerringBoneStyle() :FArmyBaseEditStyle()
{
	EdityType = S_HerringBoneStyle;
}

void FArmyHerringBoneStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	ApplyBrickNum = 0;

	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	FVector startPoint, brickWidthDirection, brickHeightDirection;
	int XNum, YNum;
	float temArea = 0.f;
	ClipperBricks.Empty();
	UpdateBrickNumAndDirectionStartPosInfo(OutAreaVertices, startPoint, brickWidthDirection, brickHeightDirection, XNum, YNum);
	FBox ClipperAreaBox(OutAreaVertices);
	FVector2D minPoint = FVector2D(ClipperAreaBox.Min);
	FVector2D maxPoint = FVector2D(ClipperAreaBox.Max);
	FVector2D delta2D = (maxPoint - minPoint);
	XNum = FMath::CeilToInt(delta2D.X / (tempMainTexWidth * FMath::Sin(PI / 4))) + 20;
	YNum = FMath::CeilToInt(delta2D.Y / (FMath::Sin(PI / 4) * tempMainTexHeight)) + 20;
	FVector dir0 = FVector(1, -1, 0).GetSafeNormal();;
	FVector dir1 = FVector(1, 1, 0).GetSafeNormal();
	dir0 = dir0.RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	dir1 = dir1.RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	startPoint = startPoint - dir1 * 100.0f;
	FVector rightStartPoint = startPoint + dir0 *(1 / 2.0 * tempMainTexWidth + InternalDist + 1 / 2.0 * tempMainTexHeight) + (1 / 2.0 * tempMainTexWidth - 1 / 2.0 * tempMainTexHeight)*dir1;
	FVector leftOffset = (tempMainTexWidth + InternalDist * 2 + tempMainTexHeight)* dir0 + (tempMainTexWidth - tempMainTexHeight) * dir1;
	FVector rightOffset = (tempMainTexWidth + InternalDist)* dir1 + (tempMainTexWidth + InternalDist) * dir0;
	for (int i = 0; i < XNum; i++)
	{
		for (int j = 0; j < YNum; j++)
		{
			TSharedPtr<FArmyBrickUnit> tempBrick = MakeShareable(new FArmyBrickUnit());
			tempBrick->polyOffset = HeightToFloor;
			tempBrick->PlaneXDir = PlaneXDir;
			tempBrick->PlaneYDir = PlaneYDir;
			tempBrick->PlaneCenterPos = PlaneCenterPos;
			tempBrick->SetBrickWidth(tempMainTexWidth);
			tempBrick->SetBrickHeight(tempMainTexHeight);
			if (i % 2 == 0)
			{
				tempBrick->RotateAroundCenterPos(45.0f + RotationAngle);
				FVector position = startPoint + (i / 2)* leftOffset + FVector(0, 1, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1)) * 1 / FMath::Sin(PI / 4)*(tempMainTexHeight + InternalDist)*j;
				tempBrick->SetPoisition(position);

			}
			else if (i % 2 == 1)
			{
				tempBrick->RotateAroundCenterPos(135.0f + RotationAngle);
				FVector position = rightStartPoint + ((i - 1) / 2)* rightOffset + FVector(0, 1, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1)) * 1 / FMath::Sin(PI / 4)*(tempMainTexHeight + InternalDist)*j;
				tempBrick->SetPoisition(position);
			}
			int index = FMath::Rand() % CurrentTextureNum + 1;
			tempBrick->ApplyClipper(OutAreaVertices, InneraHoles, index, CurrentTextureNum, GetInternalDist());

			TArray<TArray<FVector>> CutResults = tempBrick->GetCutResults();
			int number = CutResults.Num();
			for (int i = 0; i < number; ++i)
			{
				temArea += FArmyMath::CalcPolyArea3D(CutResults[i]);
			}
			if (tempBrick->IsClipper())
			{
				uint32 id = tempBrick->GetTextureID();
				TArray<TSharedPtr<FArmyBrickUnit>>* brickarry = ClipperBricks.Find(id);
				if (brickarry)
				{
					brickarry->Push(tempBrick);
				}
				else
				{
					TArray < TSharedPtr<FArmyBrickUnit>> tempBrickArray;
					tempBrickArray.Push(tempBrick);
					ClipperBricks.Add(id, tempBrickArray);
				}
			}
		}
	}

	ApplyBrickNum += FMath::CeilToInt(temArea / (tempMainTexHeight * tempMainTexWidth));

	CalculateGapVerts(OutAreaVertices, InneraHoles);
}


