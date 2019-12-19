#pragma once

#include "CoreMinimal.h"
#include "ArmyActor.h"
//#include "ArmyGenMeshComponent.h"
#include "ArmyProceduralMeshComponent.h"
#include "ArmyShapeBoardActor.generated.h"

UCLASS()
class ARMYSCENEDATA_API AXRShapeActor :public AXRActor
{
	GENERATED_BODY()

public:
	AXRShapeActor();
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaSeconds)override;

	void Destroy();

public:
	void ResetMeshTriangles( );

	void AddBoardPoints(const TArray<FVector>& PointList, float BoardHeight);

	void AddQuatGemometry(const TArray<FVector>& PointList);
	void AddVetries(const TArray<FVector>& PointList);

	void AddVerts( TArray<FDynamicMeshVertex>& Points) ;

	void SetMaterial(UMaterialInterface* Material);
	void UpdateAllVetexBufferIndexBuffer();
	class UXRProceduralMeshComponent* GetMeshComponent();

	//@¥Ú∞Áº“ XRLightmass ¡Ÿ ± SetGeometryBuffer
	void SetGeometryBuffer(TArray<FVector>& InVertexBuffer, TArray<FVector2D>& InUVs, TArray<int32>& InIndexBuffer);
	void SetGeometryBufferDirectly(TArray<FArmyProcMeshVertex> InVertexBuffer, TArray<FArmyProcMeshTriangle> InIndexBuffer);
	class UXRProceduralMeshComponent* BoardMesh;

	FGuid GetLightMapGUIDFromSMC();
	FGuid LightMapID;
private:
	int64 MaterialId;
	TArray<FArmyProcMeshVertex> TotalVertexs;
	TArray<FArmyProcMeshTriangle> TotalTriangles;
};