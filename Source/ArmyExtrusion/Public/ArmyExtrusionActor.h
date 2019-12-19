#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRBaseArea.h"
#include "IXRConstructionHelper.h"
#include "ArmyExtrusionActor.generated.h"
UCLASS()
class XREXTRUSION_API AArmyExtrusionActor : public AActor ,public IXRConstructionHelper
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArmyExtrusionActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void SetMaterial(UMaterialInterface* InMaterial);

	//@马云龙 非常无奈的写法，因为当前拥有ExtrusionActor的Area有很多，而且是 1对N的关系
	TWeakPtr<class FXRBaseArea> AttachSurfaceArea;
	TWeakPtr<class FXRHardware> AttachHardware;
	TWeakPtr<class FXRRoomSpaceArea> AttachRoomSpaceArea;

	class UXRProceduralMeshComponent* MeshComponent;

	//从构建光照后的SMC组件中获取光照图的GUID，用来序列化到Json中
	FGuid GetLightMapGUIDFromSMC();
	//从AttachSurface中获取来自Json反序列化的光照图GUID
	FGuid GetLightMapGUIDFromAttachSurface();

	//放样类如 踢脚线，顶角线，按照新的标准，截面不应该再有阴面，可以提升全屋渲染效果，消除黑边问题
	//给出截面外圈点的第一个点A和最后一个点B，该函数会自动计算左下角（或左上角，比如顶角线）的顶点C，从而可以计算完整的侧截面
	// 踢脚线截面示例
	// A*****
	//		*
	//	   *
	//	  *
	//	  *
	//	  *
	//	   *
	//	    *
	//	    *
	//	    *
	//	    *
	//	    *
	//	    *
	// C    *B
	//
    FVector2D CalculateMissingVertex(FVector2D InA, FVector2D InB, bool bReverse);

public:
	/**
	 * 通过.dxf格式的剖面文件和轨迹点生成放样
	 */
	void GenerateMeshFromFilePath(
        const FString& FilePath,
        const TArray<FVector>& TrackPoints,
        bool bReverse = false,
        bool bIsClose = true,
        bool bNeedFillMissingVertext = false,
        FVector planeXDir = FVector(1,0,0),
        FVector planeYDir = FVector(0,1,0),
        FVector planeNormal=FVector(0,0,1),
        FVector planeCenter = FVector::ZeroVector);

    /** @罗雕 创建踢脚线的放样方法，v1.7版本临时使用 */
    void GenerateSkitLineMeshFromFilePath(
        const FString& FilePath,
        const TArray<FVector>& TrackPoints,
        bool bReverse = false,
        bool bIsClose = true,
        bool bNeedFillMissingVertext = false,
        FVector planeXDir = FVector(1, 0, 0),
        FVector planeYDir = FVector(0, 1, 0),
        FVector planeNormal = FVector(0, 0, 1),
        FVector planeCenter = FVector::ZeroVector);

	/**
	* 通过.dxf格式剖面文件和轨迹点放样生成
	*/
	void GeneratePassMeshFromFilePath(const FString& FilePath, const TArray<FVector>& TrackPoints, float passWidth, bool bClose = false, FVector planeXDir = FVector(1, 0, 0), FVector planeYDir = FVector(0, 1, 0), FVector planeNormal = FVector(0, 0, 1), FVector planeCenter = FVector::ZeroVector);

	/**
	 * 通过剖面点集和轨迹点生成放样
	 */
	void GenerateMeshWithOutlineData(
        const TArray<FVector2D> &ProfileOutline,
        const TArray<FVector> &TrackPoints,
        bool bReverse,
        bool bIsClosed = false ,
        bool bNeedFillMissingVertext = false,
        FVector planeXDir = FVector(1, 0, 0),
        FVector planeYDir = FVector(0, 1, 0),
        FVector planeNormal = FVector(0, 0, 1),
        FVector planeCenter = FVector::ZeroVector);

    /** @罗雕 创建踢脚线的放样方法，v1.7版本临时使用 */
    void GenerateSkitLineMeshWithOutlineData(
        const TArray<FVector2D> &ProfileOutline,
        const TArray<FVector> &TrackPoints,
        bool bReverse,
        bool bIsClosed = false,
        bool bNeedFillMissingVertext = false,
        FVector planeXDir = FVector(1, 0, 0),
        FVector planeYDir = FVector(0, 1, 0),
        FVector planeNormal = FVector(0, 0, 1),
        FVector planeCenter = FVector::ZeroVector);

	/**
	* 替换放样Mesh的纹理
	*/
	void ReplaceTexture(class UTexture *ColorMap, class UTexture *NormalMap);

    TArray<FVector2D> GetProfileVertexes() { return ProfileVertexes; }

	/* @梁晓菲 施工周长，都是闭合区域*/
	float Circumference;
	/* @梁晓菲 施工长度*/
	float Length;
	/* @梁晓菲 后台设置的关于此放样Actor所用的dxf的长宽高*/
	FString dxfName;
	float dxfLength;
	float dxfWidth;
	float dxfHeight;
	FString UniqueCodeExtrusion;

	//@郭子阳 获取施工项的参数
	ConstructionPatameters GetConstructionParameter() ;
	//@郭子阳 获取房间名
	TSharedPtr<FXRRoom> GetRoom() override;
	//@郭子阳 当房间ID改变时发生
	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) override;

protected:
    /** @罗雕 剖面顶点数据 */
    TArray<FVector2D> ProfileVertexes;
};