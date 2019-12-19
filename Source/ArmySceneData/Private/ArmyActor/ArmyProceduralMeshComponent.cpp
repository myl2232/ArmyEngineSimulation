#include "ArmyProceduralMeshComponent.h"
#include "PrimitiveViewRelevance.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "PrimitiveSceneProxy.h"
#include "Containers/ResourceArray.h"
#include "EngineGlobals.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "LocalVertexFactory.h"
#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmyMath.h"
#include "DynamicMeshBuilder.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Developer/RawMesh/Public/RawMesh.h"
#include "ArmyStaticMeshStaticLightingMesh.h"
#include "ArmyMeshUtilities.h"
#include "ArmyProceduralMeshRender.h"
#include "ArmyWallActor.h"
#include "ArmyWindowActor.h"
#include "ArmyExtrusionActor.h"
#include "ArmyShapeBoardActor.h"
#include "ArmySceneData.h"
#include "ArmyWorldManager.h"
//DECLARE_CYCLE_STAT(TEXT("Army Create ProcMesh Proxy"), STAT_XRProcMesh_CreateSceneProxy, STATGROUP_XRProceduralMesh);
//DECLARE_CYCLE_STAT(TEXT("Army Create Mesh Section"), STAT_XRProcMesh_CreateMeshSection, STATGROUP_XRProceduralMesh);
//DECLARE_CYCLE_STAT(TEXT("Army UpdateSection GT"), STAT_XRProcMesh_UpdateSectionGT, STATGROUP_XRProceduralMesh);
//DECLARE_CYCLE_STAT(TEXT("Army UpdateSection RT"), STAT_XRProcMesh_UpdateSectionRT, STATGROUP_XRProceduralMesh);
//DECLARE_CYCLE_STAT(TEXT("Army Get ProcMesh Elements"), STAT_XRProcMesh_GetMeshElements, STATGROUP_XRProceduralMesh);
//DECLARE_CYCLE_STAT(TEXT("Army Update Collision"), STAT_XRProcMesh_UpdateCollision, STATGROUP_XRProceduralMesh);

UXRProceduralMeshComponent::UXRProceduralMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bUseComplexAsSimpleCollision = true;
	LightMapResolution = 0;
	bAutoWrapFlatLightUV = false;
	bBuildStaticLighting = false;
	SetMobility(EComponentMobility::Movable);

}

void UXRProceduralMeshComponent::PostLoad()
{
	Super::PostLoad();

	if (ProcMeshBodySetup && IsTemplate())
	{
		ProcMeshBodySetup->SetFlags(RF_Public);
	}
}


FGuid UXRProceduralMeshComponent::GetLightMapIDFromOwner() const
{
	AXRWallActor* XA = Cast<AXRWallActor>(GetOwner());
	if (XA)
	{
		return XA->GetLightMapGUIDFromAttachSurface();
	}
	AXRWindowActor* WA = Cast<AXRWindowActor>(GetOwner());
	if (WA)
	{
		return WA->GetLightMapGUIDFromAttachSurface();
	}
	AArmyExtrusionActor* EA = Cast<AArmyExtrusionActor>(GetOwner());
	if (EA)
	{
		return EA->GetLightMapGUIDFromAttachSurface();
	}
	AXRShapeActor* SA = Cast<AXRShapeActor>(GetOwner());
	if (SA)
	{
		return SA->LightMapID;
	}
	return FGuid();
}

void UXRProceduralMeshComponent::GetStaticLightingInfo(FStaticLightingPrimitiveInfo& OutPrimitiveInfo, const TArray<ULightComponent*>& InRelevantLights, const FLightingBuildOptions& Options)
{
	if (ProcMeshSections.Num() == 0)
		return;

	bool bAllEmpty = true;
	for (auto It : ProcMeshSections)
	{
		if (It.ProcVertexBuffer.Num() != 0 && It.ProcIndexBuffer.Num() != 0)
		{
			bAllEmpty = false;
			break;
		}
	}

	if (bAllEmpty)
		return;

	bool bShouldBuild = false;

	bool bArchetype = false;

	if (GetFlags() & RF_ArchetypeObject)
		bArchetype = true;

	if (bBuildStaticLighting)
		bShouldBuild = true;

	//Archetype不可以烘焙
	if (bArchetype)
		bShouldBuild = false;

	//不需要Build的Actor设置为动态模型，否则是黑色的
	if (!bShouldBuild)
	{
		SetMobility(EComponentMobility::Movable);
		return;
	}
	else
	{
		SetMobility(EComponentMobility::Static);
	}

	//如果未指认分辨率，则自动计算分辨率
	AutoSetLightMapResolution(FArmyWorldManager::Get().GetGlobalStaticLightingResScale());

	//清除所有之前的用来辅助计算的SMC
	TArray<USceneComponent*> Components;
	GetOwner()->GetComponents(Components);
	for (auto& It : Components)
	{
		if (It->GetName() == "LightmassStaticMeshComponent")
		{
			It->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			It->UnregisterComponent();
			It->ConditionalBeginDestroy();
		}
	}

	//创建StaticMeshComponent，传入Lightmass
	for (int32 LODIndex = 0; LODIndex < 1; LODIndex++)
	{
		UStaticMesh* SM = ConvertToStaticMesh();
		if (SM)
		{
			TempConvertedMeshComponentForBuildingLight = NewObject<UStaticMeshComponent>(GetOwner(), "LightmassStaticMeshComponent");
			TempConvertedMeshComponentForBuildingLight->SetStaticMesh(SM);
			TempConvertedMeshComponentForBuildingLight->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			TempConvertedMeshComponentForBuildingLight->LightmassSettings = LightmassSettings;

			for (int32 i = 0; i < GetNumMaterials(); i++)
			{
				TempConvertedMeshComponentForBuildingLight->SetMaterial(i, GetMaterial(i));
			}
			// Create a static lighting mesh for the LOD.
			FArmyStaticMeshStaticLightingMesh* StaticLightingMesh = new FArmyStaticMeshStaticLightingMesh(TempConvertedMeshComponentForBuildingLight, LODIndex, InRelevantLights);
			OutPrimitiveInfo.Meshes.Add(StaticLightingMesh);

			// Create a static lighting texture mapping for the LOD.
			OutPrimitiveInfo.Mappings.Add(new FArmyStaticMeshStaticLightingTextureMapping(
				TempConvertedMeshComponentForBuildingLight, LODIndex, StaticLightingMesh, LightMapResolution, LightMapResolution, 1, true));
		}
	}
}

