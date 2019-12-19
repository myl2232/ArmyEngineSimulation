#include "ArmyShapeBoardActor.h"
#include "DynamicMeshBuilder.h"

AXRShapeActor::AXRShapeActor()
	:MaterialId(-1)
{
	PrimaryActorTick.bCanEverTick = true;
	BoardMesh = CreateDefaultSubobject<UXRProceduralMeshComponent>(TEXT("ArmyShapeRootComponent"));
	RootComponent = BoardMesh;
}

void AXRShapeActor::BeginPlay()
{
	Super::BeginPlay();
}

void AXRShapeActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRShapeActor::Destroy()
{
	Super::Destroy();
}

void AXRShapeActor::ResetMeshTriangles()
{
	TotalVertexs.Empty();
	TotalTriangles.Empty();
}

void AXRShapeActor::AddBoardPoints(const TArray<FVector>& PointList, float InBoardHeight)
{
	TArray<FVector> temPoints;
	int number = PointList.Num();
	for (int i = 0; i < number; i++)
	{
		FVector Point;
		Point.X = PointList[i].X;
		Point.Y = PointList[i].Y;
		Point.Z = PointList[i].Z + InBoardHeight;
		temPoints.Push(Point);
	}
	TArray<FArmyProcMeshVertex> Verts;
	TArray<FArmyProcMeshTriangle> Triangles;
	//TriangluatePoints(temPoints, InBoardHeight, Verts, Triangles);
	UXRProceduralMeshComponent::Triangulate_Extrusion(temPoints, InBoardHeight, Verts, Triangles);
	FArmyProcMeshTriangle TriangleIndices;
	for (int i = 0; i < Triangles.Num(); i++)
	{
		TriangleIndices.Vertex0 = Triangles[i].Vertex0 + TotalVertexs.Num();
		TriangleIndices.Vertex1 = Triangles[i].Vertex1 + TotalVertexs.Num();
		TriangleIndices.Vertex2 = Triangles[i].Vertex2 + TotalVertexs.Num();
		TotalTriangles.Push(TriangleIndices);
	}
	TotalVertexs.Append(Verts);

}

void AXRShapeActor::AddQuatGemometry(const TArray<FVector>& PointList)
{
	int number = TotalVertexs.Num();
	if (PointList.Num() != 4)
	{
		return;
	}
	FBox box(PointList);
	float length = (PointList[2] - PointList[1]).Size();
	float height = (PointList[1] - PointList[0]).Size();
	FArmyProcMeshVertex temp;
	temp.Position = PointList[0];
	temp.UV = FVector2D(0, 0);
	temp.Color = FColor::White;
	TotalVertexs.Add(temp);
	temp.Position = PointList[1];
	temp.UV = FVector2D(0, height / 100.0f);
	TotalVertexs.Add(temp);
	temp.Position = PointList[2];
	temp.UV = FVector2D(length / 100.f, height / 100.0f);
	TotalVertexs.Add(temp);
	temp.Position = PointList[3];
	temp.UV = FVector2D(length / 100.0f, 0);
	TotalVertexs.Add(temp);
	FArmyProcMeshTriangle tri0;
	tri0.Vertex0 = 0 + number;
	tri0.Vertex1 = 1 + number;
	tri0.Vertex2 = 2 + number;
	FArmyProcMeshTriangle tri1;
	tri1.Vertex0 = 0 + number;
	tri1.Vertex1 = 2 + number;
	tri1.Vertex2 = 3 + number;
	TotalTriangles.Push(tri0);
	TotalTriangles.Push(tri1);
}

void AXRShapeActor::AddVetries(const TArray<FVector>& PointList)
{
	int number = TotalVertexs.Num();

	if (PointList.Num() % 3 != 0)
		return;

	for (int32 i = 0; i < PointList.Num(); i = i + 3)
	{
		FVector Point0 = PointList[i];
		FVector Point1 = PointList[i + 1];
		FVector Point2 = PointList[i + 2];

		float length = (Point2 - Point1).Size();
		float height = (Point1 - Point0).Size();
		FArmyProcMeshVertex temp;
		temp.Position = Point0;
		temp.UV = FVector2D(0, 0);
		temp.Color = FColor::White;
		TotalVertexs.Add(temp);
		temp.Position = Point1;
		temp.UV = FVector2D(0, height / 100.0f);
		TotalVertexs.Add(temp);
		temp.Position = Point2;
		temp.UV = FVector2D(length / 100.f, height / 100.0f);
		TotalVertexs.Add(temp);
		FArmyProcMeshTriangle tri0;
		tri0.Vertex0 = i + number;
		tri0.Vertex1 = i + 1 + number;
		tri0.Vertex2 = i + 2 + number;
		TotalTriangles.Push(tri0);
	}
	int32 i = 0;

}

