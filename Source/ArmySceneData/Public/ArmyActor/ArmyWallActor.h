#pragma  once

#include "ArmyShapeBoardActor.h"
#include "ArmyCommonTypes.h"
#include "ArmyWallActor.generated.h"

UCLASS()

class ARMYSCENEDATA_API AXRWallActor :public AXRActor
{
	GENERATED_BODY()
public:
	EActorType ActorType = WallAreaActor;
	AXRWallActor();
	TSharedPtr<class FArmyBaseArea> AttachSurface;
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaSeconds)override;
	void Destroy();
	void ResetMeshTriangles();
	void AddVerts(TArray<FDynamicMeshVertex>& Points);
	void AddGapVerts(TArray<FDynamicMeshVertex>& Points);

	void UpdateAllVetexBufferIndexBuffer();
	void SetMaterial(UMaterialInterface* Material);

	void SetBrickMaterial(UMaterialInterface* Material);

	//UXRGenMeshComponent* GetMeshComponent();
	class UXRProceduralMeshComponent* GetMeshComponent();

	//从构建光照后的SMC组件中获取光照图的GUID，用来序列化到Json中
	FGuid GetLightMapGUIDFromSMC();
	//从AttachSurface中获取来自Json反序列化的光照图GUID
	FGuid GetLightMapGUIDFromAttachSurface();

	class UXRProceduralMeshComponent* MeshComponent;

private:
	TArray<FArmyProcMeshVertex> TotalBrickVerts;
	TArray<FArmyProcMeshTriangle> TotalBirckTriangles;

	TArray<FArmyProcMeshVertex> TotalBrickGapVerts;
	TArray<FArmyProcMeshTriangle> TotalBirckGapTriangles;

	//class UXRGenMeshComponent* BrickComponent;
	//class UXRGenMeshComponent*BrickGapsComponent;

};