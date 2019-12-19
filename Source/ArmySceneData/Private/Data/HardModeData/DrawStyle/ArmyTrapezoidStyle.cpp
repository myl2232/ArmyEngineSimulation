#include "Data/HardModeData/DrawStyle/XRTrapezoidStyle.h"
FArmyTrapezoidStyle::FArmyTrapezoidStyle() :FArmyBaseEditStyle()
{
	EdityType = S_TrapeZoidStyle;
}

void FArmyTrapezoidStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	ApplyBrickNum = 0;

	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	FVector startPoint, brickWidthDirection, brickHeightDirection;
	int XNum, YNum;
	float temArea = 0.f;
	ClipperBricks.Empty();
	UpdateBrickNumAndDirectionStartPosInfo(OutAreaVertices, startPoint, brickWidthDirection, brickHeightDirection, XNum, YNum);
	for (int i = 0; i < XNum + 1; i++)
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
			FVector	brickPosition;
			if (j % 3 == 0)
			{
				brickPosition = startPoint + brickWidthDirection*tempMainTexWidth / 2 + brickHeightDirection*tempMainTexHeight / 2 + i* (tempMainTexWidth + InternalDist)*brickWidthDirection + j* (tempMainTexHeight + InternalDist)* brickHeightDirection;
			}
			else if (j % 3 == 1)
			{
				brickPosition = startPoint + brickWidthDirection*tempMainTexWidth / 2 + brickHeightDirection*tempMainTexHeight / 2 + i* (tempMainTexWidth + InternalDist)* brickWidthDirection + j* (tempMainTexHeight + InternalDist)* brickHeightDirection - brickWidthDirection*tempMainTexWidth * 2 / 3.0;
			}
			else if (j % 3 == 2)
			{
				brickPosition = startPoint + brickWidthDirection*tempMainTexWidth / 2 + brickHeightDirection*tempMainTexHeight / 2 + i* (tempMainTexWidth + InternalDist)* brickWidthDirection + j* (tempMainTexHeight + InternalDist)* brickHeightDirection - brickWidthDirection*tempMainTexWidth * 1 / 3.0;
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