bool UXRProceduralMeshComponent::GetLightMapResolution(int32& Width, int32& Height) const
{
	Width = LightMapResolution;
	Height = LightMapResolution;
	return true;
}

int32 UXRProceduralMeshComponent::GetStaticLightMapResolution() const
{
	int32 Width, Height;
	GetLightMapResolution(Width, Height);
	return FMath::Max<int32>(Width, Height);
}

ELightMapInteractionType UXRProceduralMeshComponent::GetStaticLightingType() const
{
	return LMIT_Texture;
}

bool UXRProceduralMeshComponent::ShouldRenderSelected() const
{
	AActor* Owner = GetOwner();
	if (Owner->IsSelected())
		return true;
	return false;
}

void UXRProceduralMeshComponent::InvalidateLightingCacheDetailed(bool bInvalidateBuildEnqueuedLighting, bool bTranslationOnly)
{
	Super::InvalidateLightingCacheDetailed(bInvalidateBuildEnqueuedLighting, bTranslationOnly);

	if (TempConvertedMeshComponentForBuildingLight)
	{
		for (int32 i = 0; i < TempConvertedMeshComponentForBuildingLight->LODData.Num(); i++)
		{
			FStaticMeshComponentLODInfo& LODDataElement = TempConvertedMeshComponentForBuildingLight->LODData[i];
			LODDataElement.MapBuildDataId = FGuid::NewGuid();
		}
		SetMobility(EComponentMobility::Movable);
	}

	MarkRenderStateDirty();
}


void UXRProceduralMeshComponent::SetLightmapResolution(int32 InResolution)
{
	LightMapResolution = InResolution;
}

UStaticMesh* UXRProceduralMeshComponent::ConvertToStaticMesh()
{
	FRawMesh RawMesh;

	FTransform RootTransform = GetOwner()->GetTransform();
	FMatrix WorldToRoot = RootTransform.ToMatrixWithScale().Inverse();
	FMatrix ComponentToWorld = GetComponentTransform().ToMatrixWithScale() * WorldToRoot;

	for (int32 i = 0; i < ProcMeshSections.Num(); i++)
	{
		const int32 BaseVertexIndex = RawMesh.VertexPositions.Num();

		const TArray<FArmyProcMeshVertex>& VerticeBuffer = ProcMeshSections[i].ProcVertexBuffer;
		const TArray<int32>& IndexBuffer = ProcMeshSections[i].ProcIndexBuffer;

		for (auto& It : VerticeBuffer)
		{
			RawMesh.VertexPositions.Add(ComponentToWorld.InverseTransformPosition(It.Position));
		}

		for (auto& It : IndexBuffer)
		{
			RawMesh.WedgeIndices.Add(It + BaseVertexIndex);
			RawMesh.WedgeTangentX.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentX));
			RawMesh.WedgeTangentY.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentY));
			RawMesh.WedgeTangentZ.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentZ));
			RawMesh.WedgeTexCoords[0].Add(VerticeBuffer[It].UV);
			RawMesh.WedgeColors.Add(FColor(255, 255, 255, 255));
		}

		uint32 TriNum = ProcMeshSections[i].ProcIndexBuffer.Num() / 3;
		// copy face info
		for (uint32 TriIndex = 0; TriIndex < TriNum; TriIndex++)
		{
			RawMesh.FaceMaterialIndices.Add(0 + i);
			RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
		}
	}

	// Create StaticMesh object
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(this, "genmeshtest", RF_Public | RF_Standalone);
	StaticMesh->InitResources();

	// Add source to new StaticMesh
	FStaticMeshSourceModel* SrcModel = new (StaticMesh->SourceModels) FStaticMeshSourceModel();
	SrcModel->BuildSettings.bRecomputeNormals = true;
	SrcModel->BuildSettings.bRecomputeTangents = true;
	SrcModel->BuildSettings.bRemoveDegenerates = true;
	SrcModel->BuildSettings.bUseHighPrecisionTangentBasis = false;
	SrcModel->BuildSettings.bUseFullPrecisionUVs = false;
	SrcModel->BuildSettings.bGenerateLightmapUVs = true;
	SrcModel->BuildSettings.SrcLightmapIndex = 0;
	SrcModel->BuildSettings.DstLightmapIndex = 1;
	SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);

	// Copy materials to new mesh
	for (UMaterialInterface* Material : OverrideMaterials)
	{
		StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
	}

	//// setup section info map
	//TArray<int32> UniqueMaterialIndices;
	//for (int32 MaterialIndex : RawMesh.FaceMaterialIndices)
	//{
	//	UniqueMaterialIndices.AddUnique(MaterialIndex);
	//}

	//int32 SectionIndex = 0;
	//for (int32 UniqueMaterialIndex : UniqueMaterialIndices)
	//{
	//	StaticMesh->SectionInfoMap.Set(0, SectionIndex, FMeshSectionInfo(UniqueMaterialIndex));
	//	SectionIndex++;
	//}
	//StaticMesh->OriginalSectionInfoMap.CopyFrom(StaticMesh->SectionInfoMap);

	//Set the Imported version before calling the build
	StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	StaticMesh->LightMapCoordinateIndex = 1;
	StaticMesh->RenderData = MakeUnique<FStaticMeshRenderData>();
	StaticMesh->LightmapUVVersion = (int32)ELightmapUVVersion::Latest;

	FMatrix Mat = FMatrix::Identity;
	if (bAutoWrapFlatLightUV)
	{
		FVector PlaneNormal = FVector(0, 0, 1);
		AXRWallActor* WA = Cast<AXRWallActor>(GetOwner());
		if (WA)
		{
			Mat = FMatrix(
				FVector(WA->AttachSurface->GetXDir()),
				FVector(WA->AttachSurface->GetYDir()),
				FVector(WA->AttachSurface->GetPlaneNormal()),
				FVector(0, 0, 0)
			);
			Mat = Mat.Inverse();
		}
	}

	//自动展2UV
	FArmyMeshUtilities::Get().BuildMeshLightMapUV(RawMesh, LightMapResolution, Mat, bAutoWrapFlatLightUV);
	if (!FArmyMeshUtilities::Get().GenerateRenderingMeshes(RawMesh, *StaticMesh->RenderData))
		return NULL;

	StaticMesh->CreateBodySetup();
	StaticMesh->BodySetup->InvalidatePhysicsData();
	StaticMesh->BodySetup->CreatePhysicsMeshes();
	StaticMesh->LightingGuid = FGuid::NewGuid();
	StaticMesh->CalculateExtendedBounds();

	//把计算好的LightMapUV填充会ProceduralMeshComponent中
	if (RawMesh.WedgeTexCoords[0].Num() == RawMesh.WedgeTexCoords[1].Num())
	{
		int32 BaseIndex = 0;
		for (int32 i = 0; i < ProcMeshSections.Num(); i++)
		{
			TArray<FArmyProcMeshVertex>& VerticeBuffer = ProcMeshSections[i].ProcVertexBuffer;
			const TArray<int32>& IndexBuffer = ProcMeshSections[i].ProcIndexBuffer;

			for (int32 j = 0; j < IndexBuffer.Num(); j++)
			{
 				VerticeBuffer[IndexBuffer[j]].LightMapUV = RawMesh.WedgeTexCoords[1][j + BaseIndex];
			}

			BaseIndex += IndexBuffer.Num();
		}
		return StaticMesh;
	}

	return NULL;
}

