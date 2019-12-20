
#include "Data/HardModeData/DrawStyle/ArmyContinueStyle.h"
#include "ArmyBrickUnit.h"
FArmyContinueStyle::FArmyContinueStyle() :FArmyBaseEditStyle()
{
	EdityType = S_ContinueStyle;
}

void FArmyContinueStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	ApplyBrickNum = 0;

	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	FVector startPoint, brickWidthDirection, brickHeightDirection;
	int XNum, YNum;
	float temArea = 0.f;
	ClipperBricks.Empty();
	UpdateBrickNumAndDirectionStartPosInfo(OutAreaVertices, startPoint, brickWidthDirection, brickHeightDirection, XNum, YNum);
	for (int i = 0; i < XNum; i++)
	{
		for (int j = 0; j < YNum; j++)
		{
			TSharedPtr<FArmyBrickUnit> tempBrick = MakeShareable(new FArmyBrickUnit());
			tempBrick->PlaneXDir = PlaneXDir;
			tempBrick->PlaneYDir = PlaneYDir;
			tempBrick->polyOffset = HeightToFloor;
			tempBrick->PlaneCenterPos = PlaneCenterPos;
			tempBrick->SetBrickWidth(tempMainTexWidth);
			tempBrick->SetBrickHeight(tempMainTexHeight);
			tempBrick->RotateAroundCenterPos(RotationAngle);
			FVector brickPosition = startPoint + brickWidthDirection*tempMainTexWidth / 2 + brickHeightDirection*tempMainTexHeight / 2 + i* (tempMainTexWidth + InternalDist)* brickWidthDirection + j* (tempMainTexHeight + InternalDist)* brickHeightDirection;
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

TSharedPtr<FArmyBaseEditStyle> FArmyContinueStyle::CopyTo()
{
	TSharedPtr<FArmyContinueStyle> ContinueStyle = MakeShareable(new FArmyContinueStyle());
	
	return ContinueStyle;
}


FArmySlopeContinueStyle::FArmySlopeContinueStyle() :FArmyContinueStyle()
{
	EdityType = S_SlopeContinueStyle;
	RotationAngle = 45.0f;
}

TSharedPtr<FArmyBaseEditStyle> FArmySlopeContinueStyle::CopyTo()
{
	TSharedPtr<FArmySlopeContinueStyle> SlopeContinueStyle = MakeShareable(new FArmySlopeContinueStyle());
	
	return SlopeContinueStyle;
}