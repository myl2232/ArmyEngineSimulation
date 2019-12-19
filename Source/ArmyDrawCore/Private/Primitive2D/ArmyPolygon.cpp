#include "ArmyPolygon.h"
#include "ArmyLine.h"
#include "Math/ArmyMath.h"
//#include "FloorTextureData/ArmyBaseEditStyle.h"
#include "DynamicMeshBuilder.h"
#include "SceneManagement.h"

FArmyPolygon::FArmyPolygon()
	: FillColor(FLinearColor(1, 0, 0, 1))
	, MaterialRenderProxy(nullptr)
	, Thickness(0)
	, MinPoint(ForceInitToZero)
	, MaxPoint(ForceInitToZero)
	, ZoomFactor(32.0f)
	, ZoomWithCameraHeight(true)
	, XDirRepeatDist(40.0f)
	, YDirRepeatDist(40.0f)
	, CurrentAlginMent(0)
	, OutLineColor(FColor::White)
{
}

FArmyPolygon::FArmyPolygon(FArmyPolygon* Copy)
{
    FillColor = Copy->FillColor;
    MaterialRenderProxy = Copy->MaterialRenderProxy;
    Thickness = Copy->Thickness;
    MinPoint = Copy->MinPoint;
    MaxPoint = Copy->MaxPoint;
    ZoomFactor = Copy->ZoomFactor;
    ZoomWithCameraHeight = Copy->ZoomWithCameraHeight;
    XDirRepeatDist = Copy->XDirRepeatDist;
    YDirRepeatDist = Copy->YDirRepeatDist;
    CurrentAlginMent = Copy->CurrentAlginMent;
    Vertices = Copy->Vertices;
    UsePoints = Copy->UsePoints;
    PolygonOffset = Copy->PolygonOffset;
    AlignPoint = Copy->AlignPoint;
}

void FArmyPolygon::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (MaterialRenderProxy != NULL)
	{
		const float SizeX = View->ViewRect.Width();
		const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0])*2.0f / SizeX;
		FDynamicMeshBuilder MeshBuilder;
		FVector Normal = FVector(0, 0, 1);
		FVector Tangent = FVector(1, 0, 0);
		TArray<int32> VertexIndexArray;
		for (int32 Index = 0; Index < UsePoints.Num(); Index++)
		{
			FVector2D Point = UsePoints[Index];
			float ULength = MaxPoint.X - MinPoint.X;
			float VLength = MaxPoint.Y - MinPoint.Y;
			float numU = (ULength / (ZoomFactor * Zoom));
			float numV = (VLength / (ZoomFactor * Zoom));
			float VCoord, UCoord;
			if (ZoomWithCameraHeight)
			{
				UCoord = (Point.X - MinPoint.X) * numU / ULength;
				VCoord = (Point.Y - MinPoint.Y) * numV / VLength;
			}
			else
			{

				UCoord = (Point.X - AlignPoint.X);
				VCoord = (Point.Y - AlignPoint.Y);
				FVector2D uv = FVector2D(UCoord, VCoord);
				uv -= FVector2D(0.5, 0.5f);
				float RotateAngle = 0;
				UCoord = uv.X * FMath::Cos(FMath::DegreesToRadians(RotateAngle)) - uv.Y * FMath::Sin(FMath::DegreesToRadians(RotateAngle));
				VCoord = uv.X * FMath::Sin(FMath::DegreesToRadians(RotateAngle)) + uv.Y * FMath::Cos(FMath::DegreesToRadians(RotateAngle));
				UCoord = (UCoord + 0.5f) / XDirRepeatDist;
				VCoord = (VCoord + 0.5f) / YDirRepeatDist;

			}
			VertexIndexArray.Add(MeshBuilder.AddVertex(FDynamicMeshVertex(FVector(Point, PolygonOffset), Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White)));
			if (VertexIndexArray.Num() == 3)
			{
				MeshBuilder.AddTriangle(VertexIndexArray[0], VertexIndexArray[1], VertexIndexArray[2]);
				VertexIndexArray.Empty();
			}
		}
		MeshBuilder.Draw(PDI, FMatrix::Identity, MaterialRenderProxy, DepthPriority, true, false);
	}

}

