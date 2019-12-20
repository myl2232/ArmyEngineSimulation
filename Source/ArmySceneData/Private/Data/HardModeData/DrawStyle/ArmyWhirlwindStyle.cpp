#include "Data/HardModeData/DrawStyle/ArmyWhirlwindStyle.h"
#include "ArmyBrickUnit.h"

FArmyWhirlwindStyle::FArmyWhirlwindStyle() :FArmyBaseEditStyle()
{
	EdityType = S_WhirlwindStyle;
}

void FArmyWhirlwindStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	ApplyBrickNum = 0;

	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	FVector startPoint;
	FVector brickWidthDirection;
	FVector brickHeightDirection;
	int XNum = 0;
	int YNum = 0;
	float temArea = 0.f;
	ClipperBricks.Empty();
	UpdateBrickNumAndDirectionStartPosInfo(OutAreaVertices, startPoint, brickWidthDirection, brickHeightDirection, XNum, YNum);
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
			tempBrick->RotateAroundCenterPos(RotationAngle);
			FVector	brickPosition = startPoint + brickWidthDirection*tempMainTexWidth / 2 + brickHeightDirection*tempMainTexHeight / 2 + i* (tempMainTexWidth + InternalDist)* brickWidthDirection + j* (tempMainTexHeight + InternalDist)* brickHeightDirection;
			if (i % 2 == 0 && j % 2 == 0)
			{
				tempBrick->SetRotateTextureAngle(0);
			}
			else if (i % 2 != 0 && j % 2 == 0)
			{
				tempBrick->SetRotateTextureAngle(90);
			}
			else if (i % 2 == 0 && j % 2 != 0)
			{
				tempBrick->SetRotateTextureAngle(270);
			}
			else if (i % 2 != 0 && j % 2 != 0)
			{
				tempBrick->SetRotateTextureAngle(180);
			}
			tempBrick->SetPoisition(brickPosition);
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



