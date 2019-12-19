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

	//�ӹ������պ��SMC����л�ȡ����ͼ��GUID���������л���Json��
	FGuid GetLightMapGUIDFromSMC();
	//��AttachSurface�л�ȡ����Json�����л��Ĺ���ͼGUID
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