void UXRProceduralMeshComponent::AutoSetLightMapResolution(float InScale)
{
	//对于分辨率默认为0的情况，则计算面积，自动匹配分辨率
	if (LightMapResolution == 0)
	{
		float TotalArea = 0;
		for (FArmyProcMeshSection& SectionIt : ProcMeshSections)
		{
			for (int32 Index = 0; Index < SectionIt.ProcIndexBuffer.Num() - 2; Index++)
			{
				int32 VerIndex0 = SectionIt.ProcIndexBuffer[Index];
				int32 VerIndex1 = SectionIt.ProcIndexBuffer[Index + 1];
				int32 VerIndex2 = SectionIt.ProcIndexBuffer[Index + 2];
				float AB = FVector::Dist(SectionIt.ProcVertexBuffer[VerIndex0].Position, SectionIt.ProcVertexBuffer[VerIndex1].Position);
				float AC = FVector::Dist(SectionIt.ProcVertexBuffer[VerIndex0].Position, SectionIt.ProcVertexBuffer[VerIndex2].Position);
				float CB = FVector::Dist(SectionIt.ProcVertexBuffer[VerIndex2].Position, SectionIt.ProcVertexBuffer[VerIndex1].Position);

				//不能构成三角形;  
				if (AB + AC <= CB || AB + CB <= AC || AC + CB <= AB)
					continue;

				//利用海伦公式。s=sqr(p*(p-a)(p-b)(p-c));   
				float p = (AB + AC + CB) / 2; //半周长;  
				TotalArea += FMath::Sqrt(p*(p - AB)*(p - AC)*(p - CB));
			}
		}

		//if (TotalArea < 2000)//0.2平米
		//	LightMapResolution = 64;
		//else if (TotalArea < 100000)//10平米
		//	LightMapResolution = 128;
		//else if (TotalArea < 300000)//30平米
		//	LightMapResolution = 256;
		//else if (TotalArea < 500000)//50平米
		//	LightMapResolution = 512;
		//else//大于50平米
		//	LightMapResolution = 1024.f;

		if (TotalArea < 10000)
			LightMapResolution = 64.f;
		else if (TotalArea < 100000)
			LightMapResolution = 128.f;
		else if (TotalArea < 500000)
			LightMapResolution = 256.f;
		else
			LightMapResolution = 512.f;

		LightMapResolution *= InScale;
	}
}

