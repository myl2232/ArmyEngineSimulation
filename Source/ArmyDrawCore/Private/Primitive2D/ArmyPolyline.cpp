#include "ArmyPolyline.h"

#include "DynamicMeshBuilder.h"
#include "SceneManagement.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEngineModule.h"
#include "ArmyMath.h"

FArmyPolyline::FArmyPolyline() :
	mLineType(ELineType::Lines)
	, mPivotPoint(FVector::ZeroVector)
	, mLocalTransform(FTransform::Identity)
	, LineWidth(1.0f)

{
	TriangleMaterial = FArmyEngineModule::Get().GetEngineResource()->GetLineTestMaterial();
	mDynamicMaterial = UMaterialInstanceDynamic::Create(TriangleMaterial, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
}

FArmyPolyline::FArmyPolyline(FArmyPolyline* Copy)
{
	for (auto& Vertex : Copy->LineVertices)
	{
        LineVertices.Add(Vertex);
	}
	LineWidth = Copy->LineWidth;
    mPivotPoint = Copy->mPivotPoint;
    mLineType = Copy->mLineType;
    mLocalTransform = Copy->mLocalTransform;
	SetBaseColor(Copy->GetBaseColor());
}

FArmyPolyline::~FArmyPolyline()
{
}

void FArmyPolyline::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (LineVertices.Num() >= 2)
	{
#ifdef test
		DrawWithTriangle(PDI, View);
#else
		if (mLineType == ELineType::LineStrip)
		{
			for (int i = 1; i < LineVertices.Num(); i++)
			{
				PDI->DrawLine(mLocalTransform.TransformPosition(LineVertices[i - 1] - mPivotPoint), mLocalTransform.TransformPosition(LineVertices[i] - mPivotPoint), GetBaseColor(), 1, LineWidth, 0.0f, true);
			}
		}
		else if (mLineType == ELineType::Lines)
		{
			for (int i = 1; i < LineVertices.Num(); i += 2)
			{
				PDI->DrawLine(mLocalTransform.TransformPosition(LineVertices[i - 1] - mPivotPoint), mLocalTransform.TransformPosition(LineVertices[i] - mPivotPoint), GetBaseColor(), 1, LineWidth, 0.0f, true);
			}
		}
		else if (mLineType == ELineType::Line_Loop)
		{
			int32 Size = LineVertices.Num();
			for (int i = 0; i< Size; i++)
			{
				int IndexNext = (i == Size-1) ? (i+1)% Size : i + 1;
				PDI->DrawLine(mLocalTransform.TransformPosition(LineVertices[i]-mPivotPoint),mLocalTransform.TransformPosition(LineVertices[IndexNext]-mPivotPoint),GetBaseColor(),1, LineWidth, 0.0f, true);
			}
		}
#endif
	}


}

bool FArmyPolyline::IsSelected(const FBox2D& box)
{
	return true;
}

bool FArmyPolyline::IsSelected(const FVector& Point, UArmyEditorViewportClient* InViewportClient)
{
    return FArmyMath::IsPointInOrOnPolygon2D(Point, LineVertices);
}

void FArmyPolyline::SetColor(const FLinearColor& PolyLineColor)
{
#ifdef test
	mDynamicMaterial->SetVectorParameterValue(TEXT("MainColor"), PolyLineColor);
#else
	SetBaseColor(PolyLineColor);
#endif
}

void FArmyPolyline::DrawWithTriangle(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	const float SizeX = View->ViewRect.Width();
	const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0])*2.0f / SizeX;
	FDynamicMeshBuilder MeshBuilder;
	FVector Normal = FVector(0, 0, 1);
	FVector Tangent = FVector(1, 0, 0);
	if (mLineType == Lines)
	{
		for (int i = 1; i < LineVertices.Num(); i += 2)
		{
			FVector tempStart = mLocalTransform.TransformPosition(LineVertices[i - 1] - mPivotPoint);
			FVector tempEnd = mLocalTransform.TransformPosition(LineVertices[i] - mPivotPoint);
			FVector diretion = (tempEnd - tempStart).GetSafeNormal();
			FVector vertical = diretion.RotateAngleAxis(90, FVector(0, 0, 1));
			FVector pos0 = tempStart + vertical* LineWidth * Zoom;
			FVector pos1 = tempStart - vertical * LineWidth *Zoom;
			FVector pos2 = tempEnd - vertical* LineWidth* Zoom;
			FVector pos3 = tempEnd + vertical * LineWidth * Zoom;
			FVector2D Uvs[4] =
			{
				FVector2D(0,0),
				FVector2D(0,1),
				FVector2D(1,1),
				FVector2D(1,0)
			};

			int index0 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos0, Tangent, Normal, Uvs[0], FColor::White));
			int index1 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos1, Tangent, Normal, Uvs[1], FColor::White));
			int index2 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos2, Tangent, Normal, Uvs[2], FColor::White));
			int index3 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos3, Tangent, Normal, Uvs[3], FColor::White));
			MeshBuilder.AddTriangle(index0, index1, index2);
			MeshBuilder.AddTriangle(index0, index2, index3);
		}
		MeshBuilder.Draw(PDI, FMatrix::Identity, mDynamicMaterial->GetRenderProxy(false), 1, true, false);
	}
	else
	{
		for (int i = 1; i < LineVertices.Num(); i++)
		{
			FVector tempStart = mLocalTransform.TransformPosition(LineVertices[i - 1] - mPivotPoint);
			FVector tempEnd = mLocalTransform.TransformPosition(LineVertices[i] - mPivotPoint);
			FVector diretion = (tempEnd - tempStart).GetSafeNormal();
			FVector vertical = diretion.RotateAngleAxis(90, FVector(0, 0, 1));
			FVector pos0 = tempStart + vertical* LineWidth * Zoom;
			FVector pos1 = tempStart - vertical * LineWidth *Zoom;
			FVector pos2 = tempEnd - vertical* LineWidth* Zoom;
			FVector pos3 = tempEnd + vertical * LineWidth * Zoom;
			FVector2D Uvs[4] =
			{
				FVector2D(0,0),
				FVector2D(0,1),
				FVector2D(1,1),
				FVector2D(1,0)
			};

			int index0 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos0, Tangent, Normal, Uvs[0], FColor::White));
			int index1 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos1, Tangent, Normal, Uvs[1], FColor::White));
			int index2 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos2, Tangent, Normal, Uvs[2], FColor::White));
			int index3 = MeshBuilder.AddVertex(FDynamicMeshVertex(pos3, Tangent, Normal, Uvs[3], FColor::White));
			MeshBuilder.AddTriangle(index0, index1, index2);
			MeshBuilder.AddTriangle(index0, index2, index3);
		}
		MeshBuilder.Draw(PDI, FMatrix::Identity, mDynamicMaterial->GetRenderProxy(false), 1, true, false);

	}
}
