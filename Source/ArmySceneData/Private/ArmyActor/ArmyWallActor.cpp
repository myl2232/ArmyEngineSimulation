#include "ArmyWallActor.h"
#include "ArmyProceduralMeshComponent.h"
#include "DynamicMeshBuilder.h"
#include "ArmyBaseArea.h"

AXRWallActor::AXRWallActor()
{
	/*RootComponent = CreateDefaultSubobject<USceneComponent>("WallRoot");
	BrickComponent = CreateDefaultSubobject<UXRGenMeshComponent>("BrickComponent");

	BrickComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BrickGapsComponent = CreateDefaultSubobject<UXRGenMeshComponent>("BrickGapsComponent");
	BrickGapsComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);*/

	RootComponent = CreateDefaultSubobject<USceneComponent>("WallRoot");
	MeshComponent = CreateDefaultSubobject<UXRProceduralMeshComponent>("WallMeshComponent");
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AXRWallActor::BeginPlay()
{
	Super::BeginPlay();
}

void AXRWallActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRWallActor::Destroy()
{
	Super::Destroy();
}

void AXRWallActor::ResetMeshTriangles()
{
	TotalBrickGapVerts.Empty();
	TotalBirckGapTriangles.Empty();
	TotalBrickVerts.Empty();
	TotalBirckTriangles.Empty();
}

void AXRWallActor::AddVerts(TArray<FDynamicMeshVertex>& Points)
{
	int number = TotalBrickVerts.Num();

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
		TotalBrickVerts.Add(temp);
		temp.Position = Point1;
		temp.UV = Points[i + 1].TextureCoordinate;
		temp.TangentX = Points[i + 1].TangentX;
		temp.TangentZ = Points[i + 1].TangentZ;
		temp.TangentY = Points[i + 1].GetTangentY();
		TotalBrickVerts.Add(temp);
		temp.Position = Point2;
		temp.UV = Points[i + 2].TextureCoordinate;
		temp.TangentX = Points[i + 2].TangentX;
		temp.TangentZ = Points[i + 2].TangentZ;
		temp.TangentY = Points[i + 2].GetTangentY();
		TotalBrickVerts.Add(temp);
		FArmyProcMeshTriangle tri0;
		tri0.Vertex0 = i + number;
		tri0.Vertex1 = i + 1 + number;
		tri0.Vertex2 = i + 2 + number;
		TotalBirckTriangles.Push(tri0);
	}
	//BrickComponent->AutoCaluatePlaneNormal = false;
}

void AXRWallActor::AddGapVerts(TArray<FDynamicMeshVertex>& Points)
{
	int number = TotalBrickGapVerts.Num();

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
		TotalBrickGapVerts.Add(temp);
		temp.Position = Point1;
		temp.UV = Points[i + 1].TextureCoordinate;
		temp.TangentX = Points[i + 1].TangentX;
		temp.TangentZ = Points[i + 1].TangentZ;
		temp.TangentY = Points[i + 1].GetTangentY();
		TotalBrickGapVerts.Add(temp);
		temp.Position = Point2;
		temp.UV = Points[i + 2].TextureCoordinate;
		temp.TangentX = Points[i + 2].TangentX;
		temp.TangentZ = Points[i + 2].TangentZ;
		temp.TangentY = Points[i + 2].GetTangentY();
		TotalBrickGapVerts.Add(temp);
		FArmyProcMeshTriangle tri0;
		tri0.Vertex0 = i + number;
		tri0.Vertex1 = i + 1 + number;
		tri0.Vertex2 = i + 2 + number;
		TotalBirckGapTriangles.Push(tri0);
	}
	//BrickGapsComponent->AutoCaluatePlaneNormal = false;
}

void AXRWallActor::UpdateAllVetexBufferIndexBuffer()
{
	if (TotalBrickVerts.Num()>0 && TotalBirckTriangles.Num()>0)
		MeshComponent->CreateMeshSection(0, TotalBrickVerts, TotalBirckTriangles, false, true);
	else
		MeshComponent->ClearMeshSection(0);

	if (TotalBrickGapVerts.Num() > 0 && TotalBirckGapTriangles.Num() > 0)
		MeshComponent->CreateMeshSection(1, TotalBrickGapVerts, TotalBirckGapTriangles, false, true);
	else
	{
		MeshComponent->ClearMeshSection(1);
		//MeshComponent->bAutoWrapFlatLightUV = false;
	}
}

void AXRWallActor::SetMaterial(UMaterialInterface* Material)
{
	MeshComponent->SetMaterial(0, Material);
}

void AXRWallActor::SetBrickMaterial(UMaterialInterface* Material)
{
	MeshComponent->SetMaterial(1, Material);
}

UXRProceduralMeshComponent* AXRWallActor::GetMeshComponent()
{
	return Cast<UXRProceduralMeshComponent>(MeshComponent);
}

FGuid AXRWallActor::GetLightMapGUIDFromSMC()
{
	UStaticMeshComponent* SMC = MeshComponent->TempConvertedMeshComponentForBuildingLight;
	if (SMC && SMC->LODData.IsValidIndex(0))
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[0];
		return ComponentLODInfo.MapBuildDataId;
	}
	return GetLightMapGUIDFromAttachSurface();
}

FGuid AXRWallActor::GetLightMapGUIDFromAttachSurface()
{
	if (AttachSurface.IsValid())
	{
		return AttachSurface->GetLightMapID();
	}
	return FGuid();
}

