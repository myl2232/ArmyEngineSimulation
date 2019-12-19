#include "ArmyExtrusionMeshComponent.h"

#include "RenderResource.h"
#include "LocalVertexFactory.h"
#include "PrimitiveSceneProxy.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/CollisionProfile.h"
#include "Materials/Material.h"
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "Math/ArmyMath.h"
#include "Math/TransformVectorized.h"
#include "GameFramework/PhysicsVolume.h"

class FMeshVertexBuffer : public FVertexBuffer {
public:
	TArray<FDynamicMeshVertex> Vertices;

	// FRenderResource Method
	virtual void InitRHI() override {
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo);
		void *VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

class FMeshIndexBuffer : public FIndexBuffer {
public:
	TArray<int16> Indices;

	// FRenderResource Method
	virtual void InitRHI() override {
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int16), Indices.Num() * sizeof(int16), BUF_Static, CreateInfo);
		void *IndexBufferData = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int16), RLM_WriteOnly);
		FMemory::Memcpy(IndexBufferData, Indices.GetData(), Indices.Num() * sizeof(int16));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

class FMeshVertexFactory : public FLocalVertexFactory {
public:
	FMeshVertexFactory() {}

	void Init(const FMeshVertexBuffer *VertexBuffer) {
		//check(!IsInRenderingThread());

		// Enqueue a command to rendering thread ?
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitGeneratedMeshVertexFactory,
			FMeshVertexFactory*, VertexFactory, this,
			const FMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
				FDataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TextureCoordinate, VET_Float2)
				);
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				VertexFactory->SetData(NewData);
			});
	}
};

class FMeshSceneProxy : public FPrimitiveSceneProxy {
public:
	FMeshSceneProxy(UArmyExtrusionMeshComponent *Component)
		: FPrimitiveSceneProxy(Component),
		MaterialRelevance(Component->GetMaterialRelevance(ERHIFeatureLevel::SM4)) {

		TArray<FMeshVertex> &Verts = Component->GeneratedMeshVerts;
		TArray<FMeshTriangle> &Triangles = Component->GeneratedMeshTriangles;

		VertexBuffer.Vertices.SetNumUninitialized(Verts.Num());
		FDynamicMeshVertex *DynVerts = VertexBuffer.Vertices.GetData();
		IndexBuffer.Indices.SetNumUninitialized(Triangles.Num() * 3);


		struct FTempTangenetBasis {
			FVector TangentX;
			FVector TangentY;
			FVector TangentZ;
		};
		TArray<FTempTangenetBasis> TempTangentBasis;
		TempTangentBasis.SetNumUninitialized(Verts.Num());

		FTempTangenetBasis *Basis = TempTangentBasis.GetData();
		for (int32 VertIdx = 0; VertIdx < Verts.Num(); ++VertIdx) {
			Basis[VertIdx].TangentX = FVector::ZeroVector;
			Basis[VertIdx].TangentY = FVector::ZeroVector;
			Basis[VertIdx].TangentZ = FVector::ZeroVector;
		}

		int32 NumTri = Triangles.Num();
		for (int32 TriIdx = 0; TriIdx < NumTri; ++TriIdx) {
			FMeshTriangle &Triangle = Triangles[TriIdx];
			const FVector &v0 = Verts[Triangle.Vertex0].Position;
			const FVector &v1 = Verts[Triangle.Vertex1].Position;
			const FVector &v2 = Verts[Triangle.Vertex2].Position;

			const FVector Edge01 = v1 - v0;
			const FVector Edge02 = v2 - v0;

			const FVector TangentX = Edge01.GetSafeNormal();
			const FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
			const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

			DynVerts[Triangle.Vertex0].Position = v0;
			DynVerts[Triangle.Vertex1].Position = v1;
			DynVerts[Triangle.Vertex2].Position = v2;

			DynVerts[Triangle.Vertex0].Color = Verts[Triangle.Vertex0].Color;
			DynVerts[Triangle.Vertex1].Color = Verts[Triangle.Vertex1].Color;
			DynVerts[Triangle.Vertex2].Color = Verts[Triangle.Vertex2].Color;

			DynVerts[Triangle.Vertex0].TextureCoordinate = Verts[Triangle.Vertex0].UV;
			DynVerts[Triangle.Vertex1].TextureCoordinate = Verts[Triangle.Vertex1].UV;
			DynVerts[Triangle.Vertex2].TextureCoordinate = Verts[Triangle.Vertex2].UV;

			Basis[Triangle.Vertex0].TangentX += TangentX;
			Basis[Triangle.Vertex1].TangentX += TangentX;
			Basis[Triangle.Vertex2].TangentX += TangentX;

			Basis[Triangle.Vertex0].TangentY += TangentY;
			Basis[Triangle.Vertex1].TangentY += TangentY;
			Basis[Triangle.Vertex2].TangentY += TangentY;

			Basis[Triangle.Vertex0].TangentZ += TangentZ;
			Basis[Triangle.Vertex1].TangentZ += TangentZ;
			Basis[Triangle.Vertex2].TangentZ += TangentZ;

			IndexBuffer.Indices[TriIdx * 3] = Triangle.Vertex0;
			IndexBuffer.Indices[TriIdx * 3 + 1] = Triangle.Vertex1;
			IndexBuffer.Indices[TriIdx * 3 + 2] = Triangle.Vertex2;
		}

		for (int32 VertIdx = 0; VertIdx < Verts.Num(); ++VertIdx) {
			Basis[VertIdx].TangentX.Normalize();
			Basis[VertIdx].TangentY.Normalize();
			Basis[VertIdx].TangentZ.Normalize();
			DynVerts[VertIdx].SetTangents(Basis[VertIdx].TangentX, Basis[VertIdx].TangentY, Basis[VertIdx].TangentZ);
		}




		VertexFactory.Init(&VertexBuffer);

		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		Material = Component->GetMaterial(0);
		if (Material == nullptr)
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
	}

