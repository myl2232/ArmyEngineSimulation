#include "Primitive2D/ArmyLine.h"
#include "Math/ArmyMath.h"
#include "DynamicMeshBuilder.h"
#include "SceneManagement.h"
#include "ArmyEditorViewportClient.h"
//#define TriangleDraw

FArmyLine::FArmyLine()
	:bIsDashLine(false)
	, Thickness(0.f)
	, DashSize(3.f)
	, LineWidth(1.f)
	, bUseTriangle(false)
{
	StartPoint = MakeShareable(new FArmyEditPoint);
	EndPoint = MakeShareable(new FArmyEditPoint);
	LineEditPoint = MakeShareable(new FArmyEditPoint);
#ifdef TriangleDraw
	Material = FArmyEngineModule::Get().GetEngineResource()->GetLineTestMaterial();
	Material->AddToRoot();

	mDynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
#endif // TriangleDraw
}

FArmyLine::FArmyLine(FVector InStart, FVector InEnd)
	: bIsDashLine(false)
	, Thickness(0.f)
	, DashSize(3.f)
	, LineWidth(1.0f)
	, bUseTriangle(false)
{
	StartPoint = MakeShareable(new FArmyEditPoint(InStart));
	EndPoint = MakeShareable(new FArmyEditPoint(InEnd));
	LineEditPoint = MakeShareable(new FArmyEditPoint((InStart + InEnd) / 2.f));
#ifdef TriangleDraw

	Material = FArmyEngineModule::Get().GetEngineResource()->GetLineTestMaterial();
	Material->AddToRoot();

	mDynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
#endif
}

FArmyLine::FArmyLine(FArmyLine* Copy)
{
	PriState = Copy->PriState;
	SetBaseColor(Copy->GetBaseColor());
	DepthPriority = Copy->DepthPriority;
	DepthBias = Copy->DepthBias;
	bIsDashLine = Copy->bIsDashLine;
	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	LineEditPoint = MakeShareable(new FArmyEditPoint(Copy->LineEditPoint.Get()));
	Thickness = Copy->Thickness;
	DashSize = Copy->DashSize;
	LineWidth = Copy->LineWidth;
	bUseTriangle = Copy->bUseTriangle;
#ifdef TriangleDraw
	Material = Copy->Material;
	mDynamicMaterial = UMaterialInstanceDynamic::Create(Copy->Material, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
#endif //  TriangleDraw
}

FArmyLine::FArmyLine(bool bInIsDashLine)
	:bIsDashLine(bInIsDashLine)
	, Thickness(0.f)
	, DashSize(3.f)
	, LineWidth(1.f)
	, bUseTriangle(false)
{
	StartPoint = MakeShareable(new FArmyEditPoint);
	EndPoint = MakeShareable(new FArmyEditPoint);
#ifdef TriangleDraw
	Material = FArmyEngineModule::Get().GetEngineResource()->GetLineTestMaterial();
	Material->AddToRoot();

	mDynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
#endif // TriangleDraw
}

FArmyLine::FArmyLine(const TSharedPtr<FArmyEditPoint> InStart, const TSharedPtr<FArmyEditPoint> InEnd)
	:bIsDashLine(false)
	, Thickness(0.f)
	, DashSize(3.f)
	, LineWidth(1.f)
	, bUseTriangle(false)
{
	StartPoint = InStart;
	EndPoint = InEnd;
	LineEditPoint = MakeShareable(new FArmyEditPoint((StartPoint->GetPos() + EndPoint->GetPos()) / 2.f));
#ifdef TriangleDraw
	Material = FArmyEngineModule::Get().GetEngineResource()->GetLineTestMaterial();
	Material->AddToRoot();

	mDynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL /*GArmyVC->GetWorld()->GetCurrentLevel()*/);
	mDynamicMaterial->AddToRoot();
#endif // TriangleDraw
}

FArmyLine::~FArmyLine()
{
	//StartPoint->MinusReferenceLine(SharedThis(this));
	//EndPoint->MinusReferenceLine(SharedThis(this));
}

void FArmyLine::SetStart(const FVector& InV)
{
	StartPoint->Pos = InV;
	LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
}
void FArmyLine::SetEnd(const FVector& InV)
{
	EndPoint->Pos = InV;
	LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
}
FVector FArmyLine::GetStart() const
{
	return StartPoint->Pos;
}
FVector FArmyLine::GetEnd() const
{
	return EndPoint->Pos;
}

float FArmyLine::Size () const
{
	FVector Start = GetStart();
	FVector End = GetEnd();
	if (Start.ContainsNaN() ||End.ContainsNaN())
	{
		return 0.f;
	}
	FVector Direction = (Start - End);
	return Direction.Size();
}

