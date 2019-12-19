#include "Data/HardModeData/DrawStyle/XRIntervalStyle.h"

FArmyIntervalStyle::FArmyIntervalStyle()
{
	EdityType = S_IntervalStyle;


}


FArmyIntervalStyle::~FArmyIntervalStyle()
{
	
}

void FArmyIntervalStyle::SetFirstAndSecondImage(UTexture2D* InTexture0, UTexture2D* InTexture1)
{
	MainTexture = InTexture0;
	SecondTexture = InTexture1;
	MainTexWidth = InTexture0->GetSizeX();//此铺法宽和高应该相等
	MainTexHeight = InTexture0->GetSizeY();
	//M_TextureAtlas->Reset();
	//M_TextureAtlas->AddTexture(1, InTexture0);
	//M_TextureAtlas->AddTexture(2, InTexture1);
	//MainTexture = M_TextureAtlas->Create(4096);
	MI_FloorTextureMat->SetTextureParameterValue("MainTex", MainTexture);
	SecondTextureMat->SetTextureParameterValue("MainTex", SecondTexture);

}

UTexture2D * FArmyIntervalStyle::GetSecondTexture2D() const
{
	return SecondTexture;
}

void FArmyIntervalStyle::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("secondTextureUrl", SecondTextureUrl);//TODO
	FArmyBaseEditStyle::SerializeToJson(JsonWriter);
}

void FArmyIntervalStyle::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	InJsonData->TryGetStringField("secondTextureUrl", SecondTextureUrl);
	SecondTexture = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(SecondTextureUrl);
	FArmyBaseEditStyle::Deserialization(InJsonData);

	SecondTextureMat->SetTextureParameterValue("MainTex", SecondTexture);

}

void FArmyIntervalStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
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
			if (i % 2 == 0)
			{
				tempBrick->SetTextureID(1);
				if (j % 2 == 0)
				{
					tempBrick->SetTextureID(2);
				}
			}
			else if (i % 2 != 0 && j % 2 == 0)
			{
				tempBrick->SetTextureID(1);
			}

			tempBrick->SetPoisition(brickPosition);
			tempBrick->ApplyClipper(OutAreaVertices, InneraHoles,1,1, GetInternalDist());

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