void UXRProceduralMeshComponent::Triangulate_Extrusion(const TArray<FVector>& InOutlineVertices, const float Thickness, TArray<FArmyProcMeshVertex>& OutVertices, TArray<FArmyProcMeshTriangle>& OutTriangles, float InRepeatDistance)
{
	OutVertices.Reset();
	OutTriangles.Reset();
	TArray<FVector> OutlinePoints = InOutlineVertices;
	TArray<FVector> OutlinePoints_Reversed = InOutlineVertices;

	if (FArmyMath::IsClockWise(InOutlineVertices))
	{
		FArmyMath::ReversePointList(OutlinePoints_Reversed);
	}
	else
	{
		FArmyMath::ReversePointList(OutlinePoints);
	}

	int32 PointsNum = OutlinePoints_Reversed.Num();

	FArmyProcMeshVertex CurrentVertex;
	FBox CurrentBoxTop(OutlinePoints_Reversed);
	//@打扮家 XRLightmass 临时修改 缩放UV 至 0-1范围
	FVector extent = CurrentBoxTop.GetExtent();
	float xMax = extent.X * 2;
	float yMax = extent.Y * 2;
	////////////////////////////顶面顶点//////////////////////////////////////////////
	for (int i = 0; i < PointsNum; i++)
	{
		CurrentVertex.Position = OutlinePoints_Reversed[i];
		//CurrentVertex.UV.Set(FMath::Abs(OutlinePoints_Reversed[i].Y - CurrentBoxTop.Min.Y) / yMax, FMath::Abs(OutlinePoints_Reversed[i].X - CurrentBoxTop.Min.X) / xMax);
		CurrentVertex.UV.Set(FMath::Abs(OutlinePoints_Reversed[i].Y - CurrentBoxTop.Min.Y) / InRepeatDistance, FMath::Abs(OutlinePoints_Reversed[i].X - CurrentBoxTop.Min.X) / InRepeatDistance);
		OutVertices.Push(CurrentVertex);
	}
	////////////////////////////底面顶点//////////////////////////////////////////////
	FBox CurrentBoxBottom(OutlinePoints);
	for (int i = 0; i < PointsNum; i++)
	{
		OutlinePoints[i].Z -= Thickness;
	}
	for (int i = 0; i < PointsNum; i++)
	{
		CurrentVertex.Position = OutlinePoints[i];
		CurrentVertex.UV.Set(FMath::Abs(OutlinePoints[i].Y - CurrentBoxBottom.Min.Y) / InRepeatDistance, FMath::Abs(OutlinePoints[i].X - CurrentBoxBottom.Min.X) / InRepeatDistance);
		OutVertices.Push(CurrentVertex);
	}
	////////////////////////////侧面顶点//////////////////////////////////////////////
	float LengthHorizontal = 0.0f;
	for (int i = 0, j = PointsNum - 1; i < PointsNum, j >= 0; i++, j--)
	{
		TArray<FVector> SidePoints;
		SidePoints.Push(OutlinePoints_Reversed[i]);
		SidePoints.Push(OutlinePoints[j]);
		SidePoints.Push(OutlinePoints[j == 0 ? PointsNum - 1 : j - 1]);
		SidePoints.Push(OutlinePoints_Reversed[i == PointsNum - 1 ? 0 : i + 1]);
		FBox SideBox(SidePoints);

		CurrentVertex.Position = OutlinePoints_Reversed[i];
		CurrentVertex.UV.Set(LengthHorizontal / InRepeatDistance, Thickness / InRepeatDistance);
		OutVertices.Push(CurrentVertex);

		CurrentVertex.Position = OutlinePoints[j];
		CurrentVertex.UV.Set(LengthHorizontal / InRepeatDistance, 0.0f);
		OutVertices.Push(CurrentVertex);

		LengthHorizontal += (OutlinePoints_Reversed[i == PointsNum - 1 ? 0 : i + 1] - OutlinePoints_Reversed[i]).Size();

		CurrentVertex.Position = OutlinePoints[j == 0 ? PointsNum - 1 : j - 1];
		CurrentVertex.UV.Set(LengthHorizontal / InRepeatDistance, 0.0f);
		OutVertices.Push(CurrentVertex);

		CurrentVertex.Position = OutlinePoints_Reversed[i == PointsNum - 1 ? 0 : i + 1];
		CurrentVertex.UV.Set(LengthHorizontal / InRepeatDistance, Thickness / InRepeatDistance);
		OutVertices.Push(CurrentVertex);
	}



	TArray<FVector> PolyTriangles;
	TArray<int32> IndexList;
	FArmyProcMeshTriangle TriangleIndices;
	////////////////////////////////顶面剖分//////////////////////////////////////////
	if (FArmyMath::TriangulatePoly(PolyTriangles, IndexList, OutlinePoints_Reversed, -FVector::UpVector, true))
	{
		for (int i = 0; i < IndexList.Num(); i += 3)
		{
			TriangleIndices.Vertex0 = IndexList[i];
			TriangleIndices.Vertex1 = IndexList[i + 1];
			TriangleIndices.Vertex2 = IndexList[i + 2];
			OutTriangles.Push(TriangleIndices);
		}
	}

	//////////////////////////////////底面剖分//////////////////////////////////////////
	if (FArmyMath::TriangulatePoly(PolyTriangles, IndexList, OutlinePoints, FVector::UpVector, true))
	{
		for (int i = 0; i < IndexList.Num(); i += 3)
		{
			TriangleIndices.Vertex0 = IndexList[i] + PointsNum;
			TriangleIndices.Vertex1 = IndexList[i + 1] + PointsNum;
			TriangleIndices.Vertex2 = IndexList[i + 2] + PointsNum;
			OutTriangles.Push(TriangleIndices);
		}
	}
	//////////////////////////////////侧面剖分//////////////////////////////////////////
	for (int i = 2 * PointsNum; i < OutVertices.Num(); i += 4)
	{
		TriangleIndices.Vertex0 = i;
		TriangleIndices.Vertex1 = i + 1;
		TriangleIndices.Vertex2 = i + 2;
		OutTriangles.Push(TriangleIndices);
		TriangleIndices.Vertex0 = i;
		TriangleIndices.Vertex1 = i + 2;
		TriangleIndices.Vertex2 = i + 3;
		OutTriangles.Push(TriangleIndices);
	}
}

void UXRProceduralMeshComponent::RecomputeTangents(TArray<FArmyProcMeshVertex>& InVertices, TArray<FArmyProcMeshTriangle>& InTriangles)
{
	for (int32 TriIdx = 0; TriIdx < InTriangles.Num(); TriIdx++)
	{
		FArmyProcMeshTriangle &Triangle = InTriangles[TriIdx];
		const FVector &v0 = InVertices[Triangle.Vertex0].Position;
		const FVector &v1 = InVertices[Triangle.Vertex1].Position;
		const FVector &v2 = InVertices[Triangle.Vertex2].Position;

		const FVector Edge01 = v1 - v0;
		const FVector Edge02 = v2 - v0;

		FVector TangentX = Edge01.GetSafeNormal();
		FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
		FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

		TangentX.Normalize();
		TangentZ.Normalize();
		TangentY.Normalize();

		if (InVertices.IsValidIndex(Triangle.Vertex0))
		{
			InVertices[Triangle.Vertex0].TangentX += TangentX;
			InVertices[Triangle.Vertex1].TangentX += TangentX;
			InVertices[Triangle.Vertex2].TangentX += TangentX;

			InVertices[Triangle.Vertex0].TangentY += TangentY;
			InVertices[Triangle.Vertex1].TangentY += TangentY;
			InVertices[Triangle.Vertex2].TangentY += TangentY;

			InVertices[Triangle.Vertex0].TangentZ += TangentZ;
			InVertices[Triangle.Vertex1].TangentZ += TangentZ;
			InVertices[Triangle.Vertex2].TangentZ += TangentZ;

			InVertices[Triangle.Vertex0].TangentX.Normalize();
			InVertices[Triangle.Vertex1].TangentX.Normalize();
			InVertices[Triangle.Vertex2].TangentX.Normalize();

			InVertices[Triangle.Vertex0].TangentY.Normalize();
			InVertices[Triangle.Vertex1].TangentY.Normalize();
			InVertices[Triangle.Vertex2].TangentY.Normalize();

			InVertices[Triangle.Vertex0].TangentZ.Normalize();
			InVertices[Triangle.Vertex1].TangentZ.Normalize();
			InVertices[Triangle.Vertex2].TangentZ.Normalize();
		}
	}
}