void FArmyPolygon::DrawOutLine(FPrimitiveDrawInterface* PDDI, const FSceneView* View)
{
	int number = Vertices.Num();
	for (int i = 0; i < number; i++)
	{
		PDDI->DrawLine(Vertices[i%number], Vertices[(i + 1) % number], OutLineColor, 1);
	}
}

bool FArmyPolygon::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, Vertices);
}

bool FArmyPolygon::IsSelected(const FBox2D& box)
{
	// to do 
	return false;
}

void FArmyPolygon::SetVertices(const TArray<FVector>& InVertices)
{
	Vertices = InVertices;
	PrePareCalculate(InVertices);
}

void FArmyPolygon::AddVertice(FVector InVertice)
{
	Vertices.Push(InVertice);
	PrePareCalculate(Vertices);
}

void FArmyPolygon::RemoveVertice(FVector InVertice)
{
	if (Vertices.Contains(InVertice))
	{
		Vertices.Remove(InVertice);
		PrePareCalculate(Vertices);
	}
}

void FArmyPolygon::GetLines(TArray< TSharedPtr<class FArmyLine> >& OutLines)
{
	int number = Vertices.Num();
	for (int32 i = 0; i < number; i++)
	{
		TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine(Vertices[i%number], Vertices[(i + 1) % number]));
		OutLines.Push(Line);
	}
}

void FArmyPolygon::SetDrawAlignment(uint32 inAlignment)
{
	CurrentAlginMent = inAlignment;
	if (inAlignment == 0)
	{
		AlignPoint = FVector2D(MinPoint.X, MinPoint.Y);
	}
	else if (inAlignment == 1)
	{
		float x = (MinPoint.X + MaxPoint.X) / 2.0f;
		float y = MinPoint.Y;
		AlignPoint = FVector2D(x, y);
	}
	else if (inAlignment == 2)
	{
		float x = MaxPoint.X;
		float y = MinPoint.Y;
		AlignPoint = FVector2D(x, y);

	}
	else if (inAlignment == 3)
	{
		float x = MinPoint.X;
		float y = (MinPoint.Y + MaxPoint.Y) / 2.0f;
		AlignPoint = FVector2D(x, y);

	}
	else if (inAlignment == 4)
	{
		AlignPoint = (MinPoint + MaxPoint) / 2.0f;
	}
	else if (inAlignment == 5)
	{
		float x = MaxPoint.X;
		float y = (MinPoint.Y + MaxPoint.Y) / 2.0f;
		AlignPoint = FVector2D(x, y);

	}
	else if (inAlignment == 6)
	{
		float x = MinPoint.X;
		float y = MaxPoint.Y;
		AlignPoint = FVector2D(x, y);

	}
	else if (inAlignment == 7)
	{
		float x = (MinPoint.X + MaxPoint.X) / 2.0f;
		float y = MaxPoint.Y;
		AlignPoint = FVector2D(x, y);

	}
	else if (inAlignment == 8)
	{
		AlignPoint = FVector2D(MaxPoint.X, MaxPoint.Y);
	}
}
void FArmyPolygon::SetPolygonOffset(float InOffset)
{
	PolygonOffset = InOffset;
}

void FArmyPolygon::RemoveAllVertexes()
{
	Vertices.Empty();
	UsePoints.Empty();
}

void FArmyPolygon::PrePareCalculate(const TArray<FVector>& InVertices)
{
	TArray<FVector2D> InPoints;
	for (int32 i = 0; i < InVertices.Num(); i++)
	{
		InPoints.Push(FVector2D(InVertices[i].X, InVertices[i].Y));
	}

	if (!FArmyMath::IsClockWise(InPoints))
	{
		TArray<FVector2D> NewPoints;
		for (int32 j = InPoints.Num() - 1; j >= 0; j--)
		{
			NewPoints.Push(InPoints[j]);
		}
		InPoints.Empty();
		InPoints = NewPoints;
	}
	UsePoints.Empty();
	if (FArmyMath::Triangulate2DPoly(UsePoints, InPoints))
	{
		float MinX = UsePoints[0].X;
		float MinY = UsePoints[0].Y;
		float MaxX = UsePoints[0].X;
		float MaxY = UsePoints[0].Y;
		for (int32 Index = 1; Index < UsePoints.Num(); Index++)
		{
			FVector2D Point = UsePoints[Index];
			if (Point.X < MinX)
				MinX = Point.X;
			if (Point.Y < MinY)
				MinY = Point.Y;
			if (Point.X > MaxX)
				MaxX = Point.X;
			if (Point.Y > MaxY)
				MaxY = Point.Y;
		}

		MinPoint.Set(MinX, MinY);
		MaxPoint.Set(MaxX, MaxY);
		AlignPoint = MinPoint;
	}
	SetDrawAlignment(CurrentAlginMent);

}