	virtual ~FMeshSceneProxy() {
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override {

		QUICK_SCOPE_CYCLE_COUNTER(STAT_GenMeshSceneProxy_GetDynamicMeshElements);
		checkSlow(IsInRenderingThread());

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : nullptr,
			FLinearColor(0, 0.5f, 1.f)
		);
		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy *MaterialProxy = bWireframe ? WireframeMaterialInstance : Material->GetRenderProxy(IsSelected());

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex) {
			const FSceneView* View = Views[ViewIndex];
			if (IsShown(View) && (VisibilityMap & (1 << ViewIndex))) {
				FMeshBatch &Mesh = Collector.AllocateMesh();
				FMeshBatchElement &BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = GetStaticDepthPriorityGroup();
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
		FPrimitiveViewRelevance Result;
		// If primitive is shown in view
		Result.bDrawRelevance = IsShown(View);
		// If primitive can cast shadow
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override {
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const override {
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

private:
	FMeshVertexBuffer VertexBuffer;
	FMeshIndexBuffer IndexBuffer;
	FMeshVertexFactory VertexFactory;

	UMaterialInterface *Material;
	FMaterialRelevance MaterialRelevance;
};


UArmyExtrusionMeshComponent::UArmyExtrusionMeshComponent(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
}

bool UArmyExtrusionMeshComponent::SetGeneratedMeshGeomInfo(const TArray<FMeshVertex> &Vertices, const TArray<FMeshTriangle> &Triangles) {
	GeneratedMeshVerts = Vertices;
	GeneratedMeshTriangles = Triangles;
	UpdateCollision();
	MarkRenderStateDirty();
	return true;
}

int32 UArmyExtrusionMeshComponent::GetNumMaterials() const { return 1; }

bool UArmyExtrusionMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) {
	TArray<FMeshVertex> &Verts = GeneratedMeshVerts;
	TArray<FMeshTriangle> &Triangles = GeneratedMeshTriangles;

	for (int32 i = 0; i < Verts.Num(); ++i)
		CollisionData->Vertices.Add(Verts[i].Position);

	FTriIndices TriIndices;
	int32 NumTri = Triangles.Num();
	for (int32 i = 0; i < NumTri; ++i) {
		FMeshTriangle &Triangle = Triangles[i];
		TriIndices.v0 = Triangle.Vertex0;
		TriIndices.v1 = Triangle.Vertex1;
		TriIndices.v2 = Triangle.Vertex2;
		CollisionData->Indices.Add(TriIndices);
		CollisionData->MaterialIndices.Add(i);
	}

	CollisionData->bFlipNormals = true;

	return true;

}

bool UArmyExtrusionMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const {
	return GeneratedMeshTriangles.Num() > 0;
}

FPrimitiveSceneProxy* UArmyExtrusionMeshComponent::CreateSceneProxy() {
	FPrimitiveSceneProxy *Proxy = nullptr;
	if (GeneratedMeshTriangles.Num() > 0)
		Proxy = new FMeshSceneProxy(this);
	return Proxy;
}

UBodySetup* UArmyExtrusionMeshComponent::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}

bool UArmyExtrusionMeshComponent::ShouldRenderSelected() const {
	AActor *Owner = GetOwner();
	if (Owner->IsSelected())
		return true;

	/*AArmyExtrusionActor *ShapeActor = Cast<AArmyExtrusionActor>(Owner);
	while (ShapeActor) {
		if (ShapeActor->IsSelected())
			return true;
		ShapeActor = ShapeActor->GetActorAttachedTo();
	}*/
	return false;
}

void UArmyExtrusionMeshComponent::UpdateBodySetup() {
	if (ModelBodySetup == nullptr) {
		ModelBodySetup = NewObject<UBodySetup>(this, UBodySetup::StaticClass(),TEXT("UArmyExtrusionMeshComponent"));
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UArmyExtrusionMeshComponent::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();


		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();

		CreatePhysicsState();
	}
}

FBoxSphereBounds UArmyExtrusionMeshComponent::CalcBounds(const FTransform& LocalToWorld) const {
	if (GeneratedMeshTriangles.Num() > 0) {
		const TArray<FMeshVertex> &Verts = GeneratedMeshVerts;
		const TArray<FMeshTriangle> &Triangles = GeneratedMeshTriangles;

		FVector vecMin = Verts[Triangles[0].Vertex0].Position;
		FVector vecMax = Verts[Triangles[0].Vertex0].Position;
		for (int32 TriIdx = 1; TriIdx < Triangles.Num(); ++TriIdx) {

			vecMin.X = (vecMin.X > Verts[Triangles[TriIdx].Vertex0].Position.X) ? Verts[Triangles[TriIdx].Vertex0].Position.X : vecMin.X;
			vecMin.X = (vecMin.X > Verts[Triangles[TriIdx].Vertex1].Position.X) ? Verts[Triangles[TriIdx].Vertex1].Position.X : vecMin.X;
			vecMin.X = (vecMin.X > Verts[Triangles[TriIdx].Vertex2].Position.X) ? Verts[Triangles[TriIdx].Vertex2].Position.X : vecMin.X;


			vecMin.Y = (vecMin.Y > Verts[Triangles[TriIdx].Vertex0].Position.Y) ? Verts[Triangles[TriIdx].Vertex0].Position.Y : vecMin.Y;
			vecMin.Y = (vecMin.Y > Verts[Triangles[TriIdx].Vertex1].Position.Y) ? Verts[Triangles[TriIdx].Vertex1].Position.Y : vecMin.Y;
			vecMin.Y = (vecMin.Y > Verts[Triangles[TriIdx].Vertex2].Position.Y) ? Verts[Triangles[TriIdx].Vertex2].Position.Y : vecMin.Y;



			vecMin.Z = (vecMin.Z > Verts[Triangles[TriIdx].Vertex0].Position.Z) ? Verts[Triangles[TriIdx].Vertex0].Position.Z : vecMin.Z;
			vecMin.Z = (vecMin.Z > Verts[Triangles[TriIdx].Vertex1].Position.Z) ? Verts[Triangles[TriIdx].Vertex1].Position.Z : vecMin.Z;
			vecMin.Z = (vecMin.Z > Verts[Triangles[TriIdx].Vertex2].Position.Z) ? Verts[Triangles[TriIdx].Vertex2].Position.Z : vecMin.Z;



			vecMax.X = (vecMax.X < Verts[Triangles[TriIdx].Vertex0].Position.X) ? Verts[Triangles[TriIdx].Vertex0].Position.X : vecMax.X;
			vecMax.X = (vecMax.X < Verts[Triangles[TriIdx].Vertex1].Position.X) ? Verts[Triangles[TriIdx].Vertex1].Position.X : vecMax.X;
			vecMax.X = (vecMax.X < Verts[Triangles[TriIdx].Vertex2].Position.X) ? Verts[Triangles[TriIdx].Vertex2].Position.X : vecMax.X;



			vecMax.Y = (vecMax.Y < Verts[Triangles[TriIdx].Vertex0].Position.Y) ? Verts[Triangles[TriIdx].Vertex0].Position.Y : vecMax.Y;
			vecMax.Y = (vecMax.Y < Verts[Triangles[TriIdx].Vertex1].Position.Y) ? Verts[Triangles[TriIdx].Vertex1].Position.Y : vecMax.Y;
			vecMax.Y = (vecMax.Y < Verts[Triangles[TriIdx].Vertex2].Position.Y) ? Verts[Triangles[TriIdx].Vertex2].Position.Y : vecMax.Y;



			vecMax.Z = (vecMax.Z < Verts[Triangles[TriIdx].Vertex0].Position.Z) ? Verts[Triangles[TriIdx].Vertex0].Position.Z : vecMax.Z;
			vecMax.Z = (vecMax.Z < Verts[Triangles[TriIdx].Vertex1].Position.Z) ? Verts[Triangles[TriIdx].Vertex1].Position.Z : vecMax.Z;
			vecMax.Z = (vecMax.Z < Verts[Triangles[TriIdx].Vertex2].Position.Z) ? Verts[Triangles[TriIdx].Vertex2].Position.Z : vecMax.Z;

		}



		FVector vecOrigin = ((vecMax - vecMin) / 2) + vecMin;	/* Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector */
		FVector BoxPoint = vecMax - vecMin;			/* The difference between the "Maximum Vertex" and the "Minimum Vertex" is our actual Bounds Box */
		return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
	}
	else
		return FBoxSphereBounds(ForceInit);
}