void UXRProceduralMeshComponent::CreateMeshSection_Extrude(int32 SectionIndex, const TArray<FVector>& InOutlineVertices, float InTickness)
{
	TArray<FArmyProcMeshVertex> SectionVertices;
	TArray<FArmyProcMeshTriangle> SectionTriangles;

	Triangulate_Extrusion(InOutlineVertices, InTickness, SectionVertices, SectionTriangles);

	RecomputeTangents(SectionVertices, SectionTriangles);

	// Ensure sections array is long enough
	if (SectionIndex >= ProcMeshSections.Num())
	{
		ProcMeshSections.SetNum(SectionIndex + 1, false);
	}

	// Reset this section (in case it already existed)
	FArmyProcMeshSection& NewSection = ProcMeshSections[SectionIndex];
	NewSection.Reset();

	// Copy data to vertex buffer
	const int32 NumVerts = SectionVertices.Num();
	NewSection.ProcVertexBuffer.Reset();
	NewSection.ProcVertexBuffer.AddUninitialized(NumVerts);
	for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
	{
		NewSection.ProcVertexBuffer[VertIdx] = SectionVertices[VertIdx];
		// Update bounding box
		NewSection.SectionLocalBox += SectionVertices[VertIdx].Position;
	}

	// Copy index buffer (clamping to vertex range)
	int32 NumTriIndices = SectionTriangles.Num() * 3;
	//NumTriIndices = (NumTriIndices / 3) * 3; // Ensure we have exact number of triangles (array is multiple of 3 long)

	NewSection.ProcIndexBuffer.Reset();
	NewSection.ProcIndexBuffer.AddUninitialized(SectionTriangles.Num() * 3);
	for (int32 TriangleIdx = 0; TriangleIdx < SectionTriangles.Num(); TriangleIdx++)
	{
		NewSection.ProcIndexBuffer[TriangleIdx * 3] = SectionTriangles[TriangleIdx].Vertex0;
		NewSection.ProcIndexBuffer[TriangleIdx * 3 + 1] = SectionTriangles[TriangleIdx].Vertex1;
		NewSection.ProcIndexBuffer[TriangleIdx * 3 + 2] = SectionTriangles[TriangleIdx].Vertex2;
	}

	NewSection.bEnableCollision = true;

	InvalidateLightingCache();

	UpdateLocalBounds(); // Update overall bounds
	if (!BSkitLine)
		UpdateCollision(); // Mark collision as dirty
	MarkRenderStateDirty(); // New section requires recreating scene proxy
}

void UXRProceduralMeshComponent::CreateMeshSection(int32 SectionIndex, TArray<FArmyProcMeshVertex>& InVertices, TArray<FArmyProcMeshTriangle>& InTriangles, bool bRecomputeTangets, bool bRemainLastMaterial)
{
	//SCOPE_CYCLE_COUNTER(STAT_XRProcMesh_CreateMeshSection);

	UMaterialInterface* SectionMaterialLast = bRemainLastMaterial ? GetMaterial(SectionIndex) : NULL;

	if (bRecomputeTangets)
	{
		RecomputeTangents(InVertices, InTriangles);
	}

	// Ensure sections array is long enough
	if (SectionIndex >= ProcMeshSections.Num())
	{
		ProcMeshSections.SetNum(SectionIndex + 1, false);
	}

	// Reset this section (in case it already existed)
	FArmyProcMeshSection& NewSection = ProcMeshSections[SectionIndex];
	NewSection.Reset();

	//拷贝顶点缓存
	NewSection.ProcVertexBuffer = InVertices;

	//创建包围盒
	for (auto& It : InVertices)
	{
		NewSection.SectionLocalBox += It.Position;
	}

	//拷贝索引缓存
	NewSection.ProcIndexBuffer.Reset();
	NewSection.ProcIndexBuffer.AddUninitialized(InTriangles.Num() * 3);
	for (int32 TriangleIdx = 0; TriangleIdx < InTriangles.Num(); TriangleIdx++)
	{
		NewSection.ProcIndexBuffer[TriangleIdx * 3] = InTriangles[TriangleIdx].Vertex0;
		NewSection.ProcIndexBuffer[TriangleIdx * 3 + 1] = InTriangles[TriangleIdx].Vertex1;
		NewSection.ProcIndexBuffer[TriangleIdx * 3 + 2] = InTriangles[TriangleIdx].Vertex2;
	}

	NewSection.bEnableCollision = true;

	SetMaterial(SectionIndex, SectionMaterialLast);

	InvalidateLightingCache();

	UpdateLocalBounds(); // Update overall bounds
	if (!BSkitLine)
		UpdateCollision(); // Mark collision as dirty
	MarkRenderStateDirty(); // New section requires recreating scene proxy
}

//void UXRProceduralMeshComponent::CreateMeshSection_LinearColor(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FLinearColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents, bool bCreateCollision)
//{
//	// Convert FLinearColors to FColors
//	TArray<FColor> Colors;
//	if (VertexColors.Num() > 0)
//	{
//		Colors.SetNum(VertexColors.Num());
//
//		for (int32 ColorIdx = 0; ColorIdx < VertexColors.Num(); ColorIdx++)
//		{
//			Colors[ColorIdx] = VertexColors[ColorIdx].ToFColor(false);
//		}
//	}
//
//	CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UV0, Colors, Tangents, bCreateCollision);
//}