void FArmyLine::SetStartPointer(const TSharedPtr<FArmyEditPoint> InV)
{
	if (InV.IsValid())
	{
		StartPoint = InV;
		LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
	}
}
void FArmyLine::SetEndPointer(const TSharedPtr<FArmyEditPoint> InV)
{
	if (InV.IsValid())
	{
		EndPoint = InV;
		LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
	}
}
const TSharedPtr<FArmyEditPoint> FArmyLine::GetStartPointer() const
{
	return StartPoint;
}
const TSharedPtr<FArmyEditPoint> FArmyLine::GetEndPointer() const
{
	return EndPoint;
}

bool FArmyLine::IsSelected(const FVector& Point, UArmyEditorViewportClient* InViewportClient)
{
	FVector ClosePoint = FMath::ClosestPointOnLine(StartPoint->Pos, EndPoint->Pos, Point);

	FVector2D PixPos,TempPixPos;
	InViewportClient->WorldToPixel(Point, PixPos);
	InViewportClient->WorldToPixel(ClosePoint, TempPixPos);

	if ((PixPos - TempPixPos).Size() < 10)
	{
		return true;
	}
	else
		return false;

}
TSharedPtr<FArmyEditPoint> FArmyLine::SelectEditPoint(FVector InPos, UArmyEditorViewportClient* InViewportClient)
{
	if (ShowEditPoint)
	{
		if (StartPoint->IsSelected(InPos, InViewportClient))
		{
			return StartPoint;
		}
		else if (EndPoint->IsSelected(InPos, InViewportClient))
		{
			return EndPoint;
		}
		else if (LineEditPoint->IsSelected(InPos, InViewportClient))
		{
			return LineEditPoint;
		}
	}
	return NULL;
}
bool FArmyLine::IsSelected(const FBox2D& Box)
{
	return Box.IsInside(FVector2D(StartPoint->Pos.X, StartPoint->Pos.Y)) && Box.IsInside(FVector2D(EndPoint->Pos.X, EndPoint->Pos.Y));
}
void FArmyLine::UpdateEditPoint()
{
	TSharedPtr<FArmyEditPoint> EditPoint = EditPointMap.FindRef(1);
	if (EditPoint.IsValid())
	{
		EditPoint->Pos = StartPoint->Pos;
	}
	else
	{
		TSharedPtr<FArmyEditPoint>EditPoint = MakeShareable(new FArmyEditPoint);
		EditPoint->Pos = StartPoint->Pos;
		EditPoint->OperationType = ArmyOP_SCALE;
		EditPointMap.Add(1, EditPoint);
	}
	EditPoint = EditPointMap.FindRef(2);
	if (EditPoint.IsValid())
	{
		EditPoint->Pos = EndPoint->Pos;
	}
	else
	{
		TSharedPtr<FArmyEditPoint>EditPoint = MakeShareable(new FArmyEditPoint);
		EditPoint->Pos = EndPoint->Pos;
		EditPoint->OperationType = ArmyOP_SCALE;
		EditPointMap.Add(2, EditPoint);
	}
	EditPoint = EditPointMap.FindRef(3);
	if (EditPoint.IsValid())
	{
		EditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2;
	}
	else
	{
		TSharedPtr<FArmyEditPoint>EditPoint = MakeShareable(new FArmyEditPoint);
		EditPoint->OperationType = ArmyOP_MOVE;
		EditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2;
		EditPointMap.Add(3, EditPoint);
	}
}
void FArmyLine::SetLineColor(FLinearColor InColor)
{
#ifdef TriangleDraw
	mDynamicMaterial->SetVectorParameterValue(TEXT("MainColor"), InColor);
#else
	SetBaseColor(InColor);
#endif
}
FLinearColor FArmyLine::GetLineColor() const
{
	return LineColor;
}

FVector FArmyLine::GetDirectionNormal()
{
	return (GetEnd()-GetStart()).GetSafeNormal();
}

void FArmyLine::SetUseTrianlge()
{
	bUseTriangle = true;
	Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_DashLine.M_DashLine"), NULL, LOAD_None, NULL));
	Material->AddToRoot();
}

TSharedPtr<class FArmyEditPoint> FArmyLine::GetLineEditPoint()
{
	LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
	return LineEditPoint;
}

void FArmyLine::SetZForStartAndEnd(float InZ)
{
	FVector TempPos = StartPoint->Pos;
	TempPos.Z = InZ;
	StartPoint->Pos = TempPos;

	TempPos = EndPoint->Pos;
	TempPos.Z = InZ;
	EndPoint->Pos = TempPos;
}

void FArmyLine::SetDrawDataOffset(FVector InOffset)
{
	DrawOffset = InOffset;
}

void FArmyLine::SetShouldDraw(bool bDraw)
{
	bShouldDraw = bDraw;
}

