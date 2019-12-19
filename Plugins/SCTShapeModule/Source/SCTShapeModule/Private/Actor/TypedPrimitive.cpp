#include "TypedPrimitive.h"
#include "Engine/CollisionProfile.h"
#include "PhysicsEngine/BodySetup.h"

UPNTPrimitive::UPNTPrimitive(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
}

FPrimitiveSceneProxy* UPNTPrimitive::CreateSceneProxy()
{
	FPrimitiveSceneProxy *Proxy = nullptr;
	if (Vertices.Num() > 0)
	{
		Proxy = new FPNTPrimitiveSceneProxy(this);
	}
	
	return Proxy;
}

UBodySetup* UPNTPrimitive::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}

FBoxSphereBounds UPNTPrimitive::CalcBounds(const FTransform& LocalToWorld) const {

   return BoxSphereBounds.TransformBy(LocalToWorld);
}

bool UPNTPrimitive::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	for (int32 i = 0; i < Vertices.Num(); ++i)
		CollisionData->Vertices.Add(Vertices[i].Position);

	FTriIndices TriIndices;
	int32 NumIndex = Indices.Num();
	for (int32 i = 0; i < NumIndex; i += 3) {
		TriIndices.v0 = Indices[i];
		TriIndices.v1 = Indices[i + 1];
		TriIndices.v2 = Indices[i + 2];
		CollisionData->Indices.Emplace(TriIndices);
		CollisionData->MaterialIndices.Emplace(i / 3);
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UPNTPrimitive::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return bTriangle && Vertices.Num() > 0 && GetOwner()->bHidden == 0;
}

void UPNTPrimitive::SetPrimitiveInfo(bool InbTriangle, const TArray<FPVertex> &InVertices, const TArray<uint16> &InIndices)
{
	check(InIndices.Num() > 0 && (InIndices.Num() % 2 == 0 || InIndices.Num() % 3 == 0));

	bTriangle = InbTriangle;
	Vertices = InVertices;
	Indices = InIndices;
	UpdateCollision();
	MarkRenderStateDirty();
	

	TArray<FVector> BoundBoxVetices;
	// 设置顶点之后，计算包围盒
	for (const auto & VertexRef : Vertices)
	{
		BoundBoxVetices.Emplace(VertexRef.Position);
	}
	BoxSphereBounds = FBoxSphereBounds(FBox(BoundBoxVetices));
}

void UPNTPrimitive::UpdateBodySetup() {
	if (ModelBodySetup == nullptr) {
		ModelBodySetup = NewObject<UBodySetup>(this, UBodySetup::StaticClass()); 
		ModelBodySetup->AddToRoot();
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UPNTPrimitive::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();


		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();

		CreatePhysicsState();
	}
}

FPNTPrimitiveSceneProxy::FPNTPrimitiveSceneProxy(const UPNTPrimitive *InComponent)
	: FPrimitiveSceneProxy(InComponent),
	MaterialRelevance(InComponent->GetMaterialRelevance(ERHIFeatureLevel::SM4))
{
	VertexBuffer.Vertices.SetNumUninitialized(InComponent->Vertices.Num());
	FDynamicMeshVertex *VertexData = VertexBuffer.Vertices.GetData();

	if (InComponent->bTriangle) 
	{
		struct FTangentBasis
		{
			FVector TangentX;
			FVector TangentY;
			FVector TangentZ;

			FTangentBasis()
				: TangentX(ForceInitToZero),
				TangentY(ForceInitToZero),
				TangentZ(ForceInitToZero) {}
		};

		TArray<FTangentBasis> TangentBasis;
		TangentBasis.SetNumZeroed(InComponent->Vertices.Num());

		int32 NumIndex = InComponent->Indices.Num();
		for (int32 iIndex = 0; iIndex < NumIndex; iIndex += 3)
		{
			uint16 i0 = InComponent->Indices[iIndex];
			uint16 i1 = InComponent->Indices[iIndex + 1];
			uint16 i2 = InComponent->Indices[iIndex + 2];

			const FVector &v0 = InComponent->Vertices[i0].Position;
			const FVector &v1 = InComponent->Vertices[i1].Position;
			const FVector &v2 = InComponent->Vertices[i2].Position;

			FVector Edge01 = v1 - v0;
			FVector Edge02 = v2 - v0;

			FVector TangentX = Edge01.GetSafeNormal();
			FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
			FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

			VertexData[i0].Position = v0;
			VertexData[i1].Position = v1;
			VertexData[i2].Position = v2;

			TangentBasis[i0].TangentX += TangentX;
			TangentBasis[i1].TangentX += TangentX;
			TangentBasis[i2].TangentX += TangentX;

			TangentBasis[i0].TangentY += TangentY;
			TangentBasis[i1].TangentY += TangentY;
			TangentBasis[i2].TangentY += TangentY;

			TangentBasis[i0].TangentZ += TangentZ;
			TangentBasis[i1].TangentZ += TangentZ;
			TangentBasis[i2].TangentZ += TangentZ;
		}

		for (int32 iVert = 0; iVert < InComponent->Vertices.Num(); ++iVert)
		{
			TangentBasis[iVert].TangentX.Normalize();
			TangentBasis[iVert].TangentY.Normalize();
			TangentBasis[iVert].TangentZ.Normalize();
			//VertexData[iVert].Position = InComponent->Vertices[iVert].Position;
			VertexData[iVert].SetTangents(TangentBasis[iVert].TangentX, TangentBasis[iVert].TangentY, TangentBasis[iVert].TangentZ);
		}
	}
	else
	{
		for (int32 iVert = 0; iVert < InComponent->Vertices.Num(); ++iVert)
			VertexData[iVert].Position = InComponent->Vertices[iVert].Position;
	}

	IndexBuffer.Indices.SetNum(InComponent->Indices.Num());
	FMemory::Memcpy(IndexBuffer.Indices.GetData(), InComponent->Indices.GetData(), InComponent->Indices.Num() * sizeof(uint16));

	VertexFactory.Init(&VertexBuffer);

	BeginInitResource(&VertexBuffer);
	BeginInitResource(&IndexBuffer);
	BeginInitResource(&VertexFactory);

	Material = InComponent->GetMaterial(0);
	if (Material == nullptr)
		Material = UMaterial::GetDefaultMaterial(MD_Surface);

	bTriangle = InComponent->bTriangle;
}

void FPNTPrimitiveSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, 
	const FSceneViewFamily& ViewFamily, 
	uint32 VisibilityMap, 
	FMeshElementCollector& Collector) const
{
	checkSlow(IsInParallelRenderingThread());

	FMaterialRenderProxy *MaterialProxy = Material->GetRenderProxy(IsSelected());

	int32 PrimitiveDiv = bTriangle ? 3 : 2;
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex) {
		const FSceneView* View = Views[ViewIndex];
		if (IsShown(View) && (VisibilityMap & (1 << ViewIndex))) {
			FMeshBatch &Mesh = Collector.AllocateMesh();
			FMeshBatchElement &BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = false;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / PrimitiveDiv;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = bTriangle ? PT_TriangleList : PT_LineList;
			Mesh.DepthPriorityGroup = GetStaticDepthPriorityGroup();
			Mesh.bCanApplyViewModeOverrides = false;
			Collector.AddMesh(ViewIndex, Mesh);
		}
	}
}

FPrimitiveViewRelevance FPNTPrimitiveSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	// 拷贝MaterialRelevance到FPrimitiveViewRelevance中
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}



UPNTUVPrimitive::UPNTUVPrimitive(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
}

FPrimitiveSceneProxy* UPNTUVPrimitive::CreateSceneProxy()
{
	FPrimitiveSceneProxy *Proxy = nullptr;
	if (Vertices.Num() > 0)
		Proxy = new FPNTUVPrimitiveSceneProxy(this);
	return Proxy;
}

UBodySetup* UPNTUVPrimitive::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}

FBoxSphereBounds UPNTUVPrimitive::CalcBounds(const FTransform& LocalToWorld) const {

	return BoxSphereBounds.TransformBy(LocalToWorld);
}

bool UPNTUVPrimitive::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	for (int32 i = 0; i < Vertices.Num(); ++i)
		CollisionData->Vertices.Add(Vertices[i].Position);

	FTriIndices TriIndices;
	int32 NumIndex = Indices.Num();
	for (int32 i = 0; i < NumIndex; i += 3) {
		TriIndices.v0 = Indices[i];
		TriIndices.v1 = Indices[i + 1];
		TriIndices.v2 = Indices[i + 2];
		CollisionData->Indices.Emplace(TriIndices);
		CollisionData->MaterialIndices.Emplace(i / 3);
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UPNTUVPrimitive::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return Vertices.Num() > 0;
}

void UPNTUVPrimitive::SetPrimitiveInfo(const TArray<FPUVVertex> &InVertices, const TArray<uint16> &InIndices)
{
	check(InIndices.Num() > 0 && InIndices.Num() % 3 == 0);

	Vertices = InVertices;
	Indices = InIndices;
	UpdateCollision();
	MarkRenderStateDirty();
	TArray<FVector> BoundBoxVetices;
	// 设置顶点之后，计算包围盒
	for (const auto & VertexRef : Vertices)
	{
		BoundBoxVetices.Emplace(VertexRef.Position);
	}
	BoxSphereBounds = FBoxSphereBounds(FBox(BoundBoxVetices));
}


