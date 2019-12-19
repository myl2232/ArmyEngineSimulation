#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "ArmyExtrusionMeshComponent.generated.h"

USTRUCT()
struct FMeshVertex {
	GENERATED_USTRUCT_BODY()

	FVector Position;
	FVector2D UV;
	FColor Color;

	FMeshVertex()
		: Position(FVector::ZeroVector),
		UV(FVector2D::ZeroVector),
		Color(FColor::White) {}
};

USTRUCT()
struct FMeshTriangle {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	uint16 Vertex0;

	UPROPERTY()
	uint16 Vertex1;

	UPROPERTY()
	uint16 Vertex2;
};

UCLASS(ClassGroup = (Rendering, Common), editinlinenew)
class UArmyExtrusionMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider {
	GENERATED_UCLASS_BODY()

public:
	friend class FDTOOLGenMeshSceneProxy;

	bool SetGeneratedMeshGeomInfo(const TArray<FMeshVertex> &Vertices, const TArray<FMeshTriangle> &Triangles);
	const TArray<FMeshVertex>& GetVertices() const { return GeneratedMeshVerts; }
	const TArray<FMeshTriangle>& GetTriangles() const { return GeneratedMeshTriangles; }

	UPROPERTY()
	class UBodySetup* ModelBodySetup;

	// UMeshComponent Method
	virtual int32 GetNumMaterials() const override;

	// IInterface_CollisionDataProvider Methods
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override { return false; }

	// UPrimitiveComponent Methods
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	virtual bool ShouldRenderSelected() const override;

	void UpdateBodySetup();
	void UpdateCollision();


	// USceneComponent Method
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	TArray<FMeshVertex> GeneratedMeshVerts;
	TArray<FMeshTriangle> GeneratedMeshTriangles;
};