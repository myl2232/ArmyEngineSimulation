#pragma  once
#include "CoreMinimal.h"
#include "ArmyActor.h"
//#include "ArmyGenMeshComponent.h"
#include "ArmyProceduralMeshComponent.h"
#include "ArmyWindowActor.generated.h"
enum WindowActorType
{
	FLOOR_WINDOW,
	NORMAL_WINDOW,
	RECTBAY_WINDOW,
	TRAPEBAY_WINDOW
};
UCLASS()
class ARMYSCENEDATA_API AXRWindowActor :public AXRActor
{

	GENERATED_BODY()
public: 
	AXRWindowActor();
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaSeconds)override;
	void Destroy();
	void ResetMeshTriangles();
	void AddPillarPoints(const TArray<FVector>& InPointList, float BoardHeight);
	void AddWindowGlassPoints(const TArray<FVector>& InPointList, float BoardHeight);
	void AddWindowBoardPoints(const TArray<FVector>& InPoinstList, float BoardHeight);
	void SetWindowGlassMaterial(UMaterialInterface* InMaterial);
	void SetWindowBoardMaterial(UMaterialInterface* InMaterial);
	void SetWindowPillarMaterial(UMaterialInterface* InMaterial);
	void UpdataAlloVertexIndexBuffer();
	WindowActorType CurrentWindowType = NORMAL_WINDOW;
	float WinowLength;
	float WindowHeight;
	float WindowOffsetGroundHeight;
	float BayWindowDepth;
	FString WindowStoneName;/* @梁晓菲 窗台石名字*/
	//@马云龙 记录AttachSurface
	TSharedPtr<class FArmyHardware> AttachSurface;

	//从构建光照后的SMC组件中获取光照图的GUID，用来序列化到Json中
	FGuid GetLightMapGUIDFromSMC();
	//从AttachSurface中获取来自Json反序列化的光照图GUID
	FGuid GetLightMapGUIDFromAttachSurface();

private:
	void CaculateTriangleAndVertex(const TArray<FVector>& PointList, float BoardHeight,TArray<FArmyProcMeshVertex>& OutVertexs,TArray<FArmyProcMeshTriangle>& OutTriangles,TArray<FArmyProcMeshVertex> tempTotalVertexs);
	TArray<FArmyProcMeshVertex> TotalBoardVertexes;
	TArray<FArmyProcMeshTriangle> TotalBoardTriangles;
	TArray<FArmyProcMeshVertex> TotalGlassVertexes;
	TArray<FArmyProcMeshTriangle> TotalGlassTriangles;
	TArray<FArmyProcMeshVertex> TotalPillarVertexs;
	TArray<FArmyProcMeshTriangle> TotalPillarTriagnels;

	//class UXRGenMeshComponent* WindowPillarMesh;
	//class UXRGenMeshComponent* WindowGlassMesh;
	//class UXRGenMeshComponent* WindowBoardMesh;

	class UXRProceduralMeshComponent* MeshComponent;
};