//void UXRProceduralMeshComponent::CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents, bool bCreateCollision)
//{
//	//SCOPE_CYCLE_COUNTER(STAT_XRProcMesh_CreateMeshSection);
//
//	// Ensure sections array is long enough
//	if (SectionIndex >= ProcMeshSections.Num())
//	{
//		ProcMeshSections.SetNum(SectionIndex + 1, false);
//	}
//
//	// Reset this section (in case it already existed)
//	FArmyProcMeshSection& NewSection = ProcMeshSections[SectionIndex];
//	NewSection.Reset();
//
//	// Copy data to vertex buffer
//	const int32 NumVerts = Vertices.Num();
//	NewSection.ProcVertexBuffer.Reset();
//	NewSection.ProcVertexBuffer.AddUninitialized(NumVerts);
//	for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
//	{
//		FArmyProcMeshVertex& Vertex = NewSection.ProcVertexBuffer[VertIdx];
//
//		Vertex.Position = Vertices[VertIdx];
//		Vertex.TangentZ = (Normals.Num() == NumVerts) ? Normals[VertIdx] : FVector(0.f, 0.f, 1.f);
//		Vertex.UV = (UV.Num() == NumVerts) ? UV[VertIdx] : FVector2D(0.f, 0.f);
//		Vertex.Color = (VertexColors.Num() == NumVerts) ? VertexColors[VertIdx] : FColor(255, 255, 255);
//		Vertex.TangentX = (Tangents.Num() == NumVerts) ? Tangents[VertIdx].TangentX : FVector(0,0,0);
//
//		// Update bounding box
//		NewSection.SectionLocalBox += Vertex.Position;
//	}
//
//	// Copy index buffer (clamping to vertex range)
//	int32 NumTriIndices = Triangles.Num();
//	NumTriIndices = (NumTriIndices/3) * 3; // Ensure we have exact number of triangles (array is multiple of 3 long)
//
//	NewSection.ProcIndexBuffer.Reset();
//	NewSection.ProcIndexBuffer.AddUninitialized(NumTriIndices);
//	for (int32 IndexIdx = 0; IndexIdx < NumTriIndices; IndexIdx++)
//	{
//		NewSection.ProcIndexBuffer[IndexIdx] = FMath::Min(Triangles[IndexIdx], NumVerts - 1);
//	}
//
//	NewSection.bEnableCollision = bCreateCollision;
//
//	UpdateLocalBounds(); // Update overall bounds
//	UpdateCollision(); // Mark collision as dirty
//	MarkRenderStateDirty(); // New section requires recreating scene proxy
//}

//void UXRProceduralMeshComponent::UpdateMeshSection_LinearColor(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FLinearColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents)
//{
//	// Convert FLinearColors to FColors
//	TArray<FColor> Colors;
//	if (VertexColors.Num() > 0)
//	{
//		Colors.SetNum(VertexColors.Num());
//
//		for (int32 ColorIdx = 0; ColorIdx < VertexColors.Num(); ColorIdx++)
//		{
//			Colors[ColorIdx] = VertexColors[ColorIdx].ToFColor(true);
//		}
//	}
//
//	UpdateMeshSection(SectionIndex, Vertices, Normals, UV0, Colors, Tangents);
//}

//void UXRProceduralMeshComponent::UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents)
//{
//	//SCOPE_CYCLE_COUNTER(STAT_XRProcMesh_UpdateSectionGT);
//
//	if(SectionIndex < ProcMeshSections.Num())
//	{
//		FArmyProcMeshSection& Section = ProcMeshSections[SectionIndex];
//		const int32 NumVerts = Section.ProcVertexBuffer.Num();
//
//		// See if positions are changing
//		const bool bPositionsChanging = (Vertices.Num() == NumVerts);
//
//		// Update bounds, if we are getting new position data
//		if (bPositionsChanging)
//		{
//			Section.SectionLocalBox.Init();
//		}
//
//		// Iterate through vertex data, copying in new info
//		for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
//		{
//			FArmyProcMeshVertex& ModifyVert = Section.ProcVertexBuffer[VertIdx];
//
//			// Position data
//			if (Vertices.Num() == NumVerts)
//			{
//				ModifyVert.Position = Vertices[VertIdx];
//				Section.SectionLocalBox += ModifyVert.Position;
//			}
//
//			// Normal data
//			if (Normals.Num() == NumVerts)
//			{
//				ModifyVert.TangentZ = Normals[VertIdx];
//			}
//
//			// Tangent data 
//			if (Tangents.Num() == NumVerts)
//			{
//				ModifyVert.TangentX = Tangents[VertIdx].TangentX;
//			}
//
//			// UV data
//			if (UV.Num() == NumVerts)
//			{
//				ModifyVert.UV = UV[VertIdx];
//			}
//
//			// Color data
//			if (VertexColors.Num() == NumVerts)
//			{
//				ModifyVert.Color = VertexColors[VertIdx];
//			}
//		}
//
//		if (SceneProxy)
//		{
//			// Create data to update section
//			FArmyProcMeshSectionUpdateData* SectionData = new FArmyProcMeshSectionUpdateData;
//			SectionData->TargetSection = SectionIndex;
//			SectionData->NewVertexBuffer = Section.ProcVertexBuffer;
//
//			// Enqueue command to send to render thread
//			ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
//				FProcMeshSectionUpdate,
//				FArmyProceduralMeshSceneProxy*, ProcMeshSceneProxy, (FArmyProceduralMeshSceneProxy*)SceneProxy,
//				FArmyProcMeshSectionUpdateData*, SectionData, SectionData,
//				{
//					ProcMeshSceneProxy->UpdateSection_RenderThread(SectionData);
//				}
//			);
//		}
//
//		// If we have collision enabled on this section, update that too
//		if(bPositionsChanging && Section.bEnableCollision)
//		{
//			TArray<FVector> CollisionPositions;
//
//			// We have one collision mesh for all sections, so need to build array of _all_ positions
//			for (const FArmyProcMeshSection& CollisionSection : ProcMeshSections)
//			{
//				// If section has collision, copy it
//				if (CollisionSection.bEnableCollision)
//				{
//					for (int32 VertIdx = 0; VertIdx < CollisionSection.ProcVertexBuffer.Num(); VertIdx++)
//					{
//						CollisionPositions.Add(CollisionSection.ProcVertexBuffer[VertIdx].Position);
//					}
//				}
//			}
//
//			// Pass new positions to trimesh
//			BodyInstance.UpdateTriMeshVertices(CollisionPositions);
//		}
//
//		if (bPositionsChanging)
//		{
//			UpdateLocalBounds(); // Update overall bounds
//			MarkRenderTransformDirty(); // Need to send new bounds to render thread
//		}
//	}
//}

