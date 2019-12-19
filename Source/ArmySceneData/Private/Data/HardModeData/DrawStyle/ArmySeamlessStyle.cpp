#include "ArmySeamlessStyle.h"

#include "../Mathematics/EarcutTesselator.h"

#include "ArmyTools/Public/XRClipper.h"
#include<vector.h>

FArmySeamlessStyle::FArmySeamlessStyle() :FArmyBaseEditStyle()
{
	EdityType = S_SeamlessStyle;
}

void FArmySeamlessStyle::CopyFrom(TSharedPtr<FArmyBaseEditStyle> OldStyle)
{
	FArmyBaseEditStyle::CopyFrom(OldStyle);
	TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(OldStyle);
	if (tempSeamless.IsValid())
	{
		ColorPaintValueKey = tempSeamless->ColorPaintValueKey;
	}
}

void FArmySeamlessStyle::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{

	int number = TotalVertexs.Num();
	FDynamicMeshBuilder MeshBuilder;
	for (int i = 0; i < number; i++)
	{
		MeshBuilder.AddVertex(TotalVertexs[i]);
		if (i % 3 == 0)
		{
			int index = i / 3;
			MeshBuilder.AddTriangle(index * 3, index * 3 + 1, index * 3 + 2);
		}

	}
	if (MI_FloorTextureMat)
		MeshBuilder.Draw(PDI, FMatrix::Identity, MI_FloorTextureMat->GetRenderProxy(false), 0, true);

}
void FArmySeamlessStyle::CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	TotalVertexs.Empty();
	TriangleArea(SpaceOutAreas, SpaceHoles);
}
void FArmySeamlessStyle::SetDrawArea(const TArray<FVector>& outAreas, const TArray<TArray<FVector>>& Holes)
{
	SpaceOutAreas = outAreas;
	SpaceHoles = Holes;

	ClipperAreas.Empty();
	InnerHoles.Empty();
	int number0 = outAreas.Num();
	for (int i = 0; i < number0; i++)
	{
		FVector convert = outAreas[i].X * PlaneXDir + outAreas[i].Y * PlaneYDir + PlaneCenterPos;
		ClipperAreas.Push(convert);

	}
	int numberHoles = Holes.Num();
	for (int i = 0; i < numberHoles; ++i)
	{
		TArray<FVector> hole;
		int HoleVetexNumm = Holes[i].Num();
		for (int j = 0; j < HoleVetexNumm; ++j)
		{
			FVector convert = Holes[i][j].X *PlaneXDir + Holes[i][j].Y * PlaneYDir + PlaneCenterPos;
			hole.Push(convert);
		}
		InnerHoles.Push(hole);
	}
	points.Empty();
	TotalVertexs.Empty();

	TriangleArea(outAreas, Holes);

	if (HeightToFloor != 0.0f)
	{
		SetStylePlaneOffset(HeightToFloor);
	}

}

void FArmySeamlessStyle::DrawWireFrame(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	int number0 = ClipperAreas.Num();
	for (int i = 0; i < number0; i++)
	{
		PDI->DrawLine(ClipperAreas[i%number0], ClipperAreas[(i + 1) % number0], FColor::White, 1);
	}
	int number1 = InnerHoles.Num();
	for (int i = 0; i < number1; ++i)
	{
		int number2 = InnerHoles[i].Num();
		for (int j = 0; j < number2; ++j)
		{
			PDI->DrawLine(InnerHoles[i][j%number2], InnerHoles[i][(j + 1) % number2], FColor::White, 1);
		}
	}
}

void FArmySeamlessStyle::DrawHoles(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	int number0 = ClipperAreas.Num();
	for (int i = 0; i < number0; i++)
	{
		PDI->DrawLine(ClipperAreas[i%number0], ClipperAreas[(i + 1) % number0], FLinearColor(FColor(0XFF00FFFF)), 1);
	}
	int number1 = InnerHoles.Num();
	for (int i = 0; i < number1; ++i)
	{
		int number2 = InnerHoles[i].Num();
		for (int j = 0; j < number2; ++j)
		{
			PDI->DrawLine(InnerHoles[i][j%number2], InnerHoles[i][(j + 1) % number2], FLinearColor(FColor(0XFF00FFFF)), 1);
		}
	}
}

void FArmySeamlessStyle::SetStylePlaneOffset(float InOffset)
{
	HeightToFloor = InOffset /*== 0.0f ? 0.2f : InOffset*/;
	int number = TotalVertexs.Num();
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);
	if (number != points.Num())
		return;
	for (int i = 0; i < number; ++i)
	{
		FDynamicMeshVertex& vertex = TotalVertexs[i];
		vertex.Position = PlaneXDir * points[i].X + PlaneYDir * points[i].Y + PlaneCenterPos + Normal * HeightToFloor;
	}
}

void FArmySeamlessStyle::GetVertexInfo(TArray<FDynamicMeshVertex>& AllVertexs)
{
	AllVertexs = TotalVertexs;
}

void FArmySeamlessStyle::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyBaseEditStyle::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("colorPaintValue", ColorPaintValueKey);
	JsonWriter->WriteValue("SeamlesStyleType", SeamlesStyleType);
}

void FArmySeamlessStyle::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyBaseEditStyle::Deserialization(InJsonData);
	ColorPaintValueKey = InJsonData->GetStringField("colorPaintValue");
	SeamlesStyleType = InJsonData->GetIntegerField("SeamlesStyleType");
}

void FArmySeamlessStyle::TriangleArea(const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& Holes)
{
	TArray<FVector2D> tempPoints;
	Translate::ClipperGapVertsAndTriangle(OutAreas, Holes, tempPoints);

	FBox box(OutAreas);
	int TrianglePointsNum = tempPoints.Num();
	FArmyMath::ReverPointList(tempPoints);
	points.Append(tempPoints);
	FVector Tangent = PlaneXDir;
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);

	for (int index = 0; index < TrianglePointsNum; ++index)
	{
		float VCoord, UCoord;
		UCoord = (tempPoints[index].X - box.Min.X) * 10.0f / MainTexWidth + XDirOffset * 10 / MainTexWidth;
		VCoord = (tempPoints[index].Y - box.Min.Y) *10.0f / MainTexHeight + YDirOffset * 10 / MainTexHeight;
		TotalVertexs.Push(FDynamicMeshVertex(
			PlaneXDir * tempPoints[index].X + PlaneYDir * tempPoints[index].Y + PlaneCenterPos + Normal * HeightToFloor,
			Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));

	}
}