void FArmyLine::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	/**@欧石楠 如果不需要绘制直接返回*/
	if (!bShouldDraw)
		return;

	float UniformScale = View->WorldToScreen(StartPoint->Pos).W * (4.0f / View->ViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);
	float Scale = FMath::Abs(UniformScale / 2.0f);
	if (!bIsDashLine)
	{
#ifdef TriangleDraw
		LineWidth = 1.f;
		FVector direction = (StartPoint->Pos - EndPoint->Pos).GetSafeNormal();
		FVector vertical = direction.RotateAngleAxis(90, FVector(0, 0, 1));
		FVector pos0 = StartPoint->Pos + vertical*LineWidth * Scale;
		FVector pos1 = StartPoint->Pos - vertical*LineWidth *Scale;
		FVector pos2 = EndPoint->Pos - vertical * LineWidth *Scale;
		FVector pos3 = EndPoint->Pos + vertical * LineWidth * Scale;
		float length = FVector::Dist(pos3, pos0);
		float numV = length / (200.0f*Scale);
		FVector2D UVs[4] =
		{
			FVector2D(0,0),
			FVector2D(0,1),
			FVector2D(1 * numV,1),
			FVector2D(1 * numV,0),
		};
		FDynamicMeshBuilder MeshBuilder;
		FVector Normal = FVector(0, 0, 1);
		FVector Tangent = FVector(1, 0, 0);
		MeshBuilder.AddVertex(FDynamicMeshVertex(pos0, Tangent, Normal, UVs[0], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(pos1, Tangent, Normal, UVs[1], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(pos2, Tangent, Normal, UVs[2], FColor::White));
		MeshBuilder.AddVertex(FDynamicMeshVertex(pos3, Tangent, Normal, UVs[3], FColor::White));
		MeshBuilder.AddTriangle(0, 1, 2);
		MeshBuilder.AddTriangle(0, 2, 3);
		MeshBuilder.Draw(PDI, FMatrix::Identity, mDynamicMaterial->GetRenderProxy(false), DepthPriority, true, false);

#else
        // Alpha值决定半透明绘制还是普通绘制
        if (GetBaseColor().A > 0.f && GetBaseColor().A < 1.f)
        {
            PDI->DrawTranslucentLine(StartPoint->Pos + DrawOffset, EndPoint->Pos + DrawOffset, GetBaseColor(), DepthPriority, LineWidth, DepthBias, true);
        }
        else if (GetBaseColor().A >= 1.f)
        {
            PDI->DrawLine(StartPoint->Pos + DrawOffset, EndPoint->Pos + DrawOffset, GetBaseColor(), DepthPriority, LineWidth, DepthBias, true);
        }

		/**@欧石楠选中线时显示线的操作点*/
		if (GetState() == FArmyPrimitive::OPS_Selected && ShowEditPoint)
		{
			LineEditPoint->Pos = (StartPoint->Pos + EndPoint->Pos) / 2.f;
			LineEditPoint->Draw(PDI, View);
			StartPoint->Draw(PDI, View);
			EndPoint->Draw(PDI, View);
		}
#endif

	}
	else
	{
		if (bUseTriangle)
		{
			const float SizeX = View->ViewRect.Width();
			const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0])*2.0f / SizeX;
			FVector direction = (StartPoint->Pos - EndPoint->Pos).GetSafeNormal();
			FVector vertical = direction.RotateAngleAxis(90, FVector(0, 0, 1));
			LineWidth = 0.5f;
			FVector pos0 = StartPoint->Pos + vertical*LineWidth * Zoom;
			FVector pos1 = StartPoint->Pos - vertical*LineWidth *Zoom;
			FVector pos2 = EndPoint->Pos - vertical * LineWidth *Zoom;
			FVector pos3 = EndPoint->Pos + vertical * LineWidth * Zoom;
			float length = FVector::Dist(pos3, pos0);
			float numV = length / (200.0f*Zoom);
			FVector2D UVs[4] =
			{
				FVector2D(0,0),
				FVector2D(0,1),
				FVector2D(1 * numV,1),
				FVector2D(1 * numV,0),
			};
			FDynamicMeshBuilder MeshBuilder;
			FVector Normal = FVector(0, 0, 1);
			FVector Tangent = FVector(1, 0, 0);
			MeshBuilder.AddVertex(FDynamicMeshVertex(pos0, Tangent, Normal, UVs[0], FColor::White));
			MeshBuilder.AddVertex(FDynamicMeshVertex(pos1, Tangent, Normal, UVs[1], FColor::White));
			MeshBuilder.AddVertex(FDynamicMeshVertex(pos2, Tangent, Normal, UVs[2], FColor::White));
			MeshBuilder.AddVertex(FDynamicMeshVertex(pos3, Tangent, Normal, UVs[3], FColor::White));
			MeshBuilder.AddTriangle(0, 1, 2);
			MeshBuilder.AddTriangle(0, 2, 3);
			MeshBuilder.Draw(PDI, FMatrix::Identity, Material->GetRenderProxy(false), DepthPriority, true, false);
		}
		else
		{
			DrawDashedLine(PDI, StartPoint->Pos, EndPoint->Pos, GetBaseColor(), DashSize * Scale, DepthPriority, DepthBias);
		}
	}
	FArmyPrimitive::Draw(PDI, View);
}