bool FArmyPolygon::IsInside(const FVector& Position, bool ContainInsied/* = false*/) const
{
	//判断点是否在polygon边上
	int Number = Vertices.Num();
	for (int Index = 0; Index < Number; ++Index)
	{
		FVector Start = Vertices[Index%Number];
		FVector End = Vertices[(Index + 1) % Number];
			
		Start = FVector(Start.X, Start.Y, 0);
		End = FVector(End.X, End.Y, 0);
			
		bool OnLine = FMath::PointDistToSegment(Position, Start, End) < 0.005;
				
		if (OnLine)
			return ContainInsied ? true : false;
	}

	//射线法判断该点是否在该polygon内
	//定义向右发出的一条射线--->无穷远
	const FVector& CastRay = FVector(MAX_FLT, Position.Y, Position.Z);
	
	//计算各边和射线相交情况
	int Count = 0;
	for (int Index = 0; Index < Vertices.Num(); ++Index)
	{
		FVector Start = Vertices[Index%Number];
		FVector End = Vertices[(Index + 1) % Number];

		if (IsIntersected(Start, End, Position, CastRay))
		{
			//相交
			++Count;
		}
	}

	//如果为奇数则在polygon内
	return Count % 2 == 1 ? true : false;
}

bool FArmyPolygon::IsIntersected(const FVector& Line1StartPos, const FVector& Line1EndPos, const FVector& Line2StartPos, const FVector& Line2EndPos)
{
	//快速排斥实验
	if ((Line1StartPos.X > Line1EndPos.X ? Line1StartPos.X : Line1EndPos.X) < (Line2StartPos.X < Line2EndPos.X ? Line2StartPos.X : Line2EndPos.X) ||
		(Line1StartPos.Y > Line1EndPos.Y ? Line1StartPos.Y : Line1EndPos.Y) < (Line2StartPos.Y < Line2EndPos.Y ? Line2StartPos.Y : Line2EndPos.Y) ||
		(Line2StartPos.X > Line2EndPos.X ? Line2StartPos.X : Line2EndPos.X) < (Line1StartPos.X < Line1EndPos.X ? Line1StartPos.X : Line1EndPos.X) ||
		(Line2StartPos.Y > Line2EndPos.Y ? Line2StartPos.Y : Line2EndPos.Y) < (Line1StartPos.Y < Line1EndPos.Y ? Line1StartPos.Y : Line1EndPos.Y))
	{
		return false;
	}
	//跨立实验
	if ((((Line1StartPos.X - Line2StartPos.X)*(Line2EndPos.Y - Line2StartPos.Y) - (Line1StartPos.Y - Line2StartPos.Y)*(Line2EndPos.X - Line2StartPos.X))*
		((Line1EndPos.X - Line2StartPos.X)*(Line2EndPos.Y - Line2StartPos.Y) - (Line1EndPos.Y - Line2StartPos.Y)*(Line2EndPos.X - Line2StartPos.X))) > 0 ||
		(((Line2StartPos.X - Line1StartPos.X)*(Line1EndPos.Y - Line1StartPos.Y) - (Line2StartPos.Y - Line1StartPos.Y)*(Line1EndPos.X - Line1StartPos.X))*
		((Line2EndPos.X - Line1StartPos.X)*(Line1EndPos.Y - Line1StartPos.Y) - (Line2EndPos.Y - Line1StartPos.Y)*(Line1EndPos.X - Line1StartPos.X))) > 0)
	{
		return false;
	}
	return true;
}
