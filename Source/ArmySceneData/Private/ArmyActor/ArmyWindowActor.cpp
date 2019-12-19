#include "ArmyWindowActor.h"
#include "ArmyWindow.h"
#include "ArmyRectBayWindow.h"
AXRWindowActor::AXRWindowActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("WallRoot");
	MeshComponent = CreateDefaultSubobject<UXRProceduralMeshComponent>("WindowComponent");
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->bBuildStaticLighting = true;
}

void AXRWindowActor::BeginPlay()
{
	Super::BeginPlay();
}

void AXRWindowActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRWindowActor::Destroy()
{
	Super::Destroy();
}

void AXRWindowActor::ResetMeshTriangles()
{
	TotalPillarVertexs.Empty();
	TotalPillarTriagnels.Empty();
	TotalGlassVertexes.Empty();
	TotalGlassTriangles.Empty();
	TotalBoardTriangles.Empty();
	TotalBoardVertexes.Empty();
}

void AXRWindowActor::AddPillarPoints(const TArray<FVector>& InPointList, float BoardHeight)
{
	TArray<FArmyProcMeshVertex> tempVertexs;
	TArray<FArmyProcMeshTriangle> tempTriangles;
	CaculateTriangleAndVertex(InPointList, BoardHeight, tempVertexs, tempTriangles, TotalPillarVertexs);
	TotalPillarVertexs.Append(tempVertexs);
	TotalPillarTriagnels.Append(tempTriangles);
}

void AXRWindowActor::AddWindowGlassPoints(const TArray<FVector>& InPointList, float BoardHeight)
{
	TArray<FArmyProcMeshVertex> tempVertexs;
	TArray<FArmyProcMeshTriangle> tempTriangles;
	CaculateTriangleAndVertex(InPointList, BoardHeight, tempVertexs, tempTriangles, TotalGlassVertexes);
	TotalGlassVertexes.Append(tempVertexs);
	TotalGlassTriangles.Append(tempTriangles);
}

void AXRWindowActor::AddWindowBoardPoints(const TArray<FVector>& InPoinstList, float BoardHeight)
{
	TArray<FArmyProcMeshVertex> tempVertexs;
	TArray<FArmyProcMeshTriangle> tempTriangles;
	CaculateTriangleAndVertex(InPoinstList, BoardHeight, tempVertexs, tempTriangles, TotalBoardVertexes);
	TotalBoardVertexes.Append(tempVertexs);
	TotalBoardTriangles.Append(tempTriangles);
}

void AXRWindowActor::SetWindowBoardMaterial(UMaterialInterface* InMaterial)
{
	MeshComponent->SetMaterial(0, InMaterial);
}

void AXRWindowActor::SetWindowPillarMaterial(UMaterialInterface* InMaterial)
{
	MeshComponent->SetMaterial(1, InMaterial);
}

void AXRWindowActor::SetWindowGlassMaterial(UMaterialInterface* InMaterial)
{
	MeshComponent->SetMaterial(2, InMaterial);
}

void AXRWindowActor::UpdataAlloVertexIndexBuffer()
{
	if (TotalBoardVertexes.Num() > 0 && TotalBoardTriangles.Num() > 0)
		MeshComponent->CreateMeshSection(0, TotalBoardVertexes, TotalBoardTriangles, true, true);
	if (TotalPillarVertexs.Num() > 0 && TotalPillarTriagnels.Num() > 0)
		MeshComponent->CreateMeshSection(1, TotalPillarVertexs, TotalPillarTriagnels, true, true);
	if (TotalGlassVertexes.Num() > 0 && TotalGlassTriangles.Num() > 0)
		MeshComponent->CreateMeshSection(2, TotalGlassVertexes, TotalGlassTriangles, true, true);
}

FGuid AXRWindowActor::GetLightMapGUIDFromSMC()
{
	UStaticMeshComponent* SMC = MeshComponent->TempConvertedMeshComponentForBuildingLight;
	if (SMC && SMC->LODData.IsValidIndex(0))
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[0];
		return ComponentLODInfo.MapBuildDataId;
	}
	return GetLightMapGUIDFromAttachSurface();
}

FGuid AXRWindowActor::GetLightMapGUIDFromAttachSurface()
{
	if (AttachSurface.IsValid())
	{
		FArmyWindow* Obj = AttachSurface->AsassignObj<FArmyWindow>();
		if(Obj)
			return Obj->GetLightMapID();

		FArmyRectBayWindow* Obj2 = AttachSurface->AsassignObj<FArmyRectBayWindow>();
		if (Obj2)
			return Obj2->GetLightMapID();
	}
	return FGuid();
}

void AXRWindowActor::CaculateTriangleAndVertex(const TArray<FVector>& PointList, float BoardHeight, TArray<FArmyProcMeshVertex>& OutVertexs, TArray<FArmyProcMeshTriangle>& OutTriangles, TArray<FArmyProcMeshVertex> tempTotalVertexs)
{
	TArray<FVector> temPoints;
	int number = PointList.Num();
	for (int i = 0; i < number; i++)
	{
		FVector Point;
		Point.X = PointList[i].X;
		Point.Y = PointList[i].Y;
		Point.Z = PointList[i].Z + BoardHeight;
		temPoints.AddUnique(Point);
	}
	TArray<FArmyProcMeshVertex> Verts;
	TArray<FArmyProcMeshTriangle> Triangles;
	UXRProceduralMeshComponent::Triangulate_Extrusion(temPoints, BoardHeight, Verts, Triangles);
	FArmyProcMeshTriangle TriangleIndices;
	for (int i = 0; i < Triangles.Num(); i++)
	{
		TriangleIndices.Vertex0 = Triangles[i].Vertex0 + tempTotalVertexs.Num();
		TriangleIndices.Vertex1 = Triangles[i].Vertex1 + tempTotalVertexs.Num();
		TriangleIndices.Vertex2 = Triangles[i].Vertex2 + tempTotalVertexs.Num();
		OutTriangles.Push(TriangleIndices);
	}
	OutVertexs.Append(Verts);
}

