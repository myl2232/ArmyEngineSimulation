
#pragma once

#include "VertexType.h"
#include "HardwareBuffer.h"
#include "MeshBatch.h"
#include "SceneManagement.h"
#include "RHIDefinitions.h"
#include "Materials/Material.h"
#include "Components/MeshComponent.h"
#include "Interfaces/Interface_CollisionDataProvider.h"
#include "UObject/ObjectMacros.h"
#include "TypedPrimitive.generated.h"

UCLASS()
class SCTSHAPEMODULE_API UPNTPrimitive : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_UCLASS_BODY()

public:
	friend class FPNTPrimitiveSceneProxy;

	// UMeshComponent Method
	virtual int32 GetNumMaterials() const override { return 1; }

	// UPrimitiveComponent Methods
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;

	// USceneComponent Method
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	// IInterface_CollisionDataProvider Methods
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override { return false; }

	void SetPrimitiveInfo(bool InbTriangle, const TArray<FPVertex> &InVertices, const TArray<uint16> &InIndices);
	TArray<FPVertex>& GetVertices() { return Vertices; }
	const TArray<FPVertex>& GetVertices() const { return Vertices; }
	TArray<uint16>& GetIndices() { return Indices; }
	const TArray<uint16>& GetIndices() const { return Indices; }
	void UpdateBodySetup();
	void UpdateCollision();

private:
	class UBodySetup* ModelBodySetup;	
	FBoxSphereBounds BoxSphereBounds;
	bool bTriangle;
	TArray<FPVertex> Vertices;
	TArray<uint16> Indices;
};

template <>
class TTypedVertexFactory<UPNTPrimitive> : public FLocalVertexFactory
{
public:
	void Init(const FDynVertexBuffer *InVertexBuffer)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitPNTVertexFactory,
			TTypedVertexFactory<UPNTPrimitive>*, InVertexFactory, this,
			const FDynVertexBuffer *, InVertexBuffer, InVertexBuffer,
			{
				FDataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TextureCoordinate, VET_Float2));
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				InVertexFactory->SetData(NewData);
			}
		);
	}
};

using TPNTVertexFactory = TTypedVertexFactory<UPNTPrimitive>;

class FPNTPrimitiveSceneProxy : public FPrimitiveSceneProxy
{
public:
	FPNTPrimitiveSceneProxy(const UPNTPrimitive *InComponent);
	
	virtual ~FPNTPrimitiveSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override;
	
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

private:
	bool bTriangle;

	FDynVertexBuffer VertexBuffer;
	FUint16IndexBuffer IndexBuffer;
	TPNTVertexFactory VertexFactory;

	UMaterialInterface *Material;
	FMaterialRelevance MaterialRelevance;
};



UCLASS()
class SCTSHAPEMODULE_API UPNTUVPrimitive : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_UCLASS_BODY()

public:
	friend class FPNTUVPrimitiveSceneProxy;

	// UMeshComponent Method
	virtual int32 GetNumMaterials() const override { return 1; }

	// UPrimitiveComponent Methods
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;

	// USceneComponent Method
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	// IInterface_CollisionDataProvider Methods
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override { return false; }

	void SetPrimitiveInfo(const TArray<FPUVVertex> &InVertices, const TArray<uint16> &InIndices);	
	TArray<FPUVVertex>& GetVertices() { return Vertices; }
	const TArray<FPUVVertex>& GetVertices() const { return Vertices; }
	TArray<uint16>& GetIndices() { return Indices; }
	const TArray<uint16>& GetIndices() const { return Indices; }
	void UpdateBodySetup();
	void UpdateCollision();

private:
	//UPROPERTY()
	class UBodySetup* ModelBodySetup;
	FBoxSphereBounds BoxSphereBounds;
	TArray<FPUVVertex> Vertices;
	TArray<uint16> Indices;
};

template <>
class TTypedVertexFactory<UPNTUVPrimitive> : public FLocalVertexFactory
{
public:
	void Init(const FDynVertexBuffer *InVertexBuffer)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitPNTUVVertexFactory,
			TTypedVertexFactory<UPNTUVPrimitive>*, InVertexFactory, this,
			const FDynVertexBuffer *, InVertexBuffer, InVertexBuffer,
			{
				FDataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TextureCoordinate, VET_Float2));
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				//NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(InVertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				InVertexFactory->SetData(NewData);
			}
		);
	}
};

using TPNTUVVertexFactory = TTypedVertexFactory<UPNTUVPrimitive>;

class FPNTUVPrimitiveSceneProxy : public FPrimitiveSceneProxy
{
public:
	FPNTUVPrimitiveSceneProxy(const UPNTUVPrimitive *InComponent);

	virtual ~FPNTUVPrimitiveSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

private:
	FDynVertexBuffer VertexBuffer;
	FUint16IndexBuffer IndexBuffer;
	TPNTUVVertexFactory VertexFactory;

	UMaterialInterface *Material;
	FMaterialRelevance MaterialRelevance;
};