void UPNTUVPrimitive::UpdateBodySetup() {
	if (ModelBodySetup == nullptr) {
		ModelBodySetup = NewObject<UBodySetup>(this, UBodySetup::StaticClass());
		ModelBodySetup->AddToRoot();
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UPNTUVPrimitive::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();


		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();

		CreatePhysicsState();
	}
}

FPNTUVPrimitiveSceneProxy::FPNTUVPrimitiveSceneProxy(const UPNTUVPrimitive *InComponent)
	: FPrimitiveSceneProxy(InComponent),
	MaterialRelevance(InComponent->GetMaterialRelevance(ERHIFeatureLevel::SM4))
{
	VertexBuffer.Vertices.SetNumUninitialized(InComponent->Vertices.Num());
	FDynamicMeshVertex *VertexData = VertexBuffer.Vertices.GetData();


	struct FTangentBasis
	{
		FVector TangentX;
		FVector TangentY;
		FVector TangentZ;

		FTangentBasis()
			: TangentX(ForceInitToZero),
			TangentY(ForceInitToZero),
			TangentZ(ForceInitToZero) {}
	};

	TArray<FTangentBasis> TangentBasis;
	TangentBasis.SetNumZeroed(InComponent->Vertices.Num());

	int32 NumIndex = InComponent->Indices.Num();
	for (int32 iIndex = 0; iIndex < NumIndex; iIndex += 3)
	{
		uint16 i0 = InComponent->Indices[iIndex];
		uint16 i1 = InComponent->Indices[iIndex + 1];
		uint16 i2 = InComponent->Indices[iIndex + 2];

		const FVector &v0 = InComponent->Vertices[i0].Position;
		const FVector &v1 = InComponent->Vertices[i1].Position;
		const FVector &v2 = InComponent->Vertices[i2].Position;

		FVector Edge01 = v1 - v0;
		FVector Edge02 = v2 - v0;

		FVector TangentX = Edge01.GetSafeNormal();
		FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
		FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

		/*VertexData[i0].Position = v0;
		VertexData[i1].Position = v1;
		VertexData[i2].Position = v2;*/

		TangentBasis[i0].TangentX += TangentX;
		TangentBasis[i1].TangentX += TangentX;
		TangentBasis[i2].TangentX += TangentX;

		TangentBasis[i0].TangentY += TangentY;
		TangentBasis[i1].TangentY += TangentY;
		TangentBasis[i2].TangentY += TangentY;

		TangentBasis[i0].TangentZ += TangentZ;
		TangentBasis[i1].TangentZ += TangentZ;
		TangentBasis[i2].TangentZ += TangentZ;
	}

	for (int32 iVert = 0; iVert < InComponent->Vertices.Num(); ++iVert)
	{
		TangentBasis[iVert].TangentX.Normalize();
		TangentBasis[iVert].TangentY.Normalize();
		TangentBasis[iVert].TangentZ.Normalize();
		VertexData[iVert].Position = InComponent->Vertices[iVert].Position;
		VertexData[iVert].TextureCoordinate = InComponent->Vertices[iVert].Texcoord0;
		VertexData[iVert].SetTangents(TangentBasis[iVert].TangentX, TangentBasis[iVert].TangentY, TangentBasis[iVert].TangentZ);
	}
	

	IndexBuffer.Indices.SetNum(InComponent->Indices.Num());
	FMemory::Memcpy(IndexBuffer.Indices.GetData(), InComponent->Indices.GetData(), InComponent->Indices.Num() * sizeof(uint16));

	VertexFactory.Init(&VertexBuffer);

	BeginInitResource(&VertexBuffer);
	BeginInitResource(&IndexBuffer);
	BeginInitResource(&VertexFactory);

	Material = InComponent->GetMaterial(0);
	if (Material == nullptr)
		Material = UMaterial::GetDefaultMaterial(MD_Surface);

}

void FPNTUVPrimitiveSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily,
	uint32 VisibilityMap,
	FMeshElementCollector& Collector) const
{
	checkSlow(IsInParallelRenderingThread());

	FMaterialRenderProxy *MaterialProxy = Material->GetRenderProxy(IsSelected());

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex) {
		const FSceneView* View = Views[ViewIndex];
		if (IsShown(View) && (VisibilityMap & (1 << ViewIndex))) {
			FMeshBatch &Mesh = Collector.AllocateMesh();
			FMeshBatchElement &BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = false;
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

FPrimitiveViewRelevance FPNTUVPrimitiveSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	// 拷贝MaterialRelevance到FPrimitiveViewRelevance中
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}