void UXRProceduralMeshComponent::ClearMeshSection(int32 SectionIndex)
{
	if (SectionIndex < ProcMeshSections.Num())
	{
		ProcMeshSections[SectionIndex].Reset();
		UpdateLocalBounds();
		UpdateCollision();
		MarkRenderStateDirty();
	}
}

void UXRProceduralMeshComponent::ClearAllMeshSections()
{
	ProcMeshSections.Empty();
	UpdateLocalBounds();
	UpdateCollision();
	MarkRenderStateDirty();
}

void UXRProceduralMeshComponent::SetMeshSectionVisible(int32 SectionIndex, bool bNewVisibility)
{
	if(SectionIndex < ProcMeshSections.Num())
	{
		// Set game thread state
		ProcMeshSections[SectionIndex].bSectionVisible = bNewVisibility;

		if (SceneProxy)
		{
			// Enqueue command to modify render thread info
			ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
				FProcMeshSectionVisibilityUpdate,
				FArmyProceduralMeshSceneProxy*, ProcMeshSceneProxy, (FArmyProceduralMeshSceneProxy*)SceneProxy,
				int32, SectionIndex, SectionIndex,
				bool, bNewVisibility, bNewVisibility,
				{
					ProcMeshSceneProxy->SetSectionVisibility_RenderThread(SectionIndex, bNewVisibility);
				}
			);
		}
	}
}

bool UXRProceduralMeshComponent::IsMeshSectionVisible(int32 SectionIndex) const
{
	return (SectionIndex < ProcMeshSections.Num()) ? ProcMeshSections[SectionIndex].bSectionVisible : false;
}

int32 UXRProceduralMeshComponent::GetNumSections() const
{
	return ProcMeshSections.Num();
}

void UXRProceduralMeshComponent::AddCollisionConvexMesh(TArray<FVector> ConvexVerts)
{
	if(ConvexVerts.Num() >= 4)
	{ 
		// New element
		FKConvexElem NewConvexElem;
		// Copy in vertex info
		NewConvexElem.VertexData = ConvexVerts;
		// Update bounding box
		NewConvexElem.ElemBox = FBox(NewConvexElem.VertexData);
		// Add to array of convex elements
		CollisionConvexElems.Add(NewConvexElem);
		// Refresh collision
		UpdateCollision();
	}
}

void UXRProceduralMeshComponent::ClearCollisionConvexMeshes()
{
	// Empty simple collision info
	CollisionConvexElems.Empty();
	// Refresh collision
	UpdateCollision();
}

void UXRProceduralMeshComponent::SetCollisionConvexMeshes(const TArray< TArray<FVector> >& ConvexMeshes)
{
	CollisionConvexElems.Reset();

	// Create element for each convex mesh
	for (int32 ConvexIndex = 0; ConvexIndex < ConvexMeshes.Num(); ConvexIndex++)
	{
		FKConvexElem NewConvexElem;
		NewConvexElem.VertexData = ConvexMeshes[ConvexIndex];
		NewConvexElem.ElemBox = FBox(NewConvexElem.VertexData);

		CollisionConvexElems.Add(NewConvexElem);
	}

	UpdateCollision();
}

void UXRProceduralMeshComponent::UpdateLocalBounds()
{
	FBox LocalBox(ForceInit);

	for (const FArmyProcMeshSection& Section : ProcMeshSections)
	{
		LocalBox += Section.SectionLocalBox;
	}

	LocalBounds = LocalBox.IsValid ? FBoxSphereBounds(LocalBox) : FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0); // fallback to reset box sphere bounds

	// Update global bounds
	UpdateBounds();
	// Need to send to render thread
	MarkRenderTransformDirty();
}

int32 UXRProceduralMeshComponent::GetNumMaterials() const
{
	return ProcMeshSections.Num();
}

FArmyProcMeshSection* UXRProceduralMeshComponent::GetProcMeshSection(int32 SectionIndex)
{
	if (SectionIndex < ProcMeshSections.Num())
	{
		return &ProcMeshSections[SectionIndex];
	}
	else
	{
		return nullptr;
	}
}

void UXRProceduralMeshComponent::SetProcMeshSection(int32 SectionIndex, const FArmyProcMeshSection& Section)
{
	// Ensure sections array is long enough
	if (SectionIndex >= ProcMeshSections.Num())
	{
		ProcMeshSections.SetNum(SectionIndex + 1, false);
	}

	ProcMeshSections[SectionIndex] = Section;

	UpdateLocalBounds(); // Update overall bounds
	UpdateCollision(); // Mark collision as dirty
	MarkRenderStateDirty(); // New section requires recreating scene proxy
}

FBoxSphereBounds UXRProceduralMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