void AXRShapeActor::AddVerts( TArray<FDynamicMeshVertex>& Points)
{
	int number = TotalVertexs.Num();

	if (Points.Num() % 3 != 0)
		return;

	for (int32 i = 0; i < Points.Num(); i = i + 3)
	{
		FVector Point0 = Points[i].Position;
		FVector Point1 = Points[i + 1].Position;
		FVector Point2 = Points[i + 2].Position;

	
		FArmyProcMeshVertex temp;
		temp.Position = Point0;
		temp.UV = Points[i].TextureCoordinate;
		temp.TangentX = Points[i].TangentX;
		temp.TangentZ = Points[i].TangentZ;
		temp.TangentY = Points[i].GetTangentY();
		temp.Color = FColor(255, 0, 0, 0);
		TotalVertexs.Add(temp);
		temp.Position = Point1;
		temp.UV = Points[i + 1].TextureCoordinate;
		temp.TangentX = Points[i+1].TangentX;
		temp.TangentZ = Points[i+1].TangentZ;
		temp.TangentY = Points[i+1].GetTangentY();
		TotalVertexs.Add(temp);
		temp.Position = Point2;
		temp.UV = Points[i + 2].TextureCoordinate;
		temp.TangentX = Points[i+2].TangentX;
		temp.TangentZ = Points[i+2].TangentZ;
		temp.TangentY = Points[i+2].GetTangentY();
		TotalVertexs.Add(temp);
		FArmyProcMeshTriangle tri0;
		tri0.Vertex0 = i + number;
		tri0.Vertex1 = i + 1 + number;
		tri0.Vertex2 = i + 2 + number;
		TotalTriangles.Push(tri0);
	}
	//BoardMesh->AutoCaluatePlaneNormal = false;
}

void AXRShapeActor::SetMaterial(UMaterialInterface* Material)
{
	BoardMesh->SetMaterial(0, Material);
}

void AXRShapeActor::UpdateAllVetexBufferIndexBuffer()
{
	BoardMesh->CreateMeshSection(0, TotalVertexs, TotalTriangles, true, true);
}

class UXRProceduralMeshComponent* AXRShapeActor::GetMeshComponent()
{
	return Cast<UXRProceduralMeshComponent>(RootComponent);
}

//@´ò°ç¼Ò XRLightmass SetGeometryBuffer
void AXRShapeActor::SetGeometryBuffer(TArray<FVector>& InVertexBuffer, TArray<FVector2D>& InUVs, TArray<int32>& InIndexBuffer)
{
	for (int32 i = 0; i < InVertexBuffer.Num(); i++ )
	{
		FArmyProcMeshVertex Ver;
		Ver.Position = InVertexBuffer[i];
		Ver.UV = InUVs[i];
		Ver.TangentX = FVector(0, 0, 0);
		Ver.TangentZ = FVector(0, 0, 0);
		Ver.TangentY = FVector(0, 0, 0);
		Ver.Color = FColor(255, 0, 0, 0);
		TotalVertexs.Add(Ver);
	}

	for (int32 i = 0; i < InIndexBuffer.Num(); )
	{
		FArmyProcMeshTriangle Tringle;
		Tringle.Vertex0 = InIndexBuffer[i++];
		Tringle.Vertex1 = InIndexBuffer[i++];
		Tringle.Vertex2 = InIndexBuffer[i++];
		TotalTriangles.Add(Tringle);
	}

	UpdateAllVetexBufferIndexBuffer();
}

void AXRShapeActor::SetGeometryBufferDirectly(TArray<FArmyProcMeshVertex> InVertexBuffer, TArray<FArmyProcMeshTriangle> InIndexBuffer)
{
	TotalVertexs = InVertexBuffer;
	TotalTriangles = InIndexBuffer;
}

FGuid AXRShapeActor::GetLightMapGUIDFromSMC()
{
	UStaticMeshComponent* SMC = BoardMesh->TempConvertedMeshComponentForBuildingLight;
	if (SMC && SMC->LODData.IsValidIndex(0))
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[0];
		return ComponentLODInfo.MapBuildDataId;
	}

	return LightMapID;
}