bool UXRProceduralMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	int32 VertexBase = 0; // Base vertex index for current section

	// See if we should copy UVs
	bool bCopyUVs = UPhysicsSettings::Get()->bSupportUVFromHitResults; 
	if (bCopyUVs)
	{
		CollisionData->UVs.AddZeroed(1); // only one UV channel
	}

	// For each section..
	for (int32 SectionIdx = 0; SectionIdx < ProcMeshSections.Num(); SectionIdx++)
	{
		FArmyProcMeshSection& Section = ProcMeshSections[SectionIdx];
		// Do we have collision enabled?
		if (Section.bEnableCollision)
		{
			// Copy vert data
			for (int32 VertIdx = 0; VertIdx < Section.ProcVertexBuffer.Num(); VertIdx++)
			{
				CollisionData->Vertices.Add(Section.ProcVertexBuffer[VertIdx].Position);

				// Copy UV if desired
				if (bCopyUVs)
				{
					CollisionData->UVs[0].Add(Section.ProcVertexBuffer[VertIdx].UV);
				}
			}

			// Copy triangle data
			const int32 NumTriangles = Section.ProcIndexBuffer.Num() / 3;
			for (int32 TriIdx = 0; TriIdx < NumTriangles; TriIdx++)
			{
				// Need to add base offset for indices
				FTriIndices Triangle;
				Triangle.v0 = Section.ProcIndexBuffer[(TriIdx * 3) + 0] + VertexBase;
				Triangle.v1 = Section.ProcIndexBuffer[(TriIdx * 3) + 1] + VertexBase;
				Triangle.v2 = Section.ProcIndexBuffer[(TriIdx * 3) + 2] + VertexBase;
				CollisionData->Indices.Add(Triangle);

				// Also store material info
				CollisionData->MaterialIndices.Add(SectionIdx);
			}

			// Remember the base index that new verts will be added from in next section
			VertexBase = CollisionData->Vertices.Num();
		}
	}

	CollisionData->bFlipNormals = true;
	CollisionData->bDeformableMesh = true;
	CollisionData->bFastCook = true;

	return true;
}

bool UXRProceduralMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	for (const FArmyProcMeshSection& Section : ProcMeshSections)
	{
		if (Section.ProcIndexBuffer.Num() >= 3 && Section.bEnableCollision)
		{
			return true;
		}
	}

	return false;
}

UBodySetup* UXRProceduralMeshComponent::CreateBodySetupHelper()
{
	// The body setup in a template needs to be public since the property is Tnstanced and thus is the archetype of the instance meaning there is a direct reference
	UBodySetup* NewBodySetup = NewObject<UBodySetup>(this, NAME_None, (IsTemplate() ? RF_Public : RF_NoFlags));
	NewBodySetup->BodySetupGuid = FGuid::NewGuid();

	NewBodySetup->bGenerateMirroredCollision = false;
	NewBodySetup->bDoubleSidedGeometry = false;
	NewBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;
	NewBodySetup->bMeshCollideAll = true;
	SetCollisionObjectType(ECC_VRSSM);

	return NewBodySetup;
}

void UXRProceduralMeshComponent::CreateProcMeshBodySetup()
{
	if (ProcMeshBodySetup == nullptr)
	{
		ProcMeshBodySetup = CreateBodySetupHelper();
	}
}

void UXRProceduralMeshComponent::UpdateCollision()
{
	//SCOPE_CYCLE_COUNTER(STAT_XRProcMesh_UpdateCollision);

	UWorld* World = GetWorld();
	const bool bUseAsyncCook = World && World->IsGameWorld() && bUseAsyncCooking;

	if(bUseAsyncCook)
	{
		AsyncBodySetupQueue.Add(CreateBodySetupHelper());
	}
	else
	{
		AsyncBodySetupQueue.Empty();	//If for some reason we modified the async at runtime, just clear any pending async body setups
		CreateProcMeshBodySetup();
	}
	
	UBodySetup* UseBodySetup = bUseAsyncCook ? AsyncBodySetupQueue.Last() : ProcMeshBodySetup;

	// Fill in simple collision convex elements
	UseBodySetup->AggGeom.ConvexElems = CollisionConvexElems;

	// Set trace flag
	UseBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;

	if(bUseAsyncCook)
	{
		UseBodySetup->CreatePhysicsMeshesAsync(FOnAsyncPhysicsCookFinished::CreateUObject(this, &UXRProceduralMeshComponent::FinishPhysicsAsyncCook, UseBodySetup));
	}
	else
	{
		// New GUID as collision has changed
		UseBodySetup->BodySetupGuid = FGuid::NewGuid();
		// Also we want cooked data for this
		UseBodySetup->bHasCookedCollisionData = true;
		UseBodySetup->InvalidatePhysicsData();
		UseBodySetup->CreatePhysicsMeshes();
		RecreatePhysicsState();
	}
}

void UXRProceduralMeshComponent::FinishPhysicsAsyncCook(UBodySetup* FinishedBodySetup)
{
	TArray<UBodySetup*> NewQueue;
	NewQueue.Reserve(AsyncBodySetupQueue.Num());

	int32 FoundIdx;
	if(AsyncBodySetupQueue.Find(FinishedBodySetup, FoundIdx))
	{
		//The new body was found in the array meaning it's newer so use it
		ProcMeshBodySetup = FinishedBodySetup;
		RecreatePhysicsState();

		//remove any async body setups that were requested before this one
		for(int32 AsyncIdx = FoundIdx+1; AsyncIdx < AsyncBodySetupQueue.Num(); ++AsyncIdx)
		{
			NewQueue.Add(AsyncBodySetupQueue[AsyncIdx]);
		}

		AsyncBodySetupQueue = NewQueue;
	}
}

UBodySetup* UXRProceduralMeshComponent::GetBodySetup()
{
	CreateProcMeshBodySetup();
	return ProcMeshBodySetup;
}

UMaterialInterface* UXRProceduralMeshComponent::GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const
{
	UMaterialInterface* Result = nullptr;
	SectionIndex = 0;

	if (FaceIndex >= 0)
	{
		// Look for element that corresponds to the supplied face
		int32 TotalFaceCount = 0;
		for (int32 SectionIdx = 0; SectionIdx < ProcMeshSections.Num(); SectionIdx++)
		{
			const FArmyProcMeshSection& Section = ProcMeshSections[SectionIdx];
			int32 NumFaces = Section.ProcIndexBuffer.Num() / 3;
			TotalFaceCount += NumFaces;

			if (FaceIndex < TotalFaceCount)
			{
				// Grab the material
				Result = GetMaterial(SectionIdx);
				SectionIndex = SectionIdx;
				break;
			}
		}
	}

	return Result;
}